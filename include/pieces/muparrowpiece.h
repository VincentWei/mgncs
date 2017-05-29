
#ifndef _MGUI_NCSCTRL_UPARROWPIECE_H
#define _MGUI_NCSCTRL_UPARROWPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mUpArrowPieceClass mUpArrowPieceClass;
typedef struct _mUpArrowPiece mUpArrowPiece;

#define mUpArrowPieceClassHeader(clss, superCls) \
	mArrowPieceClassHeader(clss, superCls) 

struct _mUpArrowPieceClass
{
	mUpArrowPieceClassHeader(mUpArrowPiece, mArrowPiece)
};

MGNCS_EXPORT extern mUpArrowPieceClass g_stmUpArrowPieceCls;

#define mUpArrowPieceHeader(clss) \
	mArrowPieceHeader(clss) 
	
struct _mUpArrowPiece
{
	mUpArrowPieceHeader(mUpArrowPiece)
};


#ifdef __cplusplus
}
#endif

#endif

