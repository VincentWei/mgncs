
enum enumOperator {
	OP_LT = 100,
	OP_LE,
	OP_GT,
	OP_GE,
	OP_EQ,
	OP_NE,
	OP_EQ_NOCASE = 200,
	OP_NE_NOCASE,
	OP_LIKE,
	OP_NOT_LIKE,
	OP_MATCH,
	OP_NOT_MATCH,
	OP_NOT = 300,
	OP_AND = 400,
	OP_OR  = 500,
	OP_LEFT_BACKET = 600,
	OP_RIGHT_BACKET = 700
};

enum enumOpIndex {
	OPI_LT = 0,
	OPI_LE,
	OPI_GT,
	OPI_GE,
	OPI_EQ,
	OPI_NE,
	OPI_EQ_NOCASE,
	OPI_NE_NOCASE,
	OPI_LIKE,
	OPI_NOT_LIKE,
	OPI_MATCH,
	OPI_NOT_MATCH,
	OPI_NOT ,
	OPI_AND ,
	OPI_OR  ,
	OPI_LEFT_BACKET ,
	OPI_RIGHT_BACKET,
	MAX_OPS
};

static inline mSymbol* newSymbol(int type, DWORD v)
{
	mSymbol* s = (mSymbol*)malloc(sizeof(mSymbol));
	s->type = type;
	s->data.d = v;
	return s;
}

#define RETSYM(t,v) do{ \
	*pstr = str; \
	return newSymbol(t,v); \
}while(0)

static mSymbol _op_symbols[MAX_OPS];

static void init_ops(void)
{
#define SET_OP(theOp) do{\
		_op_symbols[OPI_##theOp].type = NCS_BT_OP; \
		_op_symbols[OPI_##theOp].data.ival = OP_##theOp; \
	}while(0)

	SET_OP(LT);
	SET_OP(LE);
	SET_OP(GT);
	SET_OP(GE);
	SET_OP(EQ);
	SET_OP(NE);
	SET_OP(EQ_NOCASE);
	SET_OP(NE_NOCASE);
	SET_OP(LIKE);
	SET_OP(NOT_LIKE);
	SET_OP(MATCH);
	SET_OP(NOT_MATCH);
	SET_OP(NOT);
	SET_OP(AND);
	SET_OP(OR);
	SET_OP(LEFT_BACKET);
	SET_OP(RIGHT_BACKET);
#undef SET_OP
}

#if defined(WIN32) || !defined(HAVE_STRNDUP)
char* strndup(const char* str, size_t n)
{
  if(str) {
    size_t len = strlen(str);
    size_t nn = MIN(len,n);
    char* s = (char*)malloc(sizeof(char) * (nn + 1));

    if(s) {
      memcpy(s, str, nn);
      s[nn] = '\0';
    }

    return s;
  }

  return NULL;
}
#endif

static mSymbol * get_terminal(char** pstr)
{

#define RET_OP(theOp)  do{  \
	*pstr = str; \
	return &_op_symbols[OPI_##theOp]; \
}while(0)
	char* str;
	if(!pstr || !(str=*pstr))
		return NULL;

	while(IsSpace(*str)) str++;

	while(*str)
	{
		if(*str == '(')
		{
			str ++;
			*pstr = str;
			RET_OP(LEFT_BACKET);
		}
		else if(*str == ')')
		{
			str ++;
			*pstr = str;
			RET_OP(RIGHT_BACKET);
		}
		else if(*str == '<')
		{
			str ++;
			if(str[0] == '=')
			{
				str ++;
				RET_OP(LE);
			}
			else
				RET_OP(LT);
		}
		else if(*str == '>')
		{
			str ++;
			if(str[0] == '=')
			{
				str ++;
				RET_OP(GE);
			}
			else
				RET_OP(GT);
		}
		else if(*str == '=')
		{
			str ++;
			if(str[0] == '=')
			{
				str ++;
				RET_OP(EQ);
			}
			else if(str[0] == ':' && str[1] == '=')
			{
				str += 2;
				RET_OP(EQ_NOCASE);
			}
			else if(str[0] == '*' && str[1] == '=')
			{
				str += 2;
				RET_OP(LIKE);
			}
			else if(str[0] == '^' && str[1] == '=')
			{
				str += 2;
				RET_OP(NOT_LIKE);
			}
			else if(str[0] == '~')
			{
				str ++;
				RET_OP(MATCH);
			}
			else
				RET_OP(EQ);
		}
		else if(*str == '!')
		{
			str ++;
			if(str[0] == '=')
			{
				str ++;
				RET_OP(NE);
			}
			else if(str[0] == '~')
			{
				str ++;
				RET_OP(NOT_MATCH);
			}
			else
				RET_OP(NOT);

		}
		else if(*str == '$')
		{
			str ++;
			if(*str == '@')
			{
				str ++;
				RETSYM(NCS_BT_IDX, '@');
			}
			else if(*str == '(')
			{
				char *str_begin;
				char *str_name;
				str ++;
				str_begin = str;
				while(*str && *str != ')')
					str ++;
				str_name = strndup((const char *)str_begin, str - str_begin);
				RETSYM(NCS_BT_NAME, (DWORD)str_name);
			}
			else if(*str >= '0' && *str <= '9')
			{
				char* str_begin = str;
				int idx;
				while(*str >= '0' && *str <= '9') str ++;
				idx = mystrtol(str_begin, str, 0);
				RETSYM(NCS_BT_IDX, idx);
			}
			else
			{
				char* str_begin = str;
				char* str_name;
				while(*str && ((*str >= 'a' && *str <= 'z')
					|| (*str >='A' && *str <='Z')
					|| (*str >='0' && *str <='9')
					|| *str  == '_'))
					str ++;
				str_name = strndup(str_begin, str-str_begin);
				RETSYM(NCS_BT_NAME, (DWORD)str_name);
			}
		}
		else if(*str >='0'&& *str<='9')
		{
			char *str_begin = str;
			//int  v;
			str ++;
			if(*str == '0' && (*str == 'x' || *str =='X'))
				str ++;

			while(*str && *str >= '0' && *str <= '9')
				str ++;
			RETSYM(NCS_BT_INT, mystrtol(str_begin, str, 0));
		}
		else if(*str =='\"')
		{
			char* str_begin;
			char* str_value;
			str ++;
			str_begin = str;
			while(*str)
			{
				if(*str == '\\' && str[1] == '\"')
				{
					str ++;
				}
				else if(*str == '\"')
					break;
				str ++;
			}
			str_value = strndup(str_begin, str-str_begin);
			str ++;
			RETSYM(NCS_BT_STR, (DWORD)str_value);
		}
		str ++;
	}
	return NULL;
}


#define SYMBOLSIZE  16

mSymbol ** ncsParserTestExpression(char** pstr)
{
	char* str;
	mSymbol ** symbols = NULL;
	mSymbol * sym;
	int max_size = 0;
	int idx = 0;
	if(!pstr )
		return NULL;

	if(  !(str=*pstr))
		return NULL;

	while(*str)
	{
		sym = get_terminal(&str);
		if(!sym)
			break;
		//insert into symbols
		if(idx <= max_size)
		{
			max_size += SYMBOLSIZE;
			symbols = (mSymbol**)realloc(symbols, max_size*sizeof(mSymbol*));
		}

		if(sym->type == NCS_BT_OP)
		{
			int at = idx - 1;
			if(sym->type == OP_LEFT_BACKET || at < 0)
				symbols[idx++] = sym;
			else if(sym->type == OP_RIGHT_BACKET)
			{
				int start;
				int backets = 1;
				int pos;
				symbols[idx++] = sym;
				for(start = idx-2; backets != 0 &&  start >= 0; start --)
				{
					if(symbols[start]->type == NCS_BT_OP && symbols[start]->data.ival == OP_RIGHT_BACKET)
						backets ++;
					else if(symbols[start]->type == NCS_BT_OP && symbols[start]->data.ival == OP_LEFT_BACKET)
						backets --;
				}
				//now start is 0 or at the pos '('
				pos = start - 1;
				while(pos > 0 && symbols[start]->type == NCS_BT_OP) pos --;
				if(pos > 0 && start - pos > 1) //need changed
				{
					int end_pos;
					pos ++;
					end_pos = pos + idx - start;
					for(; pos> end_pos; pos ++)
					{
						mSymbol* tsymb = symbols[pos];
						symbols[pos] = symbols[start];
						symbols[start ] = tsymb;
						start ++;
					}

				}

			}
			else
			{
				int level = sym->data.ival / 100;
				while(at >= 0
					&& symbols[at]->type == NCS_BT_OP
					&& level > symbols[at]->data.ival/100)
						at --;
				at ++;
				if(at < 0)
					at = 0;
//					at == 0;
				if(at == idx )
					symbols[idx++] = sym;
				else
				{
					int i;
					for(i=idx;i>at; i--)
						symbols[i] = symbols[i-1];
					symbols[i] =  sym;
					idx ++;
				}
			}
		}
		else
		{
			int at = idx -1;
			int i;
			while(at >= 0
					&& symbols[at]->type == NCS_BT_OP) at --;

			at ++;
			if(at < 0)
				at = 0;
			for(i=idx; i>at; i--)
				symbols[i] = symbols[i-1];
			symbols[i] = sym;
			idx ++;
		}
	}

	return symbols;
}


BOOL ncsExecTestExpression(mSymbol** symbols, mTextExpRuntime *prt, void* self)
{
	mSymbol* stack[256];
	int   top = 0;
	int   idx ;

	for(idx=0; symbols[idx]; idx++)
	{
		if(symbols[idx]->type == NCS_BT_OP)
		{
			switch(symbols[idx]->data.ival)
			{
			case OP_LT:
			case OP_LE:
			case OP_GT:
			case OP_GE:
			case OP_EQ:
			case OP_NE:
			case OP_EQ_NOCASE:
			case OP_NE_NOCASE:
			case OP_LIKE:
			case OP_NOT_LIKE:
			case OP_MATCH:
			case OP_NOT_MATCH:
			case OP_NOT:
			case OP_AND:
			case OP_OR:
			case OP_LEFT_BACKET:
			case OP_RIGHT_BACKET:
				break;
			}
		}
		else
		{
			stack[top++] = symbols[idx];
		}
	}
	return TRUE;
}


void ncsFreeTestExpression(mSymbol** symbols)
{
	int i;
	if(!symbols)
		return;

	for(i=0; symbols[i]; i++)
	{
		if(symbols[i]->type != NCS_BT_OP)
		{
			if(symbols[i]->type == NCS_BT_STR || symbols[i]->type == NCS_BT_NAME)
			{
				if(symbols[i]->data.str)
					free(symbols[i]->data.str);
			}
			free(symbols[i]);
		}
	}

	free(symbols);

}
