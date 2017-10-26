#ifdef _MGNCSCTRL_SPINBOX

#ifndef _MGUI_NCSCTRL_SPNRPIECE_H
#define _MGUI_NCSCTRL_SPNRPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mSpinBoxPieceClass mSpinBoxPieceClass;
typedef struct _mSpinBoxPiece mSpinBoxPiece;

#define mSpinBoxPieceClassHeader(clss, superCls) \
	mSpinnerPieceClassHeader(clss, superCls)   \
	mHotPiece *(*getSpinnedPiece)(clss *);

struct _mSpinBoxPieceClass
{
	mSpinBoxPieceClassHeader(mSpinBoxPiece, mSpinnerPiece)
};

MGNCS_EXPORT extern mSpinBoxPieceClass g_stmSpinBoxPieceCls;

#define mSpinBoxPieceHeader(clss) \
	mSpinnerPieceHeader(clss)

struct _mSpinBoxPiece
{
	mSpinBoxPieceHeader(mSpinBoxPiece)
};

#ifdef __cplusplus
}
#endif

#endif
#endif //_MGNCSCTRL_SPINBOX

