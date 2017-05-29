#ifdef WIN32

#include <windows.h>

void *win_mmap(const char *file)
{
	HANDLE obj;
	HANDLE hFile;
	int fileSize;
	void *data = NULL;

	hFile = CreateFile(file, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE){
		return NULL;
	}

	fileSize = GetFileSize( hFile, NULL);

	obj = CreateFileMapping( hFile, NULL, PAGE_READWRITE,
           0, fileSize, file);

	GetLastError();
	if (obj){
	    data = MapViewOfFile( obj, FILE_MAP_WRITE, 0, 0, 0);
	}

	CloseHandle(obj);
	CloseHandle(hFile);
	return data;
}


void win_munmap(void *mem)
{
	UnmapViewOfFile(mem);
}

#endif