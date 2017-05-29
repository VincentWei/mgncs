
#ifndef _MGUI_NCSCTRL_VBOXLAYOUTPIECE_H
#define _MGUI_NCSCTRL_VBOXLAYOUTPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mVBoxLayoutPieceClass mVBoxLayoutPieceClass;
typedef struct _mVBoxLayoutPiece mVBoxLayoutPiece;

#define mVBoxLayoutPieceClassHeader(clss, superCls) \
	mBoxLayoutPieceClassHeader(clss, superCls) 

struct _mVBoxLayoutPieceClass
{
	mVBoxLayoutPieceClassHeader(mVBoxLayoutPiece, mBoxLayoutPiece)
};

MGNCS_EXPORT extern mVBoxLayoutPieceClass g_stmVBoxLayoutPieceCls;

#define mVBoxLayoutPieceHeader(clss) \
	mBoxLayoutPieceHeader(clss) 
	
struct _mVBoxLayoutPiece
{
	mVBoxLayoutPieceHeader(mVBoxLayoutPiece)
};


#ifdef __cplusplus
}
#endif

#endif

