
#ifndef _MGUI_NCSCTRL_BOXLAYOUTPIECE_H
#define _MGUI_NCSCTRL_BOXLAYOUTPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mBoxLayoutPieceClass mBoxLayoutPieceClass;
typedef struct _mBoxLayoutPiece mBoxLayoutPiece;

#define mBoxLayoutPieceClassHeader(clss, superCls) \
	mLayoutPieceClassHeader(clss, superCls) \
/** procted **/ \
	void (*recalcBox)(clss *); \
	int  (*getCellAutoSize)(clss *, int i); \
/** public **/ \
	int (*setCellInfo)(clss *, int idx, int size, int type/*=NCS_BOXLAYOUTPIECE_ST_FIXED*/, BOOL bupdate/*=FALSE*/); \
	mHotPiece* (*setCell)(clss *, int idx, mHotPiece *cell); \
	mHotPiece* (*getCell)(clss *, int idx);

struct _mBoxLayoutPieceClass
{
	mBoxLayoutPieceClassHeader(mBoxLayoutPiece, mLayoutPiece)
};

MGNCS_EXPORT extern mBoxLayoutPieceClass g_stmBoxLayoutPieceCls;

#define mBoxLayoutPieceHeader(clss) \
	mLayoutPieceHeader(clss)  \
	int left:16; \
	int top:16; \
	int right:16; \
	int bottom:16; \
	unsigned short count; \
	unsigned char  margin; \
	unsigned char  space; \
	unsigned short * cell_info; \
	mHotPiece ** cells;

	
struct _mBoxLayoutPiece
{
	mBoxLayoutPieceHeader(mBoxLayoutPiece)
};

#define mBoxLayoutPiece_getSizeType(size)  mLayoutPiece_getSizeType(size)
#define mBoxLayoutPiece_setSizeType(size, type) mLayoutPiece_setSizeType(size, type)
#define mBoxLayoutPiece_setSize(size, value)    mLayoutPiece_setSize(size, value)
#define mBoxLayoutPiece_getSize(size) mLayoutPiece_getSize(size)


enum mBoxLayoutPieceProps{
	NCSP_BOXLAYOUTPIECE_COUNT = BOXLAYOUTPIECE_PROP_BEGIN,
	NCSP_BOXLAYOUTPIECE_SPACE     = PIECECOMM_PROP_SPACE,
	NCSP_BOXLAYOUTPIECE_MARGIN    = PIECECOMM_PROP_MARGIN
};

/** protected functio */

BOOL mBoxLayoutPiece_calcSize(mBoxLayoutPiece *self, int total_size, int *sizes);

#ifdef __cplusplus
}
#endif

#endif

