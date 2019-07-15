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
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */

if(_M(te, getParaText, (int)GetDlgItemInt(hDlg, IDC_EDIT_PARAIDX, NULL, TRUE),
            (int)GetDlgItemInt(hDlg, IDC_EDIT_PARASTART, NULL, TRUE),
            copy_len < sizeof(buff) ? copy_len : sizeof(buff)-1, buff) > 0)
{
    SetDlgItemText(hDlg, IDC_LABEL_PARATXT, buff);
}

/*_M(pobj, print, "_M(pobj, test,arg1, arg2)", arg2);*/

a = "_M(pobj, test, arg1, arg2)"

_M(pobj, print, "_M(pobj, test,arg1, arg2)", arg2);

_M(pobj, print, _M(pobj2, method2, arg1, "_M(pobj, test, arg1)", arg2));

_M(pobj, print, /* _M(pobj, test, arg1, arg2) */, arg2);
_M(pobj, print, call(/* call test */a,b/* test b */), /*test c*/c, arg3);

_M(pobj, print, //test
        arg1,   //arg1
        arg2,   //arg2
  );

	

