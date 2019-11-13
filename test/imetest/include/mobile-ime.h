/*
 *   This file is part of mGNCS, a component for MiniGUI.
 * 
 *   Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
#if defined _MGNCSENGINE_DIGITPY && defined _MGNCSCTRL_IMWORDSEL

#ifndef MOBILE_IME_H
#define MOBILE_IME_H

#define MGPTI_IME   1
#define PINYIN_IME   2

#define mMobileIMManagerClassHeader(clss, super)      \
	mIMManagerClassHeader(clss, super)

#define mMobileIMManagerHeader(clss)                         \
	mIMManagerHeader(clss)

DECLARE_CLASS(mMobileIMManager, mIMManager)


/*#define mMobileIMEClassHeader(clss, super)         \
	mIMEClassHeader(clss, super)

#define mMobileIMEHeader(clss)                             \
	mIMEHeader(clss)

DECLARE_CLASS(mMobileIME, mIME)
*/

#endif
#endif // _MGNCSENGINE_IME
