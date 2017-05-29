#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#if _MGNCS_XML
//#include "mcheck.h" for debug
#include "mgncsconfig.h"

#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libxml/parser.h"
#include "mcommon.h"
#include "mobject.h"
#include "mtype.h"
#include "mreferencedobj.h"
#include "mcomponent.h"
#include "mdatabinding.h"
#include "mdatasource.h"
#include "comm_parser.h"
#include "xquery.h"
#include "mxmlds.h"

#include "mem-slab.h"

typedef struct _xml_ds_create_info {
	const char* filename;
	DWORD flags;
}xml_ds_create_info_t;

typedef struct _xml_rs_create_info {
	mXMLDataSource *source;
    xmlXPathObjectPtr pxpathobj;
	int flags;
    int field_count;
    XFIELD* fields;
}xml_rs_create_info_t;	

DWORD ncsXMLNodeGetContent(mXMLRecordSet* rs, int field_idx, xmlNodePtr node)
{
    if(!rs || !node)
        return NULL;
    if(node->_private)
    {
    switch (_c(rs)->getFieldType(rs, field_idx))
    {
        case NCS_BT_INT:
             {
                 int ret = strtol(node->_private, NULL, 0);
                 xmlFree(node->_private);
                 return (DWORD)ret;
             }
        case NCS_BT_STR:
             {
                  char* ret = (char*)node->_private;
                  return (DWORD)ret;
             }
        case NCS_BT_NUMBER:
             {
                  float ret = strtof(node->_private, NULL);
                  xmlFree(node->_private);
                  return (DWORD)ret;
              }
        default:
            return NULL;
     }
    }else{
        node->_private = (void*)xmlNodeGetContent(node);
        if(node->_private)
        {
             return ncsXMLNodeGetContent(rs, field_idx, node);
        }
        return NULL;
    }
}

BOOL ncsXMLNodeSetContent( xmlNodePtr node, DWORD value )
{
    if (!node){
        return FALSE;
    }
    xmlNodeSetContent(node, (xmlChar*)value);
    node->_private = NULL;
    return TRUE;
}

static xmlNodePtr _getField(xmlNodePtr node, char *str)
{
    if(*str == 0)
        return node;

    if(*str == '@')
    {
//    	if ( xmlHasProp(node, str+1) )
//    		return node;
//    	return NULL;
    	return (xmlNodePtr)xmlHasProp(node, str+1);
    }
    else if(*str == '$')
    {
        return _getField(node, str+1);
    }
    else 
    {
        for(node = node->children; node; node = node->next)
        {
            if(node->type != XML_ELEMENT_NODE)
                continue;
            if(xmlStrcmp(node->name, (xmlChar*)str) == 0)
            {
                str += strlen(str) + 1;
                return _getField(node, str);
            }
        }
    }
    return NULL;
}

static void mXMLDataSource_construct(mXMLDataSource* self, DWORD param)
{
	xml_ds_create_info_t* cinfo = (xml_ds_create_info_t*)param;

	Class(mDataSource).construct((mDataSource*)self, 0);
	
	if(cinfo && cinfo->flags != 0)
	{
        xmlDocPtr pxmldoc = xmlParseFile( cinfo->filename );

		if(pxmldoc != NULL)
		{
            self->fileName = strdup(cinfo->filename);
            self->pxmldoc = pxmldoc;
            self->flags = cinfo->flags;
		}
		else
        {
            fprintf(stderr, "Error: unable to parse file \"%s\"\n", cinfo->filename);
        }
	}
}

static void mXMLDataSource_destroy(mXMLDataSource* self)
{
	if(self->pxmldoc)
	{
        xmlFreeDoc( self->pxmldoc );
        xmlCleanupParser();
	}
	
    if(self->fileName)
        free(self->fileName);
	Class(mDataSource).destroy((mDataSource*)self);
}

xmlXPathObjectPtr _getXpathObject(xmlDocPtr doc, const xmlChar *szXpath)
{
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;
    context = xmlXPathNewContext(doc);
    if (context == NULL)
    {
        printf("context is NULL\n");
        return NULL;
    }
    result = xmlXPathEvalExpression(szXpath, context);
    xmlXPathFreeContext(context);
    if (result == NULL)
    {
        printf("xmlXPathEvalExpression return NULL\n");
        return NULL;
    }
    if (xmlXPathNodeSetIsEmpty(result->nodesetval))
    {
        xmlXPathFreeObject(result);
        printf("nodeset is empty\n");
        return NULL;
    }
    return result;
}

static mXMLRecordSet * mXMLDataSource_selectRecordSet(mXMLDataSource* self, const char* xpath, int select_type)
{
	xml_rs_create_info_t cinfo;
	mXMLRecordSet* rs = NULL;

    char xpath_buff[100];
	cinfo.source = self;
	cinfo.flags = select_type;
    cinfo.field_count = parse_xquery(xpath, xpath_buff, sizeof(xpath_buff)-1, &cinfo.fields);
	cinfo.pxpathobj = _getXpathObject( self->pxmldoc, xpath_buff);
    if( cinfo.pxpathobj == NULL )
    {
		fprintf(stderr, "xml XPATH GET OBJ failed! \n" );
        return NULL;
    }
	rs = NEWEX(mXMLRecordSet, (DWORD)&cinfo);
	return rs;
}

static DWORD mXMLDataSource_getValue(mXMLDataSource *self, const char *xpath, BOOL *bok)
{
	return (DWORD)NULL;

}

static BOOL mXMLDataSource_setValue(mXMLDataSource *self, const char *xpath, DWORD value)
{
	return FALSE;
}

BEGIN_MINI_CLASS(mXMLDataSource, mDataSource)
	CLASS_METHOD_MAP(mXMLDataSource, construct)
	CLASS_METHOD_MAP(mXMLDataSource, destroy)
	CLASS_METHOD_MAP(mXMLDataSource, selectRecordSet)
	CLASS_METHOD_MAP(mXMLDataSource, getValue)
	CLASS_METHOD_MAP(mXMLDataSource, setValue)
END_MINI_CLASS


static void mXMLRecordSet_construct(mXMLRecordSet* self, DWORD param)
{
	xml_rs_create_info_t *pinfo = (xml_rs_create_info_t*)param;
	Class(mRecordSet).construct((mRecordSet*)self, param);

	if(pinfo)
	{
		self->source = pinfo->source;
		self->flags = pinfo->flags;
        self->pxpathobj = pinfo->pxpathobj;
        self->field_count = pinfo->field_count;
        self->fields = pinfo->fields;
	}
	self->cur_cursor = 0;
	self->is_end = FALSE;
}

static void mXMLRecordSet_destroy(mXMLRecordSet* self)
{
    int i = 0;
    xmlNodePtr record_node, field_node;
    record_node = _c(self)->seekCursor(self, NCS_RS_CURSOR_BEGIN, 0 );
    while( !_c(self)->isEnd(self ) )
    {
        for( i=0; i<self->field_count; i++ )
        {
            field_node = _getField( record_node, self->fields[i]+1 );
            if( NULL == field_node->_private ){
                continue;
            }
            else{
                xmlFree( field_node->_private );
            }
        }
        record_node = _c(self)->seekCursor(self, NCS_RS_CURSOR_CUR, 1);
    }
    xmlXPathFreeObject( self->pxpathobj );
	Class(mRecordSet).destroy((mRecordSet*)self);
}

static int mXMLRecordSet_getFieldCount(mXMLRecordSet* self)
{
	return self->field_count;
}

static int mXMLRecordSet_getFieldType(mXMLRecordSet* self, int idx)
{
    //todo return self->fields[idx][0];
	if(idx <= 0 || idx > self->field_count)
		return -1;

    return self->fields[idx-1][0];
}

static DWORD mXMLRecordSet_getCursor(mXMLRecordSet* self)
{
	return (DWORD)self->cur_cursor;
}

static BOOL mXMLRecordSet_setCursor(mXMLRecordSet* self, DWORD index)
{
    int count = _c(self)->getRecordCount(self);
	if(index < 0 || index >= count)
    {
        self->is_end = TRUE;
		return FALSE;
    }
    self->is_end = FALSE;
    self->cur_cursor = index;
	return TRUE;
}

static int mXMLRecordSet_getRecordCount(mXMLRecordSet* self)
{
	return self->pxpathobj->nodesetval->nodeNr;
}

static DWORD mXMLRecordSet_seekCursor(mXMLRecordSet* self, int seek, int offset)
{
	int index = 0;
    int count = _c(self)->getRecordCount(self);
	if(!self || !self->source)
		return NULL;

	switch(seek)
	{
	case NCS_RS_CURSOR_BEGIN:
        {
            index = 0;
        }
        break;
	case NCS_RS_CURSOR_END:
        {
            index = count-1;
        }
        break;
    case NCS_RS_CURSOR_CUR:
        {
            index = self->cur_cursor + offset;
        }
        break;
	default:
		break;
	}
    self->cur_cursor = index;
    if ( self->cur_cursor >= count )
    {
    	self->cur_cursor = count - 1;
        self->is_end = TRUE;
		return (DWORD)NULL;
    }
    else if (self->cur_cursor < 0)
	{
    	self->cur_cursor = 0;
        self->is_end = TRUE;
		return (DWORD)NULL;
	}
    self->is_end = FALSE;
    return ( DWORD )self->pxpathobj->nodesetval->nodeTab[index];
}

static DWORD mXMLRecordSet_getField(mXMLRecordSet *self, int idx)
{
    char* str;
    xmlChar* retstr = NULL;
    xmlNodePtr cur_record, match_field;
	if(idx <= 0 || idx > self->field_count)
		return -1;
    cur_record = self->pxpathobj->nodesetval->nodeTab[self->cur_cursor];
    str = self->fields[idx-1]+1;
    match_field = _getField(cur_record, str);
    if(NULL == match_field)
    	return NULL;
    mtrace();
    return ncsXMLNodeGetContent(self, idx, match_field);
    //return xmlNodeGetContent(match_field);
#if 0
    if( match_field->type == XML_ELEMENT_NODE )
    {
        xmlNodePtr node = NULL;
        for( node = match_field; node; node = node->next )
        {
            if ( !xmlStrcmp( node->name, match_field->name ) )
            {
                xmlChar* tmpstr = xmlNodeListGetString( self->source->pxmldoc, node->xmlChildrenNode, 1 );
                retstr = xmlStrcat( retstr, tmpstr );
                strcat((char*)retstr, ",");
            }
        }
        return retstr;
    }
    return xmlNodeListGetString( self->source->pxmldoc, match_field->xmlChildrenNode, 1 );
#endif
}

static BOOL mXMLRecordSet_setField(mXMLRecordSet *self, int idx, DWORD value)
{
    char* str;
    xmlNodePtr cur_record, match_field;
	if(idx <= 0 || idx > self->field_count)
		return -1;
    cur_record = self->pxpathobj->nodesetval->nodeTab[self->cur_cursor];
    str = self->fields[idx-1]+1;
    match_field = _getField(cur_record, str);
    if(match_field != NULL)
    {
      //xmlNodeSetContent(match_field, (xmlChar*)value);
      ncsXMLNodeSetContent(match_field, value);
      return TRUE;
    }
    return FALSE;
}

static BOOL mXMLRecordSet_newRecord(mXMLRecordSet* self, int insert_type)
{
    int idx = 0;
    xmlNodePtr cur_record = self->pxpathobj->nodesetval->nodeTab[self->cur_cursor];
    xmlNodePtr new_record = xmlCopyNode( cur_record, 1 );

    for ( ; idx < self->field_count; idx++ )
    {
        _c(self)->setField(self, idx, NULL);
    }

	switch(insert_type)
	{
	case NCS_RS_INSERT_BEFORE:
        xmlAddPrevSibling( cur_record, new_record );
		break;
	case NCS_RS_INSERT_APPEND:
		_c(self)->seekCursor(self, NCS_RS_CURSOR_END, 0);
	case NCS_RS_INSERT_AFTER:
        xmlAddNextSibling( cur_record, new_record );
		break;
	default:
		return FALSE;
	}
    return TRUE;
}

static BOOL mXMLRecordSet_deleteRecord( mXMLRecordSet* self )
{
    xmlNodePtr cur_record = self->pxpathobj->nodesetval->nodeTab[self->cur_cursor];
    xmlUnlinkNode( cur_record );
    xmlFreeNode( cur_record );
    return TRUE;
}

static BOOL mXMLRecordSet_isEnd(mXMLRecordSet* self)
{
    return self->is_end;
}

BEGIN_MINI_CLASS(mXMLRecordSet, mRecordSet)
	CLASS_METHOD_MAP(mXMLRecordSet, construct)
	CLASS_METHOD_MAP(mXMLRecordSet, destroy)
	CLASS_METHOD_MAP(mXMLRecordSet, getFieldCount)
	CLASS_METHOD_MAP(mXMLRecordSet, getRecordCount)
	CLASS_METHOD_MAP(mXMLRecordSet, getFieldType)
	CLASS_METHOD_MAP(mXMLRecordSet, getCursor)
	CLASS_METHOD_MAP(mXMLRecordSet, setCursor)
	CLASS_METHOD_MAP(mXMLRecordSet, seekCursor)
	CLASS_METHOD_MAP(mXMLRecordSet, getField)
	CLASS_METHOD_MAP(mXMLRecordSet, setField)
	CLASS_METHOD_MAP(mXMLRecordSet, newRecord)
	CLASS_METHOD_MAP(mXMLRecordSet, deleteRecord)
	CLASS_METHOD_MAP(mXMLRecordSet, isEnd)
END_MINI_CLASS

/////////////////////////////////////////////////////////////////////////////

mDataSource* ncsCreateXMLDataSource(const char* source_file, int mode)
{
	xml_ds_create_info_t cinfo;
	if(source_file == NULL)
		return NULL;

	cinfo.filename = source_file;

	return (mDataSource*)NEWEX(mXMLDataSource, (DWORD)&cinfo);
}

//////////////////////

void ncsInitXMLDataSource(void)
{
	MGNCS_INIT_CLASS(mXMLDataSource);
	MGNCS_INIT_CLASS(mXMLRecordSet);
}

#endif
