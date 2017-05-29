#ifndef _MGNCS_IMAGEPIECE_H
#define _MGNCS_IMAGEPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mImagePieceClass mImagePieceClass;
typedef struct _mImagePiece mImagePiece;

#define mImagePieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls)

struct _mImagePieceClass
{
	mImagePieceClassHeader(mImagePiece, mStaticPiece)
};

MGNCS_EXPORT extern mImagePieceClass g_stmImagePieceCls;

#define mImagePieceHeader(clss) \
	mStaticPieceHeader(clss) \
	ImageDrawInfo img;

struct _mImagePiece
{
	mImagePieceHeader(mImagePiece)
};

#define mImagePiece_setAlign(self, align) ((self)->img.revert=((((self)->img.revert)&0xF0)|((align)&0xF)))
#define mImagePiece_getAlign(self)   ((self)->img.revert&0xF)

#define mImagePiece_setVAlign(self, valign) ((self)->img.revert=(((self)->img.revert&0x0F)|(((valign)&0xF)<<4)))
#define mImagePiece_getVAlign(self)  (((self)->img.revert&0xF0)>>4)

enum mImagePieceProps {
	NCSP_IMAGEPIECE_IMAGE = IMAGEPIECE_PROP_BEGIN,
	NCSP_IMAGEPIECE_ICON,
	NCSP_IMAGEPIECE_MYBITMAP,
	NCSP_IMAGEPIECE_IMAGEFILE,
	NCSP_IMAGEPIECE_ALIGN    = PIECECOMM_PROP_ALIGN,
	NCSP_IMAGEPIECE_VALIGN   = PIECECOMM_PROP_VALIGN,
	NCSP_IMAGEPIECE_DRAWMODE = PIECECOMM_PROP_DRAWMOD
};



#ifdef __cplusplus
}
#endif

#endif

