
#ifndef _MGUI_NCSCTRL_SEPARATORPIECE_H
#define _MGUI_NCSCTRL_SEPARATORPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mSeparatorPieceClass mSeparatorPieceClass;
typedef struct _mSeparatorPiece mSeparatorPiece;

#define mSeparatorPieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls)

struct _mSeparatorPieceClass
{
	mSeparatorPieceClassHeader(mSeparatorPiece, mStaticPiece)
};

MGNCS_EXPORT extern mSeparatorPieceClass g_stmSeparatorPieceCls;

#define mSeparatorPieceHeader(clss) \
	mStaticPieceHeader(clss)

struct _mSeparatorPiece
{
	mSeparatorPieceHeader(mSeparatorPiece)
};


#ifdef __cplusplus
}
#endif

#endif

