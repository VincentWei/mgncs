#ifdef _MGNCSCTRL_SPINBOX

#ifndef _MGUI_NCSCTRL_VSPINNERPIECE_H
#define _MGUI_NCSCTRL_VSPINNERPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mVSpinBoxPieceClass mVSpinBoxPieceClass;
typedef struct _mVSpinBoxPiece mVSpinBoxPiece;

#define mVSpinBoxPieceClassHeader(clss, superCls) \
	mSpinBoxPieceClassHeader(clss, superCls)

struct _mVSpinBoxPieceClass
{
	mVSpinBoxPieceClassHeader(mVSpinBoxPiece, mSpinBoxPiece)
};

MGNCS_EXPORT extern mVSpinBoxPieceClass g_stmVSpinBoxPieceCls;

#define mVSpinBoxPieceHeader(clss) \
	mSpinBoxPieceHeader(clss)

struct _mVSpinBoxPiece
{
	mVSpinBoxPieceHeader(mVSpinBoxPiece)
};

#ifdef __cplusplus
}
#endif

#endif
#endif //_MGNCSCTRL_SPINBOX
