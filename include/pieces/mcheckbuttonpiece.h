
#ifndef _MGUI_NCSCTRL_CHECKBUTTONPIECE_H
#define _MGUI_NCSCTRL_CHECKBUTTONPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mCheckButtonPieceClass mCheckButtonPieceClass;
typedef struct _mCheckButtonPiece mCheckButtonPiece;

#define mCheckButtonPieceClassHeader(clss, superCls) \
	mAbstractButtonPieceClassHeader(clss, superCls)

struct _mCheckButtonPieceClass
{
	mCheckButtonPieceClassHeader(mCheckButtonPiece, mAbstractButtonPiece)
};

MGNCS_EXPORT extern mCheckButtonPieceClass g_stmCheckButtonPieceCls;

#define mCheckButtonPieceHeader(clss) \
	mAbstractButtonPieceHeader(clss)
	
struct _mCheckButtonPiece
{
	mCheckButtonPieceHeader(mCheckButtonPiece)
};

#ifdef __cplusplus
}
#endif

#endif

