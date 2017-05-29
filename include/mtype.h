
#ifndef MGNCS_TYPE_H
#define MGNCS_TYPE_H

typedef void* HAUTOPOOL;

enum ncsBindingDataTYPE{
	NCS_BT_INT = 1,
	NCS_BT_STR,
	NCS_BT_CHAR,
    NCS_BT_NUMBER,
	NCS_BT_MAX
};

#define SBPE_OK          0
#define SBPE_READERROR   1 //psrc cannot read
#define SBPE_WRITEERROR  2 //pdest cannot write
#define SBPE_MISSMATCH   3 //type miss match

typedef void (*NCS_SET_VALUE)(void* self, DWORD dst_v);
MGNCS_EXPORT int ncsTransType(DWORD value, int src_type, int dest_type, NCS_SET_VALUE set_value, void *self);

/**
 * other to int
 */

MGNCS_EXPORT int ncsTypeToInt(DWORD value, int src_type);

#define ncsStrToInt(str)  ncsTypeToInt((DWORD)(str), NCS_BT_STR)
#define ncsCharToInt(ch)  (int)((char)(ch))

MGNCS_EXPORT int ncsTypeToChar(DWORD value, int scr_type);

static inline int ncsStrToChar(DWORD str) {
	if(str == 0)
		return 0;
	return *((const char*)(str));
}

#define ncsIntToChar(i) ((char)(i))

/**
 * user must free string by it self
 */
MGNCS_EXPORT char* ncsTypeToString(DWORD value, int src_type);

/**
 * auto delete string, use data pool
 */

MGNCS_EXPORT HAUTOPOOL ncsCreateAutoPool(int size);
MGNCS_EXPORT void ncsResetAutoPool(HAUTOPOOL hPool);
MGNCS_EXPORT void ncsFreeAutoPool(HAUTOPOOL hPool);

MGNCS_EXPORT char* ncsTypeToAutoString(DWORD value, int src_type, HAUTOPOOL hAutoPool);

#define _BEGIN_AUTO_STR_EX(size)   { HAUTOPOOL __local_auto_pool__ = ncsCreateAutoPool(size);
#define _BEGIN_AUTO_STR  _BEGIN_AUTO_STR_EX(-1)

#define _END_AUTO_STR   ncsFreeAutoPool(__local_auto_pool__); }

#define _RESET_AUTO_STR  ncsResetAutoPool(__local_auto_pool__);

#define _AUTO_STR(data,src_type) ((char*)ncsTypeToAutoString((data), (src_type), __local_auto_pool__))

#define _RET_AUTO_STR   do{ ncsFreeAutoPool(__local_auto_pool__); return; }while(0)
#define _RETV_AUTO_STR(ret)  do { ncsFreeAutoPool(__local_auto_pool__); return (ret); }while(0)

#endif


