#ifndef _MGNCS_NUMSPINNEDPIECE_H
#define _MGNCS_NUMSPINNEDPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mNumSpinnedPieceClass mNumSpinnedPieceClass;
typedef struct _mNumSpinnedPiece mNumSpinnedPiece;

#define mNumSpinnedPieceClassHeader(clss, superCls) \
	mLabelPieceClassHeader(clss, superCls)

struct _mNumSpinnedPieceClass
{
	mNumSpinnedPieceClassHeader(mNumSpinnedPiece, mLabelPiece)
};

MGNCS_EXPORT extern mNumSpinnedPieceClass g_stmNumSpinnedPieceCls;

#define mNumSpinnedPieceHeader(clss) \
	mLabelPieceHeader(clss) 

struct _mNumSpinnedPiece
{
	mNumSpinnedPieceHeader(mNumSpinnedPiece)
};


#ifdef __cplusplus
}
#endif

#endif

