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
#ifdef WIN32

typedef void (* CB_SCAN)(void *context, char *file, char *dir);

void _scan_dir(void *context, char *dir, CB_SCAN _cb_func);

#endif