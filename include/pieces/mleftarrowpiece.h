
#ifndef _MGUI_NCSCTRL_LEFTARROWPIECE_H
#define _MGUI_NCSCTRL_LEFTARROWPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mLeftArrowPieceClass mLeftArrowPieceClass;
typedef struct _mLeftArrowPiece mLeftArrowPiece;

#define mLeftArrowPieceClassHeader(clss, superCls) \
	mArrowPieceClassHeader(clss, superCls) 

struct _mLeftArrowPieceClass
{
	mLeftArrowPieceClassHeader(mLeftArrowPiece, mArrowPiece)
};

MGNCS_EXPORT extern mLeftArrowPieceClass g_stmLeftArrowPieceCls;

#define mLeftArrowPieceHeader(clss) \
	mArrowPieceHeader(clss) 
	
struct _mLeftArrowPiece
{
	mLeftArrowPieceHeader(mLeftArrowPiece)
};


#ifdef __cplusplus
}
#endif

#endif

