#ifdef WIN32

#include <windows.h>
#include <dirent.h>
#include <sys/stat.h>

#include "mbmparrayanimateframes_win.h"

void _scan_dir(void *context, char *dir, CB_SCAN _cb_func)
{
	int fd;
	DIR *dir_p;
	struct stat statbuf;
	char full_name[4096];
	struct dirent **namelist;
	int n;
	WIN32_FIND_DATA ffd;
	HANDLE hd;

	if(dir == NULL)
		return;

	sprintf(full_name, "%s\*", dir);
		
	hd = FindFirstFile(full_name, &ffd);
	if (INVALID_HANDLE_VALUE == hd) 
	{
		return;
	}
	do{
		_cb_func(context, ffd.cFileName, full_name);
	}while (FindNextFile(hd, &ffd) != 0);

	FindClose(hd);
}

#endif