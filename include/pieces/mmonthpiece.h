
#ifndef _MGUI_NCSCTRL_MONTHPIECEPIECE_H
#define _MGUI_NCSCTRL_MONTHPIECEPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mMonthPieceClass mMonthPieceClass;
typedef struct _mMonthPiece mMonthPiece;

#define mMonthPieceClassHeader(clss, superCls) \
	mContainerPieceClassHeader(clss, superCls) 

struct _mMonthPieceClass
{
	mMonthPieceClassHeader(mMonthPiece, mContainerPiece)
};

MGNCS_EXPORT extern mMonthPieceClass g_stmMonthPieceCls;

#define mMonthPieceHeader(clss) \
	mContainerPieceHeader(clss)  \
	unsigned char cur_day; \
	unsigned char hot_row; \
	unsigned char hot_col;
	
struct _mMonthPiece
{
	mMonthPieceHeader(mMonthPiece)
};

enum mMonthPieceProps{
	NCSP_MONTHPIECE_CURDAY = MONTHPIECE_PROP_BEGIN,
};

enum mMonthPieceEvent {
	NCSN_MONTHPIECE_PREVMONTH = MONTHPIECE_EVENT_BEGIN,
	NCSN_MONTHPIECE_NEXTMONTH,
	NCSN_MONTHPIECE_DAYCHANGED
};

#ifdef __cplusplus
}
#endif

#endif

