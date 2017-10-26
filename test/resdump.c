#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <sys/mman.h>

#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mpanel.h"
#include "mmainwnd.h"
#include "mdialog.h"
#include "mresmgr.h"
#include "mrdr.h"


#define _MGRM_PRINTF(fmt...) fprintf (stdout, fmt)

static void dump_mgrp_header (NCSRM_HEADER *header);
#ifdef _MGRM_DEBUG
static void dump_ui_data (HPACKAGE package, NCSRM_WINHEADER *win_header);
#endif
//static void dump_rdr_info(NCS_RDR_INFO *rdrinfo);
//static void dump_rdrset_info(HPACKAGE package, Uint32 *data);

static void dumpStringResInfo (HPACKAGE hPackage);
static void dumpTextResInfo (HPACKAGE hPackage);
static void dumpRdrResInfo (HPACKAGE hPackage);
static void dumpUIResInfo (HPACKAGE hPackage);



#define Package2TypeMaps(header) \
    (NCSRM_TYPEITEM*)((char*)header + sizeof (NCSRM_HEADER))

typedef struct _MAPFILELIST MAPFILELIST;
typedef MAPFILELIST* PMAPFILELIST;
struct _MAPFILELIST
{
    void         *data;
    long         data_size;
    Uint32       filename_id;
    PMAPFILELIST prev;
    PMAPFILELIST next;
};

typedef struct _RPINFO
{
    PMAPFILELIST head;
    int          ref;
    void         *data;
    long         data_size;
}RPINFO;

typedef RPINFO* PRPINFO;

static int append_list(HPACKAGE package, void* data,
        Uint32 filename_id, long data_size)
{
    PMAPFILELIST head = ((PRPINFO)package)->head;
    PMAPFILELIST last, list;

    list = (PMAPFILELIST)calloc(1, sizeof(MAPFILELIST));

    if (!list) return -1;

    list->data = data;
    list->filename_id = filename_id;
    list->data_size = data_size;
    list->prev = NULL;
    list->next = NULL;

    if (head == NULL) {
        ((PRPINFO)package)->head = list;
        return 0;
    }

    last = head;

    while (last->next) {
        last = last->next;
    }

    last->next = list;
    list->prev = last;

    return 0;
}

static PMAPFILELIST find_list(HPACKAGE package, Uint32 filename_id)
{
    PMAPFILELIST head = ((PRPINFO)package)->head;
    PMAPFILELIST first;

    if (!head) {
        return NULL;
    }

    first = head;

    while(first->next) {
        if (first->filename_id == filename_id) {
            return first;
        }
        first = first->next;
    }

    return NULL;
}

#if 0
static void del_alllist(HPACKAGE package)
{
    PMAPFILELIST head, list;

    head = ((PRPINFO)package)->head;

    if (!head)
        return;

    ((PRPINFO)package)->head = NULL;

    while(head) {
        munmap (head->data, head->data_size);
        list = head;
        head = head->next;

        free(list);
    }
    return;
}

static int del_list(HPACKAGE package, Uint32 filename_id)
{
    PMAPFILELIST head = ((PRPINFO)package)->head;
    PMAPFILELIST first, list;
    BOOL found = FALSE;

    if (!head) {
        return -1;
    }

    first = head;

    if (first->filename_id == filename_id) {
        ((PRPINFO)package)->head = first->next;
        if (first->next)
            first->next->prev = NULL;

        list = first;
        found = TRUE;
    }
    else {
        while(first->next) {
            if (first->next->filename_id == filename_id) {
                list = first->next;
                if(list->next){
                    first->next = list->next;
                    first->next->prev = list->prev;
                } else { // the found at the tail of the list.
                    first->next = NULL;
                }
                found = TRUE;
                break;
            }
            first = first->next;
        }
    }

    if (found == TRUE) {
        munmap (list->data, list->data_size);
        free(list);
        return 0;
    }

    return -1;
}
#endif

static NCSRM_TYPEITEM* search_type_item (NCSRM_HEADER *header, Uint16 type)
{
    NCSRM_TYPEITEM   *type_item;
    int i = 0;

    type_item = Package2TypeMaps(header);

    while (i < header->nr_sect) {
        if (type & type_item->type)
            return type_item;

        type_item ++;
    }

    return NULL;
}

static NCSRM_IDITEM*
binary_search_iditem (Uint32 *sect_base, Uint32 res_id)
{
    NCSRM_IDITEM     *id_item, *base_iditem;
    int             low, high, mid;
    NCSRM_SECTHEADER *sect_header = (NCSRM_SECTHEADER *)sect_base;

    if (!sect_base)
        return NULL;

    base_iditem =
        (NCSRM_IDITEM *)((char*)sect_base + sizeof(NCSRM_SECTHEADER));

    low = 0;
    high = sect_header->size_maps;

    while (low < high) {
        mid = (low + high)/2;
        id_item = base_iditem + mid;

        if (id_item->id < res_id)
            low = mid + 1;
        else
            high = mid;
    }

    if ((low < sect_header->size_maps)
            && ((base_iditem + low)->id == res_id))
        return base_iditem + low;
    else
        return 0;
}

static void* mmap_file (const char *file_name, long *file_size)
{
    FILE* fp = NULL;
    void* data;

    _MGRM_PRINTF ("mmap file:[%s] \n", file_name);
    if (!file_name)
        return NULL;

    if ((fp = fopen (file_name, "rb")) == NULL) {

        _MGRM_PRINTF ("RESMANAGER>mmap: can't open file: %s\n", file_name);
        return NULL;
    }

    /* get file size*/
    fseek (fp, 0, SEEK_END);
    *file_size = ftell (fp);

    data = mmap (0, *file_size, PROT_READ, MAP_PRIVATE, fileno(fp), 0);
    fclose (fp);

    if (!data || (data == MAP_FAILED)) {

        _MGRM_PRINTF ("RESMANAGER>mmap: mmap file[%s] error.\n", file_name);
        return 0;
    }

    return data;
}

static char* find_mmap_file(HPACKAGE package, Uint32 filename_id)
{
    PMAPFILELIST list;
    long         size;
    char         *data;

    if (filename_id <= 0)
        return NULL;

    list = find_list(package, filename_id);

    if (list) {
        //reference count++
        return list->data;
    }

    data = mmap_file (ncsGetString(package, filename_id), &size);

    if (data == NULL) {
        return NULL;
    }

    append_list(package, data, filename_id, size);

    return data;
}

static char* get_res_sectaddr (HPACKAGE hPackage, int type)
{
    NCSRM_HEADER     *res_head = ((PRPINFO)hPackage)->data;
    NCSRM_TYPEITEM   *type_item;

    type_item = search_type_item (res_head, type);

    if (!type_item) {
        return 0;
    }

    if (type_item->offset == 0) {
        return find_mmap_file (hPackage, type_item->filename_id);
    }
    else
        return (char*)res_head + type_item->offset;
}

static char en_locale[8] = "en_US";
//static char gLocaleValue[8] = "en_US";


static char* _get_locale_base_addr (HPACKAGE handle,
        NCSRM_SECTHEADER* sect,
        NCSRM_LOCALEITEM* locale, const char* def_locale)
{
    char cur_locale[20];
    int i = 0;
    char *sect_addr = NULL;

    while (i < sect->size_maps) {
        sprintf (cur_locale, "%s_%s", locale->language, locale->country);

        if (strcmp(def_locale, cur_locale) == 0) {
            if (locale->offset == 0) {
                sect_addr = find_mmap_file (handle, locale->filename_id);
                if (sect_addr == NULL)
                    return NULL;
            }
            else
                sect_addr = (char*)sect + locale->offset;
            break;
        }
        locale++;
        i++;
    }
    return sect_addr;
}

static NCSRM_IDITEM* getIDItem (HPACKAGE handle, Uint32 res_id, char **sect_addr)
{
    NCSRM_SECTHEADER* sect;
    NCSRM_LOCALEITEM* locale;

    *sect_addr = get_res_sectaddr(handle, res_id>>16);
    if (*sect_addr == NULL)
        return NULL;

    //find locale file according to locale map info
    if (res_id>>16 == NCSRT_TEXT) {
        sect = (NCSRM_SECTHEADER*)(*sect_addr);
        locale = (NCSRM_LOCALEITEM*)((char*)(*sect_addr) + sizeof(NCSRM_SECTHEADER) + sizeof(NCSRM_DEFLOCALE_INFO));

        *sect_addr = _get_locale_base_addr(handle, sect, locale, ncsGetDefaultLocale());
        if (!*sect_addr) {
            *sect_addr = _get_locale_base_addr(handle, sect, locale, en_locale);
        }
    }

    return binary_search_iditem ((Uint32*)*sect_addr, res_id);
}

#define IMAGE_BITMAP    0x01
#define IMAGE_MYBITMAP  0x02

typedef struct _CACHED_IMAGEDATA
{
    Uint32 res_id;
    Uint32 type;
    BOOL   is_incore;
    void   *image;
}CACHED_IMAGEDATA;

#if 0
static NCS_EVENT_HANDLER* get_event_handler (int win_id,
                    NCS_EVENT_HANDLER_INFO* handlers)
{
    if (!handlers)
        return NULL;

    while (handlers->id != -1) {

        if (handlers->id == win_id)
            return handlers->handler;

        handlers++;
    }

    return NULL;
}
#endif

#define UIDATA_ERR          -1
#define UIDATA_ST_INCORE    0
#define UIDATA_ST_FILE      1

static int get_ui_data(HPACKAGE package, Uint32 wnd_id,
                void **data, Uint32 *name_id)
{
    char        *ui;
    NCSRM_IDITEM *item;

    if ((wnd_id >>16) != NCSRT_UI)
        return UIDATA_ERR;

    item = getIDItem (package, wnd_id, &ui);

    if (item == 0) {
        _MGRM_PRINTF ("RESMANAGER>Error: No found ui resource by id(%x).\n", wnd_id);
        return UIDATA_ERR;
    }

    *name_id = item->filename_id;

    /* ST:file */
    if (item->offset == 0) {
        *data = (void*)find_mmap_file (package, item->filename_id);

        if (*data == NULL) {
            return UIDATA_ERR;
        }
#ifdef _MGRM_DEBUG
        dump_ui_data (package, (NCSRM_WINHEADER*)*data);
#endif

        return UIDATA_ST_FILE;
    }

    *data = (void*)(ui + item->offset);
#ifdef _MGRM_DEBUG
        dump_ui_data (package, (NCSRM_WINHEADER*)*data);
#endif

    /*ST:incore*/
    return UIDATA_ST_INCORE;
}

static void free_rdr_info (NCS_RDR_INFO *rdrinfo)
{
    if (rdrinfo) {
       if (rdrinfo->elements)
            free(rdrinfo->elements);

       free(rdrinfo);
    }
}

static BOOL get_rdr_info (HPACKAGE package, Uint32 rdr_id,
        NCS_RDR_INFO** rdrinfo, char** className)
{
    Uint32          *data;
    NCSRM_IDITEM     *item;
    NCSRM_RDRINFO    *info;
    int             i = 0;
	NCS_RDR_ELEMENT *elements;
    DWORD           value;

    if (rdr_id <= 0 || (rdr_id>>16 != NCSRT_RDR && rdr_id>>16 != NCSRT_RDRSET))
        return FALSE;

    *rdrinfo = calloc(1, sizeof(NCS_RDR_INFO));

    if (*rdrinfo == NULL)
        return FALSE;

    item = getIDItem (package, rdr_id, (char**)(void *)&data);

    if (item == NULL || item->offset == 0) {
        free(*rdrinfo);
        return FALSE;
    }

    data = (Uint32*)((char*)data + item->offset);

    //rdrname
    (*rdrinfo)->glb_rdr = (*rdrinfo)->ctl_rdr = ncsGetString(package, *data);
    _MGRM_PRINTF("[rdrid, rdrname] :[0x%0x, %s] \n", *data, (*rdrinfo)->glb_rdr);
    data++;

    //class name
    *className = (char*)ncsGetString(package, *data);
    _MGRM_PRINTF("[clsid, clsname] :[0x%0x, %s] \n", *data, *className);
    data++;

    info = (NCSRM_RDRINFO*)data;
    i = 0;
    //id, value
    while(info->id != -1) {
        i++;
        ++info;
    }

    elements = calloc (i+1, sizeof(NCS_RDR_ELEMENT));

    info = (NCSRM_RDRINFO*)data;
    i = 0;

    while(info->id != -1) {
        if (NCS_TYPE_IS_IMAGE(info->id)) {
            const char* file = ncsGetImageFileName (package, info->value);

            if (RegisterResFromFile(HDC_SCREEN, file))
                _MGRM_PRINTF("register res %s ok. \n", file);
            else
                _MGRM_PRINTF("register res error.\n");
            value = (DWORD)Str2Key(file);
        }
        else{
			//FIXED ME : Speical for skin
			if((WE_ATTR_TYPE_RDR==(WE_ATTR_TYPE_MASK&info->id))
				&& ((WE_ATTR_INDEX_MASK&info->id)>=1 && (WE_ATTR_INDEX_MASK&info->id)<WE_LFSKIN_NUMBER))
			{
				//global skin
				const char* file = ncsGetImageFileName(package, info->value);
				if (RegisterResFromFile(HDC_SCREEN, file))
                	_MGRM_PRINTF("register res %s ok. \n", file);
            	else
                	_MGRM_PRINTF("register res error.\n");
				value = (DWORD)file;
			}
			else if (WE_ATTR_TYPE_FONT==(WE_ATTR_TYPE_MASK&info->id)){
                const char *font_name = ncsGetString(package, info->value);
                value = (DWORD)LoadResource(font_name, RES_TYPE_FONT, 0L);
            } else {
            	value = info->value;
            }
		}

        elements[i].id = info->id;
        elements[i].value = value;

        ++i;
        ++info;
    }
    elements[i].id = -1;
    elements[i].value = -1;

    (*rdrinfo)->elements = elements;

#ifdef _MGRM_DEBUG
    dump_rdr_info(*rdrinfo);
#endif
    return TRUE;
}

static BOOL get_rdrset_info(HPACKAGE package, int rdrset_id, char** data)
{
    NCSRM_IDITEM *item;

    if (rdrset_id>>16 != NCSRT_RDRSET)
        return FALSE;

    item = getIDItem (package, rdrset_id, data);

    if (item == NULL || item->offset == 0)
        return FALSE;

    *data += item->offset;

#ifdef _MGRM_DEBUG
    _MGRM_PRINTF("data addr:%p \n", *data);
    dump_rdrset_info(package, (Uint32*)*data);
#endif
    return TRUE;
}


int main(int argc, char* argv[])
{
	if(argc <= 1)
	{
		printf("usage : %s <package-file-name> [type [type] ...]]\n", argv[0]);

		printf("\t type is : Header, String, UI, Rdr, Text\n");
		printf("\t No types mean dump all\n");
		return 0;
	}

	HPACKAGE package = ncsLoadResPackageFromFile(argv[1]);

	if(package == HPACKAGE_NULL)
	{
		printf("cannot open package :%s\n",argv[1]);
		return -1;
	}
	
	if(argc == 2)
	{
		dump_mgrp_header ((NCSRM_HEADER*) (((PRPINFO)package)->data));
		dumpStringResInfo((HPACKAGE)package);
		dumpUIResInfo((HPACKAGE)package);
		dumpRdrResInfo ((HPACKAGE)package);
		dumpTextResInfo((HPACKAGE)package);
		ncsUnloadResPackage(package);
		return 0;
	}

	int i;
	for(i=2; i<argc; i++)
	{
		if(strcasecmp(argv[i], "Header") == 0)
			dump_mgrp_header ((NCSRM_HEADER*) (((PRPINFO)package)->data));
		else if(strcasecmp(argv[i],"String") == 0)
			dumpStringResInfo(package);
		else if(strcasecmp(argv[i],"UI") == 0)
			dumpUIResInfo(package);
		else if(strcasecmp(argv[i],"Rdr") == 0)
			dumpRdrResInfo(package);
		else if(strcasecmp(argv[i],"Text") == 0)
			dumpTextResInfo(package);
	}

	ncsUnloadResPackage(package);
	
	return 0;
}


/*============================= dump info ================================*/
static void dump_mgrp_header (NCSRM_HEADER *header)
{
    NCSRM_TYPEITEM   *type_item;
    int i;

    _MGRM_PRINTF ( "\n===========================================================\n");
    _MGRM_PRINTF ( "dump resource package header:\n \
                    magic:              0x%0x \n \
                    package_version:    %s \n \
                    format_version:     %s \n \
                    char_encoding:      %s \n \
                    vendor:             %s\n \
                    section number:     0x%0x\n",
                    header->magic,
                    header->package_version,
                    header->format_version,
                    header->char_encoding,
                    header->vendor,
                    header->nr_sect);

    _MGRM_PRINTF ( "\ndump restype maps =========================================\n");

    type_item = (NCSRM_TYPEITEM*)((char*)header + sizeof(NCSRM_HEADER));

    for (i = 0; i < header->nr_sect; i++ ) {
        _MGRM_PRINTF ( "type[0x%0x]:    \n \
                        filename id:    0x%0x \n \
                        offset:         0x%0x \n",
                        type_item->type,
                        type_item->filename_id,
                        type_item->offset);

        type_item += 1;
    }

    _MGRM_PRINTF ( "=================== End of resource header ================= \n");
}
/////////////////////////////////// add for props functions ///////////////////////

#ifdef _MGRM_DEBUG
static NCS_PROP_ENTRY *get_props (HPACKAGE package, NCSRM_WINHEADER *win_header)
{
    int i;
    NCS_PROP_ENTRY *props, *each;
    NCSRM_PROP *base = (NCSRM_PROP *)((char *)win_header + win_header->offset_props);

    if (win_header->nr_props <= 0)
        return NULL;

    props = (NCS_PROP_ENTRY *)calloc(win_header->nr_props + 1, sizeof(NCS_PROP_ENTRY));

    for(i = 0, each = props; i <win_header->nr_props; i++, each++)
    {
        each->id = base->id;

        if (base->type == NCSRM_RDRTYPE_IMAGE) {
            const char* file = ncsGetImageFileName (package, base->value);

            if (!RegisterResFromFile(HDC_SCREEN, file))
                _MGRM_PRINTF("register res id(%d) error.\n", base->id);

            each->value = (DWORD)GetBitmapFromRes(Str2Key(file));
        }
		else if( base->type == NCSRM_RDRTYPE_STRING
                 || base->type == NCSRM_RDRTYPE_TEXT
                 || base->type == NCSRM_RDRTYPE_FILE )
		{
			each->value = (DWORD)ncsGetString(package, base->value);
		}
        else {
            each->value = base->value;
        }
        //FIXME ,TODO ,other type should be deal
        base++;
    }
    //end info
    each->id = -1;
    each->value = -1;

    return props;
}
#endif

/////////////////////////////////////// end of the prop functions  //////////////////////

#ifdef _MGRM_DEBUG
static void dump_ui_data (HPACKAGE package, NCSRM_WINHEADER *win_header)
{
    NCSRM_WINHEADER* ctrl_header;
    int i = 0;

    _MGRM_PRINTF ( "dump ui window resource info:\n \
                    class     ID:           0x%0x \n \
                    window    ID:           0x%0x \n \
                    serial   No.:           0x%0x \n \
                    caption   ID:           0x%0x \n \
                    renderer  ID:           0x%0x \n \
                    window style:           0x%lx \n \
                    window extended style:  0x%lx \n \
                    position(x, y, w, h):   (%d,%d,%d,%d) \n \
                    window size:            0x%0x \n \
                    properties offset:      0x%0x \n \
                    controls offset:        0x%0x \n \
                    property number:        %d \n \
                    control number:         %d \n \
                    additional data offset: 0x%0x \n \
                    additional data size:   0x%0x \n",
                    win_header->class_id,
                    win_header->wnd_id,
                    win_header->serial_num,
                    win_header->caption_id,
                    win_header->rdr_id,
                    win_header->style,
                    win_header->ex_style,
                    win_header->x,
                    win_header->y,
                    win_header->w,
                    win_header->h,
                    win_header->size,
                    win_header->offset_props,
                    win_header->offset_ctrls,
                    win_header->nr_props,
                    win_header->nr_ctrls,
                    win_header->offset_adddata,
                    win_header->size_adddata);

    if (win_header->nr_props > 0) {
        NCS_PROP_ENTRY  *props;

        _MGRM_PRINTF ( "\n------------------------------------------------------------\n");
        _MGRM_PRINTF ( "dump ui window resource properties info:\n");

        props = (NCS_PROP_ENTRY *)get_props (package, win_header);

        for (i = 0; i < win_header->nr_props; i++) {

            _MGRM_PRINTF ( "props[%d] id[0x%0x], value[0x%lx]. \n",
                    i, props->id, props->value);
            props ++;
        }
    }
    _MGRM_PRINTF ( "\nend of dumping window resource  properties info.----------------\n");

    ctrl_header = (NCSRM_WINHEADER *)((char*)win_header + win_header->offset_ctrls);

    i = win_header->nr_ctrls;
    while (i) {
        dump_ui_data (package, ctrl_header);
        ctrl_header = (NCSRM_WINHEADER*)((char*)ctrl_header + ctrl_header->size);
        i--;
    }
}
#endif

#if 0
static void dump_rdr_info(NCS_RDR_INFO *rdrinfo)
{
    NCS_RDR_ELEMENT *elements;

    if (rdrinfo == NULL)
        return;

    elements = rdrinfo->elements;

    while(elements->id != -1) {
        _MGRM_PRINTF("============ [id, value]: [%d,0x%0x] \n",
                elements->id, (int)(elements->value));
        ++elements;
    }
}

static void dump_rdrset_info(HPACKAGE package, Uint32 *data)
{
    Uint32* info = data;

    _MGRM_PRINTF("[rdrid, rdrname]:[0x%0x,%s] \n",
            *info, (char*)ncsGetString (package, *info));
    info++;
    while (*info != -1) {
        _MGRM_PRINTF("subIDs:0x%0x \n", *info);
        ++info;
    }
}
#endif

static void dumpStringResInfo (HPACKAGE hPackage)
{
    char            *sect_addr = NULL;
    NCSRM_SECTHEADER *sectHeader;
    NCSRM_IDITEM     *idItem;
    int             i = 0;

    sect_addr = get_res_sectaddr(hPackage, NCSRT_STRING);
    if (!sect_addr)
        return;

    _MGRM_PRINTF ( "\n===========================================================\n");
    //sectheader
    sectHeader = (NCSRM_SECTHEADER*)sect_addr;

    _MGRM_PRINTF ("dump string resource section info: \n"
                  "    section size:       0x%0x \n"
                  "    idmaps size:        %d \n",
            sectHeader->sect_size,
            sectHeader->size_maps);

    idItem = (NCSRM_IDITEM*)(sect_addr + sizeof(NCSRM_SECTHEADER));

    for (i = 0; i < sectHeader->size_maps; i++) {
        _MGRM_PRINTF ( "--------------------------------------\n"
                       "    id:             0x%0x \n "
                       "    filename id:    0x%0x \n "
                       "    offset:         0x%0x \n "
                       "    string:         %s \n",
                        idItem->id,
                        idItem->filename_id,
                        idItem->offset,
                        (sect_addr + idItem->offset));
        idItem++;
    }
    _MGRM_PRINTF ( "=================== End of String resource info ================= \n");
}

static void dumpTextResInfo (HPACKAGE hPackage)
{
    char            *sect_addr = NULL;
    NCSRM_SECTHEADER *sectHeader;
    NCSRM_LOCALEITEM *localeItem;
    NCSRM_DEFLOCALE_INFO *defLocale;
    NCSRM_IDITEM     *idItem;
    int             i = 0, j = 0;
    char            *data;
    int             nr_locales, nr_ids;

    sect_addr = get_res_sectaddr(hPackage, NCSRT_TEXT);
    if (!sect_addr)
        return;

    _MGRM_PRINTF ( "\n===========================================================\n");
    //sectheader
    sectHeader = (NCSRM_SECTHEADER*)sect_addr;

    _MGRM_PRINTF ("dump Text resource section info: \n"
                  "    section size:       0x%0x \n"
                  "    localemaps size:    %d \n",
                  sectHeader->sect_size,
                  sectHeader->size_maps);

    defLocale = (NCSRM_DEFLOCALE_INFO*)(sect_addr + sizeof(NCSRM_SECTHEADER));

    _MGRM_PRINTF ("dump default local info: \n"
                  "    language:        %s \n"
                  "    country:         %s \n",
                  defLocale->language,
                  defLocale->country);

    localeItem = (NCSRM_LOCALEITEM*)((char*)defLocale + sizeof(NCSRM_DEFLOCALE_INFO));
    nr_locales = sectHeader->size_maps;

    for (i = 0; i < nr_locales; i++) {
        _MGRM_PRINTF ( "--------------------------------------\n"
                       "    language:       %s \n"
                       "    country:        %s \n "
                       "    filename id:    0x%0x \n "
                       "    offset:         0x%0x \n",
                        localeItem->language,
                        localeItem->country,
                        localeItem->filename_id,
                        localeItem->offset);

        if (localeItem->offset == 0) {
            data = find_mmap_file (hPackage, localeItem->filename_id);
            if (data == NULL)
                continue;
        }
        else {
            data = (char*)sect_addr + localeItem->offset;
        }

        sectHeader = (NCSRM_SECTHEADER*)data;

        _MGRM_PRINTF ("--------------------------------------\n"
                      "    section size:       0x%0x \n "
                      "    idmaps size:        %d \n",
                      sectHeader->sect_size,
                      sectHeader->size_maps);

        idItem = (NCSRM_IDITEM*)(data + sizeof(NCSRM_SECTHEADER));
        nr_ids = sectHeader->size_maps;

        for (j = 0; j < nr_ids; j++) {
            _MGRM_PRINTF ( "--------------------------------------\n "
                           "    id:             0x%0x \n "
                           "    filename id:    0x%0x \n "
                           "    offset:         0x%0x \n "
                           "    string:         %s \n",
                            idItem->id,
                            idItem->filename_id,
                            idItem->offset,
                            data + idItem->offset);
                            //ncsGetString(hPackage, idItem->id));
            idItem++;
        }

        localeItem += 1;
    }

    _MGRM_PRINTF ( "=================== End of Text resource info ================= \n");
}

static void dumpRdrResInfo (HPACKAGE hPackage)
{
    char            *sect_addr = NULL;
    NCSRM_SECTHEADER *sectHeader;
    NCSRM_IDITEM     *idItem;
    Uint32          *data;
    int             i = 0;
    NCS_RDR_INFO    *rdrinfo = NULL;
    char            *clsname;

    sect_addr = get_res_sectaddr(hPackage, NCSRT_RDR);
    if (!sect_addr)
        return;

    _MGRM_PRINTF ( "\n===========================================================\n");
    //sectheader
    sectHeader = (NCSRM_SECTHEADER*)sect_addr;

    _MGRM_PRINTF ("dump rdr resource section info: \n"
                  "    section size:       0x%0x \n"
                  "    idmaps size:        %d \n",
            sectHeader->sect_size,
            sectHeader->size_maps);


    idItem = (NCSRM_IDITEM*)(sect_addr + sizeof(NCSRM_SECTHEADER));

    for (i = 0; i < sectHeader->size_maps; i++) {
        _MGRM_PRINTF ( "--------------------------------------\n"
                       "    id:             0x%0x \n"
                       "    filename id:    0x%0x \n"
                       "    offset:         0x%0x \n",
                        idItem->id,
                        idItem->filename_id,
                        idItem->offset);

        if (idItem->id >>16 == NCSRT_RDRSET) {
            get_rdrset_info(hPackage, idItem->id, (char**)((void *)&data));
        }
        else if (idItem->id >>16 == NCSRT_RDR) {
            get_rdr_info(hPackage, idItem->id, &rdrinfo, &clsname);
            free_rdr_info(rdrinfo);
        }

        idItem++;
    }
    _MGRM_PRINTF ( "=================== End of RDR resource info ================= \n");
}

static void dumpUIResInfo (HPACKAGE hPackage)
{
    char            *sect_addr = NULL;
    NCSRM_SECTHEADER *sectHeader;
    NCSRM_IDITEM     *idItem;
    int             i = 0;
    Uint32          name_id;
    NCSRM_WINHEADER  *header;

    sect_addr = get_res_sectaddr(hPackage, NCSRT_UI);
    if (!sect_addr)
        return;

    _MGRM_PRINTF ( "\n===========================================================\n");
    //sectheader
    sectHeader = (NCSRM_SECTHEADER*)sect_addr;

    _MGRM_PRINTF ("dump ui resource section info: \n"
                  "   section size:       0x%0x \n"
                  "   idmaps size:        %d \n",
                  sectHeader->sect_size,
                  sectHeader->size_maps);

    idItem = (NCSRM_IDITEM*)(sect_addr + sizeof(NCSRM_SECTHEADER));

    for (i = 0; i < sectHeader->size_maps; i++) {
        _MGRM_PRINTF ( "--------------------------------------\n"
                       "    id:             0x%0x \n"
                       "    filename id:    0x%0x \n"
                       "    offset:         0x%0x \n",
                        idItem->id,
                        idItem->filename_id,
                        idItem->offset);

        get_ui_data(hPackage, idItem->id, (void **)(&header), &name_id);
        idItem += 1;
    }

    _MGRM_PRINTF ( "=================== End of ui resource info ================= \n");
}

