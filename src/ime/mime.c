
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mem-slab.h"

#include "mime.h"

#ifdef _MGNCSENGINE_IME

static int sb_len_first_char (const unsigned char* mstr, int len);
static int single_retrieve_char(const char*word);

static void send_to_target_direct(mIMManager* self, UINT message, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////
// interface declear

BEGIN_MINI_CLASS(mIMIterator, mObject)
END_MINI_CLASS

static const char _ISO_8859_1[] = "ISO8859-1";

static const char* mIMEngine_getEncoding(mIMEngine *self)
{
	return _ISO_8859_1;
}

static NCSCB_RETRIEVE_CHAR mIMEngine_getRetrieveChar(mIMEngine* self)
{
	return (NCSCB_RETRIEVE_CHAR)single_retrieve_char;
}

static mIMEngine * mIMEngine_newAssocIterator(mIMEngine* self)
{
	return NULL;
}

static BOOL mIMEngine_findAssociate(mIMEngine* self, mIMIterator* it)
{
	return FALSE;
}

BEGIN_MINI_CLASS(mIMEngine, mObject)
	CLASS_METHOD_MAP(mIMEngine, getEncoding)
	CLASS_METHOD_MAP(mIMEngine, getRetrieveChar)
	CLASS_METHOD_MAP(mIMEngine, newAssocIterator)
	CLASS_METHOD_MAP(mIMEngine, findAssociate)
END_MINI_CLASS

//////////////////
//IME
void mIME_setIMEngine(mIME* self, mIMEngine* ime_engine)
{
	if(!self || !ime_engine)
		return;
	if(self->ime_engine == ime_engine)
		return ;
	if(self->ime_engine)
		_c(self->ime_engine)->release(self->ime_engine);
	DELETE(self->key_it);
	DELETE(self->assoc_it);
	self->ime_engine = ime_engine;
	self->key_it   = _c(ime_engine)->newIterator(ime_engine);
	self->assoc_it = _c(ime_engine)->newAssocIterator(ime_engine);
	self->key_it->charset_converter
		= &self->charset_conv;
	if(self->assoc_it)
		self->assoc_it->charset_converter  = &self->charset_conv;
	self->start = -1;
	self->input[0] = 0;
}

static const char* mIME_getSelWord(mIME* self, BOOL braw)
{
	return (const char*)SendMessage(self->hwnd_view, NCS_IMM_GETSELWORD, braw, 0);
}

static BOOL _isSelWordActive(mIME* self)
{
	return SendMessage(self->hwnd_view, NCS_IMM_ISSELWORDCTRLACTIVE, 0, 0);
}

static void _setSelWordActive(mIME* self, BOOL bactive)
{
	SendMessage(self->hwnd_view, NCS_IMM_SETSELWORDACTIVE, bactive, 0);
}

static void mIME_construct(mIME *self, DWORD param)
{
	mIMEngine * engine = (mIMEngine*)param;
	_c(self)->setIMEngine(self, engine);
}

static BOOL mIME_setEncoding(mIME* self, const char* encoding)
{
	if(self->ime_engine)
	{
		return ncsIMInitCharsetConv(&self->charset_conv, _c(self->ime_engine)->getEncoding(self->ime_engine),encoding,_c(self->ime_engine)->getRetrieveChar(self->ime_engine));

	}
	return FALSE;
}

static void mIME_destroy(mIME *self)
{
	if(self->ime_engine)
		_c(self->ime_engine)->release(self->ime_engine);

	ncsIMFreeCharsetConv(&self->charset_conv);

	DELETE(self->key_it);
	DELETE(self->assoc_it);

	_SUPER(mObject, self, destroy);
}

static DWORD mIME_getIMStatus(mIME *self, int code)
{
	return 0;
}

static BOOL mIME_setIMStatus(mIME *self, int code, DWORD value)
{
	return FALSE;
}

static BOOL mIME_findWords(mIME* self, const char* input, int start, int len)
{
	if(!self->ime_engine)
		return FALSE;
	return _c(self->ime_engine)->findWords(self->ime_engine, self->key_it, input, start);
}

static BOOL mIME_findAssociate(mIME* self, const char* words)
{
	if(!words || !self->assoc_it)
		return FALSE;
	return _c(self->ime_engine)->findAssociate(self->ime_engine, self->assoc_it, words);
}

static BOOL mIME_emptyKey(mIME* self)
{
	if(self->key_it)
		_c(self->key_it)->empty(self->key_it);
	return TRUE;
}

static BOOL mIME_addChar(mIME* self, int ch)
{
	int i;
	int pos;
	int start;

	if(self->len >= NCS_MAX_INPUT)
		return FALSE;

	pos = self->insert;
	//copy
	for(i = self->len; i > self->insert; i--)
		self->input[i] = self->input[i-1];
	self->input[pos] = ch;
	//reset the start
	if(self->insert < self->len) //back to start
		start = 0;
	else
		start = self->insert;

	self->input[self->len+1] = 0;
	//reget the info
	if(_c(self->ime_engine)->findWords(self->ime_engine, self->key_it, self->input, start))
	{
		self->insert ++;
		self->len ++;
		self->start = self->len - 1;
		self->start = start;
		_c(self)->update(self, NCS_IME_UPDATE_ALL);
		return TRUE;
	}
	else
	{
		//reback the value
		for(i = self->insert; i < self->len; i++)
			self->input[i] = self->input[i+1];
		self->input[self->len] = 0;
		return FALSE;
	}
}

static BOOL mIME_delChar(mIME* self, BOOL bback)
{
	int i;
	if(self->len <= 0)
		return FALSE;
	if(bback)
	{
		if(self->insert < 0)
		{
			self->insert = 0;
			return FALSE;
		}
		for(i=self->insert; i < self->len; i++)
		{
			self->input[i-1] = self->input[i];
		}
		self->insert --;
	}
	else
	{
		for(i = self->insert; i < self->len-1; i ++)
			self->input[i] = self->input[i+1];
	}
	self->input[--self->len] = 0;
	if(self->insert > self->len)
		self->insert = self->len - 1;

	self->start = 0;
    _c(self)->emptyKey(self);
	if( self->len >0 && _c(self->ime_engine)->findWords(self->ime_engine, self->key_it, self->input, self->start) )
	{
		_c(self)->update(self, NCS_IME_UPDATE_ALL);
		return TRUE;
	}else if( self->len == 0 )
        _c(self)->empty(self, TRUE);
	return FALSE;
}

static BOOL mIME_clearInput(mIME* self)
{
	self->start = self->insert = 0;
	self->len   = 0;
	self->input[0] = 0;
	return TRUE;
}

static BOOL mIME_setInsert(mIME* self, int off, int type)
{
	int cur = self->insert;
	switch(type)
	{
	case SEEK_SET:
		cur = off;  break;
	case SEEK_CUR:
		cur += off; break;
	case SEEK_END:
		cur = self->len - off - 1; break;
	}

	if(cur < 0)
		return FALSE;
	else if(cur > self->len - 1)
		return FALSE;

	if(cur == self->insert )
		return TRUE;

	self->insert = cur;
	_c(self)->update(self, NCS_IME_UPDATE_CURSOR);
	return TRUE;
}

static int mIME_onKeyDown(mIME* self, int scancode, DWORD keyflags)
{
	if(NCS_IM_SCANCODE_ISALPHA(scancode))
		return IME_STOP_MSG;

	switch(scancode)
	{
	case SCANCODE_CURSORBLOCKLEFT:
		if(_c(self)->setInsert(self, -1, SEEK_CUR))
			return IME_STOP_MSG;
	case SCANCODE_CURSORBLOCKRIGHT:
		if(_c(self)->setInsert(self, 1, SEEK_CUR))
			return IME_STOP_MSG;
		break;
	case SCANCODE_HOME:
		if(_c(self)->setInsert(self, 0, SEEK_SET))
			return IME_STOP_MSG;
		break;
	case SCANCODE_END:
		if(_c(self)->setInsert(self, 0, SEEK_END))
			return IME_STOP_MSG;
		break;
	//case SCANCODE_ENTER:
	//	if(self->ime_manager->cur_ime == self && _c(self->ime_manager)->sendKeys(self->ime_manager))
	//		return IME_STOP_MSG;
	//	break;
	case SCANCODE_ENTER:
		if(self->ime_manager->cur_ime == self && _c(self->ime_manager)->sendWords(self->ime_manager))
			return IME_STOP_MSG;
	//	if(self->ime_manager->cur_ime == self ){
    //        _c(self->ime_manager)->sendWords(self->ime_manager);
	//		return IME_STOP_MSG;
    //    }
		break;
	case SCANCODE_REMOVE:
		if(_c(self)->delChar(self, FALSE))
			return IME_STOP_MSG;
		break;
	case SCANCODE_BACKSPACE:
		if(self->len > 0 || (self->assoc_it && _c(self->assoc_it)->count(self->assoc_it) > 0))
			return IME_STOP_MSG;
	}

	return IME_CONTINUE_MSG;
}

static int mIME_onChar(mIME* self, int ch, DWORD keyflags)
{
	if(ch == 127 || ch == '\b')
	{
		if (_isSelWordActive(self))
		{
			if(self->assoc_it && _c(self->assoc_it)->count(self->assoc_it)>0)
			{
				_c(self->assoc_it)->empty(self->assoc_it);
				_c(self)->update(self, NCS_IME_UPDATE_WORDS);
				return IME_STOP_MSG;
			}
			_setSelWordActive(self, FALSE);
			_c(self)->update(self, NCS_IME_UPDATE_ALL);
			return IME_STOP_MSG;
		}
		else
		{
			if(!_c(self)->delChar(self, TRUE))
			{
				if(self->assoc_it && _c(self->assoc_it)->count(self->assoc_it)>0)
				{
					_c(self->assoc_it)->empty(self->assoc_it);
					_c(self)->update(self, NCS_IME_UPDATE_WORDS);
					return IME_STOP_MSG;
				}
				return IME_CONTINUE_MSG;
			}
			return IME_STOP_MSG;
		}
	}
	else if((ch >='A' && ch <='Z') || (ch >='a' && ch <= 'z') ||  (ch >='0' && ch <='9') )
		return  _c(self)->addChar(self, ch)?IME_STOP_MSG:IME_CONTINUE_MSG;

	return IME_CONTINUE_MSG;
}

static BOOL mIME_showIME(mIME* self)
{
	if(self->hwnd_view)
	{
		if(ShowWindow(self->hwnd_view, SW_SHOW))
		{
			InvalidateRect(self->hwnd_view, NULL,TRUE);
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL mIME_hideIME(mIME* self)
{
	if(self->hwnd_view)
	{
		_c(self)->empty(self, TRUE);
		return ShowWindow(self->hwnd_view, SW_HIDE);
	}
	return FALSE;
}

static BOOL mIME_update(mIME* self,DWORD flags)
{
	if(self->hwnd_view)
		return SendMessage(self->hwnd_view, NCS_IMM_UPDATE, flags, 0);
	return FALSE;
}

static BOOL mIME_onCaretPos(mIME* self, int x, int y)
{
	if(self->hwnd_view)
	{
		POINT pt = {x, y};
		return SendMessage(self->hwnd_view, MSG_IME_SETPOS, 0, (LPARAM)&pt);
		//return TRUE;
	}
	return FALSE;
}

static LRESULT _ime_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT ret = 0;
	mIME * ime = mIME_FromView(hwnd);
	if(message == MSG_SETFOCUS
		|| message == MSG_KILLFOCUS
		|| message == MSG_MOUSEACTIVE)
		return 0;

	if(ime->ime_manager
		//&& ime->ime_manager->cur_ime == ime
		&& _c(ime->ime_manager)->procIMMsg(ime->ime_manager, message, wParam, lParam, &ret) == IME_STOP_MSG)
		return ret;

	if(ime && ime->old_view_proc)
		return ime->old_view_proc(hwnd, message, wParam, lParam);
	return DefaultControlProc(hwnd, message, wParam, lParam);
}

void mIME_attach(mIME *ime, HWND hwnd)
{
	if(IsWindow(ime->hwnd_view))
		SetWindowAdditionalData(ime->hwnd_view, 0);
	ime->old_view_proc = SetWindowCallbackProc(hwnd, _ime_proc);
	SetWindowAdditionalData(hwnd, (DWORD)ime);
	ime->hwnd_view = hwnd;
}

void mIME_passMessage(mIME* ime, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(ime && ime->ime_manager)
		send_to_target_direct(ime->ime_manager, message, wParam,lParam);
}

static void mIME_empty(mIME* self, BOOL bUpdate)
{
	self->input[0] = 0;
	self->start = -1;
	self->insert = 0;
	self->len    = 0;
	if(self->key_it)
		_c(self->key_it)->empty(self->key_it);
	if(self->assoc_it)
		_c(self->assoc_it)->empty(self->assoc_it);
	if(bUpdate)
		_c(self)->update(self, NCS_IME_UPDATE_ALL);
}

BEGIN_MINI_CLASS(mIME, mObject)
	CLASS_METHOD_MAP(mIME, construct)
	CLASS_METHOD_MAP(mIME, setEncoding)
	CLASS_METHOD_MAP(mIME, setIMEngine)
	//CLASS_METHOD_MAP(mIME, imeMsgProc)
	CLASS_METHOD_MAP(mIME, destroy)
	CLASS_METHOD_MAP(mIME, getIMStatus)
	CLASS_METHOD_MAP(mIME, setIMStatus)
	CLASS_METHOD_MAP(mIME, findWords)
	CLASS_METHOD_MAP(mIME, findAssociate)
	CLASS_METHOD_MAP(mIME, emptyKey)
	CLASS_METHOD_MAP(mIME, addChar)
	CLASS_METHOD_MAP(mIME, delChar)
	CLASS_METHOD_MAP(mIME, setInsert)
	CLASS_METHOD_MAP(mIME, clearInput)
	CLASS_METHOD_MAP(mIME, onChar)
	CLASS_METHOD_MAP(mIME, onKeyDown)
	CLASS_METHOD_MAP(mIME, showIME)
	CLASS_METHOD_MAP(mIME, hideIME)
	CLASS_METHOD_MAP(mIME, update)
	CLASS_METHOD_MAP(mIME, onCaretPos)
	CLASS_METHOD_MAP(mIME, getSelWord)
	CLASS_METHOD_MAP(mIME, empty)
END_MINI_CLASS
//////////////////////////////////////


static void mIMManager_construct(mIMManager* self, DWORD param)
{
	_SUPER(mObject, self, construct, 0);
	self->hwnd         = HWND_NULL;
	self->hwnd_target  = HWND_NULL;
	self->header       = NULL;
	self->cur_ime      = NULL;
	self->flags        = 0;
	self->len_first_char = sb_len_first_char;
}

static void mIMManager_destroy(mIMManager* self)
{
	mIME *ime = self->header;
	while(ime)
	{
		mIME * tmp = ime->next;
		DELETE(ime);
		ime = tmp;
	}

	_SUPER(mObject, self, destroy);
}
/*
static LRESULT immanger_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int ret = 0;
	mIMManager* self = (mIMManager*)GetWindowAdditionalData2(hwnd);

	if(self)
	{
		_c(self)->procIMMsg(self, message, wParam, lParam , &ret);
		return ret;
	}
	else
		return DefaultMainWinProc(hwnd, message, wParam, lParam);
}*/

static void send_to_target_direct(mIMManager* self, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hwnd_target;
	//send to target
	if(IsWindow(self->hwnd_target))
		hwnd_target = self->hwnd_target;
	else
	{
		hwnd_target = GetActiveWindow();
		if(self->cur_ime && self->cur_ime->hwnd_view == hwnd_target)
			return ;
	}
#ifdef _MGRM_PROCESSES
    Send2ActiveWindow(mgTopmostLayer, message, wParam, lParam);
#else
	PostMessage(hwnd_target, message, wParam, lParam);
#endif

}

static int mIMManager_procIMMsg(mIMManager* self, int message, WPARAM wParam, LPARAM lParam, LRESULT *pret)
{
	//printf("---- message=%x\n", message);
	switch(message)
	{
		case MSG_SETFOCUS:
		case MSG_KILLFOCUS:
		case MSG_MOUSEACTIVE:
			//printf("---- message=%x\n", message);
			return IME_STOP_MSG;
		case MSG_IME_OPEN:
			//printf("---- ime open\n");
			if(!(self->flags & NCSF_IMM_DISABLE) && (self->cur_ime || (self->flags & NCSF_IMM_NULLIME ) ))
			{
				self->flags |= NCSF_IMM_ACTIVED;
                if(self->cur_ime)
    				_c(self->cur_ime)->showIME(self->cur_ime);
                ncsRaiseEvent((mObject*)self, NCSN_IMM_IMEOPEN, (DWORD)self->cur_ime );
				//_c(self->cur_ime)->onCaretPos(self->cur_ime, self->ime_x, self->ime_y);
			}
			return IME_STOP_MSG;
		case MSG_IME_CLOSE:
			//printf("--- ime close\n");
			if(self->flags & NCSF_IMM_SETTING_IME)
			{
				self->flags &= ~NCSF_IMM_SETTING_IME;
				return IME_STOP_MSG;
			}

			self->flags &= ~NCSF_IMM_ACTIVED;
			if(!(self->flags & NCSF_IMM_DISABLE) && self->cur_ime)
				_c(self->cur_ime)->hideIME(self->cur_ime);
            ncsRaiseEvent((mObject*) self, NCSN_IMM_IMECLOSE, 0 );
			return IME_STOP_MSG;

		case MSG_IME_SETTARGET:
			self->hwnd_target = (HWND)wParam;
			return IME_STOP_MSG;

		case MSG_IME_GETTARGET:
			*pret = (LRESULT)(self->hwnd_target);
			return IME_STOP_MSG;

		case MSG_IME_SETSTATUS:
			if(!(self->flags & NCSF_IMM_DISABLE) && self->cur_ime)
				return _c(self->cur_ime)->setIMStatus(self->cur_ime, (int)wParam, (DWORD)lParam);
			break;

		case MSG_IME_GETSTATUS:
			if(!(self->flags & NCSF_IMM_DISABLE) && self->cur_ime)
				return _c(self->cur_ime)->getIMStatus(self->cur_ime, wParam);
			break;
		case MSG_IME_SETPOS:
			{
				POINT *pt = (POINT*) lParam;
				self->ime_x = pt->x;
				self->ime_y = pt->y;
				if(!(self->flags & NCSF_IMM_DISABLE) && self->cur_ime)
					_c(self->cur_ime)->onCaretPos(self->cur_ime, pt->x, pt->y);
				break;
			}
		case MSG_IME_GETPOS:
			{
				POINT *pt = (POINT*) lParam;
				pt->x = self->ime_x;
				pt->y = self->ime_y;
				break;
			}
		case MSG_KEYDOWN:
			if(!(self->flags & (NCSF_IMM_DISABLE|NCSF_IMM_NULLIME)) && self->cur_ime && (self->flags & NCSF_IMM_ACTIVED) )
			{
				if(_c(self->cur_ime)->onKeyDown(self->cur_ime, wParam, lParam) == IME_STOP_MSG)
					return IME_STOP_MSG;
			}
			//send_to_target_direct(self, message, wParam, lParam);
			break;
		case MSG_CHAR:
			if(!(self->flags & (NCSF_IMM_DISABLE|NCSF_IMM_NULLIME)) && self->cur_ime && (self->flags & NCSF_IMM_ACTIVED) )
			{
				if(_c(self->cur_ime)->onChar(self->cur_ime, wParam, lParam) == IME_STOP_MSG)
					return IME_STOP_MSG;
			}
			//send_to_target_direct(self, message, wParam, lParam);
			break;
		default:
			break;
	}
	//return DefaultMainWinProc(self->hwnd, message, wParam, lParam);
	return IME_CONTINUE_MSG;
}

static BOOL mIMManager_sendWords(mIMManager* self)
{
	mIME *cur_ime = self->cur_ime;
	NCSCB_LEN_FIRST_CHAR  len_first_char = self->len_first_char;
	const char* word = _c(cur_ime)->getSelWord(cur_ime, 0);
	char rawword[128];
	const char* prawword;
	int len;

	if(!word)
		return FALSE;

	len = strlen(word);
	if(len <= 0)
		return FALSE;

	prawword = _c(cur_ime)->getSelWord(cur_ime, TRUE);
	if(prawword)
	{
		strncpy(rawword, prawword, 127);
		rawword[127] = 0;
	}

	while(len > 0)
	{
		int count = len_first_char((const unsigned char*)word, len);
		int i;
		unsigned int wchar = (unsigned char)word[0];
        if (count <= 0)
            break; //to avoid the dead loop
		for(i=1; i < count; i++)
			wchar |= (((unsigned char)word[i])<<(i*8));

		//PostMessage(self->hwnd_target, MSG_CHAR, (WPARAM) wchar, 0);
        send_to_target_direct(self, MSG_CHAR, (WPARAM)wchar, 0);

		len -= count;
		word += count;
	}

	_c(cur_ime)->clearInput(cur_ime);
	//try find the associate words
	_c(cur_ime)->findAssociate(cur_ime, rawword);
	//must emptykey after call findAssociate,
	//because myword referenced the memory in the key_it
	_c(cur_ime)->emptyKey(cur_ime);
	_c(cur_ime)->update(cur_ime, NCS_IME_UPDATE_ALL);
	return TRUE;
}

static BOOL mIMManager_sendKeys(mIMManager* self)
{
	int i;
	mIME *cur_ime = self->cur_ime;
	for(i = 0; i < cur_ime->len; i++)
		PostMessage(self->hwnd_target, MSG_CHAR, (WPARAM) cur_ime->input[i], 0);

	_c(cur_ime)->clearInput(cur_ime);
	_c(cur_ime)->emptyKey(cur_ime);

	_c(cur_ime)->update(cur_ime, NCS_IME_UPDATE_ALL);
    return TRUE;
}

static BOOL mIMManager_addIME(mIMManager *self, mIME*ime)
{
	mIME *t = self->header;
	while(t && t != ime) t = t->next;
	if(t)
		return TRUE;
	ime->next = self->header;
	self->header = ime;
	ime->ime_manager = self;
	return TRUE;
}

static BOOL immanger_set_cur_ime(mIMManager* self, mIME *ime)
{
	if(!ime && !(self->flags & NCSF_IMM_NULLIME))
		return FALSE;
	if(self->cur_ime == ime)
		return TRUE;

	if(self->flags & NCSF_IMM_NULLIME)
	{
		_c(self->cur_ime)->hideIME(self->cur_ime);
        self->cur_ime = ime;
		// don't unregister the IMEWindow, and don't set self->cur_ime = NULL
		return TRUE;
	}

	if(self->cur_ime)
	{
		_c(self->cur_ime)->hideIME(self->cur_ime);
	}

	if(IsWindow(ime->hwnd_view))
	{
        self->flags |= NCSF_IMM_SETTING_IME;

		if(IsWindow(self->hwnd))
			UnregisterIMEWindow(self->hwnd);
		RegisterIMEWindow(ime->hwnd_view);
        self->hwnd = ime->hwnd_view;
	}

	self->cur_ime = ime;
	_c(ime)->empty(ime, TRUE); //empty the ime
	ime->ime_manager = self;
	if(!(self->flags & NCSF_IMM_DISABLE) && (self->flags & NCSF_IMM_ACTIVED))
	{
		_c(ime)->showIME(ime);
	}
	return TRUE;
}

static BOOL mIMManager_setIME(mIMManager* self, const char* ime_name)
{
	mIME *ime = self->header;
	self->flags &= ~NCSF_IMM_NULLIME; //clear the null ime
	while(ime)
	{
		if(ime->ime_engine && strcmp(_c(ime->ime_engine)->imeName(ime->ime_engine), ime_name) == 0)
			break;
		ime = ime->next;
	}
	return immanger_set_cur_ime(self, ime);
}

static BOOL mIMManager_nextIME(mIMManager* self)
{
	mIME* ime = NULL;

	if(self->flags & NCSF_IMM_NULLIME)
	{
		self->flags &= ~NCSF_IMM_NULLIME; //clear the null ime
		ime = self->header;
	}
	else
	{
		if(self->cur_ime)
	        	ime = self->cur_ime->next;

		self->flags &= ~NCSF_IMM_NULLIME; //clear the null ime
		if(!ime)
		{
			if(self->flags & NCSF_IMM_ACTIVED)
			{
				self->flags |= NCSF_IMM_NULLIME;
			}
			else
				ime = self->header;
		}
	}

    if(immanger_set_cur_ime(self, ime))
    {
        ncsRaiseEvent((mObject*)self, NCSN_IMM_IMECHANGED, (DWORD)ime );
        return TRUE;
    }
    return FALSE;
}

static void mIMManager_setEncoding(mIMManager* self, const char* encoding)
{
	mIME *ime;
	if(!encoding)
		return ;

	self->len_first_char = ncsIMGetFirstCharCallback(encoding);

	for(ime = self->header; ime; ime = ime->next)
		_c(ime)->setEncoding(ime, encoding);
}

BEGIN_MINI_CLASS(mIMManager, mObject)
	CLASS_METHOD_MAP(mIMManager, construct)
	CLASS_METHOD_MAP(mIMManager, destroy)
	CLASS_METHOD_MAP(mIMManager, addIME)
	CLASS_METHOD_MAP(mIMManager, procIMMsg)
	CLASS_METHOD_MAP(mIMManager, sendWords)
	CLASS_METHOD_MAP(mIMManager, sendKeys)
	CLASS_METHOD_MAP(mIMManager, nextIME)
	CLASS_METHOD_MAP(mIMManager, setIME)
	CLASS_METHOD_MAP(mIMManager, setEncoding)
END_MINI_CLASS

//////////////////////////////////////////////////////
BOOL ncsInitIMEClasses(void)
{
	MGNCS_INIT_CLASS(mIMIterator);
	MGNCS_INIT_CLASS(mIMEngine);
	MGNCS_INIT_CLASS(mIME);
	MGNCS_INIT_CLASS(mIMManager);
#ifdef _MGNCSENGINE_DIGIT
	MGNCS_INIT_CLASS(mDigitIterator);
	MGNCS_INIT_CLASS(mDigitEngine);
	MGNCS_INIT_CLASS(mDigitIME);
#endif
#ifdef _MGNCSENGINE_PTI
	MGNCS_INIT_CLASS(mPtiIterator);
	MGNCS_INIT_CLASS(mPtiEngine);
#endif
#ifdef _MGNCSENGINE_PINYIN
	MGNCS_INIT_CLASS(mPinyinIterator);
	MGNCS_INIT_CLASS(mPinyinEngine);
#endif
#ifdef _MGNCSENGINE_DIGITPTI
	MGNCS_INIT_CLASS(mDigitPtiIterator);
	MGNCS_INIT_CLASS(mDigitPtiEngine);
#endif
#ifdef _MGNCSENGINE_DIGITPY
	MGNCS_INIT_CLASS(mDigitPyIterator);
	MGNCS_INIT_CLASS(mDigitPyEngine);
#endif
	return TRUE;
}

////////////////////////////////////////////////


#include "charset-convert.c"

////////////////////////////////////////////////////////
#endif //_MGNCSENGINE_IME

