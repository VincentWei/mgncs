#ifndef _MGNCS_LISTSPINNEDPIECE_H
#define _MGNCS_LISTSPINNEDPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mListSpinnedPieceClass mListSpinnedPieceClass;
typedef struct _mListSpinnedPiece mListSpinnedPiece;

#define mListSpinnedPieceClassHeader(clss, superCls) \
	mLabelPieceClassHeader(clss, superCls)

struct _mListSpinnedPieceClass
{
	mListSpinnedPieceClassHeader(mListSpinnedPiece, mLabelPiece)
};

extern mListSpinnedPieceClass g_stmListSpinnedPieceCls;

#define mListSpinnedPieceHeader(clss) \
	mLabelPieceHeader(clss) \
	const char ** list;

struct _mListSpinnedPiece
{
	mListSpinnedPieceHeader(mListSpinnedPiece)
};


enum mListSpinnedPieceProps{
	NCSP_MLISTSPINNEDPIECE_LIST = MLISTSPINNEDPIECE_PROP_BEGIN
};

#ifdef __cplusplus
}
#endif

#endif

