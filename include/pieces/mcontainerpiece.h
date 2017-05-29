
#ifndef _MGUI_NCSCTRL_PANELPIECE_H
#define _MGUI_NCSCTRL_PANELPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mContainerPieceClass mContainerPieceClass;
typedef struct _mContainerPiece mContainerPiece;

#define mContainerPieceClassHeader(clss, superCls) \
	mHotPieceClassHeader(clss, superCls) \
	void (*raiseEvent)(clss *, int event, DWORD param);

struct _mContainerPieceClass
{
	mContainerPieceClassHeader(mContainerPiece, mHotPiece)
};

MGNCS_EXPORT extern mContainerPieceClass g_stmContainerPieceCls;

#define mContainerPieceHeader(clss) \
	mHotPieceHeader(clss)  \
	mHotPiece * body;

struct _mContainerPiece
{
	mContainerPieceHeader(mContainerPiece)
};

#define RAISE_EVENT(obj,id, param)  _c(obj)->raiseEvent(obj, id, param)

#ifdef __cplusplus
}
#endif

#endif

