
#ifndef _MGUI_NCSCTRL_HSPINBOXPIECE_H
#define _MGUI_NCSCTRL_HSPINBOXPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mHSpinBoxPieceClass mHSpinBoxPieceClass;
typedef struct _mHSpinBoxPiece mHSpinBoxPiece;

#define mHSpinBoxPieceClassHeader(clss, superCls) \
	mSpinBoxPieceClassHeader(clss, superCls)   

struct _mHSpinBoxPieceClass
{
	mHSpinBoxPieceClassHeader(mHSpinBoxPiece, mSpinBoxPiece)
};

MGNCS_EXPORT extern mHSpinBoxPieceClass g_stmHSpinBoxPieceCls;

#define mHSpinBoxPieceHeader(clss) \
	mSpinBoxPieceHeader(clss)  
	
struct _mHSpinBoxPiece
{
	mHSpinBoxPieceHeader(mHSpinBoxPiece)
};

#ifdef __cplusplus
}
#endif

#endif

