
#ifndef _MGUI_NCSCTRL_ARROWBUTTONPIECE_H
#define _MGUI_NCSCTRL_ARROWBUTTONPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mArrowButtonPieceClass mArrowButtonPieceClass;
typedef struct _mArrowButtonPiece mArrowButtonPiece;

#define mArrowButtonPieceClassHeader(clss, superCls) \
	mAbstractButtonPieceClassHeader(clss, superCls)

struct _mArrowButtonPieceClass
{
	mArrowButtonPieceClassHeader(mArrowButtonPiece, mAbstractButtonPiece)
};

MGNCS_EXPORT extern mArrowButtonPieceClass g_stmArrowButtonPieceCls;

#define mArrowButtonPieceHeader(clss) \
	mAbstractButtonPieceHeader(clss) 
	
struct _mArrowButtonPiece
{
	mArrowButtonPieceHeader(mArrowButtonPiece)
};

/*
#define mArrowButtonPiece_setArrow(self, arrow) \
	((self)->revert = ((arrow)>>NCS_PIECE_PAINT_ARROW_SHIFT))
#define mArrowButtonPiece_getArrow(self) \
	((self)->revert<<NCS_PIECE_PAINT_ARROW_SHIFT)
*/

#ifdef __cplusplus
}
#endif

#endif

