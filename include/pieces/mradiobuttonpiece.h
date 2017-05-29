
#ifndef _MGUI_NCSCTRL_RADIOBUTTONPIECE_H
#define _MGUI_NCSCTRL_RADIOBUTTONPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mRadioButtonPieceClass mRadioButtonPieceClass;
typedef struct _mRadioButtonPiece mRadioButtonPiece;

#define mRadioButtonPieceClassHeader(clss, superCls) \
	mCheckButtonPieceClassHeader(clss, superCls)

struct _mRadioButtonPieceClass
{
	mRadioButtonPieceClassHeader(mRadioButtonPiece, mCheckButtonPiece)
};

MGNCS_EXPORT extern mRadioButtonPieceClass g_stmRadioButtonPieceCls;

#define mRadioButtonPieceHeader(clss) \
	mCheckButtonPieceHeader(clss)
	
struct _mRadioButtonPiece
{
	mRadioButtonPieceHeader(mRadioButtonPiece)
};

#ifdef __cplusplus
}
#endif

#endif

