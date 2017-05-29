
#ifndef _MGUI_NCSCTRL_TRACKBARPIECE_H
#define _MGUI_NCSCTRL_TRACKBARPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mTrackBarPieceClass mTrackBarPieceClass;
typedef struct _mTrackBarPiece mTrackBarPiece;

#define mTrackBarPieceClassHeader(clss, superCls) \
	mRenderablePieceClassHeader(clss, superCls) 

struct _mTrackBarPieceClass
{
	mTrackBarPieceClassHeader(mTrackBarPiece, mRenderablePiece)
};

MGNCS_EXPORT extern mTrackBarPieceClass g_stmTrackBarPieceCls;

#define mTrackBarPieceHeader(clss) \
	mRenderablePieceHeader(clss) 
	
struct _mTrackBarPiece
{
	mTrackBarPieceHeader(mTrackBarPiece)
};


#ifdef __cplusplus
}
#endif

#endif

