
#ifndef _MGUI_NCSCTRL_HBOXLAYOUTPIECE_H
#define _MGUI_NCSCTRL_HBOXLAYOUTPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mHBoxLayoutPieceClass mHBoxLayoutPieceClass;
typedef struct _mHBoxLayoutPiece mHBoxLayoutPiece;

#define mHBoxLayoutPieceClassHeader(clss, superCls) \
	mBoxLayoutPieceClassHeader(clss, superCls) 

struct _mHBoxLayoutPieceClass
{
	mHBoxLayoutPieceClassHeader(mHBoxLayoutPiece, mBoxLayoutPiece)
};

MGNCS_EXPORT extern mHBoxLayoutPieceClass g_stmHBoxLayoutPieceCls;

#define mHBoxLayoutPieceHeader(clss) \
	mBoxLayoutPieceHeader(clss) 
	
struct _mHBoxLayoutPiece
{
	mHBoxLayoutPieceHeader(mHBoxLayoutPiece)
};


#ifdef __cplusplus
}
#endif

#endif

