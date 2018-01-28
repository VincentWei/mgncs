
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"
#include "mdatabinding.h"

static BOOL processMessageHandlers(mWidget* widget, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pret);

static void processNotifyMessage(HWND hwnd, int id, int nc_code,DWORD add_data);

static mWidgetRenderer* getCtrlRDR (const char* rdrName, mWidgetClass *_class)
{
    mWidgetRenderer *rdr = NULL;
    mWidgetClass    *ptrClass = _class;

    while (!rdr && ptrClass) {
        rdr = ncsRetriveCtrlRDR(rdrName, ptrClass->className);
        ptrClass = (mWidgetClass*)ptrClass->super;
    }

    return rdr;
}

static void widget_add_comp(mWidget* self, mComponent* comp)
{
	if(!comp || !self)
		return ;

	_c(comp)->setReleated(comp, self->comps, NCS_CMPT_NEXT);
	if(self->comps)
		_c(self->comps)->setReleated(self->comps,comp,NCS_CMPT_PREV);
	self->comps = comp;
}


static void init_create_info(mWidget*self, NCS_CREATE_INFO *create_info)
{
	int i;
	if(create_info == NULL)
		return ;

	SetWindowAdditionalData(self->hwnd, create_info->user_data);
/*
	if(create_info->font_name){
        PLOGFONT font = CreateLogFontByName(create_info->font_name);
		SetWindowFont(self->hwnd, font);
    }
*/
	if(GetAValue(create_info->bk_color) != 0) //validat bk_color
	{
		SetWindowBkColor(self->hwnd,
			ncsColor2Pixel(HDC_SCREEN,create_info->bk_color));
	}

	if(create_info->props){
		NCS_PROP_ENTRY* props = create_info->props;
		for(i=0; props[i].id > 0; i++)
			_c(self)->setProperty(self, props[i].id, props[i].value);
	}

	if(create_info->rdr_info){
		NCS_RDR_INFO* rdr = create_info->rdr_info;
		_c(self)->setProperty(self, NCSP_WIDGET_RDR, (DWORD)rdr->ctl_rdr);

#if MINIGUI_MAJOR_VERSION < 3
		for(i=0; rdr->elements[i].id != -1; i++)
			ncsSetRendererElement(self->hwnd, rdr->elements[i].id, rdr->elements[i].value);
#endif
	}

	_c(self)->addChildren(self, create_info->ctrls, create_info->ctrl_count);

    //call notify
    if(create_info->notify_info && create_info->notify_info->onCreated)
        create_info->notify_info->onCreated(create_info->notify_info, (mComponent*)self, create_info->special_id);

}

/*
static const char* get_window_renderer_name(HWND hwnd)
{
    const WINDOWINFO *winfo = GetWindowInfo(hwnd);
    if(winfo && winfo->we_rdr)
        return winfo->we_rdr->name;
    return NULL;
}
*/

static void mWidget_construct(mWidget *self, DWORD addData)
{
	g_stmComponentCls.construct((mComponent*)self, addData);

	self->hwnd = (HWND)addData;
	ncsInitDrawInfo(&self->bkimg);
	//get parent's renderer
    self->renderer = getCtrlRDR(NULL, _c(self));

	if(_c(self)->createBody)
	{
		self->body = _c(self)->createBody(self);
		if(self->renderer)
			_c(Body)->setRenderer(Body, self->renderer->rdr_name);
		else
			_c(Body)->setRenderer(Body, "classic");

		_c(Body)->enable(Body,IsWindowEnabled(self->hwnd));
	}
	else
		self->body = NULL;

	SetNotificationCallback(self->hwnd,processNotifyMessage);
}

static void mWidget_destroy(mWidget *self)
{
	if(self)
	{
        PLOGFONT font = GetWindowFont(self->hwnd);
		mComponent* comp = self->comps;
		if(mWidget_getCapturedHotPiece(self))
			mWidget_releaseCapturedHotPiece();
		if(mWidget_getHoveringFocus(self))
			mWidget_releaseHoveringFocus();
		if(mWidget_getInputFocus(self))
			mWidget_releaseInputFocus();

        if (font)
            ReleaseRes(((FONT_RES *)font)->key);
		DELPIECE(self->body);
		ncsCleanImageDrawInfo(&self->bkimg);

		//release components
		while(comp)
		{
			mComponent *tcomp = comp;
			comp = _c(comp)->getReleated(comp, NCS_CMPT_NEXT);
			DELETE(tcomp);
		}
		self->comps = NULL;

		Class(mComponent).destroy((mComponent*)self);
	}
}

static int mWidget_onSizeChanged(mWidget *self, RECT *rtClient)
{
	if(self->body)
	{
		RECT rc = {0, 0, RECTWP(rtClient), RECTHP(rtClient)};
		_c((mHotPiece*)(self->body))->setRect((mHotPiece*)self->body, &rc);
	}

	return 0;
}

static void mWidget_onPaint(mWidget *self, HDC hdc, const PCLIPRGN pclip)
{
	if(self->body)
	{
		DWORD add_data = 0;
		if(GetWindowStyle(self->hwnd)&WS_DISABLED) {
			add_data = PIECE_STATE_DISABLE;
            _c((mHotPiece*)(self->body))->enable((mHotPiece*)self->body, FALSE);
        }
        else
            _c((mHotPiece*)(self->body))->enable((mHotPiece*)self->body, TRUE);

		_c((mHotPiece*)(self->body))->paint((mHotPiece*)self->body, hdc, (mObject*)self, add_data);
	}
}

static BOOL mWidget_addChildren(mWidget *self, NCS_WND_TEMPLATE* children,int count)
{
	int i;
	if(children == NULL || count <= 0)
		return FALSE;

	for(i=0; i<count; i++)
	{
		mComponent * other_comp;
		NCS_WND_TEMPLATE * wnd_tmpl = &children[i];
		mComponentClass* compCls = ncsGetComponentClass(wnd_tmpl->class_name, TRUE);

		if(compCls) //components
		{
			other_comp = compCls->createComponent((mComponent*)self, compCls, (DWORD)wnd_tmpl);
			if(!other_comp)
				continue;
			if(!INSTANCEOF(other_comp, mWidget))
				widget_add_comp(self, other_comp);
		}
		else //is a old window control
		{
		    ncsCreateWindowIndirect(wnd_tmpl, self->hwnd);
		}
	}

	return TRUE;
}

static mWidget* createWidget(HWND hWnd)
{
	mWidget * self = NULL;
	mWidgetClass *_class = NULL;
	BOOL bMainWnd = IsMainWindow(hWnd);

	if(bMainWnd){
		NCS_MAIN_CREATE_INFO *main_create_info =
			(NCS_MAIN_CREATE_INFO*)GetWindowAdditionalData(hWnd);
		_class = ncsGetMainWndClass(main_create_info?main_create_info->className:NULL);
		//set NCS_CREATE_INFO in to additonal data
		if(main_create_info)
			SetWindowAdditionalData(hWnd, (DWORD)main_create_info->create_info);
	}
	else
	{
		_class = (mWidgetClass*)ncsClassFromMagicNum(GetWindowClassAdditionalData(hWnd), FALSE, NULL);
	}

	if(_class == NULL)
		return NULL;

	self = (mWidget*)newObject((mObjectClass*)_class);
	//important, self and hwnd must be ok before calling construct
	SetWindowAdditionalData2(hWnd, (DWORD)self);

	_c(self)->construct(self, (DWORD)hWnd);

	//some message, such MSG_NCCREATE, MSG_SIZECHANGING, and so on would called before MSG_CREATE
	//so, we must initialize the message handler when NCCREATE message
	{
		NCS_CREATE_INFO * create_info = (NCS_CREATE_INFO*)GetWindowAdditionalData(hWnd);
		if(create_info && create_info->handlers)
		{
			ncsSetComponentHandlers((mComponent*)self, create_info->handlers,-1);
		}
	}


	return self;
}

static LRESULT _default_wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT ret = 0;
	mWidget * self = NULL;
	mWidgetClass *_class = NULL;

	if(message == MSG_NCCREATE){
		//call msg hanlder processor
		self = createWidget(hWnd);
		processMessageHandlers(self, message, wParam, lParam, &ret);
		return 0;
	}

    if(message == MSG_NCDESTROY){
        if(IsMainWindow(hWnd))
            DestroyMainWindow(hWnd);
        else
            DestroyWindow(hWnd);
        return 0;
    }
	else {
		self = (mWidget*) GetWindowAdditionalData2(hWnd);
        if (!self)
            return -1;

		_class = self->_class;
        if (!_class)
            return -1;

        if(message == MSG_DESTROY)
        {
            processMessageHandlers(self, message, wParam, lParam, &ret);

            SetWindowAdditionalData2(hWnd, 0);
            deleteObject((mObject*)self);
            return 0;
        }
		else if(message == MSG_CREATE) //create children control
		{
			NCS_CREATE_INFO* create_info = (NCS_CREATE_INFO*)GetWindowAdditionalData(hWnd);
			//MUST INIT CREATE INFO WHEN MSG_CREATE SEND
			//WHEN MSG_CREATE SENT, THE CONTRLS CAN BE CREATE
			//construct will set defult property value's
			init_create_info(self, create_info);
			//MSG_CREATE must called default proc firstly
			if(_class->wndProc)
				ret = _class->wndProc(self, message, wParam, create_info?create_info->user_data:0);
			processMessageHandlers(self, message, wParam, GetWindowAdditionalData(hWnd), NULL);
			return ret;
		}

        if(processMessageHandlers(self, message, wParam, lParam, &ret))
			return ret;

		if(_class->wndProc)
			return (*_class->wndProc)(self, message, wParam, lParam);
	}

    if (NCS_OBJ_TODEL(self))
        return 0;

	return DefaultControlProc(hWnd, message, wParam, lParam);
}

static LRESULT ncsWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    mWidget *self = NULL;
    LRESULT ret;
    BOOL cleanMainWnd = FALSE;

	if(message == MSG_NCCREATE){
		//call msg hanlder processor
		self = createWidget(hWnd);
		processMessageHandlers(self, message, wParam, lParam, &ret);
		return 0;
	}

    self = ncsObjFromHandle(hWnd);
    if (!self)
        return 0;

    _c(self)->addRef(self);

    if (message == MSG_NCDESTROY && IsMainWindow(hWnd)) {
        cleanMainWnd = TRUE;
    }

    ret = _default_wnd_proc(hWnd, message, wParam, lParam);
    _c(self)->release(self);

    //Note: for main window, we should cleanup to avoid memory leak.
    //please don't process it in _default_wnd_proc, it will cause
    //invalid read when release widget.
    if (cleanMainWnd) {
        MainWindowCleanup(hWnd);
    }
    return ret;
}

static BOOL mWidget_onCreate(mWidget *self, LPARAM lParam)
{
	if(Body)
		SetBodyProp(NCSP_LABELPIECE_LABEL, (DWORD)GetWindowCaption(self->hwnd));
	return TRUE;
}

static int mWidget_onLButtonDown(mWidget *self, int x, int y, DWORD keyFlags)
{
	ncsNotifyParent(self, NCSN_WIDGET_CLICKED);
	return 1;
}

#ifdef _MGNCS_GUIBUILDER_SUPPORT
static BOOL mWidget_refresh(mWidget *self)
{
	if(!_c(self)->createBody)
		return FALSE;


	DELPIECE(self->body);
    self->body = NULL;

	//remove the event
	//ncsRemoveEventListener((mObject*)self);
	self->body = _c(self)->createBody(self);
	if(self->body)
	{
		if(self->renderer)
			_c(Body)->setRenderer(Body, self->renderer->rdr_name);
		else
			_c(Body)->setRenderer(Body, "classic");

		_c(Body)->enable(Body,IsWindowEnabled(self->hwnd));

		if(_c(self)->onCreate)
			_c(self)->onCreate(self, 0);
	}
	return TRUE;
}
#endif

static LRESULT mWidget_wndProc(mWidget* self, UINT message, WPARAM wParam, LPARAM lParam)
{
	mWidgetClass* _class = self->_class;
	mHotPiece * hotfocus, * body;

	if(self->body /*&& INSTANCEOF(self->body, mHotPiece)*/)
	{
		body = (mHotPiece*)self->body;

		if(message >= MSG_FIRSTKEYMSG && message < MSG_LASTKEYMSG && GetFocusChild(self->hwnd)==HWND_NULL)
		{
			hotfocus = (mHotPiece*) mWidget_getInputFocus(self);
			if(!hotfocus)
				hotfocus = (mHotPiece*) mWidget_getCapturedHotPiece(self);
			if(!hotfocus)
				hotfocus = (mHotPiece*) mWidget_getHoveringFocus(self);
			if(hotfocus)
				_c(hotfocus)->processMessage(hotfocus, message, wParam, lParam, (mObject*)self);
			else
				_c(body)->processMessage(body, message, wParam, lParam, (mObject*)self);
		}
		else if(message >= MSG_FIRSTMOUSEMSG && message <= MSG_RBUTTONDBLCLK)
		{
			hotfocus = (mHotPiece*) mWidget_getCapturedHotPiece(self);
			if(hotfocus)
				return _c(hotfocus)->processMessage(hotfocus, message, wParam, lParam, (mObject*)self);

			hotfocus = (mHotPiece*) mWidget_getHoveringFocus(self);

			body = _c(body)->hitTest(body, LOSWORD(lParam), HISWORD(lParam));

			if(!body)
			{
				if(hotfocus)
				{
					_c(hotfocus)->processMessage(hotfocus, MSG_MOUSEMOVEIN, FALSE, 0, (mObject*)self);
				}
				mWidget_releaseHoveringFocus();
			}
			else
			{
				if(body != hotfocus)
				{
					if(hotfocus)
						_c(hotfocus)->processMessage(hotfocus, MSG_MOUSEMOVEIN, FALSE, 0, (mObject*)self);
					_c(body)->processMessage(body, MSG_MOUSEMOVEIN, TRUE, 0, (mObject*)self);
					mWidget_setHoveringFocus(self, (mObject*)body);
				}
				return _c(body)->processMessage(body, message, wParam, lParam, (mObject*)self);
			}
		}
		else if (message == MSG_MOUSEMOVEIN)
		{
			if(!wParam) //mouse move out
			{
				hotfocus = (mHotPiece*)mWidget_getHoveringFocus(self);
				if(hotfocus){
					_c(hotfocus)->processMessage(hotfocus, MSG_MOUSEMOVEIN, FALSE, 0, (mObject*)self);
					mWidget_releaseHoveringFocus();
				}
			}
			else
			{
				_c(Body)->processMessage(Body, message, wParam, lParam, (mObject*)self);
			}
		}
		else if(message == MSG_ACTIVEMENU)
		{
			_c(Body)->processMessage(Body, MSG_MOUSEMOVEIN, FALSE, 0, (mObject*)self);
			mWidget_releaseHoveringFocus();
		}
	}

    if (NCS_OBJ_TODEL(self))
        return 0;

	switch(message){
		case MSG_CREATE:
			if(CHECK_METHOD(_class,onCreate))
			{
				if(!_class->onCreate(self, lParam))
					return -1;
			}
			break;
		case MSG_PAINT:
			if(CHECK_METHOD(_class, onPaint)) {
				HDC hdc = BeginPaint (self->hwnd);
				_class->onPaint(self, hdc, (const PCLIPRGN)lParam);
				EndPaint(self->hwnd, hdc);
				return 0;
			}
			break;
		case MSG_LBUTTONDOWN:
			if(CHECK_METHOD(_class, onLButtonDown)) {
				if (_class->onLButtonDown(self, LOSWORD(lParam), HISWORD(lParam), wParam))
                    break;
                return 0;
			}
			break;
		case MSG_LBUTTONUP:
			if(CHECK_METHOD(_class, onLButtonUp)) {
				if (_class->onLButtonUp(self, LOSWORD(lParam), HISWORD(lParam), wParam))
                    break;
                return 0;
			}
			break;
		case MSG_MOUSEMOVE:
			if(CHECK_METHOD(_class, onMouseMove)) {
				if (_class->onMouseMove(self, LOSWORD(lParam), HISWORD(lParam), wParam))
                    break;
                return 0;
			}
			break;
		case MSG_KEYDOWN:
			if(CHECK_METHOD(_class, onKeyDown)) {
				if (_class->onKeyDown(self, (int)wParam, (DWORD)lParam))
                    break;

                return 0;
			}
			break;
		case MSG_KEYUP:
			if(CHECK_METHOD(_class, onKeyUp)) {
				if (_class->onKeyUp(self, (int)wParam, (DWORD)lParam))
                    break;
                return 0;
			}
			break;
		case MSG_SIZECHANGED:
			if(CHECK_METHOD(_class, onSizeChanged)) {
				return _class->onSizeChanged(self, (RECT*)lParam);
			}
			break;
		case MSG_GETDLGCODE:
			return _c(self)->dlgCode;
		case MSG_SETTEXT:
			SetWindowCaption(self->hwnd, (const char*)lParam);
			if(Body)
				SetBodyProp(NCSP_LABELPIECE_LABEL, (DWORD)(GetWindowCaption(self->hwnd)));
			InvalidateRect(self->hwnd, NULL, TRUE);
			return strlen((const char*)lParam);

		case MSG_LBUTTONDBLCLK:
			ncsNotifyParent(self, NCSN_WIDGET_DBCLICKED);
			break;

		case MSG_ENABLE:
			{

				DWORD dwStyle = GetWindowStyle(self->hwnd);
				if( ((dwStyle & WS_DISABLED) && !wParam) ||  ( !(dwStyle & WS_DISABLED) && wParam))
					return 0;
				if(wParam)
					ExcludeWindowStyle(self->hwnd, WS_DISABLED);
				else
					IncludeWindowStyle(self->hwnd, WS_DISABLED);

				ncsNotifyParent(self, wParam?NCSN_WIDGET_ENABLED:NCSN_WIDGET_DISABLED);

				if(Body)
					_c(Body)->enable(Body, (BOOL)wParam);

				InvalidateRect(self->hwnd, NULL, TRUE);
			}
			return 0;

		case MSG_ERASEBKGND:
	
			if(_c(self)->onEraseBkgnd || self->renderer->drawBkground){
				BOOL bret = TRUE;
				HDC hdc = (HDC)wParam;
				RECT rcTemp;
				if(hdc == 0)
				{
#if MINIGUI_MAJOR_VERSION >= 3
					hdc = GetSecondaryClientDC(self->hwnd);
					if(hdc == 0)
#endif
						hdc = GetClientDC(self->hwnd);
				}

				if(lParam != 0)
				{
					rcTemp = *((const RECT*)(lParam));
				/*	if(IsMainWindow(self->hwnd))
					{
						ScreenToClient(self->hwnd, &rcTemp.left, &rcTemp.top);
						ScreenToClient(self->hwnd, &rcTemp.right, &rcTemp.bottom);
					}
				*/}
				else
				{
					GetClientRect(self->hwnd, &rcTemp);
				}

//				ClipRectIntersect(hdc, &rcTemp);

				if(_c(self)->onEraseBkgnd){
					 bret = _c(self)->onEraseBkgnd(self, hdc, &rcTemp);
				}
				else{
					self->renderer->drawBkground(self, hdc, &rcTemp);
				}

				if(hdc != (HDC)wParam)
				{
#if MINIGUI_MAJOR_VERSION >= 3
					ReleaseSecondaryDC(self->hwnd, hdc);
#else
					ReleaseDC(hdc);
#endif
				}
				return bret;
			}
			break;
    }

    if (NCS_OBJ_TODEL(self))
        return 0;

	return DefaultControlProc(self->hwnd, message, wParam, lParam);
}

static BOOL mWidget_setProperty(mWidget *self, int id, DWORD value)
{
	if(id >= NCSP_WIDGET_MAX)
		return FALSE;

	switch(id){
	case NCSP_WIDGET_RDR:
		{
			//get render by name
			mWidgetRenderer* renderer = getCtrlRDR((const char*)value, _c(self));
            
			if(renderer != self->renderer)
			{
				self->renderer = renderer;
				//reset the body renderer
				if(Body)
					_c(Body)->setRenderer(Body, (const char*)value);
                SetWindowElementRenderer(self->hwnd, (const char*)value, NULL);
				//UpdateWindow(self->hwnd, TRUE);
				//SetWindowElementRenderer auto update the window
                return TRUE;
			}
		}
        break;

	case NCSP_WIDGET_TEXT:
		SetWindowText(self->hwnd, value==0?"":(char*)value);
		return TRUE;

	case NCSP_WIDGET_BKIMAGE:
		if(ncsSetImageDrawInfo(&self->bkimg, (void*)value, self->bkimg.drawMode, IMG_TYPE_BITMAP))
		{
			InvalidateRect(self->hwnd, NULL, TRUE);
			return TRUE;
		}
		return FALSE;

	case NCSP_WIDGET_BKIMAGE_FILE:
		if(ncsSetImageDrawInfoByFile(&self->bkimg, (const char*)value, self->bkimg.drawMode, FALSE))
		{
			InvalidateRect(self->hwnd, NULL, TRUE);
			return TRUE;
		}
		return FALSE;

	case NCSP_WIDGET_BKIMAGE_MODE:
		self->bkimg.drawMode = (int)value;
		InvalidateRect(self->hwnd, NULL, TRUE);
		return TRUE;
	}
	return FALSE;
}

static DWORD mWidget_getProperty(mWidget *self, int id)
{
	if(id == NCSP_WIDGET_TEXT)
		return (DWORD)GetWindowCaption(self->hwnd);

	return 0;
}


static BOOL mWidget_callUserHandler( mWidget* self, void *handler, int message, WPARAM wParam, LPARAM lParam, int *pret)
{
	int ret = 0;
	switch(message)
	{
	case MSG_NCCREATE:
		((NCS_CB_ONNCCREATE)handler)(self);
		return TRUE;
	case MSG_CREATE:
		if(!((NCS_CB_ONCREATE)handler)(self, (DWORD)lParam))
		{
			if(pret)
				*pret = -1;
		}
		return TRUE;
	case MSG_FONTCHANGED:
		return ((NCS_CB_ONVOID)handler)(self, message);
	case MSG_ACTIVE:
	case MSG_DESTROY:
	case MSG_SETFOCUS:
	case MSG_KILLFOCUS:
		((NCS_CB_ONVOID)handler)(self, message);
		return FALSE;
	case MSG_SIZECHANGING:
		((NCS_CB_ONSZCHGING)handler)(self, (const PRECT)wParam, (PRECT)lParam);
		return TRUE;
	case MSG_SIZECHANGED:
        Class(mWidget).onSizeChanged(self, (RECT*)lParam);
		ret = ((NCS_CB_ONSZCHGED)handler)(self, (PRECT)lParam);
		if(pret)
			*pret = ret;
		return ret;
	case MSG_CSIZECHANGED:
		((NCS_CB_ONCSZCHGED)handler)(self, (int)wParam, (int)lParam);
		return TRUE;
	case MSG_FONTCHANGING:
		ret = ((NCS_CB_ONFONTCHGING)handler)(self, (PLOGFONT)lParam);
		if(pret)
			*pret = ret;
		return ret;
	case MSG_ERASEBKGND:
		return ((NCS_CB_ONERASEBKGND)handler)(self, (HDC)wParam, (const PRECT)lParam);
	case MSG_PAINT:
		{
			HDC hdc = BeginPaint(self->hwnd);
			((NCS_CB_ONPAINT)handler)(self, hdc, (const PCLIPRGN)lParam);
			EndPaint(self->hwnd, hdc);
		}
		return TRUE;
	case MSG_CLOSE:
		{
			HWND hwnd = self->hwnd;
			((NCS_CB_ONCLOSE)handler)(self);
			if(ncsObjFromHandle(hwnd) != self) //user had called DestroyMainWindow in handler
				return TRUE;
			break;
		}

	case MSG_KEYDOWN:
	case MSG_CHAR:
	case MSG_KEYUP:
	case MSG_SYSKEYDOWN:
	case MSG_SYSCHAR:
	case MSG_SYSKEYUP:
	case MSG_KEYLONGPRESS:
	case MSG_KEYALWAYSPRESS:
		ret = ((NCS_CB_ONKEY)handler)(self, message, (int)wParam, (DWORD)lParam);
		if(pret)
			*pret = ret;
		return ret;

	case MSG_LBUTTONDOWN:
	case MSG_LBUTTONUP:
	case MSG_LBUTTONDBLCLK:
	case MSG_MOUSEMOVE:
	case MSG_RBUTTONDOWN:
	case MSG_RBUTTONUP:
	case MSG_RBUTTONDBLCLK:
		ret = ((NCS_CB_ONMOUSE)handler)(self, message, LOSWORD (lParam), HISWORD(lParam), (DWORD)wParam);
		if(pret)
			*pret = ret;
		return ret;

	case MSG_NCLBUTTONDOWN:
	case MSG_NCLBUTTONUP:
	case MSG_NCLBUTTONDBLCLK:
	case MSG_NCMOUSEMOVE:
	case MSG_NCRBUTTONDOWN:
	case MSG_NCRBUTTONUP:
	case MSG_NCRBUTTONDBLCLK:
		return ((NCS_CB_ONNCMOUSE)handler)(self, message, LOSWORD (lParam), HISWORD(lParam), (int)wParam);

	case MSG_HITTEST:
		ret = ((NCS_CB_ONHITTEST)handler)(self, message, (int)wParam, (int)lParam);
		if(pret) *pret = ret;
		return TRUE;

	case MSG_HSCROLL:
	case MSG_VSCROLL:
		((NCS_CB_ONSCROLL)handler)(self, message, (int)wParam, (int)lParam);
		return TRUE;

	case MSG_COMMAND:
		return ((NCS_CB_ONCMD)handler)(self, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);

	case MSG_TIMER:
		((NCS_CB_WIDGET_ONTIMER)handler)(self, (int)wParam, (DWORD)lParam);
		return TRUE;

	default:
		*pret = ((NCS_CB_ONMSG)handler)(self, message, wParam, lParam);
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////
//

static mWidget* mWidget_createComponent(mComponent* parent, mWidget* _class, DWORD addValue)
{
	mWidget * widget;

	//defaultly we create a control window
	const NCS_WND_TEMPLATE * ptmpl = (const NCS_WND_TEMPLATE*)addValue;

	if(ptmpl == NULL || parent == NULL)
		return NULL;

	widget = SAFE_CAST(mWidget, parent);
	if(!widget || !IsWindow(widget->hwnd))
		return NULL;

	return ncsCreateWindowIndirect(ptmpl, widget->hwnd);
}

static int mWidget_getId(mWidget* self)
{
	const WINDOWINFO * win_info = GetWindowInfo(self->hwnd);
	return win_info == NULL?0:win_info->id;
}

static int mWidget_setId(mWidget* self, int id)
{
	WINDOWINFO * win_info = (WINDOWINFO*)GetWindowInfo(self->hwnd);
	if(win_info)
	{
		int old_id = win_info->id;
		win_info->id = id;
		return old_id;
	}
	return id;
}

static mComponent* mWidget_getReleated(mWidget *self, int releated)
{
	if(!self)
		return NULL;

	switch(releated)
	{
	case NCS_CMPT_NEXT:
		if(IsMainWindow(self->hwnd))
			return (mComponent*)ncsObjFromHandle(GetNextMainWindow(self->hwnd));
		return (mComponent*)ncsObjFromHandle(GetNextChild(GetParent(self->hwnd), self->hwnd));
	case NCS_CMPT_PREV:
		return NULL;
	case NCS_CMPT_PARENT:
		return (mComponent*)ncsObjFromHandle(GetParent(self->hwnd));
	case NCS_CMPT_CHILDREN:
		return (mComponent*)ncsObjFromHandle(GetNextChild(self->hwnd, (HWND)0));
	}

	return NULL;
}

static mComponent* mWidget_setReleated(mWidget *self, mComponent* comp, int releated)
{
	if(!comp)
		return NULL;

	if(releated == NCS_CMPT_CHILDREN)
	{
		if(!INSTANCEOF(comp, mWidget)
			&& _c(comp)->setReleated(comp, (mComponent*)self,NCS_CMPT_PARENT))
		{
			widget_add_comp(self, comp);
			return comp;
		}
	}
	return NULL;
}

static mComponent* mWidget_getChild(mWidget* self, int id)
{
	HWND hwnd = self->hwnd;
	HWND hchild ;
	mComponent * comp = self->comps;
	while(comp)
	{
		if(_c(comp)->getId(comp) == id)
			return comp;
		comp = _c(comp)->getReleated(comp, NCS_CMPT_NEXT);
	}

	hchild = GetDlgItem(hwnd, id);

	return (mComponent*)ncsObjFromHandle(hchild);
}

static BOOL get_image_area(const RECT * prc, PBITMAP pbmp, RECT *pimg_draw_rc)
{
	if(!pbmp || !prc)
		return FALSE;

	if(pbmp->bmWidth >= RECTWP(prc) && pbmp->bmHeight >= RECTHP(prc))
		return FALSE;

	pimg_draw_rc->left = (prc->left + prc->right - pbmp->bmWidth) / 2;
	pimg_draw_rc->top  = (prc->top + prc->bottom - pbmp->bmHeight) / 2;
	pimg_draw_rc->right = pimg_draw_rc->left + pbmp->bmWidth;
	pimg_draw_rc->bottom = pimg_draw_rc->top + pbmp->bmHeight;
	return TRUE;
}

#define IS_ALPHA_BMP(pbmp) \
	(((pbmp)->bmType & (BMP_TYPE_ALPHACHANNEL | BMP_TYPE_COLORKEY)) \
	 || (((pbmp)->bmType & BMP_TYPE_ALPHA) && (pbmp)->bmAlphaMask))

static BOOL mWidget_onEraseBkgnd(mWidget* self, HDC hdc, const RECT *pinv)
{
	DWORD bkcolor;
	gal_pixel old;
	RECT rc;
	bkcolor = GetWindowBkColor(self->hwnd);
	GetClientRect(self->hwnd, &rc);
	if(pinv == NULL)
		pinv = &rc;

	if(!self->bkimg.img.pbmp || IS_ALPHA_BMP(self->bkimg.img.pbmp))
	{
		old = SetBrushColor(hdc, bkcolor);
        FillBox(hdc, pinv->left, pinv->top, RECTWP(pinv), RECTHP(pinv));
		SetBrushColor(hdc, old);
	}
	else if(self->bkimg.img.pbmp && (self->bkimg.drawMode == NCS_DM_NORMAL))
	{
		RECT rcImg;
		if(get_image_area(&rc, self->bkimg.img.pbmp, &rcImg))
		{
			CLIPRGN* pclipRgn;
			old = SetBrushColor(hdc, bkcolor);
			pclipRgn = CreateClipRgn();
			GetClipRegion(hdc, pclipRgn);
			ExcludeClipRect(hdc, &rcImg);
        	FillBox(hdc, pinv->left, pinv->top, RECTWP(pinv), RECTHP(pinv));
			SelectClipRegion(hdc, pclipRgn);
			DestroyClipRgn(pclipRgn);
			SetBrushColor(hdc, old);
		}
	}

	if(self->bkimg.img.pbmp){
		ncsImageDrawInfoDraw(&self->bkimg, hdc, &rc, NCS_ALIGN_CENTER, NCS_VALIGN_CENTER);
	}

	return TRUE;
}

static void mWidget_removeChild(mWidget* self, mComponent* comp)
{
	mComponent* prev;
	if(!comp)
		return;

	if(INSTANCEOF(comp, mWidget))
		return;

	if(self->comps == comp){
		self->comps = _c(comp)->getReleated(comp, NCS_CMPT_NEXT);
		_c(self->comps)->setReleated(self->comps, NULL, NCS_CMPT_PREV);
	}
	else
	{
		mComponent* next;
		prev = self->comps;
		next = _c(prev)->getReleated(prev, NCS_CMPT_NEXT);
		while(next && next != comp)
		{
			prev = next;
			next = _c(prev)->getReleated(prev, NCS_CMPT_NEXT);
		}
		if(next)
		{
			next = _c(comp)->getReleated(comp, NCS_CMPT_NEXT);
			_c(prev)->setReleated(prev, next, NCS_CMPT_NEXT);
			if(next)
				_c(next)->setReleated(next, prev, NCS_CMPT_NEXT);
		}
	}
}

BEGIN_CMPT_CLASS(mWidget, mComponent)
	_class->_window_proc = ncsWndProc;
	CLASS_METHOD_MAP(mWidget, construct)
	CLASS_METHOD_MAP(mWidget, destroy)
	CLASS_METHOD_MAP(mWidget, wndProc)
	CLASS_METHOD_MAP(mWidget, callUserHandler)
	CLASS_METHOD_MAP(mWidget, createComponent)
	CLASS_METHOD_MAP(mWidget, setProperty)
	CLASS_METHOD_MAP(mWidget, getProperty)
	CLASS_METHOD_MAP(mWidget, getId)
	CLASS_METHOD_MAP(mWidget, setId)
	CLASS_METHOD_MAP(mWidget, getReleated)
	CLASS_METHOD_MAP(mWidget, setReleated)
	CLASS_METHOD_MAP(mWidget, getChild)
	CLASS_METHOD_MAP(mWidget, onEraseBkgnd)
	CLASS_METHOD_MAP(mWidget, onSizeChanged)
	CLASS_METHOD_MAP(mWidget, onPaint)
	CLASS_METHOD_MAP(mWidget, onCreate)
	CLASS_METHOD_MAP(mWidget, onLButtonDown)
	CLASS_METHOD_MAP(mWidget, addChildren)
	CLASS_METHOD_MAP(mWidget, removeChild)
#ifdef _MGNCS_GUIBUILDER_SUPPORT
	CLASS_METHOD_MAP(mWidget, refresh)
#endif
END_CMPT_CLASS

static BOOL processMessageHandlers(mWidget* widget, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pret)
{
	void *handler;

	if(!widget)
		return FALSE;

	handler = ncsGetComponentHandler((mComponent*)widget, message);

	if(handler == NULL)
		return FALSE;

	return _c(widget)->callUserHandler(widget, handler, message, wParam, lParam, pret);
}

static void processNotifyMessage(HWND hWnd, int id, int nc_code,DWORD add_data)
{
	mWidget* widget;
	void *handler;
    widget = (mWidget*)GetWindowAdditionalData2(hWnd);

	if(widget)
	{
		//raise data binding
		ncsRaiseComponentBindProps((mComponent*)widget, nc_code);

		handler = ncsGetComponentHandler((mComponent*)widget, 0xFFFF0000|nc_code);
		if(handler)
		{
			((NCS_CB_NOTIFY)handler)(widget, id, nc_code, add_data);
		}
//		else
		{
			//raise event
			ncsRaiseEvent((mObject*)widget, nc_code, add_data);
		}
	}
}

/////////////////////////////////////////////////
//


BOOL ncsSetFont (HWND hWnd, const char *font_name)
{
    PLOGFONT font, of;
    
	font = (PLOGFONT)LoadResource(font_name, RES_TYPE_FONT, 0L);
    of = SetWindowFont (hWnd, font);
    
	if(of)
	    ReleaseRes(((FONT_RES *)of)->key);

	return TRUE;
}

BOOL ncsSetProperty(HWND hWnd, int id, DWORD value)
{
	mWidget *widget = (mWidget*)GetWindowAdditionalData2(hWnd);

	if(widget == NULL)
		return FALSE;

	return _c(widget)->setProperty(widget, id, value);
}

DWORD ncsGetProperty(HWND hWnd, int id)
{
	mWidget *widget = (mWidget*)GetWindowAdditionalData2(hWnd);

	if(widget == NULL)
		return 0;

	return _c(widget)->getProperty(widget, id);

}

mWidget* ncsCreateWindow (const char *class_name, const char *caption, DWORD style, DWORD ex_style, \
	int id, int x, int y, int w, int h, HWND hParent, \
	NCS_PROP_ENTRY * props, \
	NCS_RDR_INFO * rdr_info, \
	NCS_EVENT_HANDLER * handlers, \
	DWORD add_data)
{
	HWND hWnd;
	NCS_CREATE_INFO create_info = {
		props,
		rdr_info,
		handlers,
		NULL,
		0,
		add_data
	};


#if MINIGUI_MAJOR_VERSION >= 3
	hWnd = CreateWindowEx2(class_name, caption?caption:"", style, ex_style, id, x, y, w, h, hParent, \
		rdr_info?rdr_info->glb_rdr:NULL, (const WINDOW_ELEMENT_ATTR*)(rdr_info && rdr_info->elements?rdr_info->elements:NULL), (DWORD)&create_info);
#else
	hWnd = CreateWindowEx(class_name, caption?caption:"", style, ex_style, id, x, y, w, h, hParent,(DWORD)&create_info);
#endif

	if(IsWindow(hWnd))
	{
		return (mWidget*)GetWindowAdditionalData2(hWnd);
	}

	return NULL;
}

mWidget* ncsCreateMainWindow (const char *class_name, const char *caption, DWORD style, DWORD ex_style, \
	int id, int x, int y, int w, int h, HWND host, \
	HICON hIcon, HMENU hMenu,
	NCS_PROP_ENTRY * props, \
	NCS_RDR_INFO * rdr_info, \
	NCS_EVENT_HANDLER * handlers, \
	DWORD add_data)
{
	HWND hMain;
	WNDCLASS WndClass;
	NCS_CREATE_INFO create_info = {
		props,
		rdr_info,
		handlers,
		NULL,
		0,
		add_data
	};

	NCS_MAIN_CREATE_INFO main_create_info = {
		class_name,
		&create_info
	};

	MAINWINCREATE MainCreate;

	memset(&WndClass, 0, sizeof(WndClass));
	WndClass.spClassName = (char*)class_name;
	WndClass.opMask = COP_STYLE|COP_HCURSOR|COP_BKCOLOR;

	GetWindowClassInfo(&WndClass);

	MainCreate.dwStyle = style | WndClass.dwStyle;
	MainCreate.dwExStyle = ex_style | WndClass.dwExStyle;
	MainCreate.spCaption = caption?caption:"";
	MainCreate.hMenu = hMenu;
	MainCreate.hCursor = WndClass.hCursor;
	MainCreate.hIcon = hIcon;
	MainCreate.hHosting = host;
	MainCreate.MainWindowProc = ncsWndProc;
	MainCreate.lx = x;
	MainCreate.ty = y;
	MainCreate.rx = x + w;
	MainCreate.by = y + h;
	MainCreate.iBkColor = WndClass.iBkColor;
	MainCreate.dwAddData = (DWORD)&main_create_info;
	MainCreate.dwReserved = 0;

#if MINIGUI_MAJOR_VERSION >= 3
	hMain = CreateMainWindowEx(&MainCreate, \
		rdr_info?rdr_info->glb_rdr:NULL, \
		(const WINDOW_ELEMENT_ATTR*)(rdr_info && rdr_info->elements?rdr_info->elements:NULL),
		NULL, NULL);
#else
	hMain = CreateMainWindow(&MainCreate);
#endif

	if(IsWindow(hMain))
	{
		mWidget* mainwnd = (mWidget*)GetWindowAdditionalData2(hMain);
		if(mainwnd)
			_c(mainwnd)->setId(mainwnd,id);
		return mainwnd;
	}

	return NULL;
}

//create control window indirect
mWidget* ncsCreateWindowIndirect( const NCS_WND_TEMPLATE* tmpl, HWND hParent)
{
	HWND hWnd;
	NCS_RDR_INFO* rdr_info;
	if(tmpl == NULL)
		return NULL;

	rdr_info = tmpl->rdr_info;

#if MINIGUI_MAJOR_VERSION >= 3
	hWnd = CreateWindowEx2(tmpl->class_name,
		tmpl->caption?tmpl->caption:"",
		tmpl->style,
		tmpl->ex_style,
		tmpl->id,
		tmpl->x,
		tmpl->y,
		tmpl->w,
		tmpl->h,
		hParent,
		rdr_info?rdr_info->glb_rdr:NULL,
		(const WINDOW_ELEMENT_ATTR*)(rdr_info && rdr_info->elements?rdr_info->elements:NULL),
		(DWORD)&(tmpl->props));
#else
	hWnd = CreateWindowEx(tmpl->class_name,
		tmpl->caption?tmpl->caption:"",
		tmpl->style,
		tmpl->ex_style,
		tmpl->id,
		tmpl->x,
		tmpl->y,
		tmpl->w,
		tmpl->h,
		hParent,
		(DWORD)&(tmpl->props));
#endif

	if(IsWindow(hWnd))
	{
		mWidget *wnd = (mWidget*)GetWindowAdditionalData2(hWnd);
        if(wnd && tmpl->font_name){
            ncsSetFont (hWnd, tmpl->font_name);
        }
        return wnd;
	}

	return NULL;
}


mWidget* ncsCreateMainWindowIndirect(const NCS_MNWND_TEMPLATE* tmpl, HWND host)
{
	HWND hMain;
	WNDCLASS WndClass;
    NCS_RDR_INFO *rdr_info;
	NCS_MAIN_CREATE_INFO main_create_info;
	MAINWINCREATE MainCreate;

	if (tmpl == NULL)
		return NULL;

	main_create_info.className = tmpl->class_name;
	main_create_info.create_info = (NCS_CREATE_INFO*)&tmpl->props;

	rdr_info = tmpl->rdr_info;

	memset(&WndClass, 0, sizeof(WndClass));
	WndClass.spClassName = (char*)tmpl->class_name;
	WndClass.opMask = COP_STYLE|COP_HCURSOR|COP_BKCOLOR;

	GetWindowClassInfo(&WndClass);

	MainCreate.dwStyle = tmpl->style | WndClass.dwStyle;
	MainCreate.dwExStyle = tmpl->ex_style | WndClass.dwExStyle;
	MainCreate.spCaption = tmpl->caption ? tmpl->caption : "";
	MainCreate.hMenu = tmpl->hMenu;
	MainCreate.hCursor = WndClass.hCursor;
	MainCreate.hIcon = tmpl->hIcon;
	MainCreate.hHosting = host;
	MainCreate.MainWindowProc = ncsWndProc;
	MainCreate.lx = tmpl->x;
	MainCreate.ty = tmpl->y;
	MainCreate.rx = tmpl->x + tmpl->w;
	MainCreate.by = tmpl->y + tmpl->h;

	if(GetAValue(tmpl->bk_color) == 0)
		MainCreate.iBkColor = WndClass.iBkColor;
	else
		MainCreate.iBkColor = ncsColor2Pixel(HDC_SCREEN, tmpl->bk_color);
	MainCreate.dwAddData = (DWORD)&main_create_info;
	MainCreate.dwReserved = 0;

#if MINIGUI_MAJOR_VERSION >= 3
	hMain = CreateMainWindowEx(&MainCreate, \
		rdr_info?rdr_info->glb_rdr:NULL, \
		(const WINDOW_ELEMENT_ATTR*)((rdr_info && rdr_info->elements)?rdr_info->elements:NULL),
		NULL, NULL);
#else
	hMain = CreateMainWindow(&MainCreate);
#endif

	if(IsWindow(hMain))
	{
		mWidget *mainwnd =  (mWidget*)GetWindowAdditionalData2(hMain);
        if(mainwnd){
            _c(mainwnd)->setId(mainwnd, tmpl->id);
            if(tmpl->font_name){
                ncsSetFont (hMain, tmpl->font_name);
            }
            return mainwnd;
        }
	}

	return NULL;
}

/////////////////////////////////////////////////////
//main window
extern mWidgetClass* gpMainWndClass;

mWidgetClass* ncsGetMainWndClass(const char* className)
{
	WNDCLASS WndClass;
	if(className == NULL)
		return gpMainWndClass;

	WndClass.spClassName = (char*)className;
	WndClass.opMask = COP_ADDDATA;

	if(GetWindowClassInfo(&WndClass) && WndClass.dwAddData)
		return (mWidgetClass*)ncsClassFromMagicNum(WndClass.dwAddData, FALSE, NULL);

	return gpMainWndClass;
}

///////////////////////
void ncsNotifyParentEx(mWidget * self, int code, DWORD add_data)
{
	if(IsNotify(self))
	{
		int id = _c(self)->getId(self);
		HWND hwnd = self->hwnd;
		NOTIFPROC notif_proc = GetNotificationCallback (self->hwnd);

        SendNotifyMessage(GetParent(self->hwnd), MSG_COMMAND,
            (WPARAM)MAKELONG(id, code), (LPARAM)hwnd);

		if(notif_proc)
			(*notif_proc)(hwnd, id, code, add_data);

	}
}

/////////////

static HWND captured_window;
mObject * captured_hot;

void mWidget_captureHotPiece(mWidget *self, mObject *hotpice)
{
	if(!self || !hotpice)
		return;

	captured_window = self->hwnd;
	SetCapture(captured_window);
	captured_hot = hotpice;
}

mObject * mWidget_getCapturedHotPiece(mWidget *self)
{
	if(!self)
		return NULL;

	if(self->hwnd == captured_window)
		return captured_hot;

	return NULL;
}

void mWidget_releaseCapturedHotPiece()
{
	ReleaseCapture();
	captured_window = HWND_NULL;
	captured_hot = NULL;
}

/////
HWND hovering_hwnd;
mObject * hovering_piece;
void mWidget_setHoveringFocus(mWidget *self, mObject *hotpiece)
{
	if(!self || !hotpiece)
		return;

	hovering_hwnd = self->hwnd;
	hovering_piece = hotpiece;
}

mObject * mWidget_getHoveringFocus(mWidget *self)
{
	if(!self || self->hwnd != hovering_hwnd)
		return NULL;

	return hovering_piece;
}

void mWidget_releaseHoveringFocus()
{
	hovering_hwnd = HWND_NULL;
	hovering_piece = NULL;
}

/////
HWND input_hwnd;
mObject * input_piece;
void mWidget_setInputFocus(mWidget *self, mObject *hotpiece)
{
	if(!self || !hotpiece)
		return;

	input_hwnd = self->hwnd;
	input_piece = hotpiece;
}

mObject * mWidget_getInputFocus(mWidget *self)
{
	if(!self || self->hwnd != input_hwnd)
		return NULL;

	return input_piece;
}

void mWidget_releaseInputFocus()
{
	input_hwnd = HWND_NULL;
	input_piece = NULL;
}

void ncsDestroyWindow(mWidget *self, DWORD endCode)
{
    if (self) {
        //model dialog
        if (_c(self)->isMainObj(self) 
                && (GetWindowStyle(self->hwnd) & NCSS_MNWND_MODE))
            SendNotifyMessage(self->hwnd, 
                    MSG_MNWND_ENDDIALOG, 0, (LPARAM)endCode);
        else //main window and control 
            SendNotifyMessage(self->hwnd, MSG_NCDESTROY, 0, 0);
    }
}

#include "hashtable.h"

#if 1
#define MSGBIND_CALLOC(size, count)    slab_new_mem(NULL, (size)*(count), 0)
#define MSGBIND_FREE(p)                slab_free_mem(NULL, p)
#else
#define MSGBIND_CALLOC(size, count)    calloc(size, count)
#define MSGBIND_FREE(p)                free(p)
#endif

static HashTable* _message_bind_table = NULL;
#define MAX_MSGBIND_HWND  7

#ifdef _MGRM_THREADS
#include <pthread.h>
static pthread_mutex_t _msg_bind_lock;

#define MSGBIND_LOCK()   (pthread_mutex_lock(&_msg_bind_lock))
#define MSGBIND_UNLOCK() (pthread_mutex_unlock(&_msg_bind_lock))
#else
#define MSGBIND_LOCK()
#define MSGBIND_UNLOCK()
#endif

typedef struct _MsgBindInfo{
	HWND hwnd;
	int  message;
}MsgBindInfo;

static inline ht_key_t bind_info_to_key(HWND hwnd_listener, int message_id)
{
	return (ht_key_t)hwnd_listener + (ht_key_t)message_id;
}

static void free_msgbind_info(void* obj)
{
	MSGBIND_FREE(obj);
}

static MsgBindInfo* find_msgbind(HWND hwnd_listener, int message_id, BOOL bAutoCreate)
{
	MsgBindInfo * msgbind = NULL;
	ht_key_t key = bind_info_to_key(hwnd_listener, message_id);

	if(_message_bind_table == NULL)
		_message_bind_table = hash_new(MAX_MSGBIND_HWND, (freeObj)free_msgbind_info);

	if(!_message_bind_table)
		return NULL;
	
	msgbind = hash_get(_message_bind_table, key);

	if(!msgbind && bAutoCreate)
	{
		msgbind = (MsgBindInfo*)MSGBIND_CALLOC(sizeof(MsgBindInfo), 1);
		if(!msgbind)
			return NULL;
		msgbind->hwnd = hwnd_listener;
		msgbind->message = message_id;
		hash_insert(_message_bind_table, key, (void*)msgbind);
	}
	return msgbind;
}


static BOOL on_message_event(MsgBindInfo* bindInfo, mObject *sender, int eventId, DWORD param)
{
	void** __tmp ;
	MSGBIND_LOCK();
	__tmp = (void**)malloc(2*sizeof(void*));
	__tmp[0] = (void*)param;
	__tmp[1] = (void*)sender;
	SendNotifyMessage(bindInfo->hwnd, bindInfo->message, (WPARAM)eventId, (LPARAM)__tmp);
	MSGBIND_UNLOCK();
	return TRUE;
}

BOOL ncsBindEventsToMessage(mObject *sender, HWND hwnd_listener, int message, int* event_ids)
{
	BOOL bret = FALSE;
	MsgBindInfo * msgbind;
	if(!sender || message <= 0 || !IsWindow(hwnd_listener) || !event_ids)
		return FALSE;

	MSGBIND_LOCK();
	msgbind = find_msgbind(hwnd_listener, message, TRUE);
	if(msgbind)
	{
		bret = ncsAddEventListeners(sender, (mObject*)msgbind, (NCS_CB_ONOBJEVENT)on_message_event, event_ids);
	}
	MSGBIND_UNLOCK();

	return bret;
}

BOOL ncsBindEventToMessage(mObject* sender, HWND hwnd_listener, int message, int event_id)
{
	int event_ids[2] = {event_id, 0};
	return ncsBindEventsToMessage(sender, hwnd_listener, message, event_ids);
}


static int each_node_delete(HWND hwnd, MsgBindInfo* node)
{
	if(node && node->hwnd == hwnd)
	{
		ncsRemoveEventListener((mObject*)node);
		return HT_FOR_EACH_DELETE;
	}
	return 0;
}

void ncsRemoveBindWindow(HWND hwnd_listener)
{
	MSGBIND_LOCK();
	hash_for_each(_message_bind_table, (eachObj)each_node_delete, (void*)hwnd_listener);
	MSGBIND_UNLOCK();
}

void ncsRemoveBindMessage(HWND hwnd_listener, int message, mObject* sender/*=NULL*/,int event_id/*=0*/)
{
	MsgBindInfo * msgbind;
	MSGBIND_LOCK();
	msgbind = find_msgbind(hwnd_listener, message, FALSE);
	if(msgbind)
	{
		if(sender == NULL || event_id <= 0)
			ncsRemoveEventListener((mObject*)msgbind);
		else
			ncsRemoveEventConnect((mObject*)msgbind, sender, event_id);
	}
	MSGBIND_UNLOCK();
}

/* ============================================== */
void init_widget_info(void)
{
    _message_bind_table = NULL;

#ifdef _MGRM_THREADS
    pthread_mutex_init(&_msg_bind_lock, NULL);
#endif
}

void deinit_widget_info(void)
{
#ifdef _MGRM_THREADS
    pthread_mutex_destroy(&_msg_bind_lock);
#endif
}

