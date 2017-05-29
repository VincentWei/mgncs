
#ifndef _MGUI_NCSCTRL_DAYGRIDPIECE_H
#define _MGUI_NCSCTRL_DAYGRIDPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mDayGridPieceClass mDayGridPieceClass;
typedef struct _mDayGridPiece mDayGridPiece;

#define mDayGridPieceClassHeader(clss, superCls) \
	mGridBoxPieceClassHeader(clss, superCls)  \
	CELL_POINT (*day2Cell)(clss*, int day); \
	int (*cell2Day)(clss *, int row, int col);


struct _mDayGridPieceClass
{
	mDayGridPieceClassHeader(mDayGridPiece, mGridBoxPiece)
};

MGNCS_EXPORT extern mDayGridPieceClass g_stmDayGridPieceCls;

#define mDayGridPieceHeader(clss) \
	mGridBoxPieceHeader(clss)  \
	unsigned char month_days; \
	unsigned char last_month_days; \
	unsigned char weekday_of_first_day;
	
struct _mDayGridPiece
{
	mDayGridPieceHeader(mDayGridPiece)
};

enum mDayGridPieceProps{
	NCSP_DAYGRIDPIECE_MONTH_DAYS = DAYGRIDPIECE_PROP_BEGIN,
	NCSP_DAYGRIDPIECE_LASTMONTH_DAYS,
	NCSP_DAYGRIDPIECE_WEEKDAY_OF_FIRSTDAY
};

enum WeekDays{
	Sunday = 0,
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday
};


#ifdef __cplusplus
}
#endif

#endif

