#ifdef WIN32

typedef void (* CB_SCAN)(void *context, char *file, char *dir);

void _scan_dir(void *context, char *dir, CB_SCAN _cb_func);

#endif