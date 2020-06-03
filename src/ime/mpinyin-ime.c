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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSENGINE_PINYIN

#include "mem-slab.h"
#include "mime.h"
#include "pinyin-dict.c"

static void mPinyinIterator_empty(mPinyinIterator* self)
{
	self->count = 0;
	self->cur   = 0;
	self->word[0] = 0;
}

static void mPinyinIterator_set_word_direct(char* word, NCS_PINYIN_WORD_INFO* words, BOOL isAssociate)
{
	if(words->len == -2)
	{
		//strncpy(word, words->word.chars, 2);
		memcpy(word, words->word.chars, 2); // VW: avoid warning
		word[2] = 0;
	}
	else
	{
		if(words->len > 2)
		{
			int start = isAssociate ? 2 : 0;
			strncpy(word, words->word.word + start, words->len - start);
			word[words->len - start ] = 0;
		}
		else
		{
			strncpy(word, words->word.word, words->len);
			word[words->len] = 0;
		}
	}

}

static void mPinyinIterator_update_word(mPinyinIterator* self)
{
	if(self->cur < 0 || self->cur >= self->count)
		self->word[0] = 0;
	else
	{
		mPinyinIterator_set_word_direct(self->word, self->words + self->cur,self->isAssociate);
	}
}

int mPinyinIterator_count(mPinyinIterator* self)
{
	return self->count;
}

int mPinyinIterator_prev(mPinyinIterator* self)
{
	if(self->cur <= 0)
		return -1;
	self->cur --;
	mPinyinIterator_update_word(self);
	return self->cur;
}

int mPinyinIterator_next(mPinyinIterator* self)
{
	if(self->cur >= self->count -1)
		return -1;
	self->cur ++;
	mPinyinIterator_update_word(self);
	return self->cur;
}

const char* mPinyinIterator_word(mPinyinIterator* self)
{
	return self->word;
}

static int mPinyinIterator_locate(mPinyinIterator* self, int off, int type)
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

	if(self->cur == cur)
		return cur;

	self->cur = cur;
	mPinyinIterator_update_word(self);
	return self->cur;
}

static int mPinyinIterator_curIndex(mPinyinIterator* self)
{
	return self->cur;
}


BEGIN_MINI_CLASS(mPinyinIterator, mIMIterator)
	CLASS_METHOD_MAP(mPinyinIterator, empty)
	CLASS_METHOD_MAP(mPinyinIterator, count)
	CLASS_METHOD_MAP(mPinyinIterator, next)
	CLASS_METHOD_MAP(mPinyinIterator, prev)
	CLASS_METHOD_MAP(mPinyinIterator, locate)
	CLASS_METHOD_MAP(mPinyinIterator, word)
	CLASS_METHOD_MAP(mPinyinIterator, curIndex)
END_MINI_CLASS

//////////////////////////////////////////////////////////////////

///
/* 6 bit a key mask */
static const unsigned long mask[] = {
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x3F000000, 0x3FFC0000, 0x3FFFF000, 0x3FFFFFC0, 0x3FFFFFFF, 0x3FFFFFFF,
  0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF
};

///
mIMEngine* mPinyinEngine_create(const char* pdic, DWORD add_data)
{
	static mPinyinEngine _pinyin_engine;
	if(_pinyin_engine.ime_table == NULL) //have not initialize
	{
		_pinyin_engine._class = &Class(mPinyinEngine);
		_pinyin_engine.ime_table = init_pinyin_dict();
	}
	return (mIMEngine*)&_pinyin_engine;
}


#define MAKE_MATCH_KEY(inputkey)       \
	(((inputkey)[4])               \
	| ((inputkey)[3] << 6)         \
	| ((inputkey)[2] << 12)        \
	| ((inputkey)[1] << 18)        \
	| ((inputkey)[0] << 24))
static BOOL py_find_match_keys(mPinyinIterator* it, NCS_PINYIN_IME_TABLE *ime_table,int* input_keys,int start)
{
	register unsigned int startKey;
	register unsigned int endKey;
	it->matched_key1 = MAKE_MATCH_KEY(input_keys);
	it->matched_key2 = MAKE_MATCH_KEY(input_keys+5);

	if(start == 0)
		startKey = ime_table->keyIndex[input_keys[0]];
	else
		startKey = it->charIndex[start/*-1*/];

	endKey   = ime_table->keyIndex[input_keys[0]+1];

	for(; startKey < endKey; startKey ++)
	{
		unsigned long key1, key2;
		key1 = ime_table->item[startKey].key1 & mask[start+1+5];
		key2 = ime_table->item[startKey].key2 & mask[start+1];
		//printf("--- key1=0x%08X, key2=0x%08X, startKey=%d, endKey=%d, matched1=0x%08X,matched2=0x%08X\n", key1, key2, startKey, endKey,it->matched_key1, it->matched_key2 );

		if(key1 > it->matched_key1)
			break;
		if(key1 < it->matched_key1
			|| key2 < it->matched_key2)
			continue;
		else
			break;
	}
	it->startKey = startKey;
	it->charIndex[start+1] = startKey;
	it->endKey   = endKey;
	return TRUE;
}

static int py_set_word_info(NCS_PINYIN_IME_TABLE *ime_table, NCS_PINYIN_WORD_INFO* word, int index)
{
	register char* table = ime_table->wordFile;
	NCS_PINYIN_PHOFFSET * pff;
	NCS_PINYIN_PHOFFSET   poffset;

	table += ((index + 1) << 2);
	pff = (NCS_PINYIN_PHOFFSET*)table;

	poffset.off_begin = ARCH_SWAP32(pff->off_begin);
	poffset.off_end   = ARCH_SWAP32(pff->off_end);

	word->word.word = (ime_table->wordFile + ((ime_table->wordNum + 1) << 2) + poffset.off_begin);
	word->len       = poffset.off_end - poffset.off_begin;
	return 1;
}

static BOOL py_find_words(mPinyinIterator* it, NCS_PINYIN_IME_TABLE *ime_table, int input_count)
{
	int idx = it->startKey;
	int i = 0;
	int max_end = idx + PINYIN_MAX_WORD;
	register NCS_PINYIN_ITEM* item = ime_table->item;
	//char*  word_table = ime_table->wordFile + (ime_table->wordNum+1)* sizeof(NCS_PINYIN_PHOFFSET);

	if(it->startKey > it->endKey)
		return 0;

	if(max_end > it->endKey)
		max_end = it->endKey;

	while((item[idx].key1 & mask[input_count+5]) == it->matched_key1
		&& (item[idx].key2 & mask[input_count]) == it->matched_key2
		&& idx < max_end)
	{
		unsigned short phOffIdx = ARCH_SWAP16(item[idx].word_offset_idx);
		if(phOffIdx < 0xA1A1)
		{
			py_set_word_info(ime_table, it->words+i, phOffIdx);
		}
		else //sigle word
		{
			memcpy(it->words[i].word.chars, &(item[idx].word_offset_idx), 2);
			it->words[i].len = -2; //mean a single word
		}
		i ++;
		idx ++;
	}
	it->count = i;
	//printf("--- pinyin count=%d\n", it->count);
	//it->endKey = it->count + it->startKey;
	it->cur = 0;
	return i>0;
}

static BOOL py_find_associate_key(mPinyinIterator* it, NCS_PINYIN_IME_TABLE* ime_table, const char* input)
{
	int len;
	int index =  0;
	NCS_PINYIN_PHOFFSET *pff;
	NCS_PINYIN_PHOFFSET phOff;
	int offset_idx;


	len = strlen(input);
	index = ((int)((unsigned char)input[len-2]))*256 + ((unsigned char)input[len-1]);


	if(index < 0xB0A1)
	{
		return FALSE;
	}

	offset_idx = (index / 256 - 0xB0) * 94 + index % 256 - 0xA1;

	pff = (NCS_PINYIN_PHOFFSET*)(ime_table->assocFile + offset_idx * sizeof(int));

	phOff.off_begin = ARCH_SWAP32(pff->off_begin);
	phOff.off_end   = ARCH_SWAP32(pff->off_end);

	it->startKey = 72 * 94 + 1 + phOff.off_begin;
	it->endKey   = 72 * 94 + 1 + phOff.off_end;

	return TRUE;
}

static BOOL py_find_associate_words(mPinyinIterator* it, NCS_PINYIN_IME_TABLE* ime_table)
{
	//int offset_idx;
	//NCS_PINYIN_PHOFFSET* pff;
	//NCS_PINYIN_PHOFFSET  phOff;
	//const char* word_table;
	int idx = it->startKey;
	int i = 0;
	int max_words;
	int *pindex;

	max_words = it->endKey - it->startKey;
	if(max_words > PINYIN_MAX_WORD)
		max_words = PINYIN_MAX_WORD;

	//word_table = ime_table->wordFile + (ime_table->wordNum+1)* sizeof(NCS_PINYIN_PHOFFSET);
	//pff = (NCS_PINYIN_PHOFFSET*)ime_table->wordFile;
	pindex = (int*)ime_table->assocFile;

	while(i < max_words
		&& idx < it->endKey)
	{
		int  index = ARCH_SWAP32(pindex[idx]);
		py_set_word_info(ime_table, it->words + i, index);
		i ++;
		idx ++;

	}
	it->count = i;
	it->endKey = it->count + it->startKey;
	it->cur   = 0;
	return i>0;
}

BOOL mPinyinEngine_findWords(mPinyinEngine* self,       \
		mPinyinIterator* it, const char* input, \
		int start)
{
	int len;
	int i;
	int input_keys[NCS_MAX_INPUT];

	if(!it || !input)
		return FALSE;

	len = strlen(input);

	if(len <= 0)
		return FALSE;

	memset(input_keys, 0, sizeof(input_keys));

	_c(it)->empty(it);

	if(start < 0)
		start = 0;


	for(i = 0; i < start; i++)
		input_keys[i] = self->ime_table->keyMap[(int)input[i]];

	for(; start < len; start++)
	{
		input_keys[start] = self->ime_table->keyMap[(int)input[start]];
		py_find_match_keys(it, self->ime_table, input_keys, start);
	}


	if(py_find_words(it, self->ime_table, len))
	{
		it->isAssociate = FALSE;
		mPinyinIterator_update_word(it);
		return TRUE;
	}
	return FALSE;
}

BOOL mPinyinEngine_findAssociate(mPinyinEngine* self, \
		mPinyinIterator* it,
		const char* input)
{
	if(!it)
		return FALSE;

	_c(it)->empty(it);

	if(!py_find_associate_key(it, self->ime_table, input))
		return FALSE;

	if(py_find_associate_words(it, self->ime_table))
	{
		it->isAssociate = TRUE;
		mPinyinIterator_update_word(it);
		return TRUE;
	}
	return FALSE;
}

static const char _GB18030[] = "GB18030";
static const char* mPinyinEngine_getEncoding(mPinyinEngine* self)
{
	return _GB18030;
}

static int pinyin_retrieve_char(const char *word)
{
	return 2;
}
static NCSCB_RETRIEVE_CHAR mPinyinEngine_getRetrieveChar(mPinyinEngine* self)
{
	return (NCSCB_RETRIEVE_CHAR)pinyin_retrieve_char;
}

static mIMIterator* mPinyinEngine_newIterator(mPinyinEngine* self)
{
	return (mIMIterator*)NEW(mPinyinIterator);
}

static mIMIterator* mPinyinEngine_newAssocIterator(mPinyinEngine* self)
{
	return (mIMIterator*)NEW(mPinyinIterator);
}

static const char * mPinyinEngine_imeName(mPinyinEngine* self)
{
    return "pinyin";
}

BEGIN_MINI_CLASS(mPinyinEngine, mIMEngine)
	CLASS_METHOD_MAP(mPinyinEngine, imeName)
	CLASS_METHOD_MAP(mPinyinEngine, create)
	CLASS_METHOD_MAP(mPinyinEngine, findWords)
	CLASS_METHOD_MAP(mPinyinEngine, findAssociate)
	CLASS_METHOD_MAP(mPinyinEngine, getEncoding)
	CLASS_METHOD_MAP(mPinyinEngine, getRetrieveChar)
	CLASS_METHOD_MAP(mPinyinEngine, newIterator)
	CLASS_METHOD_MAP(mPinyinEngine, newAssocIterator)
END_MINI_CLASS

#endif //_MGNCSENGINE_PINYIN
