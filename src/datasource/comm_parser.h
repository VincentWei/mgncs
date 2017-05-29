
#ifndef MGNCS_COMM_PARSE_H
#define MGNCS_COMM_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

int ncsStrSpliter(char* str, char** strs, int spliter);

int ncsStrSpliterCallback(char* str, int (*on_find_one)(void* user, const char* str), int spliter, void* user);

char* ncsStrGetSurround(char* str, int ch_start, int ch_end);

char* ncsGetVarName(char* str);

char* ncsStrStrip(char*str);

static inline void get_col_rows(char* str, int *prow, int *pcol)
{
	int row = 0;
	int col = 0;
	str = (char*)strchr(str, '[');
	if(str)
	{
		str = ncsStrGetSurround(str, '[', ']');
		row = strtol(str, NULL, 0);
		str = (char*)strchr(str, '[');
		if(str)
			col = strtol(ncsStrGetSurround(str, '[',']'), NULL, 0);
	}

	if(prow)
		*prow = row;
	if(pcol)
		*pcol = col;

}

unsigned char* ncsGetFieldList(char** pstr, int* pfiled_count);

char** ncsGetNamedFieldList(char** pstr, int *pcount);


//////////////////////////////////
//grammar tree
typedef struct _mSymbol{
	int type;
	union{
		int ival;
		char* str;
		DWORD d;
	}data;
}mSymbol;

mSymbol ** ncsParserTestExpression(char** pstr);

typedef struct _mTextExpRuntime
{
	DWORD (*getValue)(void* self, mSymbol* symb);
}mTextExpRuntime;

BOOL ncsExecTestExpression(mSymbol** symbols, mTextExpRuntime *prt, void* self);

void ncsFreeTestExpression(mSymbol** symbols);

enum enumSymbolType{
	NCS_BT_IDX = NCS_BT_MAX,
	NCS_BT_NAME,
	NCS_BT_OP
};



#ifdef __cplusplus
}
#endif

#endif


