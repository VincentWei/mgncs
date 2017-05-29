
#ifndef _MGUI_NCSCTRL_DOWNARROWPIECE_H
#define _MGUI_NCSCTRL_DOWNARROWPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mDownArrowPieceClass mDownArrowPieceClass;
typedef struct _mDownArrowPiece mDownArrowPiece;

#define mDownArrowPieceClassHeader(clss, superCls) \
	mArrowPieceClassHeader(clss, superCls) 

struct _mDownArrowPieceClass
{
	mDownArrowPieceClassHeader(mDownArrowPiece, mArrowPiece)
};

MGNCS_EXPORT extern mDownArrowPieceClass g_stmDownArrowPieceCls;

#define mDownArrowPieceHeader(clss) \
	mArrowPieceHeader(clss) 
	
struct _mDownArrowPiece
{
	mDownArrowPieceHeader(mDownArrowPiece)
};


#ifdef __cplusplus
}
#endif

#endif

