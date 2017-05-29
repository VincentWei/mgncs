
#ifndef _MGNCS_HOT_PIECEC_H
#define _MGNCS_HOT_PIECEC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "piece-id.h"

typedef struct _mHotPieceClass mHotPieceClass;
typedef struct _mHotPiece mHotPiece;

#define mHotPieceClassHeader(clss, superCls) \
	mObjectClassHeader(clss, superCls) \
	mHotPiece* (*hitTest)(clss *,  int x, int y); \
	void (*paint)(clss *, HDC hdc, mObject * owner, DWORD add_data); \
	int (*processMessage)(clss *, int message, WPARAM, LPARAM, mObject *owner); \
	BOOL (*setProperty)(clss *, int id, DWORD value); \
	DWORD(*getProperty)(clss *, int id); \
	BOOL (*enable)(clss *, BOOL benable); \
	BOOL (*isEnabled)(clss *); \
	BOOL (*setRenderer)(clss *, const char* rdr_name); \
	BOOL (*setRect)(clss *, const RECT * prc); \
	BOOL (*getRect)(clss *, RECT * prc);  \
	BOOL (*autoSize)(clss *, mObject *owner, const SIZE *pszMin, const SIZE *pszMax);

struct _mHotPieceClass
{
	mHotPieceClassHeader(mHotPiece, mObject)
};

MGNCS_EXPORT extern mHotPieceClass g_stmHotPieceCls;

#define mHotPieceHeader(clss) \
    mObjectHeader(clss) \
    mHotPiece * parent;

struct _mHotPiece
{
	mHotPieceHeader(mHotPiece)
};

typedef void (*PHotPieceRdrFunc)(mHotPiece*, HDC hdc, mObject* owner, DWORD add_data);


#define SET_BIT(flag, mask)         ((flag)|=(mask))
#define IS_BIT_SET(flag, mask)      (((flag)&(mask)) == (mask))
#define CLEAR_BIT(flag, mask)       ((flag)&=~(mask))


MGNCS_EXPORT mHotPiece * ncsNewPiece(mHotPieceClass *_class, DWORD add_data);
#define NEWPIECEEX(Piece,add_data)   (Piece*)(ncsNewPiece((mHotPieceClass*)((void *)&(Class(Piece))),(DWORD)(add_data)))
#define NEWPIECE(Piece)  NEWPIECEEX(Piece, 0)
MGNCS_EXPORT void ncsDelPiece(mHotPiece* piece);
#define DELPIECE(p)  ncsDelPiece((mHotPiece*)p)


MGNCS_EXPORT void mHotPiece_update(mHotPiece *self, mObject *onwer, BOOL bUpdate);


#ifdef __cplusplus
}
#endif

#endif


