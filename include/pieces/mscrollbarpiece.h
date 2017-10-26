#ifdef _MGNCSCTRL_SCROLLBAR

#ifndef _MGUI_NCSCTRL_SCROLLBARPIECE_H
#define _MGUI_NCSCTRL_SCROLLBARPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mScrollBarPieceClass mScrollBarPieceClass;
typedef struct _mScrollBarPiece mScrollBarPiece;

#define mScrollBarPieceClassHeader(clss, superCls) \
	mRenderablePieceClassHeader(clss, superCls)

struct _mScrollBarPieceClass
{
	mScrollBarPieceClassHeader(mScrollBarPiece, mRenderablePiece)
};

MGNCS_EXPORT extern mScrollBarPieceClass g_stmScrollBarPieceCls;

#define mScrollBarPieceHeader(clss) \
	mRenderablePieceHeader(clss)

struct _mScrollBarPiece
{
	mScrollBarPieceHeader(mScrollBarPiece)
};


#ifdef __cplusplus
}
#endif

#endif
#endif //_MGNCSCTRL_SCROLLBAR
