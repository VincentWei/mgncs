
#ifndef _MGUI_NCSCTRL_RENDERABLEPIECE_H
#define _MGUI_NCSCTRL_RENDERABLEPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mRenderablePieceClass mRenderablePieceClass;
typedef struct _mRenderablePiece mRenderablePiece;

#define mRenderablePieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls) 

struct _mRenderablePieceClass
{
	mRenderablePieceClassHeader(mRenderablePiece, mStaticPiece)
};

extern mRenderablePieceClass g_stmRenderablePieceCls;

#define mRenderablePieceHeader(clss) \
	mStaticPieceHeader(clss)  \
	PHotPieceRdrFunc rdr_func;
	
struct _mRenderablePiece
{
	mRenderablePieceHeader(mRenderablePiece)
};


#ifdef __cplusplus
}
#endif

#endif

