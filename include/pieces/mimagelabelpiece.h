
#ifndef _MGUI_NCSCTRL_IMAGELABELPIECE_H
#define _MGUI_NCSCTRL_IMAGELABELPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mImageLabelPieceClass mImageLabelPieceClass;
typedef struct _mImageLabelPiece mImageLabelPiece;

#define mImageLabelPieceClassHeader(clss, superCls) \
	mPairPieceClassHeader(clss, superCls) 

struct _mImageLabelPieceClass
{
	mImageLabelPieceClassHeader(mImageLabelPiece, mPairPiece)
};

MGNCS_EXPORT extern mImageLabelPieceClass g_stmImageLabelPieceCls;

#define mImageLabelPieceHeader(clss) \
	mPairPieceHeader(clss) 
	
struct _mImageLabelPiece
{
	mImageLabelPieceHeader(mImageLabelPiece)
};

enum mImageLabelPieceProp{
	NCSP_IMAGELABELPIECE_IMAGE_SIZE = IMAGELABELPIECE_PROP_BEGIN,
	NCSP_IMAGELABELPIECE_IMAGE_SIZETYPE
};

#ifdef __cplusplus
}
#endif

#endif

