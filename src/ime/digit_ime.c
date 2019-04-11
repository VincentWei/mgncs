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

#ifdef _MGNCSENGINE_DIGIT

#include "mem-slab.h"

#include "mime.h"

///////////////////////////////////////////
// mDigitEngine
static const char* digital_map[] = {
    "abc", //2
    "def", //3
    "ghi", //4
    "jkl", //5
    "mno", //6
    "pqrs",//7
    "tuv", //8
    "wxyz"
};

#define digital_letter(digital)  digital_map[((digital)-'2')]

#define MAX_DIGIT_SUB   4


static BOOL mDigitEngine_findWords(mDigitEngine* self, mIMIterator* it, const char* input, int start)
{
    mDigitIterator * dit = (mDigitIterator*)it;
    char letter_input[NCS_MAX_INPUT];
    int len;
    int find_count;

    if(! dit || !input)
        return FALSE;

    len = strlen(input);

    if(len <= 0)
        return FALSE;

    if(start < 0)
        start = 0;
    else if(start >= len)
        return FALSE;

    for(; start < len; start ++)
    {
        char        digit = input[start];
        const char* letters;
        int         i;

        if(!isdigit(digit) || digit == '1' || digit == '0')
            return FALSE;
        //get letter
        letters = digital_letter(digit);
        find_count = dit->count;

        if(find_count == 0)
        {
            int begin = 0;
            int end   = 0;
            int count = 0;
            letter_input[start+1] = 0;
            for(i = 0; letters[i]; i++)
            {
                letter_input[start] = letters[i];
                if(_c(self)->findRange(self, letter_input, start, start+1, &begin, &end))
                {
                    //printf("--- find one %c: begin:end=%d,%d\n", letters[i], begin, end);
                    dit->finds[count][0] = begin;
                    dit->finds[count][1] = end;
                    count ++;
                }
            }
            dit->count = count;
        }
        else
        {
            int j, k;
            int finds[NCS_DIGIT_MAX_FINDS][2];
            int i_find = 0, i_complete_find = NCS_DIGIT_MAX_FINDS-1;
            letter_input[start+1] = 0;
            for(j = 0; j < dit->count; j ++)
            {
                int begin = dit->finds[j][0];
                int end   = dit->finds[j][1];
                //int wlen;
                //printf("---j=%d,dit->words=%s, dit->finds[%d][0=%d,1=%d]\n",j,dit->words[begin], j, dit->finds[j][0], dit->finds[j][1]);
                //wlen = strlen(dit->words[end]); //the value is out of the range
                //if(wlen < len)
                //    continue;
                strncpy(letter_input, dit->words[begin], start);
                for(i = 0; letters[i]; i ++)
                {
                    int tbegin = begin;
                    int tend   = end;
                    letter_input[start] = letters[i];
                    //printf("-- letter_input=%s, begin=%d,end=%d\n", letter_input,begin, end);
                    if(_c(self)->findRange(self, letter_input, start, start+1, &tbegin, &tend))
                    {
                        if(strlen(dit->words[tbegin]) == len)
                        {
                            finds[i_complete_find][0] = tbegin;
                            finds[i_complete_find][1] = tend;
                            i_complete_find --;
                        }
                        else
                        {
                            finds[i_find][0] = tbegin;
                            finds[i_find][1] = tend;
                            i_find ++;
                        }
                        if(i_find == i_complete_find){
                            break;
                        }
                    }
                }
                if(i_find == i_complete_find){
                    break;
                }
            }
            k = 0;
            for(j = NCS_DIGIT_MAX_FINDS-1 ; j > i_complete_find;  j--)
            {
                dit->finds[k][0] = finds[j][0];
                dit->finds[k][1] = finds[j][1];
                k ++;
            }
            for(j = 0; j < i_find; j ++)
            {
                dit->finds[k][0] = finds[j][0];
                dit->finds[k][1] = finds[j][1];
                k ++;
            }
            dit->count = k;
        }

        if(dit->count <= 0)
        {
            dit->count = find_count;//restore previous count;
            return FALSE;
        }
    }

    return TRUE;
}

static mIMIterator * mDigitEngine_newIterator(mDigitEngine* self)
{
    //todo
    mDigitIterator* dit = NEW(mDigitIterator);
    dit->sub_it = _c(self->sub_engine)->newIterator(self->sub_engine);

    return (mIMIterator*)dit;
}

static mIMIterator * mDigitEngine_newAssocIterator(mDigitEngine* self)
{
    return (_c(self->sub_engine)->newAssocIterator(self->sub_engine));
}

static BOOL mDigitEngine_findAssociate(mDigitEngine* self, mIMIterator *it, const char* input)
{
    if(!it || !self->sub_engine)
        return FALSE;
    return _c(self->sub_engine)->findAssociate(self->sub_engine, it, input);
}

static void mDigitEngine_destroy(mDigitEngine* self)
{
    if(self->sub_engine)
        _c(self->sub_engine)->release(self->sub_engine);
    _SUPER(mDigitEngine, self, destroy);
}

static const char * mDigitEngine_imeName(mDigitEngine* self)
{
    return "digit";
}

BEGIN_MINI_CLASS(mDigitEngine, mIMEngine)
    CLASS_METHOD_MAP(mDigitEngine, imeName)
    CLASS_METHOD_MAP(mDigitEngine, destroy)
    CLASS_METHOD_MAP(mDigitEngine, findWords)
    CLASS_METHOD_MAP(mDigitEngine, newIterator)
    CLASS_METHOD_MAP(mDigitEngine, newAssocIterator)
    CLASS_METHOD_MAP(mDigitEngine, findAssociate)
END_MINI_CLASS



/// mDigitIME
static void mDigitIME_setIMEngine(mDigitIME* self, mIMEngine* ime_engine)
{
    mDigitIterator* key_it;
    _SUPER(mIME, self, setIMEngine, ime_engine);

    key_it = (mDigitIterator*)self->key_it;
    if(key_it)
    {
        key_it->sub_it->charset_converter = &self->sub_conv;
    }
    if(self->assoc_it)
        self->assoc_it->charset_converter = &self->sub_conv;
}

static void mDigitIME_destroy(mDigitIME* self)
{
    ncsIMFreeCharsetConv(&self->sub_conv);
    _SUPER(mIME, self, destroy);
}

static BOOL mDigitIME_setEncoding(mDigitIME* self, const char* encoding)
{
    if(_SUPER(mIME, self, setEncoding, encoding))
    {
        mDigitEngine * dime = (mDigitEngine*)self->ime_engine;
        ncsIMInitCharsetConv(&self->sub_conv,  _c(dime->sub_engine)->getEncoding(dime->sub_engine), encoding, _c(dime->sub_engine)->getRetrieveChar(dime->sub_engine));
        return TRUE;
    }
    return FALSE;
}


static BOOL mDigitIME_onKeyDown(mDigitIME * self, int scancode, DWORD key_status)
{
    if(NCS_IM_SCANCODE_ISALPHA(scancode))
        return FALSE; //enable letters

    if(!_SUPER(mIME, self, onKeyDown, scancode, key_status))
    {
        //if(scancode >= SCANCODE_2 && scancode <= SCANCODE_9)
        if(scancode >= SCANCODE_2 && scancode <= SCANCODE_0)
            return TRUE;
        return FALSE;
    }
    return TRUE;
}

BEGIN_MINI_CLASS(mDigitIME, mIME)
    CLASS_METHOD_MAP(mDigitIME, setIMEngine)
    CLASS_METHOD_MAP(mDigitIME, destroy)
    CLASS_METHOD_MAP(mDigitIME, setEncoding)
    CLASS_METHOD_MAP(mDigitIME, onKeyDown)
END_MINI_CLASS


///////// mDigitIterator
static void mDigitIterator_destroy(mDigitIterator* self)
{
    if(self->sub_it)
        return ;

    _SUPER(mIMIterator, self, destroy);
}

static int mDigitIterator_count(mDigitIterator* self)
{
    return self->count;
}
static int mDigitIterator_next(mDigitIterator* self)
{
    if(self->cur >= self->count -1)
        return -1;
    return ++ self->cur;
}

static int mDigitIterator_prev(mDigitIterator* self)
{
    if(self->cur <= 0)
        return -1;
    return -- self->cur;
}

static void mDigitIterator_empty(mDigitIterator* self)
{
    self->count = 0;
    self->cur   = 0;
    if(self->sub_it)
        _c(self->sub_it)->empty(self->sub_it);
}

static int mDigitIterator_locate(mDigitIterator* self, int off, int type)
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
    return self->cur;
}

static const char* mDigitIterator_word(mDigitIterator* self)
{
    int idx;
    if(!self->words || self->cur < 0 || self->cur >= self->count)
        return NULL;
    idx = self->finds[self->cur][0];
    return self->words[idx];
}

static int mDigitIterator_curIndex(mDigitIterator* self)
{
    return self->cur;
}

BEGIN_MINI_CLASS(mDigitIterator, mIMIterator)
    CLASS_METHOD_MAP(mDigitIterator, destroy)
    CLASS_METHOD_MAP(mDigitIterator, empty)
    CLASS_METHOD_MAP(mDigitIterator, count)
    CLASS_METHOD_MAP(mDigitIterator, next)
    CLASS_METHOD_MAP(mDigitIterator, prev)
    CLASS_METHOD_MAP(mDigitIterator, locate)
    CLASS_METHOD_MAP(mDigitIterator, word)
    CLASS_METHOD_MAP(mDigitIterator, curIndex)
END_MINI_CLASS

#endif //_MGNCSENGINE_DIGIT


#ifdef _MGNCSENGINE_DIGITPTI
///////////////////////////////////////////
//mDigitPtiEngine

static const char* digital_letters[] = {
    "a\0b\0c\0", //2
    "d\0e\0f\0", //3
    "g\0h\0i\0",  //4
    "j\0k\0l\0",  //5
    "m\0n\0o\0", //6
    "p\0q\0r\0s\0",//7
    "t\0u\0v\0\0", //8
    "w\0x\0y\0z\0"
};

static int _digit_letter_len(const char* dlstr)
{
    int i = 0;
    if(!dlstr)
        return 0;
    while(*dlstr)
    {
        i ++;
        dlstr += 2;
    }
    return i;
}

#define digit_letter_at(dlstr, n) ((dlstr)+ ((n)<<1))

static mIMEngine* mDigitPtiEngine_create(const char* pdict, DWORD add_data)
{
    static mDigitPtiEngine  _digit_pti_engine = { &Class(mDigitPtiEngine) };
    static mDigitPtiEngine  _digit_pti_engine_upper = { &Class(mDigitPtiEngine) };
    mDigitPtiEngine * pengine ;

    pengine = (add_data == MGPTI_UPPER)? &_digit_pti_engine_upper:&_digit_pti_engine;

    if(pengine->sub_engine == NULL)
    {
        pengine->sub_engine = Class(mPtiEngine).create(pdict, add_data);
        pengine->case_type = add_data;
    }
    return (mIMEngine*)pengine;
}

static mIMIterator*  mDigitPtiEngine_newIterator(mDigitPtiEngine*self)
{
    mDigitPtiIterator* pit = NEW(mDigitPtiIterator);
    pit->sub_it = _c(self->sub_engine)->newIterator(self->sub_engine);
    _c(pit->sub_it)->setProperty(pit->sub_it, NCSP_IMEIT_CHARTYPE, self->case_type);
    return (mIMIterator*) pit;
}

static BOOL mDigitPtiEngine_findRange(mDigitPtiEngine* self, const char* input, int start, int len, int *pbegin, int *pend)
{
    return mPtiEngine_findWordRange((mPtiEngine*)(self->sub_engine), pbegin, pend, input, start, len);
}

static BOOL mDigitPtiEngine_findWords(mDigitPtiEngine* self, mDigitPtiIterator* it, const char* input, int start)
{
    BOOL bRet;

    if(!it)
        return FALSE;

    if((bRet = Class(mDigitEngine).findWords((mDigitEngine*)self, (mIMIterator*)it, input, start)))
        it->words = (const char**)((mPtiEngine*)(self->sub_engine))->sorted_words;

    if(start == 0 && input[1] == 0 && input[0]>='2' && input[0] <= '9')  //is the single letters
    {
        it->letters =(char*) digital_letters[input[0] - '2'];
        it->count +=  _digit_letter_len(it->letters);
        return TRUE;
    }
    else {
        it->letters = NULL;
    }

    return bRet;
}

static const char * mDigitPtiEngine_imeName(mDigitPtiEngine* self)
{
    if (self->case_type == MGPTI_UPPER)
        return "ABC*";
    return "abc*";
}

BEGIN_MINI_CLASS(mDigitPtiEngine, mDigitEngine)
    CLASS_METHOD_MAP(mDigitPtiEngine, imeName)
    CLASS_METHOD_MAP(mDigitPtiEngine, create)
    CLASS_METHOD_MAP(mDigitPtiEngine, newIterator)
    CLASS_METHOD_MAP(mDigitPtiEngine, findRange)
    CLASS_METHOD_MAP(mDigitPtiEngine, findWords)
END_MINI_CLASS

////////
static mIMIterator*  mDigitPtiIterator_subIterator(mDigitPtiIterator* self)
{
    mPtiIterator* pit;
    int cur;
    const char* lead_letter = NULL;
    pit = (mPtiIterator*)(self->sub_it);

    _c(pit)->empty(pit);

    cur = self->cur;
    if(cur < 0 || cur >= self->count)
        return NULL;
    if(self->letters)
    {
        int len = _digit_letter_len(self->letters);
        if(cur >= len)
            cur -= len;
        else
        {
            lead_letter = digit_letter_at(self->letters, cur);
        }
    }

    pit->words = self->words + self->finds[cur][0];
    pit->count = self->finds[cur][1] - self->finds[cur][0] + (lead_letter == NULL?0:1);
    pit->begin = self->finds[cur][0];
    pit->lead_letter = lead_letter;
    return self->sub_it;
}

static const char* mDigitPtiIterator_word(mDigitPtiIterator* self)
{
    int idx;
    if(!self->words || self->cur < 0 ||self->cur >= self->count)
        return NULL;

    if(self->letters)
    {
        int len = _digit_letter_len(self->letters);
        if(self->cur < len)
            return digit_letter_at(self->letters, self->cur);
        idx = self->finds[self->cur - len][0];
        return self->words[idx];
    }

    idx = self->finds[self->cur][0];
    return self->words[idx];
}

BEGIN_MINI_CLASS(mDigitPtiIterator, mDigitIterator)
    CLASS_METHOD_MAP(mDigitPtiIterator, subIterator)
    CLASS_METHOD_MAP(mDigitPtiIterator, word)
END_MINI_CLASS

#endif //_MGNCSENGINE_DIGITPTI

//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////// Pinyin
/*static unsigned int pinyin_mask[] = {
    0xF8000000, //len = 1
    0xFFC00000, //len = 2
    0xFFFE0000, //len = 3
    0xFFFFF000, //len = 4
    0xFFFFFF10, //len = 5
    0xFFFFFFFC //len = 6
};*/

#ifdef _MGNCSENGINE_DIGITPY

static unsigned int make_key(const char* pinyin, int len)
{
    unsigned int key = 0;
    int shift = 32 - 5;
    int i;
    for(i = 0; i < len && i < 5 ; i++)
    {
        unsigned int ch = tolower(pinyin[i]) - 'a' + 1;
        ch <<= shift;
        key |= ch;
        shift -= 5;
    }
    return key;
}

static int binsearch_pinyin(mDigitPyEngine* self, unsigned int key, unsigned int max_key, int len, int low, int high)
{
    int mid = 0;
    unsigned int* keys = self->keys;
    //unsigned int  mask = pinyin_mask[len-1];

    high ++;
    if(high > self->py_count)
        high = self->py_count;
    while(low <= high)
    {
        register unsigned int tkey;
        mid = (low + high) / 2;
        tkey = keys[mid];
        //printf("--- mid=%d, key=0x%08X\n", mid, tkey);
        if(tkey == key)
            return mid;
        else if(tkey < key)
            low = mid + 1;
        else
            high = mid - 1;
    }
    //printf("++++ mid=%d,low=%d,high=%d, keys[mid]=0x%08X, keys[mid+1]=0x%08X, key=0x%08X, max_key=0x%08X\n",mid, low, high, keys[mid],keys[mid+1], key, max_key);

    if(mid >= self->py_count)
        return -1;

    if(mid > 0 && keys[mid] < key && keys[mid+1] >= key && keys[mid+1] < max_key)
    {
        return mid + 1;
    }
    else if(keys[mid] > key && (keys[mid] & key) == key && keys[mid] < max_key)
        return mid;

    return -1;
}

static int find_pinyin(mDigitPyEngine* self, const char* pinyin, int len, short find[2], int begin, int end)
{
    unsigned int key = make_key(pinyin,len);
    unsigned int max_key = key + (1<<(32-len*5));
    unsigned int *keys = self->keys;
    //printf("-- pinyin = %s, begin=%d,end=%d\n", pinyin,begin, end);
    int idx = binsearch_pinyin(self, key, max_key, len, begin, end);
    //printf("--- idx=%d\n",idx);
    if(idx < 0)
        return 0;

    find[0] = idx;

    idx ++;
    for(; idx < self->py_count &&  keys[idx] < max_key; idx ++);
    //printf("-- find_pinyin: idx=%d, max_key=%x\n",idx, max_key);

    find[1] = idx;
    return 1;
}


#include "pinyin-idx.c"
static mIMEngine* mDigitPyEngine_create(const char* pdict, DWORD add_data)
{
    static mDigitPyEngine _digit_py_engine = {&Class(mDigitPyEngine)};

    if(_digit_py_engine.sub_engine == NULL)
    {
        _digit_py_engine.sub_engine = Class(mPinyinEngine).create(pdict, add_data);
        _digit_py_engine.py_count   = 407;
        _digit_py_engine.keys       = _pinyin_keys;
        _digit_py_engine.pinyins    = _pinyins;
    }
    return (mIMEngine*)&_digit_py_engine;
}

static BOOL mDigitPyEngine_findRange(mDigitPyEngine* self, const char* input, int start, int len, int *pbegin, int *pend)
{
    register int begin, end;
    short   find[2];
    begin = *pbegin;
    end   = *pend;

    if(begin < 0)
        begin = 0;
    if(end <= 0)
        end = self->py_count;

    if(find_pinyin(self, input, len,find, begin, end))
    {
        *pbegin = find[0];
        *pend   = find[1];
        return TRUE;
    }
    return FALSE;
}

static BOOL mDigitPyEngine_findWords(mDigitPyEngine* self, mIMIterator* it, const char* input, int start)
{
    if(_SUPER(mDigitEngine, self, findWords, it, input, start))
    {
        mDigitPyIterator* pit = (mDigitPyIterator*)it;
        pit->py_engine = self->sub_engine;
        pit->words = self->pinyins;
        return TRUE;
    }
    return FALSE;
}

static mIMIterator* mDigitPyEngine_newIterator(mDigitPyEngine* self)
{
    mDigitPyIterator* pit = NEW(mDigitPyIterator);
    pit->sub_it = _c(self->sub_engine)->newIterator(self->sub_engine);
    return (mIMIterator*)pit;
}

static const char * mDigitPyEngine_imeName(mDigitPyEngine* self)
{
    return "pinyin";
}

BEGIN_MINI_CLASS(mDigitPyEngine, mDigitEngine)
    CLASS_METHOD_MAP(mDigitPyEngine, imeName)
    CLASS_METHOD_MAP(mDigitPyEngine, create)
    CLASS_METHOD_MAP(mDigitPyEngine, findWords)
    CLASS_METHOD_MAP(mDigitPyEngine, findRange)
    CLASS_METHOD_MAP(mDigitPyEngine, newIterator)
END_MINI_CLASS


////mDigitPyIterator
static mIMIterator* mDigitPyIterator_subIterator(mDigitPyIterator* self)
{
    mPinyinEngine   * py_engine = (mPinyinEngine*) self->py_engine;
    const char*       pinyin;

    _c(self->sub_it)->empty(self->sub_it);

    pinyin = _c(self)->word(self);
    if(!pinyin)
        return NULL;
    if(_c(py_engine)->findWords(py_engine, self->sub_it, pinyin, 0))
    {
        return self->sub_it;
    }
    return NULL;
}

BEGIN_MINI_CLASS(mDigitPyIterator, mDigitIterator)
    CLASS_METHOD_MAP(mDigitPyIterator, subIterator)
END_MINI_CLASS

#endif //_MGNCSENGINE_DIGITPY

