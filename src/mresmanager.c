/*
** $Id: mresmanager.c 1681 2017-10-26 06:46:31Z weiym $
**
** Copyright (C) 2009 Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#if defined (WIN32)
#include <mmap-win.h>
#else
#include <sys/mman.h>
#endif

#include "mgncsconfig.h"

#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mpanel.h"
#include "mmainwnd.h"
#include "mdialog.h"
#include "mresmgr.h"
#include "mrdr.h"

//#define _MGRM_DEBUG

#ifdef _MGRM_DEBUG
#define _MGRM_PRINTF(fmt...) fprintf (stderr, fmt)

static void dump_mgrp_header (NCSRM_HEADER *header);
static void dump_ui_data (HPACKAGE package, NCSRM_WINHEADER *win_header);
static void dump_rdr_info(NCS_RDR_INFO *rdrinfo);
static void dump_rdrset_info(HPACKAGE package, Uint32 *data);

static void dumpStringResInfo (HPACKAGE hPackage);
static void dumpTextResInfo (HPACKAGE hPackage);
static void dumpRdrResInfo (HPACKAGE hPackage);
static void dumpUIResInfo (HPACKAGE hPackage);

#else
#ifdef WIN32
#define _MGRM_PRINTF
#else
#define _MGRM_PRINTF(fmt...)
#endif
#endif

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

static void del_alllist(HPACKAGE package)
{
    PMAPFILELIST head, list;

    head = ((PRPINFO)package)->head;

    if (!head)
        return;

    ((PRPINFO)package)->head = NULL;

    while(head) {
#ifdef WIN32
		win_munmap(head->data);
#else
#if HAVE_MMAP
        munmap (head->data, head->data_size);
#else
        free (head->data);
#endif
#endif
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
#ifdef WIN32
		win_munmap(list->data);
#else
#if HAVE_MMAP
        munmap (list->data, list->data_size);
#else
        free (list->data);
#endif
#endif
        free(list);
        return 0;
    }

    return -1;
}

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
#ifdef WIN32
	return win_mmap(file_name);
#else
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
#if HAVE_MMAP
    data = mmap (0, *file_size, PROT_READ, MAP_PRIVATE, fileno(fp), 0);
#else
    fseek (fp, 0, SEEK_SET);
    data = (char*)malloc(*file_size);
    fread(data, 1, *file_size, fp);
#endif
    fclose (fp);

#if HAVE_MMAP
    if (!data || (data == MAP_FAILED)) {
        _MGRM_PRINTF ("RESMANAGER>mmap: mmap file[%s] error.\n", file_name);
#else
    if (!data) {
        _MGRM_PRINTF ("RESMANAGER>mmap: read file[%s] error.\n", file_name);
#endif
        return 0;
    }

    return data;
#endif
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
    NCSRM_HEADER     *res_head;
    NCSRM_TYPEITEM   *type_item;

	if(!hPackage)
		return NULL;

	res_head = ((PRPINFO)hPackage)->data;

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
static char gLocaleValue[8] = "en_US";

const char* ncsGetDefaultLocale(void)
{
    return gLocaleValue;
}

const char* ncsSetDefaultLocale (char* language, char* country)
{
    sprintf (gLocaleValue, "%s_%s", language, country);
    return gLocaleValue;
}

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

    if (res_id == 0)
        return NULL;

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
        _MGRM_PRINTF ("RESMANAGER>Error: No found ui resource by id.\n");
        return UIDATA_ERR;
    }

    *name_id = item->filename_id;

    /* ST:file */
    if (item->offset == 0) {
        *data = find_mmap_file (package, item->filename_id);

        if (*data == NULL) {
            return UIDATA_ERR;
        }
#ifdef _MGRM_DEBUG
        dump_ui_data (package, (NCSRM_WINHEADER*)*data);
#endif

        return UIDATA_ST_FILE;
    }

    *data = ui + item->offset;
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

static void construct_wnd_template (HPACKAGE package,
        NCSRM_WINHEADER *win_header, NCS_WND_TEMPLATE *tmpl,
        NCS_EVENT_HANDLER_INFO* handlers,NCS_CREATE_NOTIFY_INFO * notify_info,  BOOL get_ctrls);

static NCS_WND_TEMPLATE* get_ctrls_info (HPACKAGE package,
        NCSRM_WINHEADER *win_header,
        NCS_EVENT_HANDLER_INFO* handlers, NCS_CREATE_NOTIFY_INFO* notify_info)
{
    NCSRM_WINHEADER      *ctrl_header;
    NCS_WND_TEMPLATE    *ctrl_data;
    int                 i = 0;

    if (win_header->nr_ctrls > 0) {

        ctrl_data = calloc (win_header->nr_ctrls, sizeof(NCS_WND_TEMPLATE));
        ctrl_header =
            (NCSRM_WINHEADER *)((char *)win_header + win_header->offset_ctrls);

        while(i < win_header->nr_ctrls) {
            construct_wnd_template (package, ctrl_header, &(ctrl_data[i]), handlers, notify_info, TRUE);
            ctrl_header =
                (NCSRM_WINHEADER *)((char*)ctrl_header + ctrl_header->size);
            i++;
        }

        return ctrl_data;
    }

    return NULL;
}

/////////////////////////////////// add for props functions ///////////////////////

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

            if (!file) {
                _MGRM_PRINTF("RESMANAGER>Error: Get image res id(%d) error.\n", base->id);
                each->value = 0;
            }
            else {
                if (!RegisterResFromFile(HDC_SCREEN, file))
                    _MGRM_PRINTF("RESMANAGER>Error: Register res id(%d) error.\n", base->id);

                each->value = (DWORD)GetBitmapFromRes(Str2Key(file));
            }
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

/////////////////////////////////////// end of the prop functions  //////////////////////

static void construct_wnd_template (HPACKAGE package,
        NCSRM_WINHEADER *win_header, NCS_WND_TEMPLATE *tmpl,
        NCS_EVENT_HANDLER_INFO* handlers,NCS_CREATE_NOTIFY_INFO * notify_info,  BOOL get_ctrls)
{
    NCS_RDR_INFO *rdrinfo;
    char         *clsname;

    if (package == HPACKAGE_NULL)
        return;

    tmpl->class_name = ncsGetString(package, win_header->class_id);
    tmpl->id = win_header->wnd_id;
    tmpl->x = win_header->x;
    tmpl->y = win_header->y;
    tmpl->w = win_header->w;
    tmpl->h = win_header->h;
    tmpl->style = win_header->style;
    tmpl->ex_style = win_header->ex_style;
    tmpl->caption = ncsGetString(package, win_header->caption_id);
	tmpl->bk_color = win_header->bk_color;
	tmpl->font_name = ncsGetString(package, win_header->font_id);
    /*tmpl->handlers = get_event_handler(win_header->wnd_id, handlers); */
    tmpl->handlers = get_event_handler(win_header->serial_num, handlers); /* use serial_num to find event handler */
    tmpl->props = get_props (package, win_header);

    //set rdrinfo
    if (win_header->rdr_id > 0
            && get_rdr_info(package, win_header->rdr_id,
                &rdrinfo, &clsname)) {
        if (ncsIsChildClass(tmpl->class_name, clsname)) {
            tmpl->rdr_info = rdrinfo;
        }
        else {
            free_rdr_info (rdrinfo);
        }
    }
    else
        tmpl->rdr_info = NULL;


    if (win_header->size_adddata > 0) {
        tmpl->user_data =
            (DWORD) ((char *)win_header + win_header->offset_adddata);
    }
    else {
        tmpl->user_data = 0;
    }

    tmpl->special_id = win_header->serial_num;
    tmpl->notify_info = notify_info;

    if(get_ctrls)
    {
        tmpl->count = win_header->nr_ctrls;
        tmpl->ctrls = get_ctrls_info (package, win_header, handlers, notify_info);
    }
    else
    {
        tmpl->count = 0;
        tmpl->ctrls = NULL;
    }
}


static void release_ctrl_info(NCS_WND_TEMPLATE*  wnd_template)
{
    free_rdr_info(wnd_template->rdr_info);
    if(wnd_template->props)
        free(wnd_template->props);
}


static void deconstruct_wnd_template (NCS_MNWND_TEMPLATE *wnd_template)
{
    if (wnd_template) {
        int i;
        release_ctrl_info((NCS_WND_TEMPLATE*)wnd_template);
        for(i = 0; i < wnd_template->count; i++)
        {
            deconstruct_wnd_template((NCS_MNWND_TEMPLATE*)(wnd_template->ctrls+i));
        }
        if(wnd_template->count > 0)
            free(wnd_template->ctrls);
    }
}

/*================================ API ================================*/
static void _set_locale_info(HPACKAGE hPackage)
{
    char* sect_addr = get_res_sectaddr(hPackage, NCSRT_TEXT);

    if (sect_addr) {
        NCSRM_DEFLOCALE_INFO* locale =
            (NCSRM_DEFLOCALE_INFO*)(sect_addr + sizeof(NCSRM_SECTHEADER));

        if (locale->language[0] && locale->country[0])
            ncsSetDefaultLocale(locale->language, locale->country);
    }
}

HPACKAGE ncsLoadResPackageFromFile (const char* fileName)
{
    void        *data;
    RPINFO      *package;
    NCSRM_HEADER *res_head;
    long        file_size;

    data = mmap_file (fileName, &file_size);

    if (data == NULL)
        return HPACKAGE_NULL;

    package = calloc (1, sizeof(RPINFO));
    if (!package)
        return HPACKAGE_NULL;

    if (append_list((HPACKAGE)package, data, -1, file_size)) {
        free (package);
        return HPACKAGE_NULL;
    }

    package->data = data;
    package->data_size = file_size;

    res_head = (NCSRM_HEADER *)data;

    //mgrp
    if (res_head->magic !=0x4d475250) {

        _MGRM_PRINTF ("RESMANAGER>Error: Header info, No MiniGUI res package:0x%0x.\n",
                res_head->magic);

        ncsUnloadResPackage((HPACKAGE)package);
        return HPACKAGE_NULL;
    }

#ifdef _MGRM_DEBUG
    dump_mgrp_header (package->data);
    dumpStringResInfo((HPACKAGE)package);
    dumpUIResInfo((HPACKAGE)package);
    dumpRdrResInfo ((HPACKAGE)package);
    dumpTextResInfo((HPACKAGE)package);
#endif

    _set_locale_info((HPACKAGE)package);
    //set ncs system renderer for default renderer
    ncsSetSystemRenderer(ncsGetString((HPACKAGE)package, NCSRM_SYSSTR_DEFRDR));

	package->ref = 1;
    return (HPACKAGE)package;
}

HPACKAGE ncsLoadResPackageFromMem (const void* mem, int size)
{
    PRPINFO package;

    package = calloc (1, sizeof(RPINFO));
    if (!package)
        return HPACKAGE_NULL;

    package->data = (void*)mem;
    package->data_size = size;

#ifdef _MGRM_DEBUG
    dump_mgrp_header (package->data);
    dumpStringResInfo((HPACKAGE)package);
    dumpUIResInfo((HPACKAGE)package);
    dumpRdrResInfo ((HPACKAGE)package);
    dumpTextResInfo((HPACKAGE)package);
#endif

    _set_locale_info((HPACKAGE)package);
    ncsSetSystemRenderer(ncsGetString((HPACKAGE)package, NCSRM_SYSSTR_DEFRDR));
	package->ref = 1;
    return (HPACKAGE)package;
}


int ncsAddRefResPackage(HPACKAGE package)
{
	PRPINFO prpinfo;
    if (package == HPACKAGE_NULL)
        return 0;

	prpinfo = (PRPINFO)package;
	if(!prpinfo)
		return 0;
	return ++prpinfo->ref;
}

void ncsUnloadResPackage (HPACKAGE package)
{
	PRPINFO prpinfo;
    if (package == HPACKAGE_NULL)
        return;

	prpinfo = (PRPINFO)package;
	if(!prpinfo || --prpinfo->ref > 0)
		return ;

    del_alllist(package);
    free (prpinfo);
}

const char* ncsGetString (HPACKAGE package, Uint32 resId)
{
    char        *string;
    NCSRM_IDITEM *item;

    if (package == HPACKAGE_NULL && resId <= 0)
        return NULL;

    if ((resId >>16) != NCSRT_STRING && (resId >>16) != NCSRT_TEXT) {
        return NULL;
    }

    item = getIDItem (package, resId, &string);

    if (item == 0) {
        _MGRM_PRINTF ("RESMANAGER>Error: Not found string resource"
                      "by id:0x%0x.\n", resId);
        return NULL;
    }

    return string + item->offset;
}

const char* ncsGetImageFileName (HPACKAGE package, Uint32 resId)
{
    char        *data;
    NCSRM_IDITEM *item;

    if (package == HPACKAGE_NULL && (resId >>16) != NCSRT_IMAGE)
        return NULL;

    item = getIDItem (package, resId, &data);

    if (item)
        return ncsGetString(package, item->filename_id);

    return NULL;
}

int ncsGetBitmap(HDC hdc, HPACKAGE package, Uint32 resId,  PBITMAP pBitmap)
{
    char        *data;
    NCSRM_IDITEM *item;
    int         ret;
    CACHED_IMAGEDATA info;

    if (package == HPACKAGE_NULL && (resId >>16) != NCSRT_IMAGE)
        return -1;

    item = getIDItem (package, resId, &data);

    if (item == 0) {
        _MGRM_PRINTF ("RESMANAGER>Error: Not found resource by id.\n");
        return -1;
    }

    if (item->offset == 0) {
        ret = LoadBitmap (hdc, pBitmap,
                ncsGetString(package, item->filename_id));
        info.is_incore = FALSE;
    }
    else {
        NCSRM_INCORE_IMAGE* incore_image = (NCSRM_INCORE_IMAGE*)(data + item->offset);
        ret = LoadBitmapFromMem (hdc, pBitmap,
                incore_image->data, incore_image->size, incore_image->ext);
        info.is_incore = TRUE;
    }

    if (ret) {
        info.image = pBitmap;
        info.type = IMAGE_BITMAP;
    }

    return ret;
}

void ncsReleaseBitmap (PBITMAP pBitmap)
{
    CACHED_IMAGEDATA* data =
        (CACHED_IMAGEDATA *)((size_t)pBitmap - offsetof(CACHED_IMAGEDATA, image));

    if (!(data->is_incore))
        UnloadBitmap (pBitmap);
}

int ncsGetMyBitmap(HPACKAGE package,
        Uint32 resId,  PMYBITMAP myBmp, RGB* pal)
{
    char        *data;
    int         ret;
    NCSRM_IDITEM *item;
    CACHED_IMAGEDATA info;

    if (package == HPACKAGE_NULL && (resId >>16) != NCSRT_IMAGE)
        return -1;

    item = getIDItem (package, resId, &data);

    if (item == 0) {
        _MGRM_PRINTF ("RESMANAGER>Error: Not found resource by id.\n");
        return -1;
    }

    if (item->offset == 0) {
        ret = LoadMyBitmap (myBmp, pal,
                ncsGetString(package, item->filename_id));
        info.is_incore = FALSE;
    }
    else {
        NCSRM_INCORE_IMAGE* incore_image = (NCSRM_INCORE_IMAGE*)(data + item->offset);
        ret = LoadMyBitmapFromMem (myBmp, pal, incore_image->data,
                incore_image->size, incore_image->ext);
        info.is_incore = TRUE;
    }

    if (ret) {
        info.image = myBmp;
        info.type = IMAGE_MYBITMAP;
    }

    return ret;
}

void ncsReleaseMyBitmap (PMYBITMAP myBmp)
{
    CACHED_IMAGEDATA* data =
        (CACHED_IMAGEDATA *)((size_t)myBmp - offsetof(CACHED_IMAGEDATA, image));

    if (!(data->is_incore))
        UnloadMyBitmap (myBmp);
}


typedef struct _ncs_connect_node {
    DWORD serial;
    mObject* obj;
    struct _ncs_connect_node * next;
}ncs_connect_node_t;

static inline ncs_connect_node_t* new_connect_node(DWORD serial)
{
    ncs_connect_node_t* node = (ncs_connect_node_t*)calloc(sizeof(ncs_connect_node_t),1);
    node->serial = serial;
    return node;
}

static inline ncs_connect_node_t* connect_node_append_serial(ncs_connect_node_t* head, DWORD serial)
{
    ncs_connect_node_t* node = NULL;
    if(head == NULL)
        return new_connect_node(serial);

    if(head->serial == serial)
        return head;

    node = head;
    while(node->next)
    {
        if(node->next->serial == serial)
            return head;
        node = node->next;
    }
    node->next = new_connect_node(serial);
    return head;
}

static ncs_connect_node_t * new_connect_node_array(NCS_EVENT_CONNECT_INFO *connects)
{
    ncs_connect_node_t* head = NULL;
    int i;
    if(!connects)
        return NULL;

    for(i = 0; connects[i].event; i ++)
    {
        head = connect_node_append_serial(head, connects[i].id_sender);
        head = connect_node_append_serial(head, connects[i].id_listener);
    }
    return head;
}

static void set_connect_node_object(ncs_connect_node_t* head, DWORD serial, mObject* obj)
{
    while(head)
    {
        if(head->serial == serial)
        {
            head->obj = obj;
            return;
        }
        head = head->next;
    }
}

typedef struct _create_notify {
    NCS_CREATE_NOTIFY_INFO info;
    ncs_connect_node_t *connect_head;
}create_notify_t;

static void cb_on_create_child(create_notify_t * info, mComponent* self, DWORD special_id)
{
    //add info the connect
    set_connect_node_object(info->connect_head, special_id, (mObject*)self);
}

#define INIT_CREATE_NOTIFY(cn, cb)  \
    ((NCS_CREATE_NOTIFY_INFO*)(cn))->onCreated = (void(*)(NCS_CREATE_NOTIFY_INFO*,mComponent*, DWORD))(cb)


static mObject* find_connect_obj(ncs_connect_node_t* head, DWORD serial)
{
    while(head && head->serial != serial)
        head = head->next;
    return head?head->obj : NULL;
}

static void connect_events(ncs_connect_node_t* head, NCS_EVENT_CONNECT_INFO* connects)
{
    int i;
    if(!head || !connects)
        return ;

    for(i=0; connects[i].event; i ++)
    {
        mObject * sender, *listener;
        if( (sender = find_connect_obj(head, connects[i].id_sender) )
            && (listener = find_connect_obj(head, connects[i].id_listener)))
            ncsAddEventListener(sender, listener, (NCS_CB_ONOBJEVENT)connects[i].event, connects[i].event_id);
    }

}

static void free_connect_nodes(ncs_connect_node_t* head)
{
    while(head)
    {
        ncs_connect_node_t * node = head;
        head = head->next;
        free(node);
    }
}

mMainWnd *ncsCreateMainWindowIndirectFromID (HPACKAGE package, Uint32 wndId,
                    HWND owner, HICON hIcon, HMENU hMenu,
                    NCS_EVENT_HANDLER_INFO* handlers,
					NCS_EVENT_CONNECT_INFO *connects,
					DWORD user_data)
{
    int         uitype;
    Uint32      id;
    mMainWnd    *mainWnd;
    NCSRM_WINHEADER  *header;
    NCS_MNWND_TEMPLATE tmpl;
    create_notify_t create_notify;

    if (package == HPACKAGE_NULL)
        return NULL;

    uitype = get_ui_data(package, wndId, (void*)&header, &id);

    if (uitype == UIDATA_ERR) {
        printf ("Error=>ResManager: ID(0x%0x) isn't valid main window id. \n", wndId);
        return NULL;
    }

    if(connects)
    {
        INIT_CREATE_NOTIFY(&create_notify, cb_on_create_child);
        create_notify.connect_head = new_connect_node_array(connects);
    }

    construct_wnd_template (package, header, (NCS_WND_TEMPLATE*)&tmpl,
            handlers, (NCS_CREATE_NOTIFY_INFO*)(connects?&create_notify:NULL), TRUE);
	tmpl.user_data = user_data;

    tmpl.hIcon = hIcon;
    tmpl.hMenu = hMenu;

    if(ncsIsChildClass(tmpl.class_name, NCSCTRL_MAINWND))
        mainWnd = (mMainWnd*)ncsCreateMainWindowIndirect (&tmpl, owner);
    else
        mainWnd = (mMainWnd*)ncsCreateWindowIndirect((NCS_WND_TEMPLATE*)&tmpl, owner);

    deconstruct_wnd_template(&tmpl);

    //insert children
	if(connects)
    {
        if(mainWnd)
            connect_events(create_notify.connect_head, connects);
        free_connect_nodes(create_notify.connect_head);
    }

    del_list(package, id);

    if (mainWnd == NULL) {
        printf ("RESMANAGER>Error: According to ID(0x%0x), create main window or dialog failure. \n", wndId);
        return NULL;
    }

    return mainWnd;
}

#ifdef _MGNCSCTRL_DIALOGBOX
int ncsCreateModalDialogFromID (HPACKAGE package, Uint32 dlgId,
                    HWND owner, HICON hIcon, HMENU hMenu,
                    NCS_EVENT_HANDLER_INFO* handlers, NCS_EVENT_CONNECT_INFO* connects)
{
    int ret = 0;
    mDialogBox * dialog = SAFE_CAST(mDialogBox,
            ncsCreateMainWindowIndirectFromID(package, dlgId, owner, hIcon, hMenu, handlers, connects, 0));

    if (dialog) {
        ret = _c(dialog)->doModal(dialog, TRUE);
        MainWindowThreadCleanup(dialog->hwnd);
    }

    return ret;
}
#endif

BOOL ncsSetSysRdr(HPACKAGE package, Uint32 rdrSetId)
{
    Uint32          *data;
    NCS_RDR_INFO    *rdrinfo;
    NCS_RDR_ELEMENT *element;
    char *clsname,  *rdrname;

    if (package == HPACKAGE_NULL)
        return FALSE;

    if (get_rdrset_info(package, rdrSetId, (char**)(void *)&data)) {
        rdrname = (char*)ncsGetString (package, *data);
        ++data;

        if (!rdrname)
            return FALSE;

        while (*data > 0) {
            if (get_rdr_info(package, *data, &rdrinfo, &clsname)) {
                element = rdrinfo->elements;
                if (element) {
                    while (element->id != -1) {
                        ncsSetElementEx(NULL, rdrname, element->id, element->value);
                        element++;
                    }
                }
                free_rdr_info(rdrinfo);
            }
            ++data;
        }

        return ncsSetSystemRenderer(rdrname);
    }
    return FALSE;
}


BOOL ncsSetWinRdr(HWND hWnd, HPACKAGE package, Uint32 rdrId)
{
    NCS_RDR_INFO    *rdrinfo;
    NCS_RDR_ELEMENT *element;
    char            *clsname;
    mWidget         *self;

    if (package == HPACKAGE_NULL && rdrId>>16 != NCSRT_RDR) {
        _MGRM_PRINTF ("RESMANAGER>Error: Invalid renderer resource id.\n");
        return FALSE;
    }

    self = ncsObjFromHandle(hWnd);

    if (!self) {
        _MGRM_PRINTF ("RESMANAGER>Error: Invalid window class.\n");
        return FALSE;
    }

    if (get_rdr_info(package, rdrId, &rdrinfo, &clsname) == FALSE) {
        _MGRM_PRINTF ("RESMANAGER>Error: Get renderer information failure.\n");
        return FALSE;
    }


    _MGRM_PRINTF ("RESMANAGER>Info: ncsSetWinRdr, clsname:%s, rdr_name:%s; self information, ctrl_rdr:%s, className:%s \n",
            clsname, rdrinfo->ctl_rdr, self->renderer->rdr_name, _c(self)->className);

    if (!ncsIsChildClass(_c(self)->className, clsname))
    {
        _MGRM_PRINTF ("RESMANAGER>Error: %s is not %s's child class.\n", clsname, _c(self)->className);
       free_rdr_info(rdrinfo);
       return FALSE;
    }

    //get renderer class
    if (strcmp(self->renderer->rdr_name, rdrinfo->ctl_rdr) != 0) {
        mWidgetRenderer *rdr = NULL;
        mWidgetClass    *ptrClass = _c(self);

        while (!rdr && ptrClass) {
            rdr = ncsRetriveCtrlRDR(rdrinfo->ctl_rdr, ptrClass->className);
            ptrClass = (mWidgetClass*)ptrClass->super;
        }

        if (!rdr) {
            _MGRM_PRINTF ("RESMANAGER>Error: Get %s renderer class failure.\n", rdrinfo->ctl_rdr);
            free_rdr_info (rdrinfo);
            return FALSE;
        }
        else {
            self->renderer = rdr;
        }
    }

    //set window element
    element = rdrinfo->elements;
    while (element && element->id != -1) {
        ncsSetElement(self, element->id, element->value);
        element++;
    }

    free_rdr_info(rdrinfo);
    SetWindowElementRenderer(hWnd, rdrinfo->ctl_rdr, NULL);

    _MGRM_PRINTF ("RESMANAGER>Info: ncsSetWinRdr ok.\n");

    return TRUE;
}

#ifdef _MGRM_DEBUG
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
                    win_header->serial_num;
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
    NCS_RDR_INFO    *rdrinfo;
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
            get_rdrset_info(hPackage, idItem->id, (char**)&data);
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

        get_ui_data(hPackage, idItem->id, (void**)&header, &name_id);
        idItem += 1;
    }

    _MGRM_PRINTF ( "=================== End of ui resource info ================= \n");
}
#endif


BOOL ncsGetWndTemplFromID(HPACKAGE package, Uint32 wndId, \
			NCS_MNWND_TEMPLATE * ptempl,
			NCS_EVENT_HANDLER_INFO* handlers)
{
	int               uitype;
	Uint32            id;
	NCSRM_WINHEADER  *header;
	if(package == 0|| wndId == 0 || ptempl == NULL)
		return FALSE;

	uitype = get_ui_data(package, wndId, (void*)&header, &id);

	if(uitype == UIDATA_ERR) {
        printf ("RESMANAGER>Error: According to ID(0x%0x), get window template information failure. \n", wndId);
		return FALSE;
    }

	construct_wnd_template(package, header, (NCS_WND_TEMPLATE*)ptempl, handlers, NULL, TRUE);

	return TRUE;

}

void ncsFreeWndTemplate(NCS_MNWND_TEMPLATE *ptempl)
{
	if(!ptempl)
		return ;
	deconstruct_wnd_template(ptempl);
}


