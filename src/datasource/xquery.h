#ifndef MGNCS_XQUERY_H
#define MGNCS_XQUERY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef char*  XFIELD;

int parse_xquery(const char* xquery, char* xpath_buff,int buf_len, XFIELD **xfield);
void free_xfields(XFIELD* fields, int count);

#ifdef __cplusplus
}
#endif

#endif


