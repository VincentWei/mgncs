#ifdef _MGNCSCTRL_SCROLLBAR

#ifndef _MGUI_NCSCTRL_SCROLLTHUMBBOXPIECE_H
#define _MGUI_NCSCTRL_SCROLLTHUMBBOXPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mScrollThumbBoxPieceClass mScrollThumbBoxPieceClass;
typedef struct _mScrollThumbBoxPiece mScrollThumbBoxPiece;

#define mScrollThumbBoxPieceClassHeader(clss, superCls) \
	mRenderablePieceClassHeader(clss, superCls)

struct _mScrollThumbBoxPieceClass
{
	mScrollThumbBoxPieceClassHeader(mScrollThumbBoxPiece, mRenderablePiece)
};

MGNCS_EXPORT extern mScrollThumbBoxPieceClass g_stmScrollThumbBoxPieceCls;

#define mScrollThumbBoxPieceHeader(clss) \
	mRenderablePieceHeader(clss)

struct _mScrollThumbBoxPiece
{
	mScrollThumbBoxPieceHeader(mScrollThumbBoxPiece)
};


#ifdef __cplusplus
}
#endif

#endif
#endif //_MGNCSCTRL_SCROLLBAR
