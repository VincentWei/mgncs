%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int top = 0;
#define MAX_DEPTH 32

enum {
    TYPE_COMMON  = 0,
    TYPE_INMARCO = 1,
};

const char* output_pattern[][2] = {
    {"_M",  "_c($1)->$2($1 @3)" },
    {"_IM", "_I($1)->$2($1 @3)" },
    {"_SUPER", "Class($1).$3(($1*)($2) @4)" },
    {"NEWV", "($1*)_newobject_args((mObjectClass*)&(Class($1)) @2)" },
    {NULL, NULL}
};

static const char* get_output_pattern(const char* name);

//////////////////////////////////
typedef struct _MARCOCALL_INFO MARCOCALL_INFO;
typedef struct _PARAM_INFO     PARAM_INFO;
typedef struct _PARAM          PARAM;

struct _PARAM_INFO {
    int type;
    union {
        char* strParam;
        MARCOCALL_INFO* marcocall_info;
    }p;
    PARAM_INFO * next;
};

struct _PARAM {
    PARAM_INFO * param_info;
    PARAM * next;
};

struct _MARCOCALL_INFO {
    char* marco_name;
    PARAM *params;
};

static MARCOCALL_INFO*  current_marcocall_info = NULL;
static PARAM         *  current_param = NULL;

static MARCOCALL_INFO * new_marcocall_info(const char* name);
static void append_param(MARCOCALL_INFO*, PARAM *param);
static void free_marcocall_info(MARCOCALL_INFO*);
static void output_marcocall_info(MARCOCALL_INFO*, FILE* f);

static PARAM *new_param();
static void  append_param_info(PARAM* param, PARAM_INFO * param_info);
static void  free_param(PARAM *param);
static void  output_param(PARAM* param, FILE*);

static PARAM_INFO* new_param_info(int type);
static PARAM_INFO* new_param_info_with_net_marcocall(MARCOCALL_INFO*);
static void  free_param_info(PARAM_INFO *param_info);
static void  output_param_info(PARAM_INFO *param_info, FILE* f);

typedef struct _PARAM_INFO_BUILDER {
    int   len;
    char  buffer[1024];
}PARAM_INFO_BUILDER;

static PARAM_INFO_BUILDER param_info_builder;
static PARAM_INFO*   reset_param_info_builder();
static void   append_char(char c);
static void   append_str(const char *str);
static void   clear_param_info_builder();

static void dump_macrocall_info(MARCOCALL_INFO* m);

#define NEW(OBJ)    (OBJ*)malloc(sizeof(OBJ))


typedef struct _STACK_NODE {
    int type:4;
    int backets:24;
    MARCOCALL_INFO*  marcocall_info;
    PARAM * param;
}STACK_NODE;

static STACK_NODE stacks[MAX_DEPTH] = { { 0, 0, NULL } };

static void push_machro(const char* name) 
{
    MARCOCALL_INFO* m = new_marcocall_info(name);
    ++top;
    stacks[top].type = TYPE_INMARCO;
    stacks[top].backets = 0;
    stacks[top].marcocall_info = m;
    stacks[top].param = current_param;
    current_marcocall_info = m;
    current_param = NULL;
    clear_param_info_builder();
}

static void pop() {
    current_param = stacks[top].param;
    --top ;
    if(stacks[top].marcocall_info)
        current_marcocall_info = stacks[top].marcocall_info;
}
static int get_type() {
    return stacks[top].type;
}

static int get_backets() {
    return stacks[top].backets;
}
static int inc_backets() {
    return ++ stacks[top].backets;
}
static int dec_backets() {
    return --stacks[top].backets;
}


%}
ID  [_A-Za-z][_A-Za-z0-9]*

%x MARCOCALL
%x STRING
%x COMMENT

%x STRING_NORMAL
%x COMMENT_NORMAL
%%

\"                     { BEGIN STRING_NORMAL;  ECHO;}
<STRING_NORMAL>\\\"    { ECHO; }
<STRING_NORMAL>\"      { ECHO; BEGIN 0; }
<STRING_NORMAL>.       { ECHO; }

\/\/.*\n               { ECHO; }
\/\*                   { BEGIN COMMENT_NORMAL; ECHO; }
<COMMENT_NORMAL>\*\/   { ECHO; BEGIN 0; }
<COMMENT_NORMAL>.      { ECHO; }


{ID}    { if(get_output_pattern(yytext)) {
                BEGIN MARCOCALL; 
                //new a marco and push it into stack
                push_machro(yytext);
                //printf("--%s\n", yytext);
            }
            else ECHO;
         }

<MARCOCALL>\(  { 
            if(get_backets() == 0)
            {
                //create a new param and param_info, then append to current macrocallinfo
                current_param = new_param();
                append_param(current_marcocall_info, current_param);
            }
            else
            {
                //in param, append
                append_char(yytext[0]);
            }
            inc_backets();
            }

<MARCOCALL>{ID} { if(get_output_pattern(yytext)) push_machro(yytext); else append_str(yytext); }

<MARCOCALL>,   { 
                    if(get_backets() == 1)
                    {
                        //end the param
                        //end the params, we can new param and append it
                        append_param_info(current_param, reset_param_info_builder());
                        current_param = new_param();
                        append_param(current_marcocall_info, current_param);
                    }
                    else
                    {
                        append_char(yytext[0]);
                    }
                }

<MARCOCALL>\/\/.*\n { append_str(yytext); }

<MARCOCALL>\"   { append_char('\"'); BEGIN STRING; }
<STRING>\\\"    { append_str("\\\""); }
<STRING>\"      { append_char('\"'); BEGIN MARCOCALL; }
<STRING>.       { append_char(yytext[0]); }

<MARCOCALL>\/\*   { append_str(yytext); BEGIN COMMENT; }
<COMMENT>\*\/     { append_str(yytext); BEGIN MARCOCALL; }
<COMMENT>.       { append_char(yytext[0]); }

<MARCOCALL>\)  {
                dec_backets();
                if(get_backets() == 0) //end the last param
                {
                    MARCOCALL_INFO * m = current_marcocall_info;
                    append_param_info(current_param, reset_param_info_builder());
                    pop(); 
                    if(get_type() == 0)
                    {
                        BEGIN 0;
                        //dump_macrocall_info(current_marcocall_info);
                        output_marcocall_info(current_marcocall_info,stdout);
                        free_marcocall_info(current_marcocall_info);
                        current_marcocall_info = NULL;
                    }
                    else
                    {
                        //append the value to current params
                        append_param_info(current_param, new_param_info_with_net_marcocall(m));
                    }
                }
                else
                {
                    //in param
                    append_char(yytext[0]);
                }
                }

<MARCOCALL>\n  { append_char(yytext[0]); }

<MARCOCALL>.   {  append_char(yytext[0]); }

.  { ECHO; }

%%

int yywrap(void)
{
    return 1;
}

int main()
{
    yylex();
    return 0;
}


static MARCOCALL_INFO * new_marcocall_info(const char* name)
{
    if(!name)
        return NULL;
    MARCOCALL_INFO * m = NEW(MARCOCALL_INFO);
    m->marco_name = strdup(name);
    m->params     = NULL;
    return m;
}

static void append_param(MARCOCALL_INFO*m, PARAM *param)
{
    if(!m || !param)
        return ;
    if(m->params == NULL)
        m->params = param;
    else
    {
        PARAM * prev = m->params;
        while(prev->next) prev = prev->next;
        prev->next = param;
    }

    param->next = NULL;
}

static void free_marcocall_info(MARCOCALL_INFO* m)
{
    PARAM *p;
    if(!m)
        return;

    if(m->marco_name)
        free(m->marco_name);
    
    p = m->params;
    while(p)
    {
        PARAM *pt = p;
        p = p->next;
        free_param(pt);
    }

    free(m);

}

static const char* get_output_pattern(const char* name)
{
    int i;
    for(i = 0; output_pattern[i][0]; i ++)
    {
        if(strcmp(name, output_pattern[i][0]) == 0)
            return output_pattern[i][1];
    }
    return NULL;
}

static PARAM * find_param_by_index(MARCOCALL_INFO *m, int n)
{
    PARAM *p = m->params;
    while(n > 0 && p)
    {
        p = p->next;
        n --;
    }
    return p;
}

static void output_marcocall_info(MARCOCALL_INFO* m, FILE *f)
{
    const char* pattern = get_output_pattern(m->marco_name);
    PARAM *p;
    int i;

    if(!pattern)
        return ;

    i = 0;
    while(pattern[i])
    {
        if(pattern[i] == '$' || pattern[i] == '@')
        {
            char index[20];
            int  j = 0;
            char type = pattern[i];
            i ++;
            while(pattern[i+j] >= '0' && pattern[i+j]<='9')
            {
                index[j] = pattern[i+j];
                j ++;
            }
            index[j] = 0;
            p = find_param_by_index(m, atoi(index)-1);
            if(p)
            {
                if( type == '$')
                {
                    output_param(p, f);
                }
                else
                {
                    while(p)
                    {
                        fprintf(f, ",");
                        output_param(p, f);
                        p = p->next;
                    }
                }
            }
            i += j;
        }
        else
        {
            fprintf(f, "%c", pattern[i++]);
        }
    }
}

static PARAM *new_param()
{
    PARAM * p = NEW(PARAM);
    p->param_info = NULL;
    p->next = NULL;
}

static void  append_param_info(PARAM* param, PARAM_INFO * param_info)
{
    if(!param || !param_info)
        return;

    if(!param->param_info)
    {
        param->param_info = param_info;
    }
    else
    {
        PARAM_INFO * pinfo;
        pinfo = param->param_info;
        while(pinfo->next)
        {
            pinfo = pinfo->next;
        }
        pinfo->next = param_info;
    }

    param_info->next = NULL;
}

static void  free_param(PARAM *param)
{
    PARAM_INFO *pinfo;
    if(!param)
        return;
    
    pinfo = param->param_info;
    while(pinfo)
    {
        PARAM_INFO *pt;
        if(pinfo->p.strParam)
        {
            if(pinfo->type == 0)
                free(pinfo->p.strParam); 
            else
                free_marcocall_info(pinfo->p.marcocall_info);
        }
        pt = pinfo;
        pinfo = pinfo->next;
        free(pt);
    }
    free(param);
}

static void  output_param(PARAM* param, FILE* f)
{
    PARAM_INFO *pinfo;
    if(!param)
        return;
    
    for(pinfo = param->param_info; pinfo; pinfo = pinfo->next)
        output_param_info(pinfo, f);
}

static PARAM_INFO* new_param_info(int type)
{
    PARAM_INFO *pinfo = NEW(PARAM_INFO);
    pinfo->next = NULL;
    pinfo->type = type;
    pinfo->p.strParam = NULL;
    return pinfo;
}

static PARAM_INFO* new_param_info_with_net_marcocall(MARCOCALL_INFO* m)
{
    PARAM_INFO * pinfo;
    if(m == NULL)
        return NULL;

    pinfo = new_param_info(1);
    pinfo->p.marcocall_info = m;
    return pinfo;
}

static void  output_param_info(PARAM_INFO *param_info, FILE *f)
{
    if(!param_info)
        return;
    if(param_info->type == 1) 
        output_marcocall_info(param_info->p.marcocall_info, f);
    else
        fprintf(f,"%s", param_info->p.strParam);
}

static PARAM_INFO*   reset_param_info_builder()
{
    PARAM_INFO * pinfo;
    if(param_info_builder.len <= 0 || param_info_builder.buffer == NULL)
        return NULL;
    
    pinfo = new_param_info(0);
    pinfo->p.strParam = strdup(param_info_builder.buffer);
    
    clear_param_info_builder();

    return pinfo;
}

static void   append_char(char c)
{
    param_info_builder.buffer[param_info_builder.len++] = c;
    param_info_builder.buffer[param_info_builder.len] = 0;
}

static void   append_str(const char *str)
{
    if(str)
    {
        int len = strlen(str);
        strcpy(param_info_builder.buffer + param_info_builder.len, str);
        param_info_builder.len += len;
    }
}

static void   clear_param_info_builder()
{
    memset(&param_info_builder, 0, sizeof(param_info_builder));

}


static void dump_macrocall_info(MARCOCALL_INFO* m)
{
    PARAM * p;
    if(!m)
        return;

    printf("%s [", m->marco_name);

    p = m->params;
    while(p)
    {
        PARAM_INFO *pinfo = p->param_info;
        while(pinfo)
        {
            if(pinfo->type == 1)
                dump_macrocall_info(pinfo->p.marcocall_info);
            else
                printf("%s ", pinfo->p.strParam);
            pinfo = pinfo->next;
        }
        p = p->next;
    }
    printf("]");

}

