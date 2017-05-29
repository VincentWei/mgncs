
#ifndef _MGUI_NCSCTRL_ARROWPIECE_H
#define _MGUI_NCSCTRL_ARROWPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mArrowPieceClass mArrowPieceClass;
typedef struct _mArrowPiece mArrowPiece;

#define mArrowPieceClassHeader(clss, superCls) \
	mRenderablePieceClassHeader(clss, superCls) 

struct _mArrowPieceClass
{
	mArrowPieceClassHeader(mArrowPiece, mRenderablePiece)
};

MGNCS_EXPORT extern mArrowPieceClass g_stmArrowPieceCls;

#define mArrowPieceHeader(clss) \
	mRenderablePieceHeader(clss) 
	
struct _mArrowPiece
{
	mArrowPieceHeader(mArrowPiece)
};

#ifdef __cplusplus
}
#endif

#endif

