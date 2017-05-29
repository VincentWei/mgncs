
#ifndef _MGUI_SIMPLE_LIST_H
#define _MGUI_SIMPLE_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mSimpleListPieceClass mSimpleListPieceClass;
typedef struct _mSimpleListPiece      mSimpleListPiece;

#define mSimpleListPieceClassHeader(clss, superCls)        \
	mContainerPieceClassHeader(clss, superCls)         \
	void (*reset)(clss*, void(*free_piece)(mHotPiece*));


#define mSimpleListPieceHeader(clss)                       \
	mContainerPieceHeader(clss)	                   \
	mLineLayoutPieceNode * cur_node;

struct _mSimpleListPieceClass {
	mSimpleListPieceClassHeader(mSimpleListPiece, mContainerPiece)
};

struct _mSimpleListPiece {
	mSimpleListPieceHeader(mSimpleListPiece)
};

extern mSimpleListPieceClass g_stmSimpleListPieceCls;

enum mSimpleListPieceProps {
	NCSP_SIMPLELISTPIECE_CURINDEX = SIMPLELISTPIECE_PROP_BEGIN,
};

enum mSimpleListPieceEvent {
	NCSN_SIMPLELISTPIECE_REACHHEAD = SIMPLELISTPIECE_EVENT_BEGIN,
	NCSN_SIMPLELISTPIECE_REACHTAIL,
	NCSN_SIMPLELISTPIECE_CHANGED
};

#define mSimpleListPiece_getLineLayout(slp) \
	((mLineLayoutPiece*)((slp)?(slp)->body:NULL))


#ifdef __cplusplus
}
#endif


#endif


