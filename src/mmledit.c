
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mdblist.h"
#include "mitem.h"
#include "mscroll_widget.h"
#include "mitem_manager.h"
#include "mitemview.h"
#include "mscrollview.h"

#include "medit.h"

#ifdef _MGNCS_OLD_MLEDIT

#include "mmledit.h"

#include "mrdr.h"

/* default base line height */
#define DEF_LINE_BASE_H         ( (GetWindowStyle(self->hwnd) & NCSS_EDIT_BASELINE) ? 2 : 0 )
/* default line above height */
#define DEF_LINE_ABOVE_H        4
/* minimal line above height */
#define MIN_LINE_ABOVE_H        1
/* default line height */
#define DEF_LINE_HEIGHT         ( GetWindowFont(self->hwnd)->size + DEF_LINE_BASE_H + \
                                 DEF_LINE_ABOVE_H )

/* default line buffer size */
#define DEF_LINE_BUFFER_SIZE    128
/* default line step block size */
#define DEF_LINE_BLOCK_SIZE     32

/* default line seperator '\n' */
#define DEF_LINESEP             '\n'

#define CH_RN                   "\r\n"

/* default title length */
#define DEF_TITLE_LEN           63

/* draw text format */
#define TE_FORMAT  (DT_LEFT | DT_TOP | DT_NOCLIP | DT_EXPANDTABS)

typedef GHANDLE HSVITEM;

#define BE_WRAP(hWnd)     (GetWindowStyle(hWnd) & NCSS_MLEDIT_AUTOWRAP)

#define myGetCaretPos(pt)  \
            ( (pt)->x = self->caretX, \
              (pt)->y = self->caretY )

#define myGetSelPos(pt) \
            ( (pt)->x = self->selX, \
              (pt)->y = self->selY )

#define mySetSelPos(x, y) \
        ( self->selX = x >= 0 ? x : self->selX, \
          self->selY = y >= 0 ? y : self->selY )


#define BE_FIRST_NODE(node) \
           (node->list.prev == &txtDoc->queue)

#define BE_LAST_NODE(node) \
           (node->list.next == &txtDoc->queue)

#define FIRSTNODE(txtDoc) \
           (list_entry((txtDoc)->queue.next, TextNode, list))
#define LASTNODE(txtDoc) \
           (list_entry((txtDoc)->queue.prev, TextNode, list))

#define NODE_LINENR(node) \
        (_c(self)->getItemHeight(self, (HSVITEM)(node)->addData) / self->nLineHeight )

#define NODE_HEIGHT(node) \
        (_c(self)->getItemHeight(self, (HSVITEM)(node)->addData))

#define NODE_INDEX(node) \
        (_c(self)->indexOf(self, (HSVITEM)(node)->addData))

#define TXTNODE_NEXT(node) \
            ( ((node)->list.next == &txtDoc->queue) ? NULL : \
                list_entry((node)->list.next, TextNode, list) )

#define TXTNODE_PREV(node) \
            ( ((node)->list.prev == &txtDoc->queue) ? NULL : \
                list_entry((node)->list.prev, TextNode, list) )


#define NODE_IS_SELECTED(node) \
            _c(self)->isSelected (self, (HSVITEM)node->addData)

#define SELECT_NODE(node, bsel) \
            bsel ? _c(self)->select (self, (HSVITEM)(node)->addData) \
                    : _c(self)->deselect (self, (HSVITEM)(node)->addData)

#define is_rn_sep(pIns) (txtDoc->lnsep == '\n' && *(pIns) == '\r' && *(pIns+1) == '\n')

#define check_caret() \
    if (txtDoc->selection.curNode) { \
        if (txtDoc->selection.curNode == txtDoc->insert.curNode && \
                txtDoc->selection.pos_lnOff == txtDoc->insert.pos_lnOff) { \
            ShowCaret (self->hwnd); \
            txtDoc->selection.curNode = NULL; \
        } \
        else \
        HideCaret (self->hwnd); \
    } \
    else \
        ShowCaret (self->hwnd);

#define REFRESH_NODE_EX(node, rcInv) \
            _c(self)->refreshItem(self, \
                    (HSVITEM)(node)->addData, rcInv)

#define REFRESH_NODE(node) \
            _c(self)->refreshItem(self, (HSVITEM)(node)->addData, NULL)

#define textdoc_insert_string(self, txtDoc, newtext, len)  \
             textdoc_insert_string_ex_2(self, txtDoc, NULL, 0, newtext, len)

#define GETMARK(cond)     ( (cond) ? (&txtDoc->selection) : \
        (&txtDoc->insert) )

#define SETITEMY(cond, item_y)    \
    if (cond) \
        self->selItemY = item_y; \
    else \
        self->curItemY = item_y;

int GUIAPI GetLastMCharLen (PLOGFONT log_font, const char* mstr, int len);
#define CHLENPREV(pStr, pIns) \
        ( GetLastMCharLen (GetWindowFont(self->hwnd), \
              (const char *)pStr, (const char *)pIns - (const char *)pStr) )

#define CHLENNEXT(pIns, len) \
        ( GetFirstMCharLen (GetWindowFont(self->hwnd), (const char *)pIns, len) )


/* -------------------------------------------------------------------------- */

static inline TextNode* textnode_alloc (void)
{
    return malloc (sizeof(TextNode));
}

static inline void textnode_free (TextNode * node)
{
    free (node);
}

static inline int teGetLineIndent (mMlEdit *self, TextNode *node)
{
    if (node && node->list.prev == &self->txtDoc.queue) {
        return self->titleIndent;
    }
    return 0;
}

static int textdoc_insert_string_ex (mMlEdit *self, TextDoc *txtDoc,
        TextNode *enode, int insert_pos, const char* newtext, int len);

static inline int textdoc_insert_string_ex_2 (mMlEdit *self,
        TextDoc *txtDoc, TextNode *enode, int insert_pos, const char* newtext, int len)
{
    TextNode *node=NULL;
    BOOL bDel;
    int ret;

    bDel = (txtDoc->selection.curNode && !enode) ? TRUE : FALSE;

    ret = textdoc_insert_string_ex (self, txtDoc, enode, insert_pos, newtext, len);

    if (!enode)
        txtDoc->insert.pos_lnOff = ret;

    node = enode ? enode : txtDoc->insert.curNode;

    if (txtDoc->change_cont && node) {
        txtDoc->change_cont (txtDoc, node);
    }

    return !bDel;
}

/* calculates line number of a text node, for wrap mode */
static int get_line_nr (mMlEdit *self, TextNode *node, int indent)
{
    HDC hdc;
    RECT rc;
    PLOGFONT logfont = GetWindowFont(self->hwnd);
    int height;

    if (!node) {
        return 0;
    }

    if (!BE_WRAP(self->hwnd))
        return 1;
    hdc = GETDC(self);

    SelectFont (hdc, logfont);

    height = logfont->size + GetTextAboveLineExtra(hdc) + GetTextBellowLineExtra(hdc);
    rc.left = 0;
    rc.top = 0;
    rc.right = self->visWidth;
    rc.bottom = height;

    if (self->lnChar) {
        char *content, chln = 0;
        int outlen;
        content = (char*)node->content.string;
        outlen = node->content.txtlen;

        if (outlen > 0 && content[outlen-1] == self->txtDoc.lnsep) {
            outlen --;
            if (self->txtDoc.lnsep == '\n' && content[outlen-1] == '\r')
                outlen --;
            outlen ++;
            chln = content[outlen-1];
            content[outlen-1] = self->lnChar;
        }
        DrawTextEx (hdc, content, outlen, &rc, indent,
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT);
        if (chln)
            content[outlen-1] = chln;
    }
    else {
        DrawTextEx (hdc, (const char*)(node->content.string), node->content.txtlen, &rc, indent,
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT);
    }

    RELEASEDC(self, hdc);

    return (RECTH(rc)/height);
}

static int recalc_max_len (mMlEdit *self)
{
    HDC hdc;
    list_t *me;
    SIZE txtsize;
    TextNode *node;
    TextDoc *txtDoc = &self->txtDoc;

    self->maxLen = 0;
    self->maxNode = NULL;
    self->secLen = 0;
    self->secNode = NULL;

    hdc = GETDC(self);

    list_for_each (me, &txtDoc->queue) {
        node = list_entry (me, TextNode, list);
        GetTabbedTextExtent(hdc, (const char*)(node->content.string),
                        node->content.txtlen, &txtsize);
        if (txtsize.cx > self->maxLen || self->maxLen == 0) {
            self->maxLen = txtsize.cx;
            self->maxNode = node;
        }
        else if (txtsize.cx >= self->secLen) {
            self->secLen = txtsize.cx;
            self->secNode = node;
        }
    }

    RELEASEDC(self, hdc);

    return 0;
}

static int revise_max_len (mMlEdit *self, TextNode *node, int textlen)
{
    if (textlen > self->maxLen) {
        if (node != self->maxNode) {
            self->secLen = self->maxLen;
            self->secNode = self->maxNode;
        }
        self->maxLen = textlen;
        self->maxNode = node;
    } else if (textlen > self->secLen) {
        if (node != self->maxNode) {
            self->secLen = textlen;
            self->secNode = node;
            return 0;
        } else {
            self->maxLen = textlen;
        }
    } else {
        if (node == self->maxNode) {
            recalc_max_len (self);
            return 1;
        } else if (node == self->secNode) {
            recalc_max_len (self);
            return 0;
        }
        return 0;
    }

    return 1;
}

/* set text node line width, for non wrap mode */
static int set_line_width (mMlEdit *self, TextNode *node, int indent)
{
    HDC hdc;
    SIZE txtsize = {0, 0};

    if (!node || BE_WRAP(self->hwnd))
        return -1;

    hdc = GETDC(self);
    SelectFont (hdc, GetWindowFont(self->hwnd));
    GetTabbedTextExtent(hdc, (const char*)(node->content.string),
                        node->content.txtlen, &txtsize);
    RELEASEDC(self, hdc);

    if (revise_max_len (self, node, txtsize.cx + indent) > 0) {
        _c(self)->setProperty (self, NCSP_SWGT_CONTWIDTH, self->maxLen + 1);
    }

    return 0;
}

static int getCaretWidth (mMlEdit *self)
{
    if (self->caretShape == NCS_CARETSHAPE_LINE) {
        return 1;
    } else if (self->caretShape == NCS_CARETSHAPE_BLOCK) {
        SIZE chsize = {0, 0};
        TextDoc *txtDoc = &self->txtDoc;
        TextNode *node = txtDoc->insert.curNode;
        char *pIns = ( char*)(node->content.string) + txtDoc->insert.pos_lnOff;
        int chlen = CHLENNEXT(pIns, node->content.txtlen - txtDoc->insert.pos_lnOff);
        int width = GetWindowFont(self->hwnd)->size;

        HDC hdc = GETDC(self);
        GetTextExtent (hdc, pIns, chlen, &chsize);
        RELEASEDC(self, hdc);

        if (chsize.cx > 0)
            width = chsize.cx;

        return width;
    }

    return 1;
}

/* sets the current caret position in the virtual content window */
static void mySetCaretPos (mMlEdit *self, int x, int y)
{
    if (x >= 0)
       self->caretX = x;
    else
       x = self->caretX;

    if (y >= 0)
       self->caretY = y;
    else
       y = self->caretY;

    _c(self)->contentToViewport (self, &x, &y);
    _c(self)->viewportToWindow (self, &x, &y);
    SetCaretPos (self->hwnd, x, y);

    ChangeCaretSize (self->hwnd, getCaretWidth(self),
            self->nLineHeight - self->nLineAboveH- self->nLineBaseH);

    if ( (self->flags & NCSF_MLEDIT_FOCUSED) &&
             !(self->flags & NCSF_MLEDIT_NOCARET)
             && !(self->txtDoc.selection.curNode)) {
        ActiveCaret (self->hwnd);
        ShowCaret (self->hwnd);
    }
    return;
}

static void teNodeInit (TextDoc *txtDoc, TextNode* node, TextNode *prenode)
{
    HSVITEM preitem = 0;
    int linenr = 1, indent;
    mMlEdit *self = (mMlEdit *)txtDoc->fn_data;

    if (!node)
        return;

    indent = teGetLineIndent (self, node);

    linenr = get_line_nr(self, node, indent);
    set_line_width (self, node, indent);

    if (prenode)
        preitem = (HSVITEM)prenode->addData;

    node->addData = (DWORD) _c(self)->createItem (self, preitem,
            0, -1, self->nLineHeight *linenr, NULL, (DWORD)node, NULL, TRUE);
}

static void teNodeDel (TextDoc *txtDoc, TextNode *node)
{
    mMlEdit *self = (mMlEdit *)txtDoc->fn_data;
    HSVITEM hsvi = (HSVITEM) node->addData;

    node->addData = 0;
    _c(self)->removeItem (self, hsvi);
}

static void teNodeChange (TextDoc *txtDoc, BOOL bSel)
{
    mMlEdit *self = (mMlEdit *)txtDoc->fn_data;

    if (!bSel) {
        self->curItemY = _c(self->itemList)->getItemYPos(self->itemList,
                                     (HSVITEM)txtDoc->insert.curNode->addData);
    } else {
        self->selItemY = _c(self->itemList)->getItemYPos(self->itemList,
                                     (HSVITEM)txtDoc->selection.curNode->addData);
    }
}

static void teChangeCont (TextDoc *txtDoc, TextNode *node)
{
    int indent, linenr;
    mMlEdit *self = (mMlEdit *)txtDoc->fn_data;

    if (!node)
        return;

    indent = teGetLineIndent (self, node);

    linenr = get_line_nr(self, node, indent);
    _c(self)->setItemHeight (self, (HSVITEM)node->addData,
                        self->nLineHeight*linenr);
    set_line_width (self, node, indent);

    return;
}

/*
 * textdoc_reset : resets or initializes the properties of a textdoc to
 *                 default values
 */
static int textdoc_reset (mMlEdit *self, TextDoc *txtDoc)
{
    txtDoc->lnsep = DEF_LINESEP;
    txtDoc->nDefLineSize = DEF_LINE_BUFFER_SIZE;
    txtDoc->nBlockSize = DEF_LINE_BLOCK_SIZE;

    txtDoc->init_fn = teNodeInit;
    txtDoc->change_fn = teNodeChange;
    txtDoc->change_cont = teChangeCont;
    txtDoc->del_fn = teNodeDel;
    txtDoc->fn_data = (void*)self;

    return 0;
}

/*
 * teResetData : resets status data
 */
static void teResetData (mMlEdit *self)
{
    self->desCaretX = 0;

    self->curItemY = 0;
    self->selItemY = 0;

    self->maxNode = NULL;
    self->maxLen = 0;
    self->secNode = NULL;
    self->secLen = 0;

    self->flags = 0;
}

static BOOL textnode_is_selected (mMlEdit *self, TextDoc *txtDoc, TextNode *node)
{
    if (!txtDoc->selection.curNode)
        return FALSE;

    return _c(self)->isSelected (self, (HSVITEM)node->addData);
}

static TextMark* get_start_mark (mMlEdit *self)
{
    TextDoc *txtDoc = &self->txtDoc;

    if (self->curItemY < self->selItemY
            || (self->curItemY == self->selItemY
                && txtDoc->insert.pos_lnOff < txtDoc->selection.pos_lnOff) )
        return &txtDoc->insert;
    else
        return &txtDoc->selection;
}

/* Gets the start and end selection points in a text node */
static void get_selection_points (mMlEdit *self,
        TextNode *node, int *pos_start, int *pos_end)
{
    TextDoc *txtDoc = &self->txtDoc;
    TextMark *markStart = get_start_mark (self);
    TextMark *markEnd = (markStart == &txtDoc->insert) ?
                          &txtDoc->selection : &txtDoc->insert;

    if (node == txtDoc->insert.curNode || node == txtDoc->selection.curNode) {
        if (txtDoc->insert.curNode == txtDoc->selection.curNode) {
            *pos_start = markStart->pos_lnOff;
            *pos_end = markEnd->pos_lnOff;
        } else if (node == markStart->curNode) {
            *pos_start = markStart->pos_lnOff;
            *pos_end = node->content.txtlen;
        } else {
            *pos_start = 0;
            *pos_end = markEnd->pos_lnOff;
        }
    } else {
        *pos_start = 0;
        *pos_end = node->content.txtlen;
    }
}

static void setup_dc (mMlEdit *self, HDC hdc, BOOL bSel)
{
    DWORD dwStyle = GetWindowStyle (self->hwnd);

    if (!bSel) {
        SetBkMode (hdc, BM_TRANSPARENT);
        SetBkColor (hdc, GetWindowBkColor (self->hwnd));

        SetTextColor (hdc, ncsColor2Pixel(hdc,
                    ncsGetElement(self, (dwStyle & WS_DISABLED) ? NCS_FGC_DISABLED_ITEM : NCS_FGC_WINDOW)));
    } else {
        SetBkMode (hdc, BM_OPAQUE);

        SetTextColor (hdc, ncsColor2Pixel(hdc,
                    ncsGetElement(self, (dwStyle & WS_DISABLED) ? NCS_FGC_DISABLED_ITEM : NCS_FGC_SELECTED_ITEM)));

        if (self->hwnd == GetFocus(GetParent(self->hwnd))) {
            SetBkColor (hdc, ncsColor2Pixel(hdc, ncsGetElement(self, NCS_BGC_SELECTED_ITEM)));
        } else {
            SetBkColor (hdc, ncsColor2Pixel(hdc, ncsGetElement(self, NCS_BGC_SELITEM_NOFOCUS)));
        }
    }
}

/*
 * teDrawItem : draw text node/item, including multi lines in wrap mode
 */
static void teDrawItem (mItemView *self, HSVITEM hsvi, HDC hdc, RECT *rcDraw)
{
    UINT format;
    char *content;
    mMlEdit *me_self = (mMlEdit*)self;
    TextNode *node;
    TextDoc *txtDoc;
    DTFIRSTLINE fl;
    unsigned char chln = 0;
    RECT rcTxt = *rcDraw;
    int txtlen, outlen, indent;
    int i, pos_start, pos_end, outchars, line_nr, outw, selout = 0;

    txtDoc = &me_self->txtDoc;

    node = (TextNode*)_c(me_self)->getAddData(me_self, hsvi);
    if (!node)
        return;

    content = (char*)node->content.string;
    txtlen = node->content.txtlen;
    indent = teGetLineIndent (me_self, node);

    if (txtlen <= 0 && indent <= 0)
        return;

    setup_dc (me_self, hdc, FALSE);

    format = TE_FORMAT;

    if (BE_WRAP(me_self->hwnd))
        format |= DT_WORDBREAK;

    SetTextAboveLineExtra (hdc, me_self->nLineAboveH);
    SetTextBellowLineExtra (hdc, me_self->nLineBaseH);

    /* draw title */
    if (me_self->title && indent > 0)
        DrawText (hdc, me_self->title, strlen(me_self->title), &rcTxt, format);

    outlen = txtlen;
    if (outlen > 0 && content[outlen-1] == txtDoc->lnsep) {
        outlen --;
        if (txtDoc->lnsep == '\n' &&
            (outlen > 0 && content[outlen-1] == '\r'))
            outlen --;
        if (me_self->lnChar) {
            outlen ++;
            chln = content[outlen-1];
            content[outlen-1] = me_self->lnChar;
        }
    }

    if (outlen <= 0)
        return;

    /* draw not selected node line text */
    if (!textnode_is_selected(me_self, txtDoc, node)) {
        DrawTextEx (hdc, content, outlen, &rcTxt, indent, format);
        if (chln)
            content[outlen-1] = chln;
        return;
    }

    /* draw selected node */
    get_selection_points (me_self, node, &pos_start, &pos_end);
    line_nr = RECTH(rcTxt) / me_self->nLineHeight;
    for (i = 0; i < line_nr; i++) {
        int startx, starty;

        if (i > 0) indent = 0;
        /* calc line info */
        DrawTextEx2(hdc, content, outlen, &rcTxt, indent, format, &fl);
        startx = rcTxt.left;
        starty = rcTxt.top;
        outw = indent;
        outchars = 0;
        if (pos_start > 0) { /* first: not selected section */
            setup_dc (me_self, hdc, FALSE);
            outchars = MIN(pos_start,fl.nr_chars);
            outw += TabbedTextOutLen (hdc, startx + outw, starty, content, outchars);
            content += outchars;
        }
        if (pos_start < fl.nr_chars && pos_end > 0) { /* second: selected section */
            outchars = MIN(pos_end, fl.nr_chars) -  MAX(pos_start,0);

            setup_dc (me_self, hdc, TRUE);
            outw += TabbedTextOutLen (hdc, startx + outw, starty, content, outchars);
            if (!(me_self->exFlags & NCSF_MLEDIT_EX_SETFOCUS)
                    && !(me_self->exFlags & NCSF_MLEDIT_EX_KILLFOCUS)) {
                ncsNotifyParent ((mWidget *)me_self, NCSN_EDIT_SELCHANGED);
            }
            content += outchars;
            selout += outchars;
        }
        if (pos_end < fl.nr_chars) { /* third: not selected section */
            setup_dc (me_self, hdc, FALSE);
            outchars = fl.nr_chars - MAX(pos_end, 0);
            outw += TabbedTextOutLen (hdc, startx + outw, starty, content, outchars);
            content += outchars;
        }
        pos_start -= fl.nr_chars;
        pos_end -= fl.nr_chars;
        outlen -= fl.nr_chars;
        rcTxt.top += me_self->nLineHeight;
    }

    if (chln)
        content[outlen-1] = chln;

    return;
}

/* ------------------------------ text document/buffer ------------------------ */

static BOOL set_current_node (TextDoc *txtDoc,
        TextNode *newnode, BOOL bSel, BOOL bChange)
{
    TextMark *mark;
    TextNode *oldnode;

    mark = GETMARK(bSel);
    oldnode = mark->curNode;

    if (newnode == oldnode)
        return FALSE;

    mark->curNode = newnode;
    mark->pos_lnOff = 0;

    /* called when the current insertion node is changed */
    if (bChange && txtDoc->change_fn)
        txtDoc->change_fn (txtDoc, bSel);

    return TRUE;
}

/*
 * textnode_create: creat a new text node and initialize it with text
 */
static TextNode* textnode_create (TextDoc *txtDoc, const char *line, int len)
{
    TextNode *newnode;

    if ( !(newnode = textnode_alloc ()) )
        return NULL;

    /* create a new blank line */
    if (!line || len < 0) len = 0;

    if ( !(testr_alloc (&newnode->content, len, txtDoc->nBlockSize)) ) {
        textnode_free (newnode);
        return NULL;
    }

    testr_setstr (&newnode->content, line, len);
    newnode->addData = 0;

    return newnode;
}

/*
 * textnode_destroy: destroy a text node
 */
static void textnode_destroy (TextNode *node)
{
    if (node) {
        list_del (&node->list);
        testr_free (&node->content);
        textnode_free (node);
    }
}

/*
 * textdoc_free : free TextDoc nodes
 * Description  : only changes the status fields of a TextDoc object, does not
 *                affect the properties.
 */
static void textdoc_free (TextDoc *txtDoc)
{
    TextNode *node;

    if (!txtDoc) return;

    while (!list_empty(&txtDoc->queue)) {
        node = list_entry (txtDoc->queue.next, TextNode, list);
        textnode_destroy (node);
    }
    txtDoc->insert.pos_lnOff = 0;
    txtDoc->insert.curNode = NULL;
    txtDoc->selection.curNode = 0;
    txtDoc->selection.pos_lnOff = 0;
}

/*
 * txtAddNode : add a textnode after a specified node
 * params     : node - the previous text node, if NULL, the new node will be
 *                     inserted at the tail.
 */
static TextNode *txtAddNode (TextDoc *txtDoc,
        const char*pLine, int len, TextNode *node)
{
    TextNode *newnode;
    mMlEdit *self = (mMlEdit *)txtDoc->fn_data;

    if ( !(newnode = textnode_create (txtDoc, pLine, len)))
        return NULL;

    if (node)
        list_add (&newnode->list, &node->list);
    else
        list_add_tail (&newnode->list, &txtDoc->queue);

    if (txtDoc->init_fn) txtDoc->init_fn(txtDoc, newnode, node);

    if (newnode)
        self->curLen += newnode->content.txtlen;

    return newnode;
}

/*
 * txtDelNode : deletes a text node
 */
static void txtDelNode (TextDoc *txtDoc, TextNode *node)
{
    mMlEdit *self = (mMlEdit *)txtDoc->fn_data;

    self->curLen -= node->content.txtlen;

    /* deletes scrollview item */
    if (txtDoc->del_fn)
        txtDoc->del_fn(txtDoc, node);

    textnode_destroy (node);
}


static char* get_limited_str(HWND hWnd, const char* str, int bytes)
{
    HDC hdc ;
    char *dst = NULL;
    static SIZE size;
    static int pos, i = 0, eff_word;
    static int eff_chars, *pos_chars;
    mMlEdit *self = (mMlEdit*)ncsObjFromHandle(hWnd);

    if ((pos_chars = malloc (bytes * sizeof(int))) == NULL) {
        return NULL;
    }

    hdc = GETDC(self);
    eff_word = GetTextExtentPoint (hdc, str,
        bytes + 4, -1, &eff_chars, pos_chars, NULL, &size);
    RELEASEDC(self, hdc);

    if (eff_word > 0) {
        if ((dst = malloc (eff_chars)) == NULL) {
            free(pos_chars);
            return NULL;
        }
        memset (dst, 0, eff_chars);

        for (i = eff_chars - 1; i >= 0; i--) {
            if (pos_chars[i+1] <= bytes && pos_chars[i] < bytes ) {
                pos = i;
                memcpy(dst, str, pos_chars[i+1]);
                break;
            }
        }
    }

    free(pos_chars);
    return dst;
}

/*
 * textdoc_settext : setting TextDoc object using a new text content and
 *                   free the old one
 * Params          : content - new text content, if NULL, the content of the
 *                             TextDoc object will not be changed; if content
 *                             is a null string, txtDoc content will be cleared.
 * TODO            : for not null-terminated text
 */
static int textdoc_settext (TextDoc *txtDoc, const char*content)
{
    const char *pLine, *ptmp;
    char *str = NULL;
    mMlEdit *self = (mMlEdit *)txtDoc->fn_data;

    if (!txtDoc || !content) return -1;

    /* free the old text content */
    textdoc_free (txtDoc);
    /*now, total length should be 0*/
    self->curLen = 0;

    if (self->hardLimit != -1 &&
            self->hardLimit < strlen(content)) {
        str = get_limited_str(self->hwnd, content, self->hardLimit);
    }
    else {
        str = (char*)content;
    }

    ptmp = pLine = str;

    if (str) {
        while (*ptmp != '\0') {
            if (*ptmp == txtDoc->lnsep) {
                /* adds a new line, including the line seperator */
                txtAddNode (txtDoc, pLine, ptmp-pLine+1, NULL);
                pLine = ptmp + 1;
            }
            ptmp ++;
        }
    }
    /* adds a new blank line or the last line without a line seperator */
    txtAddNode (txtDoc, pLine, ptmp-pLine, NULL);

    set_current_node (txtDoc, FIRSTNODE(txtDoc), FALSE, TRUE);

    if (self->hardLimit != -1 &&
            self->hardLimit < strlen(content)) {
        free(str);
    }

    return 0;
}

/*
 * teSetCaretPos : sets the caret position according to the current insertion
 *                 point or sets the selection caret position according to the
 *                 current selection point of the text doc.
 * Description   : should be called after the current insert point or the
 *                 selection point of the text document is changed
 */
static void teSetCaretPos (mMlEdit *self)
{
    HDC hdc;
    TextNode *node;
    TextMark *mark;
    int h, indent, endh;
    TextDoc *txtDoc = &self->txtDoc;

    mark = GETMARK(txtDoc->selection.curNode);
    node = mark->curNode;

    if (!node) return;

    hdc = GETDC(self);

    SelectFont (hdc, GetWindowFont(self->hwnd));

    if (txtDoc->selection.curNode)
        h = self->selItemY;
    else
        h = self->curItemY;

    indent = teGetLineIndent (self, node);

    if (BE_WRAP(self->hwnd)) {
        RECT rc;
        int w;
        SIZE txtsize = {0, 0};
        int txtlen, linenr, lineidx;
        const unsigned char *pLine, *pIns;
        DTFIRSTLINE fl;

        rc.left = 0;
        rc.top = 0;
        rc.right = self->contWidth;
        rc.bottom = GetWindowFont(self->hwnd)->size;

        linenr = NODE_LINENR(node);
        pLine = node->content.string;
        pIns = pLine + mark->pos_lnOff;
        txtlen = node->content.txtlen;
        /*
        if (pLine[txtlen-1] == txtDoc->lnsep)
            txtlen --;
        */
        lineidx = 0;
        while (txtlen > 0) {
            DrawTextEx2 (hdc, (const char*)pLine, txtlen, &rc, indent,
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT, &fl);
            if (pLine + fl.nr_chars > pIns)
                break;
            else if (pLine + fl.nr_chars == pIns) {
#if 0
                if (lineidx < linenr-1) {
                    pLine += fl.nr_chars;
                    lineidx ++;
                    indent = 0;
                }
#endif
                break;
            }
            pLine += fl.nr_chars;
            txtlen -= fl.nr_chars;
            rc.top += self->nLineHeight;
            lineidx ++;
            indent = 0;
        }

        GetTabbedTextExtent(hdc, (const char*)pLine, pIns - pLine, &txtsize);

        w = indent + txtsize.cx;
        h += self->nLineHeight * lineidx;

        self->desCaretX = w;
        endh = (h < self->contY) ? h : h + self->nLineHeight;
        _c(self)->makePosVisible(self, -1, endh > 0 ? endh : 0);
        if (txtDoc->selection.curNode)
            mySetSelPos (w, h + self->nLineAboveH);
        else
        {
            /* FIXME */
            if ( self->caretShape == NCS_CARETSHAPE_BLOCK &&
                     RECTW(rc) - w <= (GetWindowFont(self->hwnd)->size)/2 ) {
                _c(self)->makePosVisible(self, -1, endh + self->nLineHeight);
                mySetCaretPos (self, indent,
                        h + self->nLineAboveH + self->nLineHeight);
            }
            else {
                mySetCaretPos (self, w, h + self->nLineAboveH);
            }
        }
    }
    else {
        SIZE txtsize;

        GetTabbedTextExtent(hdc, (const char*)(node->content.string),
                mark->pos_lnOff, &txtsize);
        self->desCaretX = txtsize.cx + indent;
        endh = (h < self->contY) ? h : h + self->nLineHeight;
        _c(self)->makePosVisible(self,
                        txtsize.cx + indent, endh > 0 ? endh : 0);
        if (txtDoc->selection.curNode)
            mySetSelPos (txtsize.cx + indent, h + self->nLineAboveH);
        else
            mySetCaretPos (self, txtsize.cx + indent, h + self->nLineAboveH);
    }

    RELEASEDC(self, hdc);
    return;
}

static void teSetCaretPosOnChar (mMlEdit *self, int old_caret_pos)
{
    TextNode *node;
    TextMark *mark;
    int h, indent, endh;
    HDC hdc;
    TextDoc *txtDoc = &self->txtDoc;

    /*initialize line info*/
    self->nrDiffLine = -1;
    self->wNoChanged = 0;
    self->wLastLine  = 0;

    mark = GETMARK(txtDoc->selection.curNode);
    node = mark->curNode;

    if (!node) return;

    hdc = GETDC(self);
    SelectFont (hdc, GetWindowFont(self->hwnd));

    if (txtDoc->selection.curNode)
        h = self->selItemY;
    else
        h = self->curItemY;

    indent = teGetLineIndent (self, node);
    old_caret_pos = MIN(old_caret_pos, mark->pos_lnOff);

    if (BE_WRAP(self->hwnd)) {
        RECT rc;
        int w;
        SIZE txtsize = {0, 0};
        int oldtxtlen, newtxtlen, linenr, lineidx;
        const unsigned char *newLine, *oldLine, *pIns, *oldIns;
        DTFIRSTLINE oldfl, newfl;

        rc.left = 0;
        rc.top = 0;
        rc.right = self->contWidth;
        rc.bottom = GetWindowFont(self->hwnd)->size;

        oldLine = self->teBuff.string;
        oldtxtlen = self->teBuff.txtlen;

        linenr = NODE_LINENR(node);
        newLine = node->content.string;
        newtxtlen = node->content.txtlen;
        pIns = newLine + mark->pos_lnOff;
        oldIns = oldLine + old_caret_pos;
        lineidx = 0;

        newfl.width = 0;
        while (old_caret_pos > 0) {
            DrawTextEx2 (hdc, (const char*)oldLine, oldtxtlen, &rc,
                    lineidx ? 0: indent,
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT, &oldfl);

            DrawTextEx2 (hdc, (const char*)newLine, newtxtlen, &rc,
                    lineidx ? 0 : indent,
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT, &newfl);

            /*compare two string */
            if (oldfl.nr_chars != newfl.nr_chars)
            {
                self->nrDiffLine = lineidx;

                if (oldfl.nr_chars < newfl.nr_chars)
                    self->wNoChanged = oldfl.width;
                else
                    self->wNoChanged = newfl.width;
            }

            if (oldLine + oldfl.nr_chars > oldIns) {
                self->nrDiffLine = lineidx;
                GetTabbedTextExtent(hdc, (const char*)oldLine,
                        oldIns - oldLine, &txtsize);
		if (txtsize.cx < self->wNoChanged)
                   self->wNoChanged = txtsize.cx;
            }

            if (self->nrDiffLine != -1)
                break;

            newLine += newfl.nr_chars;
            oldLine += oldfl.nr_chars;
            oldtxtlen -= oldfl.nr_chars;
            newtxtlen -= newfl.nr_chars;
            lineidx ++;
            old_caret_pos -= oldfl.nr_chars;
        }

        if (self->nrDiffLine == -1) {
            self->nrDiffLine = lineidx;
            GetTabbedTextExtent(hdc, (const char*)oldLine,
                    old_caret_pos, &txtsize);
            self->wNoChanged = txtsize.cx;
        }
        if (lineidx == 0)
            self->wNoChanged += indent;

        newLine = node->content.string;
        newtxtlen = node->content.txtlen;
        lineidx = 0;

        while (newtxtlen > 0) {
            DrawTextEx2 (hdc, (const char*)newLine, newtxtlen, &rc, indent,
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT, &newfl);
            if (newLine + newfl.nr_chars > pIns)
                break;
            else if (newLine + newfl.nr_chars == pIns) {
#if 0
                if (lineidx < linenr-1) {
                    newLine += newfl.nr_chars;
                    lineidx ++;
                    indent = 0;
                }
#endif
                break;
            }
            newLine += newfl.nr_chars;
            newtxtlen -= newfl.nr_chars;
            rc.top += self->nLineHeight;
            lineidx ++;
            indent = 0;
        }

        GetTabbedTextExtent(hdc, (const char*)newLine, pIns - newLine, &txtsize);

        w = indent + txtsize.cx;
        h += self->nLineHeight * lineidx;

        self->desCaretX = w;
        endh = (h < self->contY) ? h : h + self->nLineHeight;

        _c(self)->makePosVisible(self, -1, endh > 0 ? endh : 0);
        if (txtDoc->selection.curNode) {
            mySetSelPos (w, h + self->nLineAboveH);
        }
        else
        {
            /* FIXME */
            if ( self->caretShape == NCS_CARETSHAPE_BLOCK &&
                     RECTW(rc) - w <= (GetWindowFont(self->hwnd)->size)/2 ) {
                _c(self)->makePosVisible(self, -1, endh + self->nLineHeight);
                mySetCaretPos (self, indent,
                        h + self->nLineAboveH + self->nLineHeight);
            } else {
                mySetCaretPos (self, w, h + self->nLineAboveH);
            }
        }
    }
    else {
        SIZE txtsize;

        self->nrDiffLine = 0;

        GetTabbedTextExtent(hdc,
                (const char*)(node->content.string), old_caret_pos, &txtsize);
        self->wNoChanged = indent + txtsize.cx;

        GetTabbedTextExtent(hdc, (const char*)(node->content.string),
                mark->pos_lnOff, &txtsize);
        self->desCaretX = txtsize.cx + indent;

        endh = (h < self->contY) ? h : h + self->nLineHeight;
        _c(self)->makePosVisible(self,
                        txtsize.cx + indent, endh > 0 ? endh : 0);
        if (txtDoc->selection.curNode) {
            mySetSelPos (txtsize.cx + indent, h + self->nLineAboveH);
        }
        else
            mySetCaretPos (self, txtsize.cx + indent, h + self->nLineAboveH);
    }

    RELEASEDC(self, hdc);
    return;
}

static void get_selected_rect (mMlEdit *self, RECT *rcSel)
{
    TextDoc *txtDoc = &self->txtDoc;

    rcSel->left = 0;
    rcSel->right = 0;
    if (self->curItemY < self->selItemY) {
        rcSel->top = self->curItemY;
        rcSel->bottom = self->selItemY + NODE_HEIGHT(txtDoc->selection.curNode);
    } else {
        rcSel->top = self->selItemY;
        rcSel->bottom = self->curItemY + NODE_HEIGHT(txtDoc->insert.curNode);
    }
    return;
}

/*
 * te_unselect_all : unselects the selected parts
 * Params          : bChange - whether to change the insertion point to the
 *                             position of the selection point
 */
static void te_unselect_all (mMlEdit *self, BOOL bChange)
{
    RECT rcSel;
    POINT pt;
    TextDoc *txtDoc = &self->txtDoc;

    pt.x = 0;
    pt.y = 0;

    if (txtDoc->selection.curNode) {
        get_selected_rect (self, &rcSel);

        if (bChange) {
            txtDoc->insert = txtDoc->selection;
            self->curItemY = self->selItemY;
            myGetSelPos (&pt);
        }

        txtDoc->selection.curNode = NULL;
        _c(self)->deselectAll(self);
        _c(self)->refreshRect (self, &rcSel);

        if (bChange) {
            mySetCaretPos (self, pt.x, pt.y);
            self->desCaretX = pt.x;
        }
    }
}

static void insert_string (TextDoc *txtDoc,
        TextNode *curnode, int insert_pos, const char *newtext, int len)
{
    unsigned char *pLn, *pIns;
    StrBuffer *strbuff = &curnode->content;
    mMlEdit *self = (mMlEdit *)txtDoc->fn_data;

    if (len > 0) {
        pLn = testr_realloc (strbuff, strbuff->txtlen + len);
        if (!pLn) return;
        pIns = pLn + insert_pos;
        memmove (pIns + len, pIns, strbuff->txtlen+1 - insert_pos);
        memcpy (pIns, newtext, len);

        if (self->hardLimit !=-1 &&
            self->curLen + len > self->hardLimit) {
            Ping ();
            ncsNotifyParent ((mWidget *)self, NCSN_EDIT_MAXTEXT);
            return;

        }
    }
    else {
        pIns = strbuff->string + insert_pos;
        memmove (pIns + len, pIns, strbuff->txtlen+1 - insert_pos);
        pLn = testr_realloc (strbuff, strbuff->txtlen + len);
    }

    self->curLen += len;
    strbuff->txtlen += len;
}
/*
 * delete_selection : deletes the selected texts
 */
static int delete_selection (TextDoc *txtDoc)
{
    int pos_start, pos_end;
    int pos_start2, pos_end2;
    TextNode *node, *startnode, *endnode;
    TextMark *markStart, *markEnd;
    mMlEdit *self = (mMlEdit *)txtDoc->fn_data;

    markStart = get_start_mark (self);
    markEnd = (markStart == &txtDoc->insert) ?
                          &txtDoc->selection : &txtDoc->insert;

    startnode = markStart->curNode;
    endnode = markEnd->curNode;

    get_selection_points (self, endnode, &pos_start, &pos_end);
    get_selection_points (self, startnode, &pos_start2, &pos_end2);

    txtDoc->selection.curNode = NULL;

    _c(self)->freeze (self, TRUE);

    insert_string (txtDoc, endnode, pos_end, NULL, pos_start-pos_end);

    if (startnode != endnode) {

        while ( (node = TXTNODE_NEXT(startnode)) != endnode ) {
            txtDelNode (txtDoc, node);
        }

        if (pos_start2 == 0) {
            txtDelNode (txtDoc, startnode);
            startnode = NULL;
            txtDoc->insert.curNode = endnode;
            txtDoc->insert.pos_lnOff = 0;
            txtDoc->change_fn (txtDoc, FALSE);
        }
        else {
            char del[1] = {127};
            textdoc_insert_string_ex (self, txtDoc, startnode, pos_end2-1, NULL,
                                      pos_start2-pos_end2+1);
            textdoc_insert_string_ex_2 (self, txtDoc, startnode, pos_start2, del, 1);
            txtDoc->insert.curNode = startnode;
            endnode = NULL;
        }
    }

    if (txtDoc->change_cont) {
        txtDoc->change_cont (txtDoc, endnode);
        txtDoc->change_cont (txtDoc, startnode);
    }

    txtDoc->selection.curNode = NULL;
    _c(self)->deselectAll (self);
    _c(self)->freeze (self, FALSE);

    return pos_start2;
}

static TextNode* insert_ln_sep (TextDoc *txtDoc,
        TextNode *curnode, int insert_pos, BOOL bChRn)
{
    TextNode *newnode;
    unsigned char *pIns;
    int len = bChRn ? 2 : 1;
    StrBuffer *strbuff = &curnode->content;
    mMlEdit *self = (mMlEdit *)txtDoc->fn_data;

    pIns = strbuff->string + insert_pos;

    newnode = txtAddNode ( txtDoc, (const char*)pIns, strbuff->txtlen -
                          (pIns-strbuff->string), curnode );

    strbuff->txtlen = insert_pos + len; /* add a line sep */
    if (*pIns == '\0') {
        testr_realloc (strbuff, strbuff->txtlen);
        pIns = strbuff->string + insert_pos ;
    }

    if (bChRn)
        strncpy((char*)pIns, CH_RN, len);
    else
        *pIns = txtDoc->lnsep;
    *(pIns + len) = '\0';

    self->curLen += len;
    return newnode;
}


/*
 * textdoc_insert_string_ex :
 *          inserts a text string(not including line seperator) into
 *          the text buffer at the specified insertion point, or makes
 *          some special operations (inserts line sep, del, bs, ...)
 * params : enode      - the specified node to operate on
 *          insert_pos - the designed insert position
 *          newtext    - text to insert
 *          len        - len of text to insert or remove
 */
static int
textdoc_insert_string_ex (mMlEdit *self, TextDoc *txtDoc,
        TextNode *enode, int insert_pos, const char* newtext, int len)
{
    TextNode *curnode, *newnode;
    StrBuffer *strbuff;
    unsigned char *pIns;

    if (!enode && txtDoc->selection.curNode) {
        insert_pos = delete_selection (txtDoc);
        if (len == 1 && (*newtext == '\b' || *newtext == 127) )
            return insert_pos;
        else {
            txtDoc->insert.pos_lnOff = insert_pos;
        }
    }

    if (!enode) { /* operates on the current insertion point */
        curnode = txtDoc->insert.curNode;
        insert_pos = txtDoc->insert.pos_lnOff;
    }
    else {
        curnode = enode;
    }

    strbuff = &curnode->content;
    pIns = strbuff->string + insert_pos;

    if ( len == 1 || (len == 2 && strcmp(newtext, CH_RN) == 0) ) {
        if ( *newtext == txtDoc->lnsep || (strncmp(newtext, CH_RN, 2) == 0) ) {
            /* add a new line */
            newnode = insert_ln_sep (txtDoc, curnode,
                                             insert_pos, len == 2);
            if (!enode) {
                txtDoc->insert.curNode = newnode;
                insert_pos = 0;
            }
            return insert_pos;
        }

        /* len == 1, other cases */
        switch (*newtext) {
        case '\b':  /* BS */
            if (pIns == strbuff->string) { /* line head */
                TextNode *node;
                int cplen;
                char *tmpstr = NULL;

                if (BE_FIRST_NODE(curnode))
                    return insert_pos;
                node = TXTNODE_PREV(curnode);

                cplen = node->content.txtlen;

                tmpstr = malloc(cplen + 1);
                if (tmpstr != NULL)
                {
                    memcpy (tmpstr, node->content.string, cplen);
                    tmpstr [cplen] = '\0';

                    /* deletes the previous node*/
                    txtDelNode (txtDoc, node);

                    /* adds the previous line at the beginning of the current line */
                    insert_pos += textdoc_insert_string_ex (self, txtDoc,
                                        enode, insert_pos,
                                       (const char*)tmpstr, cplen-1);

                    txtDoc->change_fn (txtDoc, FALSE);
                    free(tmpstr);
                }
                return insert_pos;
            }
            len = - CHLENPREV((const char *)(strbuff->string), pIns);
            break;

        case 127: /* DEL */
            if (*pIns == '\0')
                return insert_pos;
            if ( *pIns == txtDoc->lnsep || is_rn_sep(pIns) ) {
                TextNode *node = TXTNODE_NEXT(curnode);
                int oldpos = insert_pos;
                if (node->content.string[0] != txtDoc->lnsep &&
                                !is_rn_sep(node->content.string)) {

                    int cplen = node->content.txtlen;
                    char *tmpstr = NULL;

                    /*next node have info*/
                    *pIns = '\0';

                    self->curLen -= 1;

                    curnode->content.txtlen = pIns - curnode->content.string;
                    tmpstr = malloc(cplen + 1);
                    if (tmpstr == NULL)
                        break;
                    memcpy (tmpstr, node->content.string, cplen);
                    tmpstr[cplen]='\0';

                    txtDelNode (txtDoc, node);
                    textdoc_insert_string_ex (self, txtDoc, enode, insert_pos,
                                    (const char*)tmpstr, cplen);
                    insert_pos = oldpos;
                    free(tmpstr);
                }
                else
                    txtDelNode (txtDoc, node);

                return insert_pos;
            }
            else {
                int chlen = CHLENNEXT(pIns, (curnode->content.txtlen +
                                             strbuff->string - pIns) );
                pIns += chlen;
                insert_pos += chlen;
                len = -chlen;
            }
            break;
        }
    }

    insert_string (txtDoc, curnode, insert_pos, newtext, len);
    insert_pos += len;

    return insert_pos;
}

static void begin_selection (mMlEdit *self)
{
    self->txtDoc.selection = self->txtDoc.insert;
    self->selItemY = self->curItemY;
    self->selX = self->caretX;
    self->selY = self->caretY;
    SELECT_NODE(self->txtDoc.selection.curNode, TRUE);
}

static void te_make_caret_visible (mMlEdit *self, BOOL bSel)
{
    POINT pt;

    if (!bSel)
        myGetCaretPos (&pt);
    else
        myGetSelPos (&pt);

    if (pt.y > self->contY)
        pt.y = (pt.y / self->nLineHeight + 1) * self->nLineHeight;
    else
        pt.y = (pt.y / self->nLineHeight) * self->nLineHeight;
    _c(self)->makePosVisible (self, pt.x, pt.y);

    if (!bSel) {
        mySetCaretPos (self, -1, -1);
    }
}
/*
 * set_caret_pos : Sets the caret/selection position in a node according to
 *                 x,y values
 */
static int
set_caret_pos (mMlEdit *self, TextNode *node, int x, int y, BOOL bSel)
{
    RECT rc;
    SIZE txtsize;
    DTFIRSTLINE fl;
    HDC hdc;
    TextDoc *txtDoc = &self->txtDoc;
    const unsigned char *string = node->content.string;
    const unsigned char *pLine = string;
    int txtlen = node->content.txtlen;
    int line, indent, h = 0, out_chars, ln_chars;

    indent = teGetLineIndent (self, node);
    line = y / self->nLineHeight;
    if (!bSel)
        h = self->curItemY + line * self->nLineHeight;
    else
        h = self->selItemY + line * self->nLineHeight;

    if (txtlen == 0 || string[0] == txtDoc->lnsep || is_rn_sep(string)) {
        if (!bSel) {
            txtDoc->insert.pos_lnOff = 0;
            mySetCaretPos (self, 0 + indent, h + self->nLineAboveH);
        }
        else {
            txtDoc->selection.pos_lnOff = 0;
            mySetSelPos (0 + indent, h + self->nLineAboveH);
        }
        return 0;
    }

    /* FIXME */
    hdc = GETDC(self);

    if (NODE_LINENR(node) > 1) {  /* multi line case */
        int i = 0;

        rc.left = 0;
        rc.top = 0;
        rc.right = self->contWidth;
        rc.bottom = self->nLineHeight;

        i = 0;
        while (1) {
            if (i > 0) indent = 0;
            DrawTextEx2 (hdc, (const char*)pLine, txtlen, &rc, indent,
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT, &fl);
            if (i == line)
                break;
            pLine += fl.nr_chars;
            txtlen -= fl.nr_chars;
            i++;
        }
        ln_chars = fl.nr_chars;
    }
    else {
        pLine = string;
        ln_chars = txtlen;
    }


    if (pLine[ln_chars-1] == txtDoc->lnsep) {
        /* add caret before line seperator */
        ln_chars --;
        if (ln_chars > 0 && is_rn_sep((pLine + ln_chars - 1)) )
            ln_chars --;
    }

    if (x - indent <= 0) {
        out_chars = 0;
        txtsize.cx = 0;
    }
    else
        out_chars = GetTabbedTextExtentPoint (hdc, (const char*)pLine, ln_chars,
                        x - indent, NULL, NULL, NULL, &txtsize);

    RELEASEDC(self, hdc);

    if (!bSel) {
        txtDoc->insert.pos_lnOff = pLine - string + out_chars;
        mySetCaretPos (self, txtsize.cx + indent, h + self->nLineAboveH);
    }
    else {
        txtDoc->selection.pos_lnOff = pLine - string + out_chars;
        mySetSelPos (txtsize.cx + indent, h + self->nLineAboveH);
    }

    return 0;
}

static void teOnMouseDown (mMlEdit *self,
        TextNode *node, POINT *pt, int item_y, BOOL bShift)
{
    TextDoc *txtDoc = &self->txtDoc;

    if (!node) return;

    if (!bShift) {
        txtDoc->insert.curNode = node;
        self->curItemY = item_y;
    }
    else {
        txtDoc->selection.curNode = node;
        self->selItemY = item_y;
    }

    set_caret_pos (self, node, pt->x, pt->y, bShift);
}

/*
 * te_cursor_shift : process shift keys, including left and right arrow, Home,
 *                   End keys.
 * Params          : bShift - Whether shift key is in pressed status
 */
static void te_cursor_shift (mMlEdit *self, int kcode, BOOL bShift)
{
    int len;
    POINT pt;
    unsigned char *pIns;
    TextMark *mark;
    TextNode *node, *curnode;
    TextDoc *txtDoc = &self->txtDoc;

    /* unselect the selected first */
    if (!bShift && txtDoc->selection.curNode) {
        te_unselect_all (self, TRUE);
    }

    mark = bShift ? &txtDoc->selection : &txtDoc->insert;

    if (bShift && !mark->curNode) {
        begin_selection (self);
    }

    node = mark->curNode;
    pIns = node->content.string + mark->pos_lnOff;

    if (kcode == SCANCODE_CURSORBLOCKRIGHT) {
        if (*pIns == '\0')
            return;
        else if (*pIns == txtDoc->lnsep || is_rn_sep(pIns)) {
            curnode = TXTNODE_NEXT(node);
            set_current_node (txtDoc, curnode, bShift, FALSE);

            if (bShift) {
                SELECT_NODE(node, (NODE_IS_SELECTED(curnode) ? FALSE : TRUE) );
                SELECT_NODE(curnode, TRUE);
                self->selItemY += NODE_HEIGHT(node);
            }
            else
                self->curItemY += NODE_HEIGHT(node);
        }
        else {
            mark->pos_lnOff += CHLENNEXT(pIns, (node->content.string +
                                                node->content.txtlen - pIns));
        }
    }
    else if (kcode == SCANCODE_CURSORBLOCKLEFT) {
        len = -1;
        if (pIns == node->content.string) {
            if (BE_FIRST_NODE(node))
                return;
            else {
                curnode = TXTNODE_PREV(node);
                set_current_node (txtDoc, curnode, bShift, FALSE);

                if (bShift) {
                    SELECT_NODE(node, (NODE_IS_SELECTED(curnode) ? FALSE : TRUE) );
                    SELECT_NODE(curnode, TRUE);
                    self->selItemY -= NODE_HEIGHT(TXTNODE_PREV(node));
                }
                else
                    self->curItemY -= NODE_HEIGHT(TXTNODE_PREV(node));

                mark->pos_lnOff = TXTNODE_PREV(node)->content.txtlen - 1;
            }
        }
        else {
            mark->pos_lnOff -= CHLENPREV((const char *)(node->content.string), pIns);
        }
    }
    else if (kcode == SCANCODE_HOME || kcode == SCANCODE_END) {
        myGetCaretPos (&pt);
        pt.y -= self->curItemY;
        pt.x = (kcode == SCANCODE_HOME)? 0 : self->contWidth - 1;
        if (node)
            self->desCaretX = pt.x;
        teOnMouseDown (self, node, &pt, self->curItemY, bShift);
        te_make_caret_visible (self, bShift);
        goto SFRETURN;
    }

    teSetCaretPos (self);

SFRETURN:
    if (bShift) {
        REFRESH_NODE(mark->curNode);
        check_caret ();
    }
}

static void te_cursor_updown (mMlEdit *self, int len, BOOL bShift)
{
    int item_y;
    POINT pt;
    TextNode *node;
    TextMark *mark;
    TextDoc *txtDoc = &self->txtDoc;

    TextNode *org_node;

    if (!bShift && txtDoc->selection.curNode) {
        te_unselect_all (self, TRUE);
    }

    mark = bShift ? &txtDoc->selection : &txtDoc->insert;

    if (bShift && !mark->curNode) {
        begin_selection (self);
    }
    node = mark->curNode;
    org_node = mark->curNode;
    item_y = bShift ? self->selItemY : self->curItemY;

    if (bShift) {
        myGetSelPos (&pt);
    }
    else
        myGetCaretPos (&pt);

    pt.y += len * self->nLineHeight;
    pt.x = self->desCaretX;

    if (len == 1) {  /* go to next line */
        if (pt.y >= item_y + NODE_HEIGHT(node)) {
            /* go to next node */
            if (BE_LAST_NODE(node))
                return;
            item_y += NODE_HEIGHT(node);
            node = TXTNODE_NEXT(node);
        }
    }
    else if (len == -1) {
        if (pt.y < item_y) {
            /* go to previous node */
            if (BE_FIRST_NODE(node))
                return;
            node = TXTNODE_PREV(node);
            item_y -= NODE_HEIGHT(node);
        }
    }
    else {
        /* TODO */
        return;
    }
    pt.y -= item_y;

    teOnMouseDown (self, node, &pt, item_y, bShift);
    te_make_caret_visible (self, bShift);

    if (bShift) {
        if(NODE_IS_SELECTED(node)) {
            if (node != org_node)
                SELECT_NODE(org_node, FALSE);
        }
        else {
            SELECT_NODE(node, TRUE);
        }
        if (node != org_node)
            REFRESH_NODE(org_node);
        REFRESH_NODE(node);
        check_caret();
    }
}

static int te_set_selection (mMlEdit *self)
{
    TextNode *node;
    RECT rcSel;
    TextMark *markStart, *markEnd;
    TextDoc *txtDoc = &self->txtDoc;

    markStart = get_start_mark (self);
    markEnd = (markStart == &txtDoc->insert) ?
                              &txtDoc->selection : &txtDoc->insert;

    node = markStart->curNode;
    while (node) {
        SELECT_NODE (node, TRUE);
        if (node == markEnd->curNode)
            break;
        node = TXTNODE_NEXT (node);
    }

    get_selected_rect (self, &rcSel);
    _c(self)->refreshRect (self, &rcSel);
    check_caret ();

    return 0;
}

static int te_select_all (mMlEdit *self)
{
    TextMark *selection = &self->txtDoc.selection;
    TextMark *insertion = &self->txtDoc.insert;
    TextNode *last;

    if (selection->curNode) {
        te_unselect_all (self, TRUE);
    }

    insertion->pos_lnOff = 0;
    insertion->curNode = FIRSTNODE(&self->txtDoc);
    self->curItemY = 0;
    teSetCaretPos (self);

    last = LASTNODE(&self->txtDoc);
    selection->curNode = last;
    selection->pos_lnOff = last->content.txtlen;
    self->selItemY = _c(self)->getTotalHeight (self) - NODE_HEIGHT(last);
    /* FIXME, optimize */
    teSetCaretPos (self);

    return te_set_selection (self);
}



static void te_cursor_move (mMlEdit *self, POINT *pt)
{
    int item_h;
    RECT rcInv;
    TextNode *curnode;
    POINT oldpt, newpt;
    TextDoc *txtDoc = &self->txtDoc;
    TextMark *selection = &txtDoc->selection;

    /* begin to select */
    if (!selection->curNode) {
        begin_selection (self);
        return;
    }

    curnode = selection->curNode;

    item_h = _c(self)->getItemHeight (self, curnode->addData);
    if (pt->y >= self->selItemY + item_h) {
        TextNode *next = TXTNODE_NEXT (curnode);
        if (next) {
            selection->curNode = next;
            self->selItemY += item_h;
        }
        else {
            if (selection->pos_lnOff != curnode->content.txtlen) {
                selection->pos_lnOff = curnode->content.txtlen;
                REFRESH_NODE(curnode);
                check_caret ();
            }
            return;
        }
        /* FIXME */
        pt->y = self->selItemY + item_h + self->nLineHeight/2;
    }
    else if (pt->y < self->selItemY) {
        TextNode *prev = TXTNODE_PREV (curnode);
        if (prev) {
            selection->curNode = prev;
            item_h = _c(self)->getItemHeight (self, (HSVITEM)prev->addData);
            self->selItemY -= item_h;
        }
        else {
            if (selection->pos_lnOff != 0) {
                selection->pos_lnOff = 0;
                REFRESH_NODE(curnode);
                check_caret ();
            }
            return;
        }
        pt->y = self->selItemY - self->nLineHeight/2;
    }

    if (self->flags & NCSF_MLEDIT_MOVE) {
        _c(self)->makePosVisible (self, pt->x, pt->y);
    }
    pt->y -= self->selItemY;

    /* moves to a new line */
    if (curnode && selection->curNode != curnode) {
        /* moves off the current selected line */
        if (NODE_IS_SELECTED(selection->curNode))
            SELECT_NODE(curnode, FALSE);
        else
            SELECT_NODE(selection->curNode, TRUE);
        REFRESH_NODE(curnode);
    }

    myGetSelPos (&oldpt);
    set_caret_pos (self, selection->curNode, pt->x, pt->y, TRUE);
    myGetSelPos (&newpt);

    if (oldpt.x != newpt.x && oldpt.y == newpt.y)
    {
        rcInv.left   = newpt.x > oldpt.x ? oldpt.x : newpt.x;
        rcInv.top    = 0;
        rcInv.right  = oldpt.x > newpt.x ? oldpt.x : newpt.x;
        rcInv.bottom = rcInv.top + self->nLineHeight - self->nLineBaseH;

        REFRESH_NODE_EX(selection->curNode, &rcInv);
        UpdateWindow(self->hwnd, TRUE);
    }else if (oldpt.y != newpt.y) {
        rcInv.left   = 0;
        rcInv.top    = 0;
        rcInv.right  = rcInv.left + self->contWidth;
        rcInv.bottom = abs(newpt.y - oldpt.y)
            + self->nLineHeight - self->nLineBaseH;

        REFRESH_NODE_EX(selection->curNode, &rcInv);
        UpdateWindow(self->hwnd, TRUE);
    }

    check_caret ();
}



static void textedit_reset_content (mMlEdit *self,
        const char *newtext, BOOL bLast)
{
    TextNode *node=NULL;
    TextDoc *txtDoc = &self->txtDoc;

    node = FIRSTNODE(&self->txtDoc);

    while (node) {
        /* the item list will be free at the "_c(self)->super->resetContent(...)"
        ** but the node->addData can't be reset,
        ** so we should be reset it first
        */
        node->addData = 0;
        node = TXTNODE_NEXT(node);
    }

    _c(self)->super->resetContent((mEdit *)self);
    //_c(self)->resetContent (self);
    teResetData (self);

    _c(self)->freeze (self, TRUE);
    textdoc_settext (txtDoc, newtext);
    _c(self)->freeze (self, FALSE);

    if ( bLast && !(GetWindowStyle(self->hwnd) & NCSS_EDIT_READONLY) ) {
        TextNode *node = LASTNODE(txtDoc);
        txtDoc->insert.curNode = node;
        txtDoc->insert.pos_lnOff = node->content.txtlen;
        teNodeChange (txtDoc, FALSE);
        teSetCaretPos (self);
    }
    else {
        mySetCaretPos (self, teGetLineIndent(self, FIRSTNODE(txtDoc)),
                    0 + self->nLineAboveH);
    }
    ncsNotifyParent ((mWidget *)self, NCSN_EDIT_UPDATE);
    self->contDirty = TRUE;
}

/*
 * textdoc_insert_text : insert a multi-line text at the insertion point
 */
static int textdoc_insert_text (mMlEdit *self, const char* newtext, int len)
{
    const char *pLine, *ptmp;
    TextNode *node, *newnode, *nextnode;
    int leftlen = len;
    TextDoc *txtDoc = &self->txtDoc;

    if (!txtDoc || !newtext || len < 0) return -1;

    pLine = ptmp = newtext;
    /* insert first strings */
    while (*ptmp != txtDoc->lnsep && *ptmp != '\0' && leftlen > 0) {
        ptmp ++;
        leftlen --;
    }
    textdoc_insert_string (self, txtDoc, pLine, ptmp-pLine);

    if (*ptmp == '\0' || leftlen <= 0)
        return 0;

    node = txtDoc->insert.curNode;

    /* make next node */
    if (ptmp > pLine  && is_rn_sep(ptmp - 1)) {
        textdoc_insert_string (self, txtDoc, CH_RN, 2);
    }
    else {
        textdoc_insert_string (self, txtDoc, (const char *)(&txtDoc->lnsep), 1);
    }
    ptmp ++;
    leftlen --;
    pLine = ptmp;
    nextnode = TXTNODE_NEXT (node);

    /* insert lines */
    newnode = node;
    while (leftlen > 0 && *ptmp != '\0') {
        if (*ptmp == txtDoc->lnsep) {
            newnode = txtAddNode (txtDoc, pLine, ptmp-pLine+1, newnode);
            pLine = ptmp + 1;
        }
        ptmp ++;
        leftlen --;
    }

    txtDoc->selection.curNode =NULL;
    txtDoc->selection.pos_lnOff = 0;

    /* insert last strings */
    if (nextnode) {
        set_current_node (txtDoc, nextnode, FALSE, TRUE);
        textdoc_insert_string (self, txtDoc, pLine, ptmp-pLine);
    }

    return 0;
}


static int textedit_insert_text (mMlEdit *self, const char* text, int len)
{
    int ret;
    TextDoc *txtDoc = &self->txtDoc;

    if (!text || len <= 0)
        return -1;

    ret = textdoc_insert_text (self, text, len);

    teNodeChange (txtDoc, FALSE);
    teSetCaretPos (self);

    REFRESH_NODE(txtDoc->insert.curNode);
    ncsNotifyParent ((mWidget *)self, NCSN_EDIT_CHANGE);
    self->contDirty = TRUE;
    return ret;
}

/*
 * get_node_by_idx : Get node and y position by node index
 */
static TextNode* get_node_by_idx (mMlEdit *self, int line, int *item_y)
{
    TextNode *node;
    int nr = 0, ypos = 0;
    TextDoc *txtDoc = &self->txtDoc;

    if (line < 0)
        return NULL;

    node = FIRSTNODE(txtDoc);
    while (node) {
        if (nr == line)
            break;
        nr ++;
        ypos += NODE_HEIGHT(node);
        node = TXTNODE_NEXT(node);
    }

    if (nr != line)
        return NULL;

    if (item_y)
        *item_y = ypos;
    return node;
}

/*
 * te_set_position : Sets insertion or selection position
 * Params          : mark - insertion or selection point
 */
static TextNode *te_set_position (mMlEdit *self, TextMark *mark,
                 int line, int char_pos, int *item_y, int *newpos)
{
    TextNode *markNode;
    int *pos_chars;
    int nr_chars;

    if (char_pos < 0)
        return NULL;
    if ( !(markNode = get_node_by_idx (self, line, item_y)) )
        return NULL;
    if (char_pos > markNode->content.txtlen)
        return NULL;

    pos_chars = ALLOCATE_LOCAL (markNode->content.txtlen * sizeof(int));
    nr_chars = GetTextMCharInfo (GetWindowFont (self->hwnd),
                    (const char*)(markNode->content.string),
                    markNode->content.txtlen, pos_chars);

    if (char_pos > nr_chars) {
        DEALLOCATE_LOCAL (pos_chars);
        return NULL;
    }

    if (char_pos == nr_chars)
        char_pos = markNode->content.txtlen;
    else
        char_pos = pos_chars[char_pos];

    DEALLOCATE_LOCAL (pos_chars);

    if (newpos)
        *newpos = char_pos;
    return markNode;
}

static int textedit_set_pos_ex (mMlEdit *self, int line_pos, int char_pos, BOOL bSel)
{
    int item_y, newpos;
    TextMark *mark;
    TextNode *node;
    TextDoc *txtDoc = &self->txtDoc;

    if ( !(mark = GETMARK(bSel)) )
        return -1;

    if ( !(node = te_set_position (self, mark,
                    line_pos, char_pos, &item_y, &newpos)) )
        return -1;

    /* unselect the former seleted before mark is changed */
    if (txtDoc->selection.curNode) {
        te_unselect_all (self, TRUE);
    }
    SETITEMY(bSel, item_y);

    mark->pos_lnOff = newpos;
    mark->curNode = node;

    if (bSel)
        te_set_selection (self);
    else
        teSetCaretPos (self);

    return mark->pos_lnOff;
}

static int textedit_get_pos_ex (mMlEdit *self, int *line_pos, int *char_pos, BOOL bSel)
{
    int nr_chars = 0;
    TextMark *mark;
    TextDoc *txtDoc = &self->txtDoc;

    mark = GETMARK(bSel);
    if (!mark->curNode)
        return -1;

    if (line_pos) {
        *line_pos = NODE_INDEX(mark->curNode);
    }
    if (char_pos) {
        nr_chars = GetTextMCharInfo (GetWindowFont (self->hwnd),
                      (const char*)mark->curNode->content.string, mark->pos_lnOff, NULL);
        *char_pos = nr_chars;
    }

    return mark->pos_lnOff;
}

static int real_to_line_pos (mMlEdit *self, int pos, int *line, int *row)
{
    list_t *me;
    TextNode *node;
    int lnr, chr;

    lnr = 0;
    chr = pos;

    list_for_each (me, &self->txtDoc.queue) {
        node = list_entry (me, TextNode, list);
        if (pos <= node->content.txtlen)
            break;
        lnr ++;
        chr -= node->content.txtlen;
    }

    *line = lnr;
    *row = chr;

    return 0;
}

static int line_to_real_pos (mMlEdit *self, int line, int row)
{
    list_t *me;
    TextNode *node;
    int _pos, l = 0;

    _pos = row;

    list_for_each (me, &self->txtDoc.queue) {
        node = list_entry (me, TextNode, list);
        if (l >= line)
            break;
        l++;
        _pos += node->content.txtlen;
    }

    return _pos;
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


static void mMlEdit_construct (mMlEdit *self, DWORD addData)
{
    g_stmEditCls.construct((mEdit *)self, addData);

    self->hardLimit = -1;
    self->curLen = 0;

    self->nLineHeight = DEF_LINE_HEIGHT;
    self->nLineAboveH = DEF_LINE_ABOVE_H;
    self->nLineBaseH  = DEF_LINE_BASE_H;

     if (GetWindowStyle(self->hwnd) & NCSS_MLEDIT_TITLE) {
        self->title = FixStrAlloc (DEF_TITLE_LEN+1);
        self->title[0] = '\0';
        self->titleIndent = 0;
    }
    else {
        self->title = NULL;
        self->titleIndent = 0;
    }

    self->lnChar = 0;
    self->caretShape = NCS_CARETSHAPE_LINE;

    self->cp = &defTextCopyPaste;

    self->hStepVal = 0;
    self->vStepVal = self->nLineHeight;

    teResetData (self);

    testr_alloc (&(self->teBuff), 128, 256);

    CreateCaret (self->hwnd, NULL, getCaretWidth(self),
            self->nLineHeight - self->nLineAboveH- self->nLineBaseH);

    mySetCaretPos (self, self->desCaretX, self->nLineAboveH);
}

static void mMlEdit_destroy (mMlEdit *self)
{
	if ((GetWindowStyle(self->hwnd) & NCSS_MLEDIT_TITLE) && self->title) {
        FreeFixStr (self->title);
    }
    DestroyCaret (self->hwnd);
    _c(self)->removeAll(self);

    textdoc_free (&self->txtDoc);
    testr_free (&(self->teBuff));

    Class(mEdit).destroy((mEdit*)self);
}

static int mMlEdit_wndProc (mMlEdit *self, int message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case MSG_CREATE:
            {
                unsigned int i;
                char *caption;
                DWORD dwStyle;
                TextDoc *txtDoc = &self->txtDoc;

                _c(self->itemList)->freeze(self->itemList, TRUE);

                _c(self)->setItemDraw(self, teDrawItem);
                INIT_LIST_HEAD(&txtDoc->queue);
                memset (&txtDoc->insert, 0, sizeof(txtDoc->insert));
                txtDoc->selection.curNode = NULL;
                textdoc_reset (self, txtDoc);

                dwStyle = GetWindowStyle(self->hwnd);
                caption = strdup(GetWindowCaption(self->hwnd));

                if (NCSS_EDIT_UPPERCASE & dwStyle)
                {
                    for (i = 0; i < strlen(caption); i ++)
                        caption[i] = toupper (caption[i]);
                }
                else if (NCSS_EDIT_LOWERCASE & dwStyle)
                {
                    for (i = 0; i < strlen(caption); i ++)
                        caption[i] = tolower (caption[i]);
                }

                textdoc_settext (txtDoc, caption);

                _c(self->itemList)->freeze(self->itemList, FALSE);

                free (caption);
                return 0;
            }

        case MSG_FONTCHANGED:
            {
                TextDoc *txtDoc = &self->txtDoc;
                TextNode *node = FIRSTNODE(txtDoc);
				HDC hdc ;

                DestroyCaret (self->hwnd);
                self->nLineHeight = GetWindowFont(self->hwnd)->size +
                    self->nLineAboveH + self->nLineBaseH;
                CreateCaret (self->hwnd, NULL, getCaretWidth(self),
                        self->nLineHeight - self->nLineAboveH- self->nLineBaseH);

                while (node) {
                    txtDoc->change_cont (txtDoc, node);
                    node = TXTNODE_NEXT(node);
                }
                teSetCaretPos (self);
				hdc = GETDC(self);
                SelectFont(hdc, GetWindowFont(self->hwnd));
                _c(self)->setTitle(self, self->title, self->title?strlen(self->title):0);
				RELEASEDC(self, hdc);

                InvalidateRect (self->hwnd, NULL, TRUE);
                return 0;
            }
        case MSG_SETFOCUS:
            {
                self->flags |= NCSF_MLEDIT_FOCUSED;
                self->contDirty = FALSE;
                mySetCaretPos (self, -1, -1);
                self->exFlags |= NCSF_MLEDIT_EX_SETFOCUS;
                self->exFlags |= ~NCSF_MLEDIT_EX_KILLFOCUS;
                InvalidateRect (self->hwnd, NULL, TRUE);
                break;
            }
        case MSG_KILLFOCUS:
            {
                self->flags &= ~NCSF_MLEDIT_FOCUSED;
                HideCaret (self->hwnd);
                if (self->contDirty == TRUE) {
                    ncsNotifyParent ((mWidget *)self, NCSN_EDIT_CONTCHANGED);
                    self->contDirty = FALSE;
                }
                self->exFlags &= ~NCSF_MLEDIT_EX_SETFOCUS;
                self->exFlags |= NCSF_MLEDIT_EX_KILLFOCUS;
                if (!(GetWindowStyle(self->hwnd) & NCSS_EDIT_NOHIDESEL)) {
                    te_unselect_all (self, FALSE);
                }
                InvalidateRect (self->hwnd, NULL, TRUE);
                break;
            }
        case MSG_CHAR:
        {
            if (wParam == 127) /* BS */
                wParam = '\b';

            if (!(_c(self)->onChar (self, wParam, lParam)))
                return 0;

            break;
        }

    }

    return Class(mEdit).wndProc((mEdit*)self, message, wParam, lParam);
}


static void mMlEdit_onPaint (mMlEdit *self, HDC hdc, const PCLIPRGN pclip)
{
     RECT rcDraw, rcVis;
     int h, indent = 0;

     _c(self)->getVisRect (self, &rcVis);

     ClipRectIntersect (hdc, &rcVis);

     rcDraw.left = -self->contX;
     rcDraw.top = -self->contY;

     if (self->contWidth > self->visWidth)
         rcDraw.right = rcDraw.left + self->contWidth;
     else
         rcDraw.right = rcDraw.left + self->visWidth;

     if (self->contHeight > self->visHeight)
         rcDraw.bottom = rcDraw.top + self->contHeight;
     else
         rcDraw.bottom = rcDraw.top + self->visHeight;

     _c(self)->viewportToWindow (self, &rcDraw.left, &rcDraw.top);
     _c(self)->viewportToWindow (self, &rcDraw.right, &rcDraw.bottom);

     h = self->nLineHeight - 1;

    if (GetWindowStyle(self->hwnd) & NCSS_EDIT_BASELINE) {
        SetPenColor (hdc, ncsColor2Pixel(hdc, ncsGetElement(self, NCS_FGC_WINDOW)));
        while (h < RECTH(rcDraw)) {
            indent = (h == self->nLineHeight - 1) ? self->titleIndent : 0;
            DrawHDotLine (hdc, rcDraw.left + indent, rcDraw.top+h, RECTW(rcDraw)-indent);
            h += self->nLineHeight;
        }
    }

     _c(self)->super->onPaint ((mEdit *)self, hdc, pclip);

     if ((self->exFlags & NCSF_MLEDIT_EX_SETFOCUS)){
         self->exFlags &= ~NCSF_MLEDIT_EX_SETFOCUS;
     }
     else if (self->exFlags & NCSF_MLEDIT_EX_KILLFOCUS) {
         self->exFlags &= ~NCSF_MLEDIT_EX_KILLFOCUS;
     }
}

static void mMlEdit_onChar (mMlEdit *self, WPARAM wParam, LPARAM lParam)
{
    int chlen;
    unsigned char ch[3];
    TextNode *node;
    TextMark *mark;
    TextDoc *txtDoc;
    int old_caret_pos;
    RECT rcInv;
    int oldlinenr, newlinenr, i;
    DWORD dwStyle = GetWindowStyle(self->hwnd);

    if (dwStyle & NCSS_EDIT_READONLY) {
        Ping();
        return;
    }

    ch [0] = LOBYTE (wParam);
    ch [1] = HIBYTE (wParam);
    ch [2] = (0x0ff0000 & wParam) >> 16;

    if (ch[2]) {
        chlen = 3;
    }
    else if (ch[1]) {
        chlen = 2;
    }
    else {
        chlen = 1;

        if ( ch[0] < 0x20 && ch[0] != '\b' && ch[0] != '\t'
                && ch[0] != '\n' && ch[0] != '\r' ) {
            return;
        }

        if (dwStyle & NCSS_EDIT_UPPERCASE) {
            ch [0] = toupper (ch[0]);
        }
        else if (dwStyle & NCSS_EDIT_LOWERCASE) {
            ch [0] = tolower (ch[0]);
        }

        /* ENTER */
        if (ch [0] == '\r') {
            ch [0] = self->txtDoc.lnsep;
        }
    }

    node = self->txtDoc.insert.curNode;
    txtDoc = &(self->txtDoc);
    mark = GETMARK(txtDoc->selection.curNode);
    old_caret_pos = mark->pos_lnOff;

    oldlinenr = NODE_LINENR(node);
    testr_copy(&(self->teBuff), &(node->content));

    if (self->hardLimit == -1 || (self->curLen + chlen <= self->hardLimit)
            || (chlen == 1 && (ch[0] == '\b' || ch[0] == 127))) {
        if (textdoc_insert_string (self, &self->txtDoc, (const char*)ch, chlen)) {
            teSetCaretPosOnChar (self, old_caret_pos);

            rcInv.left   = self->wNoChanged;
            rcInv.top    = self->nrDiffLine * self->nLineHeight;
            rcInv.right  = self->contWidth;
            rcInv.bottom = rcInv.top + self->nLineHeight - self->nLineBaseH;
            REFRESH_NODE_EX(node, &rcInv);

            newlinenr = NODE_LINENR(node);
            rcInv.left = 0;
            for (i = self->nrDiffLine + 1; i < MAX(oldlinenr, newlinenr); i++) {
                rcInv.top += self->nLineHeight;
                rcInv.bottom = rcInv.top + self->nLineHeight  - self->nLineBaseH;
                REFRESH_NODE_EX(node, &rcInv);
            }
            UpdateWindow(self->hwnd, TRUE);
        }
    }
    else {
        Ping ();
        ncsNotifyParent ((mWidget *)self, NCSN_EDIT_MAXTEXT);
    }

    if (node != self->txtDoc.insert.curNode) {
        node = self->txtDoc.insert.curNode;
        teNodeChange (&self->txtDoc, FALSE);
        teSetCaretPos (self);
        REFRESH_NODE(node);
    }

    teSetCaretPos (self);
    ncsNotifyParent ((mWidget *)self, NCSN_EDIT_CHANGE);
    self->contDirty = TRUE;
    return ;
}

static int mMlEdit_onLButtonDown (mMlEdit *self, int x, int y, DWORD key_flag)
{
    int mouseX = x;
    int mouseY = y;
    HSVITEM hsvi = 0;
    int nItem;
    POINT pt, itm_pt = {0,0};
    TextNode *node = NULL;
    TextDoc *txtDoc = &self->txtDoc;
    TextMark *selection = &txtDoc->selection;

    if (!GetCapture()) {
        SetCapture(self->hwnd);
        self->flags |= NCSF_MLEDIT_SELECT;
    }

    _c(self)->windowToViewport (self, &mouseX, &mouseY);
    _c(self)->viewportToContent (self, &mouseX, &mouseY);
    pt.x = mouseX;
    pt.y = mouseY;
    nItem = _c(self)->inItem (self, mouseX, mouseY, &hsvi, &itm_pt);
    if (nItem >= 0) {
        _c(self)->makePosVisible (self, pt.x, pt.y);
        pt.y -= itm_pt.y;
    }

    if (hsvi)
        node = (TextNode*)_c(self)->getAddData(self, hsvi);

    if (selection->curNode) {
        te_unselect_all (self, !(key_flag & KS_SHIFT));
    }

    if (!(key_flag & KS_SHIFT)) {
        if (node)
            self->desCaretX = pt.x;
        teOnMouseDown (self, node, &pt, itm_pt.y, FALSE);

        ncsNotifyParent ((mWidget *)self, NCSN_WIDGET_CLICKED);
        ncsNotifyParent ((mWidget *)self, NCSN_EDIT_CHANGE);

    }
    else {
        teOnMouseDown (self, node, &pt, itm_pt.y, TRUE);
        te_set_selection (self);
    }

    return 0;
}

static int mMlEdit_onLButtonUp (mMlEdit *self, int x, int y, DWORD key_flag)
{
    if (GetCapture() == self->hwnd)
        ReleaseCapture();
    if (self->flags & NCSF_MLEDIT_SELECT)
        self->flags &= ~NCSF_MLEDIT_SELECT;
    if (self->flags & NCSF_MLEDIT_MOVE)
        self->flags &= ~NCSF_MLEDIT_MOVE;

    ncsNotifyParent ((mWidget *)self, NCSN_WIDGET_CLICKED);
    return 0;
}

static int mMlEdit_onMouseMove(mMlEdit* self, int x, int y, DWORD key_flags)
{
    LPARAM lParam;
    POINT pt;
    RECT rcVis;

    if (GetCapture() != self->hwnd || !(self->flags & NCSF_MLEDIT_SELECT))
        return 0;

    lParam = MAKELONG(x, y);

    ScreenToClient (self->hwnd, &x, &y);

    _c(self)->getVisRect (self, &rcVis);

    if (!PtInRect (&rcVis, x, y)) {
        if ( !(self->flags & NCSF_MLEDIT_MOVE) ) {
            self->flags |= NCSF_MLEDIT_MOVE;
            SetAutoRepeatMessage (self->hwnd, MSG_MOUSEMOVE, 0, lParam);
        }
    }
    else {
        if ( self->flags & NCSF_MLEDIT_MOVE ) {
            self->flags &= ~NCSF_MLEDIT_MOVE;
            SetAutoRepeatMessage (0, 0, 0, 0);
        }
    }

    _c(self)->windowToViewport (self, &x, &y);
    _c(self)->viewportToContent (self, &x, &y);

    pt.x = x;
    pt.y = y;

    te_cursor_move (self, &pt);

    return 0;
}

static int mMlEdit_onKeyDown(mMlEdit* self, int scancode, DWORD key_flags)
{
    switch (scancode)
    {
        case SCANCODE_REMOVE:
            _c(self)->onChar (self, 127, 0);
            return 0;
        case SCANCODE_CURSORBLOCKLEFT:
        case SCANCODE_CURSORBLOCKRIGHT:
        case SCANCODE_HOME:
        case SCANCODE_END:
            te_cursor_shift (self, scancode, key_flags & KS_SHIFT);
            return 0;
        case SCANCODE_CURSORBLOCKUP:
            te_cursor_updown (self, -1, key_flags & KS_SHIFT);
            return 0;
        case SCANCODE_CURSORBLOCKDOWN:
            te_cursor_updown (self, 1, key_flags & KS_SHIFT);
            return 0;
        case SCANCODE_C:
            if (key_flags & KS_CTRL)
                _c(self)->copy(self);
            return 0;
        case SCANCODE_V:
            if (key_flags & KS_CTRL)
                _c(self)->paste(self);
            return 0;
        case SCANCODE_X:
            if (key_flags & KS_CTRL)
                _c(self)->cut(self);
            return 0;
        case SCANCODE_A:
            if (key_flags & KS_CTRL) {
                te_select_all (self);
            }
            return 0;
        case SCANCODE_Z:
            if (key_flags & KS_CTRL)
                _c(self)->undo(self);
            return 0;
        case SCANCODE_R:
            if (key_flags & KS_CTRL)
                _c(self)->redo(self);
            return 0;
    }
    return 0;
}

static void mMlEdit_setContent (mMlEdit *self, const char* str, int start, int len)
{
    int i;
    char* buffer;
    DWORD dwStyle;

    if (!str /*|| len == 0*/)
        return;

    if (len == -1 || len > strlen(str)-start)
    {
        buffer = strdup(str + start);
    } else {
        buffer = (char *)calloc(len + 1 ,sizeof(char));
        strncpy(buffer, str + start, len);
    }
    dwStyle = GetWindowStyle(self->hwnd);

    if (dwStyle & NCSS_EDIT_UPPERCASE) {
        for (i = 0; i < strlen(buffer); i ++)
            buffer[i] = toupper (buffer[i]);
    } else if (dwStyle & NCSS_EDIT_LOWERCASE) {
        for (i = 0; i < strlen(buffer); i ++)
            buffer[i] = tolower (buffer[i]);
    }

    textedit_reset_content (self, buffer, TRUE);

    free (buffer);
}

static void mMlEdit_replaceText(mMlEdit *self, const char* str,
        int start, int len, int replace_start, int replace_end)
{
    int i;
    char* buffer;
    DWORD dwStyle;
    int total_len;

    if (!str || len == 0)
        return;

    total_len = _c(self)->getTextLength(self);

    if (replace_start < 0 || replace_start > total_len)
        replace_start = total_len;

    if (replace_end < 0 || replace_end > total_len)
        replace_end = total_len;

    if (replace_end < replace_start)
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

    _c(self)->setSel(self, replace_start, replace_end);

    textedit_insert_text (self, buffer, strlen(buffer));
}

static void mMlEdit_insert(mMlEdit *self, const char* str, int start, int len, int start_at)
{
    _c(self)->replaceText(self, str, start, len, start_at, start_at);
}

static void mMlEdit_append(mMlEdit *self, const char* str, int start, int len)
{
    _c(self)->replaceText(self, str, start, len, -1, -1);
}

static int mMlEdit_getTextLength(mMlEdit *self)
{
    list_t *me;
    TextNode *node;
    int total_len = 0;

    list_for_each (me, &self->txtDoc.queue) {
        node = list_entry (me, TextNode, list);
        total_len += node->content.txtlen;
    }

    return total_len;
}

static int mMlEdit_getContent(mMlEdit *self, char *strbuff, int len, int start, int end)
{
    list_t *me;
    TextNode *node;
    char *pch = strbuff;
    int t_len = 0, copy_len = 0, total_len ;

    total_len = _c(self)->getTextLength(self);

    if (end < 0 || end > total_len)
        end = total_len;

    if(start < 0 || start > total_len || start > end)
        return 0;

    len = MIN (len, end - start);

    if (!strbuff || len <= 0)
        return 0;

    list_for_each (me, &self->txtDoc.queue) {
        node = list_entry (me, TextNode, list);
        if (start > node->content.txtlen){
            start -= node->content.txtlen;
            break;
        }
        copy_len = MIN(node->content.txtlen - start, len - t_len);
        if (copy_len <= 0) break;
        memcpy (pch, node->content.string + start, copy_len);
        start = 0;
        pch += copy_len;
        t_len += copy_len;
    }
    *pch = '\0';

    return t_len;
}

static int mMlEdit_setTextAttr(mMlEdit *self, int start, int end, TextAttribute *attr)
{
    //TODO
    return 0;
}

static TextAttribute* mMlEdit_getTextAttr(mMlEdit *self, int start, int end, int *p_end)
{
    //TODO
    return NULL;
}

static int mMlEdit_setSel(mMlEdit *self, int start, int end)
{
    int total_len;
    int line, row;

    total_len = _c(self)->getTextLength(self);

    if (start < 0 || start > total_len)
        start = total_len;

    if (end < 0 || end > total_len)
        end = total_len;

    if (start > end)
        return -1;

    real_to_line_pos (self, start, &line, &row);
    textedit_set_pos_ex (self, line, row, FALSE);

    real_to_line_pos (self, end, &line, &row);
    textedit_set_pos_ex (self, line, row, TRUE);
    return 0;
}

static int mMlEdit_getSel(mMlEdit *self, int *pstart, int *pend)
{
    int line, row;
    int _pos1, _pos2;

    textedit_get_pos_ex (self, &line, &row, TRUE);
    _pos1 = line_to_real_pos(self, line, row);

    textedit_get_pos_ex (self, &line, &row, FALSE);
    _pos2 = line_to_real_pos(self, line, row);

    if (_pos1 < _pos2)
    {
        *pstart = _pos1;
        *pend = _pos2;
    } else {
        *pstart = _pos2;
        *pend = _pos1;
    }

    return 0;
}

static RECT *mMlEdit_setMargin(mMlEdit *self, int left, int top, int right, int bottom)
{
    self->leftMargin   = left;
    self->topMargin    = top;
    self->rightMargin  = right;
    self->bottomMargin = bottom;

    InvalidateRect(self->hwnd, NULL, TRUE);
    return 0;
}

static void mMlEdit_copy (mMlEdit *self)
{
    char *buff;
    int start, end, len;

    _c(self)->getSel(self, &start, &end);
    len = end - start;
    buff = (char *)calloc (len + 1, sizeof(char));
    _c(self)->getContent(self, buff, len, start, end);
    self->cp->addCopyInfo(buff, len, NULL, NULL);

    free (buff);
}

static void mMlEdit_cut (mMlEdit *self)
{
    _c(self)->copy(self);
    _c(self)->onChar (self, 127, 0);
}

static void mMlEdit_paste (mMlEdit *self)
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
            textedit_insert_text (self, (char *)pi->str, pi->len);
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

static TextCopyPaste *mMlEdit_setCopyPaste(mMlEdit *self, TextCopyPaste *cp)
{
    TextCopyPaste *old = self->cp;

    if(cp != NULL)
        self->cp = cp;

    return old;
}

static void mMlEdit_undo (mMlEdit *self)
{
    //TODO
}

static void mMlEdit_redo (mMlEdit *self)
{
    //TODO
}

static TextUndoRedoInfo *mMlEdit_setUndoRedo(mMlEdit *self, TextUndoRedoInfo *ur)
{
    //TODO
    return NULL;
}

static void mMlEdit_makevisible (mMlEdit *self, int pos)
{
    int line, row;
    HSVITEM item;

    real_to_line_pos (self, pos, &line, &row);
    item = _c(self)->getItem(self, line);
    _c(self)->showItem(self, item);
}


static void mMlEdit_resetContent(mMlEdit *self)
{
    textedit_reset_content (self, "", FALSE);
}

static void mMlEdit_moveContent (mMlEdit *self)
{
    TextDoc *txtDoc = &self->txtDoc;
    TextNode *node = FIRSTNODE(txtDoc);

    /* reset caret pos in the real window */
    if (self->flags & NCSF_MLEDIT_FOCUSED)
        mySetCaretPos (self, -1, -1);

    while (node) {
    	txtDoc->change_cont (txtDoc, node);
        node = TXTNODE_NEXT(node);
    }
    _c(self)->super->moveContent((mEdit *)self);
}

static int mMlEdit_setTitle (mMlEdit *self, const char *newTitle, int tLen)
{
    int len;
    HDC hdc;
    SIZE txtsize = {0, 0};

    if (!self->title || !newTitle
            || !(GetWindowStyle(self->hwnd) & NCSS_MLEDIT_TITLE))
        return -1;

    if (tLen >= 0)
        len = (tLen > DEF_TITLE_LEN) ? DEF_TITLE_LEN : tLen;
    else
        len = DEF_TITLE_LEN;

    strncpy (self->title, newTitle, len);
    self->title[len] = '\0';

    hdc = GETDC(self);
    SelectFont (hdc, GetWindowFont(self->hwnd));
    GetTextExtent(hdc, self->title, strlen(self->title), &txtsize);
    self->titleIndent = txtsize.cx;
    RELEASEDC(self, hdc);

    mySetCaretPos (self, self->titleIndent, 0 + self->nLineAboveH);

    InvalidateRect(self->hwnd, NULL, TRUE);

    return strlen (self->title);
}

static int mMlEdit_getTitle (mMlEdit *self, char *buffer, int tlen)
{
   int len, title_len;

    if (!self->title
            || !(GetWindowStyle(self->hwnd) & NCSS_MLEDIT_TITLE))
        return -1;

    title_len = strlen (self->title);

    if (!buffer)
        return title_len;

    if (tlen >= 0)
        len = (tlen > DEF_TITLE_LEN) ? DEF_TITLE_LEN : tlen;
    else
        len = DEF_TITLE_LEN;

    strncpy (buffer, self->title, len);
    buffer[len] = '\0';

    return title_len;
}

static BOOL mMlEdit_setProperty (mMlEdit *self, int id, DWORD value)
{
    if(id >= NCSP_MLEDIT_MAX)
        return FALSE;

    switch (id)
    {
        case NCSP_EDIT_LIMITTEXT :
            {
                int newLimit = (int)value;

                if (newLimit >= 0) {
                    if (self->curLen > newLimit) {
                        Ping ();
                        return FALSE;
                    }
                    else {
                        self->hardLimit = newLimit;
                        return TRUE;
                    }
                }
                return FALSE;
            }
        case NCSP_EDIT_CARETPOS :
            {
                int line, row;

                real_to_line_pos (self, (int)value, &line, &row);
                textedit_set_pos_ex (self, line, row, FALSE);

                return TRUE;
            }
        case NCSP_MLEDIT_LINECOUNT :
            return FALSE;
        case NCSP_MLEDIT_LINEHEIGHT :
            {
                int new_h;
                int height = (int)value;

                if (height <= 0)
                    return FALSE;

                new_h = GetWindowFont(self->hwnd)->size + DEF_LINE_BASE_H + MIN_LINE_ABOVE_H;
                new_h = MAX (new_h, height);

                if (new_h == self->nLineHeight)
                    return FALSE;

                self->nLineHeight = new_h;
                self->nLineAboveH = new_h - GetWindowFont(self->hwnd)->size - self->nLineBaseH;

                //textedit_reset_content (self, "", FALSE);

				{
					int txtLen = mMlEdit_getTextLength(self);
					char *oldText = (char *)malloc(txtLen * sizeof(char) + 1);
					mMlEdit_getContent(self, oldText, txtLen, 0, -1);
					textedit_reset_content (self, oldText, FALSE);
					free(oldText);
				}
                return TRUE;

            }
        case NCSP_MLEDIT_LINEFEEDISPCHAR :
            {
                self->lnChar = (unsigned char)value;
                _c(self)->refreshRect(self, NULL);
                return TRUE;
            }
        case NCSP_MLEDIT_LINESEP :
            {
                int length = _c(self)->getTextLength(self);
				self->txtDoc.lnsep = (unsigned char)value;
                if (length) {
                    char* text = calloc(1, length + 1);
                    if (text) {
                        if (GetWindowText(self->hwnd, text, length)) {
                            SetWindowText(self->hwnd, text);
                        }
                    }
                }
                return TRUE;
            }
        case NCSP_MLEDIT_CARETSHAPE:
            {
				int width, height;
                if (value == self->caretShape)
                    return FALSE;

                self->caretShape = (unsigned char)value;

                width = getCaretWidth(self);
                height = self->nLineHeight - self->nLineAboveH- self->nLineBaseH;
                if (!ChangeCaretSize (self->hwnd, width, height)) {
                    DestroyCaret (self->hwnd);
                    CreateCaret (self->hwnd, NULL, width, height);
                }
                mySetCaretPos (self, -1, -1);
                return TRUE;
            }
        case NCSP_MLEDIT_NUMOFPARAGRAPHS :
            return FALSE;
    }

	return Class(mEdit).setProperty ((mEdit*)self, id, value);
}

static DWORD mMlEdit_getProperty (mMlEdit *self, int id)
{
    if(id >= NCSP_MLEDIT_MAX)
        return FALSE;

    switch (id)
    {
        case NCSP_EDIT_LIMITTEXT :
            return self->hardLimit;
        case NCSP_EDIT_CARETPOS :
            {
                int line, row;

                textedit_get_pos_ex (self, &line, &row, FALSE);
                return (DWORD)line_to_real_pos(self, line, row);
            }
        case NCSP_MLEDIT_LINECOUNT :
            return (DWORD)_c(self)->getTotalHeight(self) / self->nLineHeight;
        case NCSP_MLEDIT_LINEHEIGHT :
            return (DWORD)self->nLineHeight;
        case NCSP_MLEDIT_LINEFEEDISPCHAR :
            return (DWORD)self->lnChar;
        case NCSP_MLEDIT_LINESEP :
            return (DWORD)self->txtDoc.lnsep;
        case NCSP_MLEDIT_CARETSHAPE :
            return (DWORD)self->caretShape;
        case NCSP_MLEDIT_NUMOFPARAGRAPHS :
            {
                list_t *me;
                int total_num = 0;

                list_for_each (me, &self->txtDoc.queue) {
                    total_num ++;
                }

                return (DWORD)total_num;
            }
    }

 	return Class(mEdit).getProperty ((mEdit*)self, id);
}


static BOOL mMlEdit_autoWrapChanged(mMlEdit *self)
{
    int length = _c(self)->getTextLength(self);
    char *text;

    text = calloc(length, 1);
    if (!text)
        return FALSE;

    GetWindowText(self->hwnd, text, length);
    SetWindowText(self->hwnd, text);
    return TRUE;
}

#ifdef _MGNCS_GUIBUILDER_SUPPORT
static BOOL mMlEdit_refresh(mWidget *self)
{
    return mMlEdit_autoWrapChanged((mMlEdit*)self);
}
#endif

BEGIN_CMPT_CLASS (mMlEdit, mEdit)
    CLASS_METHOD_MAP (mMlEdit, construct)
    CLASS_METHOD_MAP (mMlEdit, destroy)
    CLASS_METHOD_MAP (mMlEdit, setProperty)
    CLASS_METHOD_MAP (mMlEdit, getProperty)
    CLASS_METHOD_MAP (mMlEdit, wndProc)
    CLASS_METHOD_MAP (mMlEdit, onPaint)
    CLASS_METHOD_MAP (mMlEdit, onChar)
    CLASS_METHOD_MAP (mMlEdit, onLButtonDown)
    CLASS_METHOD_MAP (mMlEdit, onLButtonUp)
    CLASS_METHOD_MAP (mMlEdit, onMouseMove)
    CLASS_METHOD_MAP (mMlEdit, onKeyDown)
    CLASS_METHOD_MAP (mMlEdit, setContent)
    CLASS_METHOD_MAP (mMlEdit, replaceText)
    CLASS_METHOD_MAP (mMlEdit, insert)
    CLASS_METHOD_MAP (mMlEdit, append)
    CLASS_METHOD_MAP (mMlEdit, getTextLength)
    CLASS_METHOD_MAP (mMlEdit, getContent)
    CLASS_METHOD_MAP (mMlEdit, setTextAttr)
    CLASS_METHOD_MAP (mMlEdit, getTextAttr)
    CLASS_METHOD_MAP (mMlEdit, setSel)
    CLASS_METHOD_MAP (mMlEdit, getSel)
    CLASS_METHOD_MAP (mMlEdit, setMargin)
    CLASS_METHOD_MAP (mMlEdit, copy)
    CLASS_METHOD_MAP (mMlEdit, cut)
    CLASS_METHOD_MAP (mMlEdit, paste)
    CLASS_METHOD_MAP (mMlEdit, setCopyPaste)
    CLASS_METHOD_MAP (mMlEdit, undo)
    CLASS_METHOD_MAP (mMlEdit, redo)
    CLASS_METHOD_MAP (mMlEdit, setUndoRedo)
    CLASS_METHOD_MAP (mMlEdit, makevisible)
    CLASS_METHOD_MAP (mMlEdit, resetContent)
    CLASS_METHOD_MAP (mMlEdit, moveContent)
    CLASS_METHOD_MAP (mMlEdit, setTitle)
    CLASS_METHOD_MAP (mMlEdit, getTitle)
#ifdef _MGNCS_GUIBUILDER_SUPPORT
	CLASS_METHOD_MAP(mMlEdit, refresh)
#endif
END_CMPT_CLASS

#endif /*ifdef _MGNCS_OLD_MLEDIT*/

