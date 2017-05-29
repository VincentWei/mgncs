#ifdef WIN32

void * win_mmap(const char *file);

void win_munmap(void *mem);

#endif