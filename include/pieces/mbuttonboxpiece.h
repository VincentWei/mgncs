
#ifndef _MGUI_NCSCTRL_BUTTONBOXPIECE_H
#define _MGUI_NCSCTRL_BUTTONBOXPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mButtonBoxPieceClass mButtonBoxPieceClass;
typedef struct _mButtonBoxPiece mButtonBoxPiece;

#define mButtonBoxPieceClassHeader(clss, superCls) \
	mRenderablePieceClassHeader(clss, superCls) 

struct _mButtonBoxPieceClass
{
	mButtonBoxPieceClassHeader(mButtonBoxPiece, mRenderablePiece)
};

MGNCS_EXPORT extern mButtonBoxPieceClass g_stmButtonBoxPieceCls;

#define mButtonBoxPieceHeader(clss) \
	mRenderablePieceHeader(clss) 
	
struct _mButtonBoxPiece
{
	mButtonBoxPieceHeader(mButtonBoxPiece)
};


#ifdef __cplusplus
}
#endif

#endif

