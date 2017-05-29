
#ifndef _MGUI_NCSCTRL_LAYOUTPIECE_H
#define _MGUI_NCSCTRL_LAYOUTPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mLayoutPieceClass mLayoutPieceClass;
typedef struct _mLayoutPiece mLayoutPiece;

#define mLayoutPieceClassHeader(clss, superCls) \
	mHotPieceClassHeader(clss, superCls) 

struct _mLayoutPieceClass
{
	mLayoutPieceClassHeader(mLayoutPiece, mHotPiece)
};

MGNCS_EXPORT extern mLayoutPieceClass g_stmLayoutPieceCls;

#define mLayoutPieceHeader(clss) \
	mHotPieceHeader(clss) 
	
struct _mLayoutPiece
{
	mLayoutPieceHeader(mLayoutPiece)
};


#ifdef __cplusplus
}
#endif

#endif

