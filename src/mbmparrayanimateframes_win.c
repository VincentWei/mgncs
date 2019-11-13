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
