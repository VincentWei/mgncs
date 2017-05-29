
#ifndef _MGUI_NCSCTRL_RIGHTARROWPIECE_H
#define _MGUI_NCSCTRL_RIGHTARROWPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mRightArrowPieceClass mRightArrowPieceClass;
typedef struct _mRightArrowPiece mRightArrowPiece;

#define mRightArrowPieceClassHeader(clss, superCls) \
	mArrowPieceClassHeader(clss, superCls) 

struct _mRightArrowPieceClass
{
	mRightArrowPieceClassHeader(mRightArrowPiece, mArrowPiece)
};

MGNCS_EXPORT extern mRightArrowPieceClass g_stmRightArrowPieceCls;

#define mRightArrowPieceHeader(clss) \
	mArrowPieceHeader(clss) 
	
struct _mRightArrowPiece
{
	mRightArrowPieceHeader(mRightArrowPiece)
};


#ifdef __cplusplus
}
#endif

#endif

