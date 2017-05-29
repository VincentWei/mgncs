#ifndef _MGNCS_RECTPIECE_H
#define _MGNCS_RECTPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mRectPieceClass mRectPieceClass;
typedef struct _mRectPiece mRectPiece;

#define mRectPieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls)

struct _mRectPieceClass
{
	mRectPieceClassHeader(mRectPiece, mStaticPiece)
};

MGNCS_EXPORT extern mRectPieceClass g_stmRectPieceCls;

#define mRectPieceHeader(clss) \
	mStaticPieceHeader(clss) \
	DWORD fill_color; \
	DWORD border_color; \
	unsigned int border_size:12; \
	unsigned int x_radius:10; \
	unsigned int y_radius:10;

struct _mRectPiece
{
	mRectPieceHeader(mRectPiece)
};

enum mRectPieceProps {
	NCSP_RECTPIECE_FILLCOLOR    = RECTPIECE_PROP_BEGIN,
	NCSP_RECTPIECE_BORDERCOLOR,
	NCSP_RECTPIECE_BORDERSIZE,
	NCSP_RECTPIECE_XRADIUS,
	NCSP_RECTPIECE_YRADIUS,
	NCSP_RECTPIECE_FILLCLR_RED, //FILLCOLOR 
	NCSP_RECTPIECE_FILLCLR_GREEN,
	NCSP_RECTPIECE_FILLCLR_BLUE,
	NCSP_RECTPIECE_FILLCLR_ALPHA,
	NCSP_RECTPIECE_BRDCLR_RED, //BorderColor
	NCSP_RECTPIECE_BRDCLR_GREEN,
	NCSP_RECTPIECE_BRDCLR_BLUE,
	NCSP_RECTPIECE_BRDCLR_ALPHA,
	NCSP_RECTPIECE_MAX
};


#ifdef __cplusplus
}
#endif

#endif

