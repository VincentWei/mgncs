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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mhotpiece.h"
#include "mstaticpiece.h"
#include "mlabelpiece.h"
#include "mlistspinnedpiece.h"

static BOOL mListSpinnedPiece_setProperty(mListSpinnedPiece *self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_LABELPIECE_LABEL:
		return FALSE;
	case PIECECOMM_PROP_POS:
		if(!self->list)
			return FALSE;
		self->str = self->list[(int)value];
		return TRUE;
	case NCSP_MLISTSPINNEDPIECE_LIST:
		self->list = (const char **)value;
		return TRUE;
	}
	return Class(mLabelPiece).setProperty((mLabelPiece*)self, id, value);
}

BEGIN_MINI_CLASS(mListSpinnedPiece, mLabelPiece)
	CLASS_METHOD_MAP(mListSpinnedPiece, setProperty)
END_MINI_CLASS

