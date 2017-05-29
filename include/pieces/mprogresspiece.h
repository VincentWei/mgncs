
#ifndef _MGUI_NCSCTRL_PROGRESSPIECE_H
#define _MGUI_NCSCTRL_PROGRESSPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mProgressPieceClass mProgressPieceClass;
typedef struct _mProgressPiece mProgressPiece;

#define mProgressPieceClassHeader(clss, superCls) \
	mRenderablePieceClassHeader(clss, superCls)  \
	int (*step)(clss *self, int offset/*=0*/, BOOL bdec); 

struct _mProgressPieceClass
{
	mProgressPieceClassHeader(mProgressPiece, mRenderablePiece)
};

extern mProgressPieceClass g_stmProgressPieceCls;

#define mProgressPieceHeader(clss) \
	mRenderablePieceHeader(clss)  \
	int max; \
	int min; \
	int cur; \
	int step; 
	
struct _mProgressPiece
{
	mProgressPieceHeader(mProgressPiece)
};

enum mProgressPieceProp{
	NCSP_PROGRESSPIECE_MAXPOS = PIECECOMM_PROP_MAX,
	NCSP_PROGRESSPIECE_MINPOS = PIECECOMM_PROP_MIN,
	NCSP_PROGRESSPIECE_LINESTEP = PIECECOMM_PROP_LINESTEP,
	NCSP_PROGRESSPIECE_CURPOS   = PIECECOMM_PROP_POS
};


#ifdef __cplusplus
}
#endif

#endif

