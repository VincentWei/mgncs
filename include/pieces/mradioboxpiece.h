
#ifndef _MGUI_NCSCTRL_RADIOBOXPIECE_H
#define _MGUI_NCSCTRL_RADIOBOXPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mRadioBoxPieceClass mRadioBoxPieceClass;
typedef struct _mRadioBoxPiece mRadioBoxPiece;

#define mRadioBoxPieceClassHeader(clss, superCls) \
	mRenderablePieceClassHeader(clss, superCls) 

struct _mRadioBoxPieceClass
{
	mRadioBoxPieceClassHeader(mRadioBoxPiece, mRenderablePiece)
};

MGNCS_EXPORT extern mRadioBoxPieceClass g_stmRadioBoxPieceCls;

#define mRadioBoxPieceHeader(clss) \
	mRenderablePieceHeader(clss) 
	
struct _mRadioBoxPiece
{
	mRadioBoxPieceHeader(mRadioBoxPiece)
};


//enum mCheckBoxPieceProps{
//	NCSP_CHECKBUTTONPIECE_CONTENT = CHECKBUTTONPIECE_PROP_BEGIN
//};

#ifdef __cplusplus
}
#endif

#endif

