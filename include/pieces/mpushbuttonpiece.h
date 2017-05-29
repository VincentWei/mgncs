
#ifndef _MGUI_NCSCTRL_PUSHBUTTONPIECE_H
#define _MGUI_NCSCTRL_PUSHBUTTONPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mPushButtonPieceClass mPushButtonPieceClass;
typedef struct _mPushButtonPiece mPushButtonPiece;

#define mPushButtonPieceClassHeader(clss, superCls) \
	mAbstractButtonPieceClassHeader(clss, superCls)

struct _mPushButtonPieceClass
{
	mPushButtonPieceClassHeader(mPushButtonPiece, mAbstractButtonPiece)
};

MGNCS_EXPORT extern mPushButtonPieceClass g_stmPushButtonPieceCls;

#define mPushButtonPieceHeader(clss) \
	mAbstractButtonPieceHeader(clss) \
	mHotPiece * content;
	
struct _mPushButtonPiece
{
	mPushButtonPieceHeader(mPushButtonPiece)
};

#ifdef __cplusplus
}
#endif

#endif

