
#ifndef _MGUI_NCSCTRL_WIDGETWRAPPERPIECE_H
#define _MGUI_NCSCTRL_WIDGETWRAPPERPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mWidgetWrapperPieceClass mWidgetWrapperPieceClass;
typedef struct _mWidgetWrapperPiece mWidgetWrapperPiece;

#define mWidgetWrapperPieceClassHeader(clss, superCls) \
	mHotPieceClassHeader(clss, superCls)

struct _mWidgetWrapperPieceClass
{
	mWidgetWrapperPieceClassHeader(mWidgetWrapperPiece, mHotPiece)
};

MGNCS_EXPORT extern mWidgetWrapperPieceClass g_stmWidgetWrapperPieceCls;

#define mWidgetWrapperPieceHeader(clss) \
	mHotPieceHeader(clss)  \
	mObject * widget;

struct _mWidgetWrapperPiece
{
	mWidgetWrapperPieceHeader(mWidgetWrapperPiece)
};

#define MSG_SETPIECE_PROPERTY  MSG_USER + 1234
#define MSG_GETPIECE_PROPERTY  MSG_USER + 1235

#define RAISE_EVENT(obj,id, param)  _c(obj)->raiseEvent(obj, id, param)

#ifdef __cplusplus
}
#endif

#endif

