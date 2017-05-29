#ifndef _MGNCS_TOOLIMAGEPIECE_H
#define _MGNCS_TOOLIMAGEPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../mpopmenumgr.h"
#include "../mtoolitem.h"

typedef struct _mToolImagePieceClass mToolImagePieceClass;
typedef struct _mToolImagePiece mToolImagePiece;

#define mToolImagePieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls)

struct _mToolImagePieceClass
{
	mToolImagePieceClassHeader(mToolImagePiece, mStaticPiece)
};

MGNCS_EXPORT extern mToolImagePieceClass g_stmToolImagePieceCls;

#define mToolImagePieceHeader(clss) \
	mStaticPieceHeader(clss) \
	mToolImage * toolImg;

struct _mToolImagePiece
{
	mToolImagePieceHeader(mToolImagePiece)
};


enum mToolImagePieceProps {
	NCSP_TOOLIMAGEPIECE_TOOLIMAGE = TOOLIMAGEPIECE_PROP_BEGIN
};



#ifdef __cplusplus
}
#endif

#endif

