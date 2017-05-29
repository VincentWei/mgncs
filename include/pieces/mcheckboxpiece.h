
#ifndef _MGUI_NCSCTRL_CHECKBOXPIECE_H
#define _MGUI_NCSCTRL_CHECKBOXPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mCheckBoxPieceClass mCheckBoxPieceClass;
typedef struct _mCheckBoxPiece mCheckBoxPiece;

#define mCheckBoxPieceClassHeader(clss, superCls) \
	mRenderablePieceClassHeader(clss, superCls) 

struct _mCheckBoxPieceClass
{
	mCheckBoxPieceClassHeader(mCheckBoxPiece, mRenderablePiece)
};

MGNCS_EXPORT extern mCheckBoxPieceClass g_stmCheckBoxPieceCls;

#define mCheckBoxPieceHeader(clss) \
	mRenderablePieceHeader(clss) 
	
struct _mCheckBoxPiece
{
	mCheckBoxPieceHeader(mCheckBoxPiece)
};

enum mCheckBoxPieceProps{
	NCSP_CHECKBUTTONPIECE_CONTENT = CHECKBUTTONPIECE_PROP_BEGIN
};


#ifdef __cplusplus
}
#endif

#endif

