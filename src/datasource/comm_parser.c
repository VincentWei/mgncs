///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#if _MGNCSDB_DATASOURCE

#include "comm_parser.h"

#define IsSpace(ch)  ((ch) == ' ' || (ch)=='\t' || (ch)=='\n' || (ch)=='\r')

static int mystrtol(const char* start, const char* end, int type)
{
	if(end == NULL)
		return strtol(start, NULL, type);
	else{
		char szbuff[100];
		int len = end - start;
		strncpy(szbuff, start, len > (sizeof(szbuff)-1)?(sizeof(szbuff)-1):len);
		szbuff[len] = 0;
		return strtol(szbuff, NULL, type);
	}
}

char* ncsStrStrip(char*str)
{
	char *str_end;
	if(!str || *str==0)
		return NULL;

	while(*str && IsSpace(*str)) str++;

	str_end = str + strlen(str)	-1;
	while(str_end > str && IsSpace(*str_end)) str_end--;

	str_end[1] = 0;

	return str;
}

int ncsStrSpliterCallback(char* str, int (*on_find_one)(void* user, const char* str), int spliter, void* user)
{
	char *str_end = str;
	int  count = 0;
	if(str == NULL || *str == 0)
		return 0;


	while(*str_end)
	{
		if( (spliter==0 && IsSpace(*str_end)) ||  *str_end == spliter)
		{
			if(str_end == str)
			{
				str++;
				str_end++;
				continue;
			}
			//find one
			*str_end = '\0';
			//call onece
			on_find_one(user, str);
			str_end ++;

			str = str_end;
			if(spliter == 0) //skip the end
			{
				int diff ;
				while(*str_end && IsSpace(*str_end)) str_end ++;
				diff = str_end - str;
				if(diff > 0){
					int i;
					for(i=0; str_end[i+diff] ; i++)str[i] = str_end[i+diff] ;
				}
				str = str_end;
			}
			count ++;
		}
		else
		{
			str_end++;
		}
	}

	if(str != str_end)
	{
		*str_end = 0;
		on_find_one(user,str);
		count ++;
	}

	return count;
}

//static int _on_spliter(char ***strs, char *name)
static int _on_spliter(void *strs_org, const char *name_org)
{
	char ***strs = (char ***)strs_org;
	char *name = (char *)name_org;

	if(*strs){
		**strs = name;
		(*strs) ++;
	}
	return 1;
}

int ncsStrSpliter(char* str, char** strs, int spliter)
{
	return ncsStrSpliterCallback(str, _on_spliter, spliter, &strs);
}


char* ncsStrGetSurround(char* str, int ch_start, int ch_end)
{
	char *str_end;
	if(str == NULL)
		return NULL;

	while(*str && *str != ch_start) str ++;
	if(!*str)
		return NULL;
	str ++;

	str_end = str;

	while(*str_end && *str_end != ch_end) str_end ++;
	* str_end = 0;
	return str;

}

char* ncsGetVarName(char* str)
{
	char *str_end;
	if(str==NULL)
		return str;

	while(*str && *str != '$') str ++;

	if(*str != '$')
		return NULL;

	str ++;

	if(*str == '(')
		return ncsStrGetSurround(str, '(', ')');

	str_end = str;

	while(*str_end && *str_end!=','
				&& *str_end !=':'
				&& *str_end !=';'
				&& *str_end !='<'
				&& *str_end !='>'
				&& *str_end !='.') str_end ++;
	*str_end = '\0';
	return str;
}


unsigned char* ncsGetFieldList(char** pstr, int* pfield_count)
{
	char* str;
	unsigned char* field_list = NULL;
	int field_count = 0;
	if(!pstr || (str = *pstr)==NULL)
		return NULL;

	str = ncsStrGetSurround(str,'{','}');

	if(str)
	{
		char* list_end;
		char* field_name;

		list_end = str + strlen(str) -1;
		field_name  = str;

		field_count = ncsStrSpliter(str,NULL, ',');

		if(field_count > 0)
		{
			int idx = 0;
			field_list = (unsigned char*)calloc(field_count+1,1);
			while(field_name < list_end){
				char * var = ncsGetVarName(field_name);
				if(var == NULL)
					break;
				field_name += (strlen(field_name)+1);

				field_list[idx++] = strtol(var, NULL, 0);
			}
			field_list[idx] = '\0';
		}
	}

	if(pfield_count)
		*pfield_count = field_count;
	*pstr = str;
	return field_list;

}

char** ncsGetNamedFieldList(char** pstr, int *pcount)
{
	char* str;
	char** field_list = NULL;
	int count = 0;
	char * list_end, *field_name;

	if(!pstr || (str=*pstr)==NULL)
		return NULL;

	str = ncsStrGetSurround(str, '{', '}');

	if(!str)
		return NULL;

	list_end = str + strlen(str) - 1;
	field_name = str;

	count = ncsStrSpliter(str, NULL, ',');
	if(count > 0)
	{
		int idx = 0;
		field_list = (char**)calloc(count, sizeof(char*));
		while(field_name < list_end)
		{
			char *var = ncsGetVarName(field_name);
			if(var == NULL)
				break;
			field_name += (strlen(field_name)+1);
			field_list[idx++] = strdup(var);
		}
	}

	if(pcount)
		*pcount = count;
	return field_list;
}

///////////////////////////////////////////////////////
//

#include "str_expression.c"

void init_common_parser(void)
{
	init_ops();
}

#endif

