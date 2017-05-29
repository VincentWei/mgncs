
#ifndef _MGUI_NCSCTRL_THUMBBOXPIECE_H
#define _MGUI_NCSCTRL_THUMBBOXPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mThumbBoxPieceClass mThumbBoxPieceClass;
typedef struct _mThumbBoxPiece mThumbBoxPiece;

#define mThumbBoxPieceClassHeader(clss, superCls) \
	mRenderablePieceClassHeader(clss, superCls) 

struct _mThumbBoxPieceClass
{
	mThumbBoxPieceClassHeader(mThumbBoxPiece, mRenderablePiece)
};

MGNCS_EXPORT extern mThumbBoxPieceClass g_stmThumbBoxPieceCls;

#define mThumbBoxPieceHeader(clss) \
	mRenderablePieceHeader(clss) 
	
struct _mThumbBoxPiece
{
	mThumbBoxPieceHeader(mThumbBoxPiece)
};


#ifdef __cplusplus
}
#endif

#endif

