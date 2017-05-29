
#ifndef _MGUI_NCSCTRL_LINELAYOUTPIECE_H
#define _MGUI_NCSCTRL_LINELAYOUTPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mLineLayoutPieceClass mLineLayoutPieceClass;
typedef struct _mLineLayoutPiece mLineLayoutPiece;

#define NCS_LINELAYOUTPIECENODE_FLAG_FIXED 0x00 //the size of piece is fixed
#define NCS_LINELAYOUTPIECENODE_FLAG_AUTO   0x01 //the size of piece must bigger than def_size
typedef struct _mLineLayoutPieceNode {
	mHotPiece * piece;
	struct _mLineLayoutPieceNode * next, *prev;
	unsigned int flags:8;
	unsigned int def_min_size:12;
	unsigned int def_max_size:12;
}mLineLayoutPieceNode;

#define mLineLayoutPieceClassHeader(clss, superCls) \
	mLayoutPieceClassHeader(clss, superCls) \
	BOOL (*addPiece)(clss*, mHotPiece *piece, int type, int min_size, int max_size); \
	BOOL (*appendPiece)(clss*, mHotPiece *piece, int type, int min_size, int max_size); \
	void (*recalc)(clss*); \
	mLineLayoutPieceNode * (*find)(clss *,mHotPiece *piece); \
	void (*reset)(clss*, void (*free_piece)(mHotPiece*));

struct _mLineLayoutPieceClass
{
	mLineLayoutPieceClassHeader(mLineLayoutPiece, mLayoutPiece)
};

extern mLineLayoutPieceClass g_stmLineLayoutPieceCls;

#define mLineLayoutPieceHeader(clss) \
	mLayoutPieceHeader(clss)  \
	int left:16; \
	int top:16; \
	int right:16; \
	int bottom:16; \
	unsigned short direction:1; \
	unsigned short autoWrap:1; \
	unsigned short line_height:14; \
	unsigned char  margin; \
	unsigned char  space; \
	mLineLayoutPieceNode *node; \
	mLineLayoutPieceNode *tail_node;


struct _mLineLayoutPiece
{
	mLineLayoutPieceHeader(mLineLayoutPiece)
};

enum mLineLayoutPieceProps{
	NCSP_LINELAYOUTPIECE_COUNT = LINELAYOUTPIECE_PROP_BEGIN,
	NCSP_LINELAYOUTPIECE_LINE_HEIGHT,
	NCSP_LINELAYOUTPiECE_AUTOWRAP  = PIECECOMM_PROP_AUTOWRAP,
	NCSP_LINELAYOUTPIECE_DIRECTION = PIECECOMM_PROP_DIRECTION,
	NCSP_LINELAYOUTPIECE_SPACE     = PIECECOMM_PROP_SPACE,
	NCSP_LINELAYOUTPIECE_MARGIN    = PIECECOMM_PROP_MARGIN
};

#ifdef __cplusplus
}
#endif

#endif

