
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#define MARGIN_EDIT_LEFT        1
#define MARGIN_EDIT_TOP         1
#define MARGIN_EDIT_RIGHT       2
#define MARGIN_EDIT_BOTTOM      1

/* default line buffer size */
#define DEF_LINE_BUFFER_SIZE    128
/* default line step block size */
#define DEF_LINE_BLOCK_SIZE     32
/* default tip length */
#define DEF_TIP_LEN             63

#define check_caret() \
            if(self->selStart != self->selEnd) \
                HideCaret(self->hwnd); \
            else \
                ShowCaret(self->hwnd);

int GUIAPI GetLastMCharLen (PLOGFONT log_font, const char* mstr, int len);
#define CHLENPREV(pStr, pIns) \
        ( GetLastMCharLen (GetWindowFont(self->hwnd), (const char *)pStr, (const char *)pIns - (const char *)pStr) )

#define CHLENNEXT(pIns, len) \
        ( GetFirstMCharLen (GetWindowFont(self->hwnd), (const char *)pIns, len) )


#define shift_one_char_r(pos) \
        { \
            int len = CHLENNEXT((self->content.string + self->pos), (self->content.txtlen - self->pos)); \
            self->pos += len; \
        }

#define shift_one_char_l(pos) \
        { \
            int len = CHLENPREV((const char* )(self->content.string),(const char* )(self->content.string + self->pos) ); \
            self->pos -= len; \
        }

#define BEGIN_TEXT_PREPARE(self, dwStyle, text, len)  \
    if((dwStyle) & NCSS_SLEDIT_PASSWORD) { \
        (text) = FixStrAlloc(len+1); \
        memset((text), self->passwdChar, (len)); \
        if((len) > 0) \
            (text)[(len)] = 0; \
    }

#define END_TEXT_PREPARE(text, dwStyle) \
        if((dwStyle) & NCSS_SLEDIT_PASSWORD)  \
            FreeFixStr(text);

static inline int get_text_extent(mSlEdit* self, int len/*=-1*/, SIZE *psize)
{
    int ret;
    HDC hdc;
	DWORD dwStyle;
    char* string = (char*)self->content.string;
    hdc = GETDC(self);
    dwStyle  = GetWindowStyle(self->hwnd);
    if(len <= 0)
        len = self->content.txtlen;
    BEGIN_TEXT_PREPARE(self, dwStyle, string, len)
    ret = GetTextExtent(hdc, (const char*)string, len, psize);
    END_TEXT_PREPARE(string, dwStyle)
    RELEASEDC(self, hdc);
    return ret;
}

static inline int get_text_extentpoint(mSlEdit* self, int len/* =-1 */, int max_extent, SIZE *psize)
{
    int ret;
    HDC hdc;
	DWORD dwStyle;
    char* string = (char*)self->content.string;
    hdc = GETDC(self);
    dwStyle  = GetWindowStyle(self->hwnd);
    if(len <= 0)
        len = self->content.txtlen;
    BEGIN_TEXT_PREPARE(self, dwStyle, string, len)
    ret = GetTextExtentPoint(hdc, (const char*)string, len, max_extent, NULL, NULL, NULL, psize);
    END_TEXT_PREPARE(string, dwStyle)
    RELEASEDC(self, hdc);
    return ret;
}


static int sledit_settext (mSlEdit *self, const char *newtext)
{
    int len, txtlen;

    txtlen = strlen (newtext);
    len = (txtlen <= 0) ? self->nBlockSize : txtlen;

    if (self->hardLimit >= 0 && txtlen > self->hardLimit) {
        return -1;
    }

    /* free the old text */
    if (self->content.string)
        testr_free (&self->content);

    if (!testr_alloc (&self->content, len, self->nBlockSize))
        return -1;

    if (newtext && txtlen > 0)
        testr_setstr (&self->content, newtext, txtlen);
    else
        self->content.txtlen = 0;

    return 0;
}

static void setup_dc (mSlEdit *self, HDC hdc, BOOL bSel)
{
    DWORD dwStyle = GetWindowStyle (self->hwnd);
    if (!bSel) {
        SetBkMode (hdc, BM_TRANSPARENT);
        SetTextColor (hdc, ncsColor2Pixel(hdc, 
                    ncsGetElement(self, (dwStyle & WS_DISABLED) ? NCS_FGC_DISABLED_ITEM : NCS_FGC_WINDOW)));
        SetBkColor (hdc, GetWindowBkColor (self->hwnd));
    }
    else {
        SetBkMode (hdc, BM_OPAQUE);

        SetTextColor (hdc, ncsColor2Pixel(hdc, 
                    ncsGetElement(self, (dwStyle & WS_DISABLED) ? NCS_FGC_DISABLED_ITEM : NCS_FGC_SELECTED_ITEM)));

        if (self->status & NCST_SLEDIT_FOCUSED)
            SetBkColor (hdc, ncsColor2Pixel(hdc, ncsGetElement(self, NCS_BGC_SELECTED_ITEM)));
        else
            SetBkColor (hdc, ncsColor2Pixel(hdc, ncsGetElement(self, NCS_BGC_SELITEM_NOFOCUS)));
    }
}

static void update_view_rect(mSlEdit * self)
{
	int width, height;
	int font_width = 8;
	PLOGFONT logfont;
	FONTMETRICS fm;

	GetClientRect(self->hwnd, &self->rcVis);

	width = RECTW(self->rcVis);

	logfont = GetWindowFont(self->hwnd);
	GetFontMetrics(logfont, &fm);
	font_width = fm.max_width;

	if(width <= (self->leftMargin + self->rightMargin + font_width))
	{
		if(self->leftMargin < (width - font_width)/2)
		{
			self->rcVis.left += self->leftMargin;
			self->rcVis.right = self->rcVis.left + font_width ;
		}
		else {
			self->rcVis.right -= self->rightMargin;
			self->rcVis.left = self->rcVis.right - font_width ;
		}
	}
	else {
		self->rcVis.left += self->leftMargin;
		self->rcVis.right -= self->rightMargin;
	}

	height = RECTH(self->rcVis);
	self->rcVis.top += (height - fm.font_height) / 2;
	self->rcVis.bottom = self->rcVis.top + fm.font_height;

}



static void get_content_width (mSlEdit *self)
{
	update_view_rect(self);

    self->nVisW = self->rcVis.right - self->rcVis.left;
}

static void set_line_width (mSlEdit *self)
{
    SIZE txtsize;
    DWORD dwStyle = GetWindowStyle(self->hwnd);
    int old_w = self->nContW;

    get_text_extent(self, -1, &txtsize);

    if (txtsize.cx > self->nVisW)
        self->nContW = txtsize.cx;
    else
        self->nContW = self->nVisW;

    if (dwStyle & NCSS_EDIT_RIGHT) {
        self->nContX += (self->nContW - old_w);
    }
    else if (dwStyle & NCSS_EDIT_CENTER) {
        self->nContX += (self->nContW - old_w) >> 1;
    }

    if (self->nContX + self->nVisW > self->nContW) {
        self->nContX = self->nContW - self->nVisW;
    }
    else if (self->nContX < 0)
        self->nContX = 0;

    if (self->nContW > self->nVisW)
        self->startX = 0;
    else if (dwStyle & NCSS_EDIT_RIGHT) {
        self->startX = self->nVisW - txtsize.cx;
    }
    else if (dwStyle & NCSS_EDIT_CENTER) {
        self->startX = (self->nVisW - txtsize.cx) >> 1;
    }
    else {
        self->startX = 0;
    }
}

/*
 * set caret position according to the current edit position.
 */

static BOOL make_pos_visible (mSlEdit *self, int x)
{
    if (x - self->nContX > self->nVisW) {
        self->nContX = x - self->nVisW;
        return TRUE;
    }
    else if (x < self->nContX) {
        self->nContX = x;
        return TRUE;
    }
    return FALSE;
}

static BOOL make_charpos_visible (mSlEdit *self, int charPos, int *cx)
{
    SIZE txtsize;

    if (charPos <= 0)
        txtsize.cx = 0;
    else {
        get_text_extent(self, charPos, &txtsize); 
    }
    if (cx)
        *cx = self->startX + txtsize.cx;
    return make_pos_visible (self, self->startX + txtsize.cx);
}

static void calc_content_width(mSlEdit *self, int charPos, int *cx)
{
    SIZE txtsize;

    if (charPos <= 0)
        txtsize.cx = 0;
    else {
       get_text_extent(self, charPos, &txtsize); 
    }
    if (cx)
        *cx = txtsize.cx;

}

static void
calc_charpos_cx (mSlEdit *self, int charPos, int *cx)
{
    SIZE txtsize;

    if (charPos <= 0)
        txtsize.cx = 0;
    else {
       get_text_extent(self, charPos, &txtsize); 
    }
    if (cx)
        *cx = self->startX + txtsize.cx;

}

static inline BOOL sleContentToVisible (mSlEdit *self, int *x)
{
    if (x)
        *x -= self->nContX;
    else
        return -1;

    if (*x < 0 || *x > self->nVisW)
        return -1;
    return 0;
}

static inline BOOL sleVisibleToWindow (mSlEdit *self, int *x)
{
    if (x)
        *x += self->leftMargin;
    return TRUE;
}

static inline void sleContentToWindow (mSlEdit *self, int *x)
{
    sleContentToVisible (self, x);
    sleVisibleToWindow (self, x);
}

static inline void sleWindowToContent (mSlEdit *self, int *x)
{
    *x -= self->leftMargin;
    *x += self->nContX;
}

/* sets the current caret position in the virtual content window */
static void mySetCaretPos (mSlEdit *self, int x)
{
    if (sleContentToVisible(self, &x) < 0) {
        HideCaret (self->hwnd);
    }
    else {
        sleVisibleToWindow (self, &x);
        SetCaretPos (self->hwnd, x, self->startY);
        if (self->status & NCST_SLEDIT_FOCUSED) {
            ActiveCaret (self->hwnd);
            ShowCaret (self->hwnd);
        }
    }
}

static BOOL edtSetCaretPos (mSlEdit *self)
{
    BOOL bRefresh;
    int cx;

    bRefresh = make_charpos_visible (self, self->editPos, &cx);

    mySetCaretPos (self, cx);
    if (bRefresh) {
        InvalidateRect (self->hwnd, NULL, TRUE);
    }
    return bRefresh;
}

static void set_edit_caret_pos (mSlEdit *self)
{
    int cx;

    make_charpos_visible (self, self->editPos, &cx);
    mySetCaretPos (self, cx);
}

static void set_caret_pos (mSlEdit *self, int x, BOOL bSel)
{
    int out_chars;
    SIZE txtsize;


    sleWindowToContent (self, &x);

    if (x - self->startX <= 0) {
        out_chars = 0;
        txtsize.cx = 0;
    }
    else {
        out_chars = get_text_extentpoint(self, -1, x - self->startX, &txtsize);
    }
    if (!bSel) {
        self->editPos = out_chars;
        self->selStart = self->selEnd = self->editPos;
        mySetCaretPos (self, txtsize.cx + self->startX);
    }
    else {
        if (out_chars > self->editPos) {
            self->selStart = self->editPos;
            self->selEnd = out_chars;
        }
        else {
            self->selEnd = self->editPos;
            self->selStart = out_chars;
        }
    }

}

static void sledit_refresh_caret (mSlEdit *self, BOOL bInited)
{
    SIZE    txtsize;
    int     outchar;
    DWORD   dwStyle = GetWindowStyle(self->hwnd);

    outchar = get_text_extentpoint(self, -1, 0, &txtsize);

    if (bInited) {
        if (dwStyle & NCSS_EDIT_RIGHT)
            self->editPos = outchar;
        else if (dwStyle & NCSS_EDIT_CENTER)
            self->editPos = outchar/2;
        else
            self->editPos = 0;
    }
    if (!edtSetCaretPos (self)) {
        InvalidateRect (self->hwnd, NULL, TRUE);
    }
}

static void recalcSize (mSlEdit *self, BOOL bInited)
{
    get_content_width (self);

    self->nContX = 0;
    self->nContW = self->rcVis.right - self->rcVis.left;

    set_line_width (self);

    self->startY  = self->rcVis.top;

    sledit_refresh_caret (self, bInited);
}

static inline void edtChangeCont (mSlEdit *self)
{
    set_line_width (self);
}


static int get_caretpos_x(HWND hWnd)
{
    POINT pt;

    GetCaretPos(hWnd, &pt);

    return pt.x;
}

static int sleInsertText (mSlEdit *self, char *newtext, int inserting)
{
    int  deleted;
    unsigned char *pIns, *content = self->content.string;

    //delete the seleted
    if(self->selStart != self->selEnd) {
        deleted = self->selEnd - self->selStart;
        memmove (content + self->selStart, content + self->selEnd,
                        self->content.txtlen - self->selEnd);
        self->content.txtlen -= deleted;

        self->editPos = self->selStart;
        self->selEnd = self->selStart;
        ShowCaret(self->hwnd);

        if (!newtext)
            inserting = 0;
    }

    if ( !(self->status & NCST_SLEDIT_REPLACE) && inserting > 0
            && self->hardLimit >= 0
            && ((self->content.txtlen + inserting) > self->hardLimit)) {
        Ping ();
        ncsNotifyParent ((mWidget*)self, NCSN_EDIT_MAXTEXT);
        inserting = self->hardLimit - self->content.txtlen;
        return 0;
    }
    else if ( (self->status & NCST_SLEDIT_REPLACE) && inserting > 0
                && self->hardLimit >= 0
                && (self->editPos + inserting > self->hardLimit) ) {
        Ping ();
        ncsNotifyParent ((mWidget*)self, NCSN_EDIT_MAXTEXT);
        inserting = self->hardLimit - self->editPos;
    }

    if (inserting > 0) {
        content = testr_realloc (&self->content, self->content.txtlen +
                                  inserting);
        pIns = content + self->editPos;
        if ( !(self->status & NCST_SLEDIT_REPLACE) )
            memmove (pIns + inserting, pIns,
                    self->content.txtlen+1 - self->editPos);
        memcpy (pIns, newtext, inserting);
    }
    else if (inserting < 0) {
        pIns = content + self->editPos;
        memmove (pIns + inserting, pIns,
                self->content.txtlen+1 - self->editPos);
        content = testr_realloc (&self->content, self->content.txtlen +
                                 inserting);
    }

    if (inserting <= 0 || !(self->status & NCST_SLEDIT_REPLACE))
        self->content.txtlen += inserting;
    else {
        int add_len = inserting - self->content.txtlen + self->editPos;
        if (add_len > 0) {
            self->content.txtlen += add_len;
            self->content.string[self->content.txtlen] = '\0';
        }
    }
    self->editPos += inserting;
    self->selStart = self->selEnd = self->editPos;

    edtChangeCont (self);
    InvalidateRect (self->hwnd, NULL, TRUE);

    if (!edtSetCaretPos (self)) {
        InvalidateRect (self->hwnd, NULL, TRUE);
    }
    self->changed = TRUE;
    ncsNotifyParent ((mWidget*)self, NCSN_EDIT_CHANGE);

    return 0;
}

static int sleInsertText_refresh (mSlEdit *self, char *newtext, int inserting)
{
    int  deleted;
    unsigned char *pIns, *content = self->content.string;

    //delete the seleted
    if(self->selStart != self->selEnd) {
        deleted = self->selEnd - self->selStart;
        memmove (content + self->selStart, content + self->selEnd,
                        self->content.txtlen - self->selEnd);
        self->content.txtlen -= deleted;

        self->editPos = self->selStart;
        self->selEnd = self->selStart;
        ShowCaret(self->hwnd);
        if (!newtext)
            inserting = 0;
    }

    if ( !(self->status & NCST_SLEDIT_REPLACE) && inserting > 0 && self->hardLimit >= 0
                && ((self->content.txtlen + inserting) > self->hardLimit)) {
        Ping ();
        ncsNotifyParent ((mWidget*)self, NCSN_EDIT_MAXTEXT);
        inserting = self->hardLimit - self->content.txtlen;
        //return 0;
    }
    else if ( (self->status & NCST_SLEDIT_REPLACE) && inserting > 0
            && self->hardLimit >= 0
            && (self->editPos + inserting > self->hardLimit) ) {
        Ping ();
        ncsNotifyParent ((mWidget*)self, NCSN_EDIT_MAXTEXT);
        inserting = self->hardLimit - self->editPos;
    }

    if (inserting > 0) {
        content = testr_realloc (&self->content, self->content.txtlen +
                                  inserting);
        pIns = content + self->editPos;
        if ( !(self->status & NCST_SLEDIT_REPLACE) )
            memmove (pIns + inserting, pIns,
                    self->content.txtlen+1 - self->editPos);
        memcpy (pIns, newtext, inserting);
    }
    else if (inserting < 0) {
        pIns = content + self->editPos;
        memmove (pIns + inserting, pIns,
                self->content.txtlen+1 - self->editPos);
        content = testr_realloc (&self->content, self->content.txtlen +
                                 inserting);
    }

    if (inserting <= 0 || !(self->status & NCST_SLEDIT_REPLACE))
        self->content.txtlen += inserting;
    else {
        int add_len = inserting - self->content.txtlen + self->editPos;
        if (add_len > 0) {
            self->content.txtlen += add_len;
            self->content.string[self->content.txtlen] = '\0';
        }
    }
    self->editPos += inserting;
    self->selStart = self->selEnd = self->editPos;

    edtChangeCont (self);

    return 0;
}

static void esright_backspace_refresh(mSlEdit *self, int del)
{
    RECT scroll_rc, refresh_rc;
    int sel_start_x, sel_end_x;
    int old_caret_x, cur_caret_x;
    int scroll_len;
    int old_editpos_x, cur_editpos_x;

    scroll_rc.top = self->rcVis.top;
    scroll_rc.bottom = self->rcVis.bottom;

    if (self->nContW <= self->nVisW){
        if (self->selStart != self->selEnd){

            calc_charpos_cx(self, self->selStart, &sel_start_x);
            calc_charpos_cx(self, self->selEnd, &sel_end_x);
            if (self->selStart < self->selEnd){
                scroll_rc.right = sel_end_x + 1;
            }
            else{
                scroll_rc.right = sel_start_x + 1;
            }

            scroll_rc.left = self->rcVis.left;
            sleInsertText_refresh (self, NULL, del);

            scroll_len = abs(sel_start_x - sel_end_x);
            ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);
            edtSetCaretPos (self);
        }
        else{
            calc_charpos_cx(self, self->editPos, &old_editpos_x);
            scroll_rc.left = self->rcVis.left;
            calc_charpos_cx(self, self->editPos + del, &cur_editpos_x);

            sleInsertText_refresh (self, NULL, del);

            edtSetCaretPos (self);
            scroll_rc.right = old_editpos_x + 1;

            scroll_len = abs(cur_editpos_x - old_editpos_x);
            ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);

        }

    }
    else{

        if (self->nContX <= 0){//left scroll window while the head of text is in the self->rcVis
            if (self->selStart != self->selEnd){

                refresh_rc.top = self->rcVis.top;
                refresh_rc.bottom = self->rcVis.bottom;
                refresh_rc.right = self->rcVis.right;

                scroll_rc.right = self->rcVis.right;

                calc_charpos_cx(self, self->selStart, &sel_start_x);
                calc_charpos_cx(self, self->selEnd, &sel_end_x);
                if (self->selStart < self->selEnd){
                    scroll_rc.left = sel_start_x + 1;
                }
                else{
                    scroll_rc.left = sel_end_x + 1;
                }

                sleInsertText_refresh (self, NULL, del);
                edtSetCaretPos (self);

                scroll_len = abs(sel_start_x - sel_end_x);
                refresh_rc.left = refresh_rc.right - scroll_len;
                ScrollWindow(self->hwnd, -scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(self->hwnd, &refresh_rc, TRUE);

                if (self->nContW <= self->nVisW){
                    InvalidateRect(self->hwnd, NULL, TRUE);
                }

            }
            else{
                refresh_rc.top = self->rcVis.top;
                refresh_rc.bottom = self->rcVis.bottom;
                refresh_rc.right = self->rcVis.right;

                scroll_rc.right = self->rcVis.right;
                old_caret_x = get_caretpos_x(self->hwnd);


                sleInsertText_refresh (self, NULL, del);
                edtSetCaretPos (self);
                cur_caret_x = get_caretpos_x(self->hwnd);
                scroll_rc.left = cur_caret_x;

                scroll_len = abs(cur_caret_x - old_caret_x);
                refresh_rc.left = refresh_rc.right - scroll_len;
                ScrollWindow(self->hwnd, -scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(self->hwnd, &refresh_rc, TRUE);

                if (self->nContW <= self->nVisW){
                    InvalidateRect(self->hwnd, NULL, TRUE);
                }
            }

        }
        else{//right scroll window while self->nContX > 0
            if (self->selStart != self->selEnd){

                refresh_rc.top = self->rcVis.top;
                refresh_rc.bottom = self->rcVis.bottom;
                refresh_rc.left = self->rcVis.left;

                calc_charpos_cx(self, self->selStart, &sel_start_x);
                calc_charpos_cx(self, self->selEnd, &sel_end_x);
                if (self->selStart < self->selEnd){
                    scroll_rc.right = sel_end_x + 1 - self->nContX;
                }
                else{
                    scroll_rc.right = sel_start_x + 1 - self->nContX;
                }

                scroll_rc.left = self->rcVis.left;
                sleInsertText_refresh (self, NULL, del);

                scroll_len = abs(sel_start_x - sel_end_x);
                refresh_rc.right = refresh_rc.left + scroll_len;
                ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);
                edtSetCaretPos (self);
                InvalidateRect(self->hwnd, &refresh_rc, TRUE);

                if (self->nContW <= self->nVisW){
                    InvalidateRect(self->hwnd, NULL, TRUE);
                }

                if (self->nContX <= 0){
                    InvalidateRect(self->hwnd, NULL, TRUE);
                }
            }
            else{
                int old_ncontw;

                refresh_rc.top = self->rcVis.top;
                refresh_rc.bottom = self->rcVis.bottom;
                refresh_rc.left = self->rcVis.left;

                scroll_rc.left = self->rcVis.left;
                old_ncontw = self->nContW;
                sleInsertText_refresh (self, NULL, del);
                edtSetCaretPos (self);
                cur_caret_x = get_caretpos_x(self->hwnd);
                scroll_rc.right = cur_caret_x;

                scroll_len = abs(self->nContW - old_ncontw);
                refresh_rc.right = refresh_rc.left + abs(scroll_len);
                ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(self->hwnd, &refresh_rc, TRUE);

                if (self->nContW <= self->nVisW){
                    InvalidateRect(self->hwnd, NULL, TRUE);
                }

                if (self->nContX <= 0){
                    InvalidateRect(self->hwnd, NULL, TRUE);
                }
            }
        }
    }

    self->changed = TRUE;
    ncsNotifyParent ((mWidget*)self, NCSN_EDIT_CHANGE);
}

static void esright_del_refresh(mSlEdit *self, int del)
{
    RECT scroll_rc, refresh_rc;
    int sel_start_x, sel_end_x;
    int old_caret_x, cur_caret_x;
    int scroll_len;
    int old_editpos_x, cur_editpos_x;

    scroll_rc.top = self->rcVis.top;
    scroll_rc.bottom = self->rcVis.bottom;

    if (self->nContW <= self->nVisW){
        if (self->selStart != self->selEnd){

            calc_charpos_cx(self, self->selStart, &sel_start_x);
            calc_charpos_cx(self, self->selEnd, &sel_end_x);
            if (self->selStart < self->selEnd){
                scroll_rc.right = sel_end_x + 1;
            }
            else{
                scroll_rc.right = sel_start_x + 1;
            }

            scroll_rc.left = self->rcVis.left;
            sleInsertText_refresh (self, NULL, del);

            scroll_len = abs(sel_start_x - sel_end_x);
            ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);
            edtSetCaretPos (self);
        }
        else{
            old_caret_x = get_caretpos_x(self->hwnd);
            scroll_rc.left = self->rcVis.left;
            sleInsertText_refresh (self, NULL, del);
            edtSetCaretPos (self);
            cur_caret_x = get_caretpos_x(self->hwnd);
            scroll_rc.right = cur_caret_x;

            scroll_len = cur_caret_x - old_caret_x;
            ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);

        }

    }
    else{
        if (self->nContX <=0){//left scroll window while the head of text is in the self->rcVis
            if (self->selStart != self->selEnd){

                refresh_rc.top = self->rcVis.top;
                refresh_rc.bottom = self->rcVis.bottom;
                refresh_rc.right = self->rcVis.right;

                scroll_rc.right = self->rcVis.right;

                calc_charpos_cx(self, self->selStart, &sel_start_x);
                calc_charpos_cx(self, self->selEnd, &sel_end_x);
                if (self->selStart < self->selEnd){
                    scroll_rc.left = sel_start_x + 1;
                }
                else{
                    scroll_rc.left = sel_end_x + 1;
                }

                sleInsertText_refresh (self, NULL, del);
                edtSetCaretPos (self);

                scroll_len = abs(sel_start_x - sel_end_x);
                refresh_rc.left = refresh_rc.right - scroll_len;
                ScrollWindow(self->hwnd, -scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(self->hwnd, &refresh_rc, TRUE);

                if (self->nContW <= self->nVisW){
                    InvalidateRect(self->hwnd, NULL, TRUE);
                }

            }
            else{
                refresh_rc.top = self->rcVis.top;
                refresh_rc.bottom = self->rcVis.bottom;
                refresh_rc.right = self->rcVis.right;

                scroll_rc.right = self->rcVis.right;

                scroll_rc.left = get_caretpos_x(self->hwnd);
                calc_charpos_cx(self, self->editPos, &old_editpos_x);

                sleInsertText_refresh (self, NULL, del);
                calc_charpos_cx(self, self->editPos, &cur_editpos_x);
                edtSetCaretPos (self);

                scroll_len = old_editpos_x - cur_editpos_x;
                refresh_rc.left = refresh_rc.right - scroll_len;
                ScrollWindow(self->hwnd, -scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(self->hwnd, &refresh_rc, TRUE);

                if (self->nContW <= self->nVisW){
                    InvalidateRect(self->hwnd, NULL, TRUE);
                }
            }

        }
        else{//right scroll window while self->nContX > 0
            if (self->selStart != self->selEnd){

                refresh_rc.top = self->rcVis.top;
                refresh_rc.bottom = self->rcVis.bottom;
                refresh_rc.left = self->rcVis.left;

                calc_charpos_cx(self, self->selStart, &sel_start_x);
                calc_charpos_cx(self, self->selEnd, &sel_end_x);
                if (self->selStart < self->selEnd){
                    scroll_rc.right = sel_end_x + 1 - self->nContX;
                }
                else{
                    scroll_rc.right = sel_start_x + 1 - self->nContX;
                }

                scroll_rc.left = self->rcVis.left;
                sleInsertText_refresh (self, NULL, del);

                scroll_len = abs(sel_start_x - sel_end_x);
                refresh_rc.right = refresh_rc.left + scroll_len;
                ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);
                edtSetCaretPos (self);
                InvalidateRect(self->hwnd, &refresh_rc, TRUE);

                if (scroll_len >= self->nVisW){
                    InvalidateRect(self->hwnd, NULL, TRUE);
                }

                if (self->nContW <= self->nVisW){
                    InvalidateRect(self->hwnd, NULL, TRUE);
                }

                if (self->nContX <= 0){
                    InvalidateRect(self->hwnd, NULL, TRUE);
                }
            }
            else{
                refresh_rc.top = self->rcVis.top;
                refresh_rc.bottom = self->rcVis.bottom;
                refresh_rc.left = self->rcVis.left;

                old_caret_x = get_caretpos_x(self->hwnd);
                scroll_rc.left = self->rcVis.left;
                sleInsertText_refresh (self, NULL, del);
                edtSetCaretPos (self);
                cur_caret_x = get_caretpos_x(self->hwnd);
                scroll_rc.right = cur_caret_x;

                scroll_len = cur_caret_x - old_caret_x;
                refresh_rc.right = refresh_rc.left + abs(scroll_len);
                ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);
                InvalidateRect(self->hwnd, &refresh_rc, TRUE);

                if (self->nContW <= self->nVisW){
                    InvalidateRect(self->hwnd, NULL, TRUE);
                }

                if (self->nContX <= 0){
                    InvalidateRect(self->hwnd, NULL, TRUE);
                }
            }
        }
    }
    self->changed = TRUE;
    ncsNotifyParent ((mWidget*)self, NCSN_EDIT_CHANGE);
}

static void esleft_del_refresh(mSlEdit *self, int del)
{
    int txtlen_x;
    int scroll_len;
    int sel_start_x, sel_end_x;
    int old_caret_x, cur_caret_x;
    RECT scroll_rc, del_rc, refresh_rc;
    int old_nContX = self->nContX;

    scroll_rc.top = self->rcVis.top;
    scroll_rc.bottom = self->rcVis.bottom;
    del_rc.bottom = self->rcVis.bottom;
    del_rc.top = self->rcVis.top;
    refresh_rc.top = self->rcVis.top;
    refresh_rc.bottom = self->rcVis.bottom;

     if (self->selStart != self->selEnd){

         if (self->selStart < self->selEnd){
              calc_charpos_cx(self, self->selEnd, &sel_end_x);
              scroll_rc.left = sel_end_x;
         }
         else{
             calc_charpos_cx(self, self->selStart, &sel_start_x);
             scroll_rc.left = sel_start_x;
         }

         scroll_rc.right = self->rcVis.right;
        }
     else{
         calc_charpos_cx(self, self->editPos, &scroll_rc.left);
         scroll_rc.left = scroll_rc.left - old_nContX;
         scroll_rc.right = self->rcVis.right;
     }

     old_caret_x = get_caretpos_x(self->hwnd);
     calc_charpos_cx(self, self->content.txtlen, &txtlen_x);

     sleInsertText_refresh (self, NULL, del);

     calc_charpos_cx(self, self->editPos, &del_rc.left);
     del_rc.left = del_rc.left - old_nContX;
     del_rc.right = scroll_rc.left;

     scroll_len = del_rc.left - scroll_rc.left;

     if (abs(sel_start_x - sel_end_x) >= self->nVisW){
         InvalidateRect(self->hwnd, NULL, TRUE);
     }
     else{
         if ((old_nContX > 0) && ((txtlen_x - old_nContX) <= self->rcVis.right)){
             cur_caret_x = get_caretpos_x(self->hwnd);
             if (cur_caret_x != old_caret_x){
                 scroll_len = cur_caret_x - old_caret_x;
                 scroll_rc.left = self->rcVis.left;
                 scroll_rc.right = old_caret_x;

                 ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);

                 refresh_rc.left = old_caret_x;
                 refresh_rc.right = cur_caret_x;

                 InvalidateRect(self->hwnd, &refresh_rc, TRUE);
             }
             else if (self->selStart != self->selEnd) {
                 scroll_len = sel_start_x - sel_end_x;
                 scroll_rc.left = self->rcVis.left;
                 scroll_rc.right = old_caret_x;

                 ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);

                 refresh_rc.left = old_caret_x;
                 refresh_rc.right = cur_caret_x;

                 InvalidateRect(self->hwnd, &refresh_rc, TRUE);
             }
             else{
                 InvalidateRect(self->hwnd, NULL, TRUE);
             }
         }
         else{
             int rl;

             InvalidateRect(self->hwnd, &del_rc, TRUE);
             ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);
             calc_charpos_cx(self, self->content.txtlen, &rl);

             if (((self->nContW - self->nContX) == self->nVisW) && (old_nContX > 0)){
                 InvalidateRect(self->hwnd, NULL, TRUE);
             }
             else if (rl < self->nVisW){
                refresh_rc.left = rl;
                refresh_rc.right = self->rcVis.right;

                InvalidateRect(self->hwnd, &refresh_rc, TRUE);
             }
             else{
                 refresh_rc.left = self->rcVis.right - abs(scroll_len);
                 refresh_rc.right = self->rcVis.right;

                 InvalidateRect(self->hwnd, &refresh_rc, TRUE);
            }
        }
    }
}

static void esright_input_char_refresh (mSlEdit *self, char *charBuffer, int chars)
{
    RECT scroll_rc;
    int scroll_len;
    int cur_caret_x;
    int old_ncontw, cur_ncontw;

    if (self->selStart != self->selEnd) {
        sleInsertText (self, (char* )charBuffer, chars);
    }
    else {//left scroll window
        scroll_rc.top = self->rcVis.top;
        scroll_rc.bottom = self->rcVis.bottom;
        calc_content_width(self, self->content.txtlen, &old_ncontw);
        sleInsertText_refresh (self, (char* )charBuffer, chars);

        calc_content_width(self, self->content.txtlen, &cur_ncontw);
        scroll_len = abs(old_ncontw- cur_ncontw);
        cur_caret_x = get_caretpos_x(self->hwnd);
        scroll_rc.left = self->rcVis.left;
        scroll_rc.right = cur_caret_x;

        ScrollWindow(self->hwnd, -scroll_len, 0, &scroll_rc, NULL);
        UpdateWindow(self->hwnd, TRUE);
        edtSetCaretPos (self);

        self->changed = TRUE;
        ncsNotifyParent ((mWidget*)self, NCSN_EDIT_CHANGE);
    }
}

static void esleft_input_char_refresh (mSlEdit *self, char *charBuffer, int chars)
{
    int old_caretpos_x; //, cur_caretpos_x;
    int old_sel_start, old_sel_end;
    int old_sel_start_x, old_sel_end_x, cur_sel_start_x;
    RECT scroll_rc, refresh_rc;
    int scroll_len;
    int old_edit_pos_x, cur_edit_pos_x;
    int old_nContX; //, cur_ncontx;
    int char_size, sel_size;
    int txtlen_x;

    old_sel_start = self->selStart;
    old_sel_end = self->selEnd;

    old_caretpos_x = get_caretpos_x(self->hwnd);
    calc_charpos_cx(self, self->editPos, &old_edit_pos_x);
    calc_charpos_cx(self, self->content.txtlen, &txtlen_x);
    old_nContX= self->nContX;
    calc_charpos_cx(self, old_sel_start, &old_sel_start_x);
    calc_charpos_cx(self, old_sel_end, &old_sel_end_x);
    old_sel_start_x = old_sel_start_x - self->nContX;
    old_sel_end_x = old_sel_end_x - self->nContX;

    sleInsertText_refresh (self, (char* )charBuffer, chars);

    calc_charpos_cx(self, self->editPos, &cur_edit_pos_x);
    calc_charpos_cx(self, old_sel_start, &cur_sel_start_x);
    //cur_ncontx = self->nContX;
    //cur_caretpos_x = get_caretpos_x(self->hwnd);
    char_size = cur_edit_pos_x - cur_sel_start_x;
    sel_size = abs(old_sel_start_x - old_sel_end_x);

    scroll_rc.top = self->rcVis.top;
    scroll_rc.bottom = self->rcVis.bottom;
    refresh_rc.top = self->rcVis.top;
    refresh_rc.bottom = self->rcVis.bottom;

    if (char_size >= self->nVisW){
        InvalidateRect(self->hwnd, NULL, TRUE);
    }
    else{
        if (old_sel_start != old_sel_end){

            if (sel_size >= self->nVisW){
                InvalidateRect(self->hwnd, NULL, TRUE);
            }
            else{
                if (char_size < sel_size){
                    //right scroll window
                    if ((old_nContX > 0)
                            && ((txtlen_x - old_nContX) <= self->rcVis.right)) {
                        scroll_len = sel_size - char_size;
                        scroll_rc.left = self->rcVis.left;
                        scroll_rc.right = old_sel_start_x < old_sel_end_x
                                ? old_sel_start_x : old_sel_end_x;

                        ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);

                        refresh_rc.left = scroll_rc.right;
                        refresh_rc.right = refresh_rc.left + sel_size + scroll_len;

                        InvalidateRect(self->hwnd, &refresh_rc, TRUE);
                    }
                    else{//left scroll window
                        scroll_len = -(sel_size - char_size);
                        scroll_rc.left = old_sel_start_x < old_sel_end_x
                            ? old_sel_end_x : old_sel_start_x;
                        scroll_rc.right = self->rcVis.right;

                        ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);

                        refresh_rc.left = scroll_rc.left - sel_size;
                        refresh_rc.right = scroll_rc.left;

                        InvalidateRect(self->hwnd, &refresh_rc, TRUE);
                    }
                }
                else{//when char_size > sel_size right scroll window
                    scroll_len = char_size - sel_size;
                    scroll_rc.left = old_sel_start_x < old_sel_end_x
                        ? old_sel_end_x : old_sel_start_x;
                    scroll_rc.right = self->rcVis.right;

                    ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);

                    refresh_rc.left = scroll_rc.left - sel_size;
                    refresh_rc.right = refresh_rc.left + char_size + 1;

                    InvalidateRect(self->hwnd, &refresh_rc, TRUE);
                }
            }
        }
        else{
            int right;
            char_size = cur_edit_pos_x - old_edit_pos_x;
            calc_charpos_cx(self, self->content.txtlen, &right);
            if (old_caretpos_x < self->rcVis.right){//right scroll window
                scroll_len = cur_edit_pos_x - old_edit_pos_x;
                if ((self->rcVis.right - old_caretpos_x) < scroll_len){//
                    scroll_len = -(scroll_len - (self->rcVis.right- old_caretpos_x));
                    scroll_rc.left = self->rcVis.left;
                    scroll_rc.right = old_caretpos_x;
                    ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);

                    refresh_rc.left = self->rcVis.right - char_size;
                    refresh_rc.right = self->rcVis.right;

                    InvalidateRect(self->hwnd, &refresh_rc, TRUE);
                }
                else{
                    scroll_rc.left = old_caretpos_x;
                    scroll_rc.right = (right < self->rcVis.right)
                        ? right+1 : self->rcVis.right;
                    ScrollWindow(self->hwnd, scroll_len, 0, &scroll_rc, NULL);

                    refresh_rc.left = self->rcVis.right - char_size;
                    refresh_rc.right = self->rcVis.right;

                    InvalidateRect(self->hwnd, &refresh_rc, TRUE);
                }
            }
            else{//left scroll window
                scroll_len = old_edit_pos_x - cur_edit_pos_x;
                ScrollWindow(self->hwnd, scroll_len, 0, &self->rcVis, NULL);

                refresh_rc.left = self->rcVis.right - char_size;
                refresh_rc.right = self->rcVis.right;

                InvalidateRect(self->hwnd, &refresh_rc, TRUE);
            }

            if (((self->nContW - self->nContX) == self->nVisW)
                    && (old_nContX > 0)){
                InvalidateRect(self->hwnd, NULL, TRUE);
            }
        }
    }
}

static int sleSetSel (mSlEdit *self, int sel_start, int sel_end)
{
    if (self->content.txtlen <= 0)
        return -1;

    if (sel_start < 0)
        sel_start = 0;
    if (sel_end < 0)
        sel_end = self->content.txtlen;
    if (sel_start == sel_end)
        return -1;

    self->selStart = sel_start;
    self->selEnd = sel_end;
    HideCaret(self->hwnd);
    InvalidateRect(self->hwnd, NULL, TRUE);

    return self->selEnd - self->selStart;
}


///////////////////////////////////////////
//////////////// copy & paste /////////////
///////////////////////////////////////////

static void defAddCopyInfo (const char *str,
        int len,  TextAttribute *atrr, void *user_info)
{
    SetClipBoardData (CBNAME_TEXT,
                    (void *)str, len, CBOP_NORMAL);
}

static void defEndCopyInfo (void)
{
    //TODO
}

static BOOL defIsEmpty (void)
{
    return (0 == GetClipBoardDataLen (CBNAME_TEXT));
}

static PasteInfo *defGetPasteInfo (void)
{
    int len;
    PasteInfo *pi = NULL;

    if ((len = GetClipBoardDataLen (CBNAME_TEXT)) > 0)
    {
        pi = (PasteInfo *)calloc (1, sizeof(PasteInfo));
        pi->len = len;
        pi->str = (char *)malloc (len * sizeof(char) + 1);
        GetClipBoardData (CBNAME_TEXT, pi->str, pi->len);
        pi->attr = NULL;
        pi->user_info = NULL;
        pi->next = NULL;
    }

    return pi;
}

static TextCopyPaste defTextCopyPaste =
{
    defAddCopyInfo,
    defEndCopyInfo,
    defIsEmpty,
    defGetPasteInfo
};
////////////////////////////////////////////////



static void mSlEdit_construct (mSlEdit *self, DWORD addData)
{
    unsigned int i;
    char *caption;
    DWORD dwStyle;

    g_stmEditCls.construct((mEdit *)self, addData);

    self->status       = 0;
    self->editPos      = 0;
    self->selStart     = 0;
    self->selEnd       = 0;
    self->leftMargin   = MARGIN_EDIT_LEFT;
    self->topMargin    = MARGIN_EDIT_TOP;
    self->rightMargin  = MARGIN_EDIT_RIGHT;
    self->bottomMargin = MARGIN_EDIT_BOTTOM;
    self->nBlockSize   = DEF_LINE_BLOCK_SIZE;
    self->content.string = NULL;
    self->content.buffsize = 0;
    self->content.txtlen = 0;
    self->hardLimit    = -1;
    self->passwdChar   = '*';
    self->changed      = FALSE;

    dwStyle = GetWindowStyle(self->hwnd);

    if (dwStyle & NCSS_SLEDIT_TIP) {
        self->tipText = FixStrAlloc (DEF_TIP_LEN + 1);
        self->tipText[0] = '\0';
    }
    else
        self->tipText = NULL;
    
    caption = strdup(GetWindowCaption(self->hwnd));

    if (dwStyle & NCSS_EDIT_UPPERCASE) {
        for (i = 0; i < strlen(caption); i ++)
            caption[i] = toupper (caption[i]);
    } else if (dwStyle & NCSS_EDIT_LOWERCASE) {
        for (i = 0; i < strlen(caption); i ++)
            caption[i] = tolower (caption[i]);
    }

    sledit_settext (self, caption);

    if (dwStyle & NCSS_SLEDIT_PASSWORD) {
        SetWindowFont(self->hwnd, GetSystemFont (SYSLOGFONT_DEFAULT));
    }

    CreateCaret (self->hwnd, NULL, 1, (GetWindowFont(self->hwnd))->size);
    get_content_width (self);

    self->cp = &defTextCopyPaste;

    free(caption);
}

static void mSlEdit_destroy (mSlEdit *self)
{
    DestroyCaret (self->hwnd);

    if ((GetWindowStyle(self->hwnd) & NCSS_SLEDIT_TIP) && self->tipText) {
        FreeFixStr (self->tipText);
    }
	if(self->tipText)
		FreeFixStr(self->tipText);

    if (self->content.string) {
        testr_free (&self->content);
    }
    Class(mEdit).destroy((mEdit*)self);
}

static BOOL mSlEdit_setProperty (mSlEdit *self, int id, DWORD value)
{
    if(id >= NCSP_SLEDIT_MAX)
        return FALSE;

    switch (id)
    {
        case NCSP_SLEDIT_TIPTEXT:
            {
                int len;
                char *tip = (char *)value;

                if (!(GetWindowStyle(self->hwnd) & NCSS_SLEDIT_TIP) || !tip)
                    return FALSE;

				if(!tip)
					len = 0;
				else
					len = strlen(tip);
				if(self->tipText)
                {
                    FreeFixStr (self->tipText);
                }
				if(len > 0)
				{
					self->tipText = FixStrAlloc(len+1);
					strcpy (self->tipText, tip);
				}
				else
					self->tipText = NULL;

                if (self->content.txtlen <= 0)
                    InvalidateRect (self->hwnd, NULL, TRUE);

                return TRUE;
            }
        case NCSP_SLEDIT_PWDCHAR:
            {
                if (self->passwdChar != (int)value) {
                    self->passwdChar = (int)value;
                    if (GetWindowStyle(self->hwnd) & NCSS_SLEDIT_PASSWORD) {
                        InvalidateRect (self->hwnd, NULL, TRUE);
                    }
                }
                return TRUE;
            }
        case NCSP_EDIT_LIMITTEXT:
            {
                if ((int)value > 0 && self->content.txtlen > (int)value) {
                    Ping ();
                    return FALSE;
                }
                else {
					if((int)value <= 0)
						self->hardLimit = -1;
					else
						self->hardLimit = (int)value;
                    return TRUE;
                }
            }
        case NCSP_EDIT_CARETPOS:
            {
                self->editPos = (int)value;
                edtSetCaretPos (self);
                return TRUE;
            }
        default:
            break;
    }

	return Class(mEdit).setProperty ((mEdit *)self, id, value);
}

static DWORD mSlEdit_getProperty (mSlEdit *self, int id)
{
    if(id >= NCSP_SLEDIT_MAX)
        return FALSE;

    switch (id)
    {
        case NCSP_SLEDIT_TIPTEXT:
            {
                if (!(GetWindowStyle(self->hwnd) & NCSS_SLEDIT_TIP))
                    return (DWORD)NULL;

                return (DWORD)self->tipText;
            }
        case NCSP_SLEDIT_PWDCHAR:
            {
                return (DWORD)self->passwdChar;
            }
        case NCSP_EDIT_LIMITTEXT:
            {
                return (DWORD)self->hardLimit;
            }
        case NCSP_EDIT_CARETPOS:
            {
                return (DWORD)self->editPos;
            }
		case NCSP_WIDGET_TEXT:
			{
				self->content.string[self->content.txtlen] = 0;
				return (DWORD)self->content.string;
			}
        default:
            break;
    }

	return Class(mEdit).getProperty ((mEdit *)self, id);
}

static int mSlEdit_wndProc (mSlEdit *self, int message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case MSG_FONTCHANGING:
        {
            if (GetWindowStyle(self->hwnd) & NCSS_SLEDIT_PASSWORD)
                return -1;
            return 0;
        }
        case MSG_FONTCHANGED:
        {
            PLOGFONT pLogFont = GetWindowFont(self->hwnd);
            DestroyCaret (self->hwnd);
            CreateCaret (self->hwnd, NULL, 1, pLogFont->size);

            recalcSize (self, FALSE);
			InvalidateRect(self->hwnd, NULL, TRUE);

            return 0;
        }
        case MSG_CHAR:
        {
            if (!(_c(self)->onChar (self, wParam, lParam)))
                return 0;

            break;
        }
        case MSG_KILLFOCUS:
        {
            BOOL refresh = FALSE;
            DWORD dwStyle = GetWindowStyle(self->hwnd);

            if (self->status & NCST_SLEDIT_FOCUSED)
            {
                self->status &= ~NCST_SLEDIT_FOCUSED;
                HideCaret (self->hwnd);
                if ( self->selStart != self->selEnd
                        || ((dwStyle & NCSS_SLEDIT_TIP) && self->tipText!=NULL  && self->content.txtlen <= 0)) {
                    refresh = TRUE;
                }
                if (self->selStart != self->selEnd && !(dwStyle & NCSS_EDIT_NOHIDESEL))
                    self->selStart = self->selEnd = self->editPos;

                if (refresh)
                    InvalidateRect (self->hwnd, NULL, TRUE);

                //ncsNotifyParent (self, EN_KILLFOCUS);

                if (self->changed){
                    self->changed = FALSE;
                    ncsNotifyParent ((mWidget*)self, NCSN_EDIT_CONTCHANGED);
                }
            }

            break;
        }
        case MSG_SETFOCUS:
        {
            DWORD dwStyle = GetWindowStyle(self->hwnd);

            if (self->status & NCST_SLEDIT_FOCUSED)
                break;

            self->status |= NCST_SLEDIT_FOCUSED;

            ActiveCaret (self->hwnd);
            ShowCaret(self->hwnd);

            if (dwStyle & NCSS_SLEDIT_AUTOSELECT) {
                if (sleSetSel (self, 0, self->content.txtlen) <= 0
                        && (dwStyle & NCSS_SLEDIT_TIP) && self->tipText!=NULL)
                    InvalidateRect (self->hwnd, NULL, TRUE);
                if (lParam == 1)
                    self->status |= NCST_SLEDIT_TMP;
            }
            else if ( (dwStyle & NCSS_EDIT_NOHIDESEL && self->selStart != self->selEnd)
                    || ((dwStyle & NCSS_SLEDIT_TIP) && self->tipText!=NULL && self->content.txtlen <= 0)) {
                InvalidateRect (self->hwnd, NULL, TRUE);
            }

            //ncsNotifyParent ((mWidget*)self, EN_SETFOCUS);
            break;
        }
        case MSG_ENABLE:
        {
            DWORD dwStyle = GetWindowStyle(self->hwnd);

            if ((dwStyle & WS_DISABLED) && wParam)
                ExcludeWindowStyle(self->hwnd, WS_DISABLED);
            else if (!(dwStyle & WS_DISABLED) && !wParam)
                IncludeWindowStyle(self->hwnd, WS_DISABLED);
            else
                return 0;

            InvalidateRect (self->hwnd, NULL, TRUE);
            return 0;
        }
        case MSG_GETDLGCODE:
        {
            return DLGC_WANTCHARS| DLGC_HASSETSEL| DLGC_WANTARROWS| DLGC_WANTENTER;
        }
    }

	return Class(mEdit).wndProc((mEdit*)self, message, wParam, lParam);
}

static int mSlEdit_onSizeChanged (mSlEdit *self, RECT *rcClient)
{
    recalcSize (self, FALSE);
    return 0;
}

static void mSlEdit_onPaint (mSlEdit *self, HDC hdc, const PCLIPRGN pclip)
{
    int outw = 0;
    int startX;
    int startY = self->startY;
    char *dispBuffer, *passwdBuffer = NULL;
    StrBuffer *content = &self->content;
    DWORD   dwStyle = GetWindowStyle(self->hwnd);

    if ((dwStyle & NCSS_SLEDIT_TIP) && self->tipText && content->txtlen <= 0 &&
            GetFocus(GetParent(self->hwnd)) != self->hwnd)
    {
        //setup_dc (self, hdc, FALSE);
        SetBkMode (hdc, BM_TRANSPARENT);
        SetTextColor (hdc, ncsColor2Pixel(hdc, 
                    ncsGetElement(self, NCS_FGC_DISABLED_ITEM)));
        TextOut (hdc, self->leftMargin, startY, self->tipText);
		goto RETURN;
    }

    if (dwStyle & NCSS_SLEDIT_PASSWORD) {
        dispBuffer = FixStrAlloc (content->txtlen + 1);
        memset (dispBuffer, self->passwdChar, content->txtlen);
        passwdBuffer = dispBuffer;
        if(content->txtlen > 0)
            dispBuffer[content->txtlen] = 0;
    }
    else {
        dispBuffer = (char*)content->string;
    }

    if (dwStyle & NCSS_EDIT_BASELINE) {
        SetPenColor (hdc, ncsColor2Pixel(hdc, ncsGetElement(self, NCS_FGC_WINDOW)));
        DrawHDotLine (hdc, self->rcVis.left, self->rcVis.bottom,
                    self->rcVis.right - self->rcVis.left);
    }

    ClipRectIntersect (hdc, &self->rcVis);

    if (self->selStart != self->selEnd) {//select chars
        startX = self->startX;
        sleContentToWindow (self, &startX);
        /* draw first normal chars */
        if (self->selStart > 0) {
            setup_dc (self, hdc, FALSE);
            outw += TextOutLen (hdc, startX, startY, dispBuffer, self->selStart);
            dispBuffer += self->selStart;
        }

        /* draw selected chars, selected number should be (selEnd - selStart + 1) */
        setup_dc (self, hdc, TRUE);
        outw += TextOutLen (hdc, startX + outw, startY, dispBuffer, self->selEnd - self->selStart);

        /* draw others */
        if (self->selEnd < content->txtlen) {
            dispBuffer += self->selEnd - self->selStart;
            setup_dc (self, hdc, FALSE);
            outw += TextOutLen (hdc, startX + outw, startY, dispBuffer,
                    content->txtlen - self->selEnd);
        }
    }
    else {
        setup_dc (self, hdc, FALSE);
        startX = self->startX;
        sleContentToWindow (self, &startX);
        outw += TextOutLen (hdc, startX, startY, dispBuffer, content->txtlen);
    }

RETURN:
    if (dwStyle & NCSS_SLEDIT_PASSWORD && passwdBuffer)
        FreeFixStr (passwdBuffer);
}

static void mSlEdit_onChar (mSlEdit *self, WPARAM wParam, LPARAM lParam)
{
    int chars;
    unsigned char charBuffer [3];
    DWORD dwStyle = GetWindowStyle(self->hwnd);

    if (wParam == 127) // BS
        wParam = '\b';

    if (dwStyle & NCSS_EDIT_READONLY)
        return ;

    charBuffer [0] = LOBYTE (wParam);
    charBuffer [1] = HIBYTE (wParam);
    charBuffer [2] = (0x00ff0000 & wParam) >> 16;

    if (charBuffer [2]) {
        chars = 3;
    }
    else if (HIBYTE (wParam)) {
        chars = 2;
    }
    else {
        chars = 1;

        if (dwStyle & NCSS_EDIT_UPPERCASE) {
            charBuffer [0] = toupper (charBuffer[0]);
        }
        else if (dwStyle & NCSS_EDIT_LOWERCASE) {
            charBuffer [0] = tolower (charBuffer[0]);
        }
    }

    if (chars == 1) {
        if (charBuffer [0] < 0x20 && charBuffer[0] != '\b') //FIXME
            return ;
    }

    if (wParam == '\b') { //backspace
        int del;

        if (self->editPos == 0 && self->selStart == self->selEnd)
            return ;
        del = -CHLENPREV((const char *)(self->content.string),
                (self->content.string + self->editPos) );

        if (dwStyle & NCSS_EDIT_LEFT) {
            esleft_del_refresh(self, del);
            set_edit_caret_pos(self);

            self->changed = TRUE;
            ncsNotifyParent ((mWidget*)self, NCSN_EDIT_CHANGE);
        }
        else if (dwStyle & NCSS_EDIT_RIGHT) {
            esright_backspace_refresh(self, del);
        }
        else if (dwStyle & NCSS_EDIT_CENTER) {
            sleInsertText (self, NULL, del);
        }
        else {
            sleInsertText (self, NULL, del);
        }
    }
    else{
        if (dwStyle & NCSS_EDIT_LEFT) {
            esleft_input_char_refresh(self, (char*)charBuffer, chars);
            set_edit_caret_pos(self);

            self->changed = TRUE;
            ncsNotifyParent ((mWidget*)self, NCSN_EDIT_CHANGE);
        }
        else if (dwStyle & NCSS_EDIT_RIGHT) {
            esright_input_char_refresh(self, (char*)charBuffer, chars);
        }
        else if (dwStyle & NCSS_EDIT_CENTER) {
            sleInsertText (self, (char* )charBuffer, chars);

        }
        else {
            sleInsertText (self, (char* )charBuffer, chars);
        }
    }
    return ;
}

static int mSlEdit_onLButtonDown (mSlEdit *self, int x, int y, DWORD key_flag)
{
    if (self->status & NCST_SLEDIT_TMP) {
        self->status &= ~NCST_SLEDIT_TMP;
        return 0;
    }

    if (y < self->rcVis.top || y > self->rcVis.bottom)
        return 0;

    set_caret_pos (self, x, FALSE);

    ActiveCaret (self->hwnd);
    ShowCaret(self->hwnd);
    SetCapture(self->hwnd);

    InvalidateRect(self->hwnd, NULL, TRUE);

    return 0;
}

static int mSlEdit_onLButtonUp (mSlEdit *self, int x, int y, DWORD key_flag)
{
    if (GetCapture() == self->hwnd)
        ReleaseCapture();
    //NotifyParent (hWnd,  GetDlgCtrlID(hWnd), EN_CLICKED);

    if (self->selStart != self->selEnd)
        ncsNotifyParent ((mWidget*)self, NCSN_EDIT_SELCHANGED);

    return 0;
}

static int mSlEdit_onKeyDown(mSlEdit* self, int scancode, DWORD key_flags)
{
    BOOL    bChange = FALSE;
    DWORD   dwStyle = GetWindowStyle (self->hwnd);

    switch (scancode)
    {
        case SCANCODE_KEYPADENTER:
        case SCANCODE_ENTER:
            ncsNotifyParent ((mWidget*)self, NCSN_SLEDIT_ENTER);
            return 0;

        case SCANCODE_HOME:
            {
                BOOL refresh = FALSE;

                if(key_flags & KS_SHIFT) {
                    make_pos_visible (self, self->startX);
                    self->selStart = 0;
                    self->selEnd = self->editPos;
                    check_caret();
                    InvalidateRect (self->hwnd, &self->rcVis, TRUE);

                    ncsNotifyParent ((mWidget*)self, NCSN_EDIT_SELCHANGED);

                    return 0;
                }

                if (self->selStart != self->selEnd) {
                    ShowCaret(self->hwnd);
                    refresh = TRUE;
                }

                self->editPos  = 0;
                self->selStart = self->selEnd = 0;

                if (!edtSetCaretPos (self) && refresh)
                    InvalidateRect (self->hwnd, &self->rcVis, TRUE);

                return 0;
            }

        case SCANCODE_END:
            {
                BOOL refresh = FALSE;

                if(key_flags & KS_SHIFT) {
                    self->selStart = self->editPos;
                    self->selEnd = self->content.txtlen;
                    check_caret();
                    make_charpos_visible (self, self->selEnd, NULL);
                    InvalidateRect(self->hwnd, NULL, TRUE);

                    ncsNotifyParent ((mWidget*)self, NCSN_EDIT_SELCHANGED);

                    return 0;
                }

                if (self->selStart != self->selEnd) {
                    ShowCaret(self->hwnd);
                    refresh = TRUE;
                }

                self->editPos = self->content.txtlen;
                self->selStart = self->selEnd = self->editPos;

                if (!edtSetCaretPos (self) && refresh)
                    InvalidateRect (self->hwnd, NULL, TRUE);

                return 0;
            }

        case SCANCODE_CURSORBLOCKLEFT:
            {
                BOOL refresh = FALSE;

                if(key_flags & KS_SHIFT) {
                    if (self->selStart == self->selEnd) {
                        self->selStart = self->selEnd = self->editPos;
                    }

                    if (self->selStart < self->editPos ||
                            self->selStart == self->selEnd) {
                        if (self->selStart > 0)
                            shift_one_char_l(selStart);
                    }
                    else {
                        shift_one_char_l(selEnd);
                    }

                    check_caret ();
                    make_charpos_visible (self, self->selStart, NULL);
                    InvalidateRect (self->hwnd, NULL, TRUE);

                    ncsNotifyParent ((mWidget*)self, NCSN_EDIT_SELCHANGED);
                    return 0;
                }

                if (self->editPos > 0) {
                    shift_one_char_l (editPos);
                }

                if (self->selStart != self->selEnd) {
                    ShowCaret (self->hwnd);
                    ActiveCaret (self->hwnd);
                    refresh = TRUE;
                }

                self->selStart = self->selEnd = self->editPos;
                if (!edtSetCaretPos (self) && refresh) {
                    InvalidateRect (self->hwnd, NULL, TRUE);
                }

                return 0;
            }

        case SCANCODE_CURSORBLOCKRIGHT:
            {
                BOOL refresh = FALSE;

                if(key_flags & KS_SHIFT) {
                    if(self->selStart == self->selEnd)
                        self->selStart = self->selEnd = self->editPos;

                    if (self->selStart == self->selEnd ||
                            self->selEnd > self->editPos) {
                        if (self->selEnd < self->content.txtlen)
                            shift_one_char_r (selEnd);
                    }
                    else {
                        shift_one_char_r (selStart);
                    }

                    check_caret ();
                    make_charpos_visible (self, self->selEnd, NULL);
                    InvalidateRect (self->hwnd, NULL, TRUE);
                    ncsNotifyParent ((mWidget*)self, NCSN_EDIT_SELCHANGED);
                    return 0;
                }

                if (self->editPos < self->content.txtlen) {
                    shift_one_char_r (editPos);
                }

                if (self->selStart != self->selEnd) {
                    ShowCaret (self->hwnd);
                    ActiveCaret (self->hwnd);
                    refresh = TRUE;
                }

                self->selStart = self->selEnd = self->editPos;
                if (!edtSetCaretPos (self) && refresh)
                    InvalidateRect (self->hwnd, NULL, TRUE);
            }
            return 0;

        case SCANCODE_REMOVE:
            {
                int del;
                int oldpos = self->editPos;

                if ((dwStyle & NCSS_EDIT_READONLY))
                    return 0;
                if (self->editPos == self->content.txtlen
                        && self->selStart == self->selEnd)
                    return 0;

                if (self->selStart == self->selEnd &&
                        self->editPos < self->content.txtlen) {
                    shift_one_char_r(editPos);
                }

                del = oldpos - self->editPos;
                if (dwStyle & NCSS_EDIT_LEFT) {
                    esleft_del_refresh(self, del);

                    if (!edtSetCaretPos (self)) {
                        InvalidateRect (self->hwnd, NULL, TRUE);
                    }

                    self->changed = TRUE;
                    ncsNotifyParent ((mWidget*)self, NCSN_EDIT_CHANGE);
                }
                else if (dwStyle & NCSS_EDIT_RIGHT) {
                    esright_del_refresh(self, del);
                }
                else if (dwStyle & NCSS_EDIT_CENTER) {
                    sleInsertText (self, NULL, del);
                }
                else {
                    sleInsertText (self, NULL, del);
                }

            }
            break;

        case SCANCODE_A:
            {
                if (key_flags & KS_CTRL) {
                    sleSetSel (self, 0, self->content.txtlen);
                }
                return 0;
            }

        case SCANCODE_C:
            {
                if (key_flags & KS_CTRL)
                    _c(self)->copy(self);
                return 0;
            }
        case SCANCODE_X:
            {
                if (key_flags & KS_CTRL)
                    _c(self)->cut(self);
                return 0;
            }
        case SCANCODE_V:
            {
                if (key_flags & KS_CTRL)
                    _c(self)->paste(self);
                return 0;
            }
        case SCANCODE_INSERT:
            self->status ^= NCST_SLEDIT_REPLACE;
            break;

        default:
            break;
    } //end switch

    if (bChange){
        self->changed = TRUE;
        ncsNotifyParent ((mWidget*)self, NCSN_EDIT_CHANGE);
    }

    return 0;
}

static int mSlEdit_onMouseMove(mSlEdit* self, int x, int y, DWORD key_flags)
{
    RECT rcClient;
    BOOL refresh = TRUE;

    if (GetCapture () != self->hwnd)
        return 0;

    ScreenToClient(self->hwnd, &x, &y);
    GetClientRect(self->hwnd, &rcClient);

    if(x >= 0 && x < rcClient.right
            && y >= 0 && y < rcClient.bottom) {//in edit window
        set_caret_pos (self, x, TRUE);
        if (self->editPos == self->selStart)
            make_charpos_visible (self, self->selEnd, NULL);
        else
            make_charpos_visible (self, self->selStart, NULL);
    }
    else if (y < 0) {
        self->nContX = 0;
        self->selStart = 0;
        self->selEnd = self->editPos;
    }
    else if(y >= rcClient.bottom) {
        self->selStart = self->editPos;
        self->selEnd = self->content.txtlen;
        self->nContX = self->nContW - (self->rcVis.right - self->rcVis.left);
    }
    else if (x < 0) {
        if (self->selEnd == self->content.txtlen) {
            set_caret_pos (self, 0, TRUE);
            make_charpos_visible (self, self->selStart, NULL);
            goto quit;
        }

        if (self->selStart == 0)
            return 0;

        shift_one_char_l (selStart);
        refresh = make_charpos_visible (self, self->selStart, NULL);
    }
    else if (x >= rcClient.right) {
        if (self->selStart == 0) {
            set_caret_pos (self, rcClient.right - 1, TRUE);
        }

        if (self->selEnd == self->content.txtlen)
            return 0;

        shift_one_char_r (selEnd);
        refresh = make_charpos_visible (self, self->selEnd, NULL);
    }

quit:
    check_caret();

    if (refresh)
        InvalidateRect(self->hwnd, NULL, TRUE);

    return 0;
}

static int mSlEdit_setContent (mSlEdit *self, const char* str, int start, int len)
{
    int i;
    char* buffer;
    DWORD dwStyle;
	int   slen = 0;

    if (!str)
	{
		_c(self)->resetContent(self);
        return 0;
	}

	slen = strlen(str + start);

    if (len == -1 || len > slen)
    {
		len = slen;
        buffer = strdup(str + start);
    } else {
        buffer = (char *)calloc(len + 1 ,sizeof(char));
        strncpy(buffer, str + start, len);
		buffer[len] = 0;
    }

    dwStyle = GetWindowStyle(self->hwnd);

    if (dwStyle & NCSS_EDIT_UPPERCASE) {
        for (i = 0; i < strlen(buffer); i ++)
            buffer[i] = toupper (buffer[i]);
    } else if (dwStyle & NCSS_EDIT_LOWERCASE) {
        for (i = 0; i < strlen(buffer); i ++)
            buffer[i] = tolower (buffer[i]);
    }

    self->selStart = 0;
    self->selEnd = 0;

    len = sledit_settext (self, buffer);
    recalcSize (self, TRUE);

    free (buffer);

    self->changed = TRUE;
    ncsNotifyParent ((mWidget*)self, NCSN_EDIT_UPDATE);

    return len < 0 ? 0 : len;
}

static void mSlEdit_replaceText(mSlEdit *self, const char* str,
        int start, int len, int replaceStart, int replaceEnd)
{
    int i;
    char* buffer;
    DWORD dwStyle;

    if (!str || len == 0)
        return;

    if (replaceStart < 0 || replaceStart > self->content.txtlen)
        replaceStart = self->content.txtlen;

    if (replaceEnd < 0 || replaceEnd > self->content.txtlen)
        replaceEnd = self->content.txtlen;

    if (replaceEnd < replaceStart)
        return;

    dwStyle = GetWindowStyle(self->hwnd);
/*
    if (dwStyle & NCSS_EDIT_READONLY)
        return;
*/
    if (len == -1 || len > strlen(str)-start)
    {
        buffer = strdup(str + start);
    } else {
        buffer = (char *)calloc(len + 1 ,sizeof(char));
        strncpy(buffer, str + start, len);
    }

    if (dwStyle & NCSS_EDIT_UPPERCASE) {
        for (i = 0; i < strlen(buffer); i ++)
            buffer[i] = toupper (buffer[i]);
    } else if (dwStyle & NCSS_EDIT_LOWERCASE) {
        for (i = 0; i < strlen(buffer); i ++)
            buffer[i] = tolower (buffer[i]);
    }

    self->status &= NCST_SLEDIT_REPLACE;
    self->editPos = replaceStart;
    self->selStart = replaceStart;
    self->selEnd = replaceEnd;

    sleInsertText (self, (char* )buffer, strlen(buffer));

	free(buffer);
    self->changed = TRUE;
    ncsNotifyParent ((mWidget*)self, NCSN_EDIT_UPDATE);

    return;
}

static void mSlEdit_insert(mSlEdit *self, const char* str, int start, int len, int insertStart)
{
    _c(self)->replaceText(self, str, start, len, insertStart, insertStart);
}

static void mSlEdit_append(mSlEdit *self, const char* str, int start, int len)
{
    _c(self)->replaceText(self, str, start, len, -1, -1);
}

static int mSlEdit_getTextLength(mSlEdit *self)
{
    return self->content.txtlen;
}

static int mSlEdit_getContent(mSlEdit *self, char *strBuff, int bufLen, int start, int end)
{
    int len;

    if (end < 0 || end > self->content.txtlen)
        end = self->content.txtlen;

    if(start < 0 || start > self->content.txtlen || start > end)
        return -1;

	if(strBuff == NULL || bufLen <=0)
		return end-start;

    len = MIN ((int)bufLen, end - start);
    memcpy (strBuff, self->content.string + start, len);
    strBuff [len] = '\0';

    return len;
}

static int mSlEdit_setTextAttr(mSlEdit *self, int start, int end, TextAttribute *attr)
{
    //TODO
    return 0;
}

static TextAttribute* mSlEdit_getTextAttr(mSlEdit *self, int start, int end, int *pEnd)
{
    //TODO
    return NULL;
}

static int mSlEdit_setSel(mSlEdit *self, int start, int end)
{
    if (start < 0 || start > self->content.txtlen)
        start = self->content.txtlen;

    if (end < 0 || end > self->content.txtlen)
        end = self->content.txtlen;

    if (start > end)
        return -1;

    return sleSetSel (self, start, end);
}

static int mSlEdit_getSel(mSlEdit *self, int *pStart, int *pEnd)
{
    *pStart = self->selStart;
    *pEnd = self->selEnd;
    return 0;
}

static RECT *mSlEdit_setMargin(mSlEdit *self, int left, int top, int right, int bottom)
{
    self->leftMargin   = left;
    self->topMargin    = top;
    self->rightMargin  = right;
    self->bottomMargin = bottom;

	recalcSize(self, TRUE);

    InvalidateRect(self->hwnd, NULL, TRUE);
    return 0;
}

static void mSlEdit_copy (mSlEdit *self)
{
    self->cp->addCopyInfo((const char*)(self->content.string + self->selStart),
            self->selEnd - self->selStart, NULL, NULL);
}

static void mSlEdit_cut (mSlEdit *self)
{
    if(!(GetWindowStyle(self->hwnd) & NCSS_EDIT_READONLY)) {
        self->cp->addCopyInfo((const char*)(self->content.string + self->selStart),
                self->selEnd - self->selStart, NULL, NULL);
        sleInsertText (self, NULL, 0);
    }
}

static void mSlEdit_paste (mSlEdit *self)
{
    PasteInfo *pi, *pre;

    if (GetWindowStyle(self->hwnd) & NCSS_EDIT_READONLY)
        return;

    pi =self->cp->getPasteInfo();

    //FIXME, the default copyPaste have no list....
    while (pi)
    {
        if (pi->len > 0 && pi->str)
        {
            sleInsertText (self, (char *)pi->str, pi->len);
            free(pi->str);
        }
        if (pi->attr)
            free(pi->attr);
        if (pi->user_info)
            free(pi->user_info);
        pre = pi;
        pi = pi->next;
        free(pre);
    }
}

static TextCopyPaste *mSlEdit_setCopyPaste(mSlEdit *self, TextCopyPaste *cp)
{
    TextCopyPaste *old = self->cp;

    if(cp != NULL)
        self->cp = cp;

    return old;
}

static void mSlEdit_undo (mSlEdit *self)
{
    //TODO
}

static void mSlEdit_redo (mSlEdit *self)
{
    //TODO
}

static TextUndoRedoInfo *mSlEdit_setUndoRedo(mSlEdit *self, TextUndoRedoInfo *ur)
{
    //TODO
    return NULL;
}

static void mSlEdit_makevisible (mSlEdit *self, int pos)
{
	BOOL bRefresh;
	int  cx;
	if(pos < 0)
		pos = 0;
	else if(pos > self->content.txtlen)
		pos = self->content.txtlen;
    self->selStart = self->selEnd = self->editPos = pos;

	bRefresh = make_charpos_visible(self, pos, &cx);

	if(pos < self->content.txtlen && cx >= self->rcVis.right - 1)
	{
		make_charpos_visible(self, pos + 1, &cx);
		bRefresh = TRUE;
	}

	mySetCaretPos(self, cx);
	if(bRefresh)
	    InvalidateRect (self->hwnd, NULL, TRUE);
}

static void mSlEdit_resetContent(mSlEdit *self)
{
    self->selStart = 0;
    self->selEnd = 0;

    sledit_settext (self, "");
    recalcSize (self, TRUE);

    self->changed = TRUE;
    ncsNotifyParent ((mWidget*)self, NCSN_EDIT_UPDATE);

    return _c(self)->super->resetContent((void*)self);
}


#ifdef _MGNCS_GUIBUILDER_SUPPORT
static BOOL mSlEdit_refresh(mSlEdit *self)
{
	DWORD dwStyle = GetWindowStyle(self->hwnd);
	if(dwStyle & (NCSS_EDIT_UPPERCASE|NCSS_EDIT_LOWERCASE))
	{
		int i;
		if(dwStyle&NCSS_EDIT_UPPERCASE)
		{
			for(i=0; i<self->content.txtlen; i++)
				self->content.string[i] = toupper(self->content.string[i]);
		}
		else
		{
			for(i=0; i<self->content.txtlen; i++)
				self->content.string[i] = tolower(self->content.string[i]);
		}
	}
	return TRUE;
}
#endif


BEGIN_CMPT_CLASS (mSlEdit, mEdit)
    CLASS_METHOD_MAP (mSlEdit, construct)
    CLASS_METHOD_MAP (mSlEdit, destroy)
    CLASS_METHOD_MAP (mSlEdit, setProperty)
    CLASS_METHOD_MAP (mSlEdit, getProperty)
    CLASS_METHOD_MAP (mSlEdit, wndProc)
    CLASS_METHOD_MAP (mSlEdit, onSizeChanged)
    CLASS_METHOD_MAP (mSlEdit, onPaint)
    CLASS_METHOD_MAP (mSlEdit, onChar)
    CLASS_METHOD_MAP (mSlEdit, onLButtonDown)
    CLASS_METHOD_MAP (mSlEdit, onLButtonUp)
    CLASS_METHOD_MAP (mSlEdit, onMouseMove)
    CLASS_METHOD_MAP (mSlEdit, onKeyDown)
    CLASS_METHOD_MAP (mSlEdit, setContent)
    CLASS_METHOD_MAP (mSlEdit, replaceText)
    CLASS_METHOD_MAP (mSlEdit, insert)
    CLASS_METHOD_MAP (mSlEdit, append)
    CLASS_METHOD_MAP (mSlEdit, getTextLength)
    CLASS_METHOD_MAP (mSlEdit, getContent)
    CLASS_METHOD_MAP (mSlEdit, setTextAttr)
    CLASS_METHOD_MAP (mSlEdit, getTextAttr)
    CLASS_METHOD_MAP (mSlEdit, setSel)
    CLASS_METHOD_MAP (mSlEdit, getSel)
    CLASS_METHOD_MAP (mSlEdit, setMargin)
    CLASS_METHOD_MAP (mSlEdit, copy)
    CLASS_METHOD_MAP (mSlEdit, cut)
    CLASS_METHOD_MAP (mSlEdit, paste)
    CLASS_METHOD_MAP (mSlEdit, setCopyPaste)
    CLASS_METHOD_MAP (mSlEdit, undo)
    CLASS_METHOD_MAP (mSlEdit, redo)
    CLASS_METHOD_MAP (mSlEdit, setUndoRedo)
    CLASS_METHOD_MAP (mSlEdit, makevisible)
    CLASS_METHOD_MAP (mSlEdit, resetContent)
#ifdef _MGNCS_GUIBUILDER_SUPPORT
    CLASS_METHOD_MAP (mSlEdit, refresh)
#endif
END_CMPT_CLASS
