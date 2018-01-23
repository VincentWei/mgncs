#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncsconfig.h"
#if _MGNCSDB_XML
#include "mcommon.h"
#include "mtype.h"
#include "xquery.h"

static inline int is_space(char ch)
{
	return (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n');
}

static inline const char* skip_space(const char* str)
{
	while(str[0] != '\0' && is_space(str[0]))
		str ++;
	return str;
}

static int get_xitem_maps_count(const char* xitem_maps)
{
	int count = 0;
	if(xitem_maps == NULL)
		return 0;

	xitem_maps = skip_space(xitem_maps);

	if(*xitem_maps == 0 || *xitem_maps != '{')
		return 0;

	while(xitem_maps[0] && xitem_maps[0] != '}')
	{
		if(xitem_maps[0] == ',')
			count ++;
		xitem_maps ++;
	}
	if(xitem_maps[0] == '}')
		count ++;

	return count;
}

static const char* get_type(const char* xitem_maps, int *ptype)
{
	int i = 0;
	*ptype = NCS_BT_STR;

	while(xitem_maps[i]  && !is_space(xitem_maps[i]) && xitem_maps[i] != '$' && xitem_maps[i] != ',' && xitem_maps[i] != '}')
	{
		i ++;
	}

	if(i <= 0)
		return xitem_maps;

	if(strncmp(xitem_maps, "int", i) == 0)
		*ptype = NCS_BT_INT;
	else if(strncmp(xitem_maps, "string", i) == 0)
		*ptype = NCS_BT_STR;
	else if(strncmp(xitem_maps, "number", i) == 0)
		*ptype = NCS_BT_NUMBER;

	return xitem_maps + i;
}

static const char* get_xfield(const char* xitem_maps, XFIELD* xfield)
{
	char szFieldInfo[256];
	int  field_size = 0;
	int type = 0;
	if(!xitem_maps || !xfield)
		return NULL;

	if(!xitem_maps [0])
		return NULL;

	while(xitem_maps[0] == '{' || xitem_maps[0] == ',')
	{
		xitem_maps ++;
		xitem_maps = skip_space(xitem_maps);
		if(!xitem_maps [0] || xitem_maps[0] == '}')
			return NULL;
	}

	//get type
	xitem_maps = get_type(xitem_maps, &type);

	szFieldInfo[field_size++] = (char)(type&0xFF);

	//try get '$'
	while(xitem_maps[0] && xitem_maps[0] != '$' && xitem_maps[0] != ',' && xitem_maps[0] != '}')
		xitem_maps ++;

	//szFieldInfo[field_size ++] = '$';

	if(xitem_maps[0] == '$')
		xitem_maps ++;

	while(xitem_maps[0]  && xitem_maps[0] != ',' && xitem_maps[0] != '}' && !is_space(xitem_maps[0]))
	{
		if(xitem_maps[0] == '.')
		{
			szFieldInfo[field_size ++] = '\0';
		}
		else if(xitem_maps[0] == '@')
		{
			szFieldInfo[field_size ++] = '\0';
			szFieldInfo[field_size ++] = xitem_maps[0];
		}
		else
			szFieldInfo[field_size ++] = xitem_maps[0];
		xitem_maps ++;
	}

	szFieldInfo[field_size ++] = '\0';
	szFieldInfo[field_size ++] = '\0';

	*xfield = (XFIELD) malloc(field_size);
	memcpy(*xfield, szFieldInfo, field_size);


	return ++xitem_maps;
}

int parse_xquery(const char* xquery, char* xpath_buff,int buf_len, XFIELD **xfield)
{
	const char* xitem_maps = NULL;
	int count = 0;
	if(xquery == NULL || xpath_buff == NULL || xfield == NULL)
		return 0;

	//splite the query
	xitem_maps = strstr(xquery, ":{");

	if(xitem_maps && buf_len > (xitem_maps - xquery))
	{
		buf_len = xitem_maps - xquery;
	}

	strncpy(xpath_buff, xquery, buf_len);

	xpath_buff[buf_len] = 0;

	*xfield = NULL;

	if(xitem_maps)
	{
		int i;
		xitem_maps ++;
		count = get_xitem_maps_count(xitem_maps);
		if(count <= 0)
			return 0;

		*xfield = (XFIELD*)calloc(sizeof(XFIELD), count);
		for(i = 0; i < count && xitem_maps; i ++)
		{
			xitem_maps = get_xfield(xitem_maps, (*xfield) + i);
		}
	}

	return count;

}

void free_xfields(XFIELD* fields, int count)
{
	int i;
	if(!fields)
		return;
	for(i = 0; i < count; i++)
		if(fields[i]) free(fields[i]);

	free(fields);
}

#if 0
int main(int argc, const char* argv[])
{
	char szXPath[100];
	XFIELD* fields;
	int count;
	int i;

	count = parse_xquery(argv[1], szXPath, sizeof(szXPath)-1, &fields);

	printf("%s:\n", argv[1]);
	printf("xpath:%s\n", szXPath);
	printf("field:%d:\n", count);
	for(i = 0; i < count; i++)
	{
		const char* str = fields[i]+1;
		printf("\t%d", fields[i][0]);
		while(*str)
		{
			printf(":%s", str);
			str += (strlen(str)+1);
		}
		printf("\n");
	}

	free_xfields(fields,count);
}
#endif

#endif
