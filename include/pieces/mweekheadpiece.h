
#ifndef _MGUI_NCSCTRL_WEEKHEADPIECE_H
#define _MGUI_NCSCTRL_WEEKHEADPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mWeekHeadPieceClass mWeekHeadPieceClass;
typedef struct _mWeekHeadPiece mWeekHeadPiece;

#define mWeekHeadPieceClassHeader(clss, superCls) \
	mRenderablePieceClassHeader(clss, superCls) 

struct _mWeekHeadPieceClass
{
	mWeekHeadPieceClassHeader(mWeekHeadPiece, mRenderablePiece)
};

MGNCS_EXPORT extern mWeekHeadPieceClass g_stmWeekHeadPieceCls;

#define mWeekHeadPieceHeader(clss) \
	mRenderablePieceHeader(clss)  \
	char ** week_names;
	
struct _mWeekHeadPiece
{
	mWeekHeadPieceHeader(mWeekHeadPiece)
};

enum mWeekHeadPieceProps{
	NCSP_WEEKHEADPIECE_WEEKNAME = WEEKHEADPIECE_PROP_BEGIN
};

#ifdef __cplusplus
}
#endif

#endif

