
#ifndef _MGUI_NCSCTRL_PAIRPIECE_H
#define _MGUI_NCSCTRL_PAIRPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mPairPieceClass mPairPieceClass;
typedef struct _mPairPiece mPairPiece;

#define mPairPieceClassHeader(clss, superCls) \
	mLayoutPieceClassHeader(clss, superCls)

struct _mPairPieceClass
{
	mPairPieceClassHeader(mPairPiece, mLayoutPiece)
};

MGNCS_EXPORT extern mPairPieceClass g_stmPairPieceCls;

#define mPairPieceHeader(clss) \
	mLayoutPieceHeader(clss)  \
	DWORD flags; \
	mHotPiece *first; \
	mHotPiece *second;

struct _mPairPiece
{
	mPairPieceHeader(mPairPiece)
};

#define NCS_PAIRPIECE_DIRECTION_MASK 0x80000000
#define NCS_PAIRPIECE_FIRST_MASK     0x40000000
#define NCS_PAIRPIECE_SIZE_TYPE_MASK 0x30000000
#define NCS_PAIRPIECE_SIZE_MASK      0x00000FFF
#define NCS_PAIRPIECE_SPACE_MASK     0x0FF00000
#define NCS_PAIRPIECE_MARGIN_MASK    0x000FF000

#define mPairPiece_setVert(self)   ((self)->flags |= NCS_PAIRPIECE_DIRECTION_MASK)
#define mPairPiece_setHorz(self)   ((self)->flags &= ~NCS_PAIRPIECE_DIRECTION_MASK)
#define mPairPiece_isVert(self)    ((self)->flags & NCS_PAIRPIECE_DIRECTION_MASK)

#define mPairPiece_setReverse(self)      ((self)->flags |= NCS_PAIRPIECE_FIRST_MASK)
#define mPairPiece_cancelReverse(self)   ((self)->flags &= ~NCS_PAIRPIECE_FIRST_MASK)
#define mPairPiece_reversed(self)        ((self)->flags & NCS_PAIRPIECE_FIRST_MASK)
//#define mPairPiece_getFirst(self)   (((self)->flags&NCS_PAIRPIECE_FIRST_MASK)?self->second:self->first)
#define mPairPiece_getFirst(self)   ((self)->first)
//#define mPairPiece_getSecond(self)   (((self)->flags&NCS_PAIRPIECE_FIRST_MASK)?self->first:self->second)
#define mPairPiece_getSecond(self)  ((self)->second)
#define mPairPiece_setFirst(self, piece)   do{ \
	if(((self)->flags&NCS_PAIRPIECE_FIRST_MASK)) \
		(self)->second=(piece); \
	else \
		(self)->first=(piece); \
}while(0)

#define mPairPiece_setSecond(self, piece)   do{ \
	if(((self)->flags&NCS_PAIRPIECE_FIRST_MASK)) \
		(self)->first=(piece); \
	else \
		(self)->second=(piece); \
}while(0)



enum mPairPieceSizeType{
	NCS_PAIRPIECE_ST_FIXED = NCS_LAYOUTPIECE_ST_FIXED,
	NCS_PAIRPIECE_ST_PERCENT = NCS_LAYOUTPIECE_ST_PERCENT,
	NCS_PAIRPIECE_ST_AUTO = NCS_LAYOUTPIECE_ST_AUTO
};

#define SET_BIT_VALUE(dword, value, mask,offset)  ((dword) = ((dword)& (~(mask))) | (((value)<<(offset))&mask))
#define GET_BIT_VALUE(dword, mask, offset)   (((dword)&(mask))>>(offset))

#define mPairPiece_setFirstSizeType(self, type)  SET_BIT_VALUE(self->flags, type, NCS_PAIRPIECE_SIZE_TYPE_MASK,28)
#define mPairPiece_getFirstSizeType(self)        GET_BIT_VALUE(self->flags, NCS_PAIRPIECE_SIZE_TYPE_MASK, 28)
#define mPairPiece_setFirstSize(self, size)      (self->flags = (self->flags&(~NCS_PAIRPIECE_SIZE_MASK))|((size)&NCS_PAIRPIECE_SIZE_MASK))
#define mPairPiece_getFirstSize(self)            (int)(self->flags&NCS_PAIRPIECE_SIZE_MASK)

#define mPairPiece_setSpace(self, space)         SET_BIT_VALUE(self->flags, space, NCS_PAIRPIECE_SPACE_MASK, 20)
#define mPairPiece_getSpace(self)                (int)GET_BIT_VALUE(self->flags, NCS_PAIRPIECE_SPACE_MASK, 20)

#define mPairPiece_setMargin(self, margin)       SET_BIT_VALUE(self->flags, margin, NCS_PAIRPIECE_MARGIN_MASK, 12)
#define mPairPiece_getMargin(self)               (int)GET_BIT_VALUE(self->flags, NCS_PAIRPIECE_MARGIN_MASK, 12)

enum mPairPieceProps{
	NCSP_PAIRPIECE_SECOND_AS_FIRST = PAIRPIECE_PROP_BEGIN,
	NCSP_PAIRPIECE_FIRST_SIZE_TYPE,
	NCSP_PAIRPIECE_FIRST_SIZE,
	NCSP_PAIRPIECE_FIRST,
	NCSP_PAIRPIECE_SECOND,
	NCSP_PAIRPIECE_DIRECTION = PIECECOMM_PROP_DIRECTION,
	NCSP_PAIRPIECE_SPACE     = PIECECOMM_PROP_SPACE,
	NCSP_PAIRPIECE_MARGIN    = PIECECOMM_PROP_MARGIN
};

#ifdef __cplusplus
}
#endif

#endif

