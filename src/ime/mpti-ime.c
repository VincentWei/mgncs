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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSENGINE_PTI

#include "mem-slab.h"
#include "mime.h"

#include "mgpti_dict.c"

inline static int word_1st_hash(int letter)
{
	int l = tolower(letter) - 'a';
	if(l < 0 || l >= 26)
		return -1;
	return l;
}


static mIMEngine* mPtiEngine_create(const char* pdict, DWORD add_data)
{
	return (mIMEngine*)&_pti_engine;
}

static mIMIterator* mPtiEngine_newIterator(void)
{
	return (mIMIterator*)NEW(mPtiIterator);
}

static int pti_get_first_letter_start(mPtiEngine *self, int letter)
{
	MGPTI_1ST_HASH_BUCKET* hbkt_1st;
	int i;
	int idx = -1;
	letter = tolower(letter) - 'a';
	if(letter < 0)
		return 0;
	if(letter >= 26)
		return self->max_index + 1;
	hbkt_1st = self->buckets[letter];
	for(i=0; i < 26 && idx < 0 ; i++)
		idx = hbkt_1st->indices[i];
	return idx;
}

static int binary_find_word(const char** words, const char* input, int start, int low, int high)
{
	int mid;
	while(low <= high)
	{
		int cmp;
		//int j;
		mid = (low + high) / 2;
		if(!sorted_words[mid])
			return -1;
		cmp = strcmp(input + start, sorted_words[mid] + start);

		if(cmp == 0)
		{
			low = mid ;
			break;
		}
		if(cmp < 0)
		{
			high = mid - 1;
		}
		else
			low = mid + 1;

	}

    if(low == high)
    {
        if(!sorted_words[low])
            return -1;
    }

	return low;
}

BOOL mPtiEngine_findWordRange(mPtiEngine* self, int* pbegin, int* pend, const char* input, int start, int len)
{
	int begin, end;
	int i;
	begin = *pbegin;
	end   = *pend;
	if(len == 1 && start <= 0)
	{
		begin = pti_get_first_letter_start(self, input[0]);
		end   = pti_get_first_letter_start(self, input[0] + 1);
		if(begin <= -1)
			return FALSE;

		*pbegin = begin;
		*pend   = end;
		return TRUE;
	}

	if(start < 1)
	{
		MGPTI_1ST_HASH_BUCKET * hbkt_1st ;
		int h1 = word_1st_hash(input[0]);
		int h2;
		if(h1 < 0)
			return FALSE;
		hbkt_1st = self->buckets[h1];
		h2 = word_1st_hash(input[1]);
		if(h2 < 0)
			return FALSE;
		begin = hbkt_1st->indices[h2];
		if(begin < 0)
			return FALSE;
		end = -1;
		for(i = h2+1; i < 26 && end < 0; i ++)
			end = hbkt_1st->indices[i];
		if(end < 0)
		{
			pti_get_first_letter_start(self, input[0]+1);
		}
		start = 1;
	}

	if(start < len)
	{
		//find in the start
		begin = binary_find_word((const char**)self->sorted_words, input, start, begin, end);
		if(begin < 0)
			return FALSE;
		if(begin > self->max_index || (self->sorted_words[begin] && strncmp(input + start, self->sorted_words[begin] + start, len - start) != 0))
			return FALSE;
		//find end
		if(input[start] < 'z')
		{
			char szInput[128];
			int  tend;
			strncpy(szInput, input, len);
			szInput[len] = 0;
			szInput[start] ++;
			tend = binary_find_word((const char**)self->sorted_words, szInput, start, begin, end);
			if(tend > 0)
				end = tend;
		}
        if(end >= self->max_index)
            end = self->max_index + 1;
	}

	*pbegin = begin;
	*pend   = end;
	return TRUE;
}

static BOOL mPtiEngine_findWords(mPtiEngine* self, mPtiIterator* it, const char* input, int start)
{
	int len;
	int begin = 0, end = 0;

	if(!it)
		return FALSE;

	len = strlen(input);

	begin = it->begin;
	end    = begin + it->count;

	if(!mPtiEngine_findWordRange(self, &begin, &end, input, start, len))
		return FALSE;

	it->begin = begin;
	it->count = end - begin;
	it->words = (const char**)(self->sorted_words + begin);
	it->cur = 0;
	return it->count > 0;
}

static const char * mPtiEngine_imeName(mPtiEngine* self)
{
    return "abc*";
}

BEGIN_MINI_CLASS(mPtiEngine, mIMEngine)
	CLASS_METHOD_MAP(mPtiEngine, imeName)
	CLASS_METHOD_MAP(mPtiEngine, create)
	CLASS_METHOD_MAP(mPtiEngine, newIterator)
	CLASS_METHOD_MAP(mPtiEngine, findWords)
END_MINI_CLASS

//////////////////////////////////////////////////////
//mPtiIterator
static void mPtiIterator_empty(mPtiIterator*self)
{
	self->begin = 0;
	self->words = NULL;
	self->count = 0;
	self->cur   = 0;
	self->upper_buffer[0] = 0;
}

static const char* mpti_iterator_word(mPtiIterator* self)
{
	if(self->lead_letter)
	{
		if(self->cur == 0)
			return self->lead_letter;
		else
			return self->words?self->words[self->cur-1]:NULL;
	}
	else
		return self->words?self->words[self->cur]:NULL;
}

static inline void mpti_iterator_update_upper(mPtiIterator* self)
{
    if ( self->char_type == MGPTI_UPPER)
    {
        int i;
        const char* str = mpti_iterator_word(self);
        for (i = 0; str[i]; i++)
        {
            self->upper_buffer[i] = toupper(str[i]);
        }
        self->upper_buffer[i] = 0;
    }

}

static int mPtiIterator_count(mPtiIterator*self)
{
	return self->count;
}

static int mPtiIterator_next(mPtiIterator* self)
{
	if(self->cur >= self->count -1)
		return -1;
	++self->cur;
    mpti_iterator_update_upper(self);
    return self->cur;
}

static int mPtiIterator_prev(mPtiIterator* self)
{
	if(self->cur <= 0)
		return -1;
	--self->cur;
    mpti_iterator_update_upper(self);
    return self->cur;
}

static const char* mPtiIterator_word(mPtiIterator* self)
{
    //if get upper
    // return upper_buffer;
    //else
    if (self->char_type == MGPTI_UPPER)
    {
        return self->upper_buffer;
    }
    return mpti_iterator_word(self);
}

static int mPtiIterator_locate(mPtiIterator* self, int off, int type)
{
	int cur = 0;
	switch(type)
	{
	case SEEK_SET:
		cur = off;               break;
	case SEEK_CUR:
		cur = self->cur + off;   break;
	case SEEK_END:
		cur = self->count -1 + off; break;
	}

	if(cur < 0)
		cur = 0;
	else if(cur >= self->count)
		cur = self->count - 1;

	self->cur = cur;
    mpti_iterator_update_upper(self);
	return self->cur;
}

static int mPtiIterator_curIndex(mPtiIterator* self)
{
	return self->cur;
}

static DWORD mPtiIterator_setProperty(mPtiIterator* self, int id, DWORD value)
{
    switch (id)
    {
        case NCSP_IMEIT_CHARTYPE:
        {
            self->char_type = value;
            return TRUE;
        }
    }
    return FALSE;
}

static DWORD mPtiIterator_getProperty(mPtiIterator* self, int id)
{
    switch (id)
    {
        case NCSP_IMEIT_CHARTYPE:
        {
            return self->char_type;
        }
    }
    return -1;
}

BEGIN_MINI_CLASS(mPtiIterator, mIMIterator)
	CLASS_METHOD_MAP(mPtiIterator, empty)
	CLASS_METHOD_MAP(mPtiIterator, count)
	CLASS_METHOD_MAP(mPtiIterator, prev)
	CLASS_METHOD_MAP(mPtiIterator, next)
	CLASS_METHOD_MAP(mPtiIterator, curIndex)
	CLASS_METHOD_MAP(mPtiIterator, locate)
	CLASS_METHOD_MAP(mPtiIterator, word)
    CLASS_METHOD_MAP(mPtiIterator, setProperty)
    CLASS_METHOD_MAP(mPtiIterator, getProperty)
END_MINI_CLASS

#endif //_MGNCSENGINE_PTI

