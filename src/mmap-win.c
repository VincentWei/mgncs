/* 
    This file is part of mGNCS, a component for MiniGUI.

    Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

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
