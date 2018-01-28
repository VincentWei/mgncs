
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSCTRL_MONTHCALENDAR

static const char *chMon_ES[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug",
                            "Sep", "Oct", "Nov", "Dec"};
#if 0
static const char *chMon_EL[] = {"January", "Febuary", "March", "April", "May", "June",
                            "July", "August", "September", "October", "November", "December"};
static const char *chMon_C[] = {"Ò»ÔÂ", "¶þÔÂ", "ÈýÔÂ", "ËÄÔÂ", "ÎåÔÂ", "ÁùÔÂ", "ÆßÔÂ", "°ËÔÂ",
                            "¾ÅÔÂ", "Ê®ÔÂ", "Ê®Ò»ÔÂ", "Ê®¶þÔÂ"};
static const char *chWeek_C[] = {"ÈÕ", "Ò»", "¶þ", "Èý", "ËÄ", "Îå", "Áù"};
static const char *chWeek_E[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char *chWeek_E_S[] = {"S", "1", "2", "3", "4", "5", "6"};
#endif

#define MINWNDRECT_W    200
#define MINWNDRECT_H    120

#define MONFIELD_W       70
#define YEARFIELD_W      70
#define WEEKFIELD_W      30
#define WEEKFIELD_H(hwnd)      (GetWindowFont(hwnd)->size+4)
#define WEEK_BORDER      5
#define WEEK_VBORDER1    2
#define WEEK_VBORDER2    3
#define TEXT_BORDER      5
#define LINE_D           2
#define HEADER_HEIGHT    25

// --------------------------------------------------------------------------

// find out whether a year is a leap year
static int IsLeapYear (int year)
{
    return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)?1:0;
}


// get month length from year and month
static int GetMonLen (int year, int month)
{
	static char month_days[]={
		31,28,31,30,31,30,31,31,30,31,30,31
	};
	return month_days[month-1]+(month==2?IsLeapYear(year):0);
}

static int GetYearLen (int year)
{
	return 365 + IsLeapYear(year);
}

// get previous month length from year and month
static int GetPMonLen (int year, int month)
{
    if (month > 1 && month <= 12) return GetMonLen (year, month - 1);
    else if (month == 1) return GetMonLen (year - 1, 12);
    else return -1;
}


static int MyGetWeekDay (int year, int month, int day)
{
    // days from 2036/12/31
    int weekday, weekday1 = 3, daylen = 0, i;

    if (year < 2037) return -1;
    for (i = 2037; i < year; i++) {
        daylen += GetYearLen (i);
    }
    for (i = 1; i < month; i++)    {
        daylen += GetMonLen (year, i);
    }
    daylen += day;
    weekday = (weekday1 + daylen) - (int)((weekday1 + daylen) / 7)*7;
    return weekday;
}

// get weekday from date
static int GetWeekDay (int year, int month, int day)
{
    struct tm nowday;

    if (year < 1970) return -1;
    if (month < 1 || month > 12) return -1;
    if (day < 1 || day > 31) return -1;

    if (year >= 2037) return MyGetWeekDay (year, month, day);
    memset (&nowday, 0, sizeof (nowday));
    nowday.tm_sec = 0;
    nowday.tm_min = 0;
    nowday.tm_hour = 0;
    nowday.tm_mday = day;
    nowday.tm_mon = month-1;
    nowday.tm_year = year-1900;

    if (mktime(&nowday) == -1) {
        return -1;
    }
    else
        return nowday.tm_wday;
}


static int event_ids[] = {
	  NCSN_SPNRPIECE_INC,
	 NCSN_SPNRPIECE_DEC,
	0
};

static mHotPiece * get_yearspin_piece(mMonthCalendar *self)
{
	mPairPiece * pair = (mPairPiece*)self->body;
	mHBoxLayoutPiece * hheader;
	if(!pair)
		return NULL;

	hheader = (mHBoxLayoutPiece*)pair->first;
	return hheader?_c(hheader)->getCell(hheader, 2):NULL;
}

static mHotPiece * get_monthspin_piece(mMonthCalendar *self)
{
	mPairPiece * pair = (mPairPiece*)self->body;
	mHBoxLayoutPiece * hheader;
	if(!pair)
		return NULL;

	hheader = (mHBoxLayoutPiece*)pair->first;
	return hheader?_c(hheader)->getCell(hheader, 0):NULL;
}

static mHotPiece * get_monthgrid_piece(mMonthCalendar *self)
{
	mPairPiece * pair_month;
	mPairPiece * pair = (mPairPiece*)self->body;

	if(!pair)
		return NULL;
	pair_month = (mPairPiece*)pair->second;
	return pair_month?pair_month->second:NULL;
}

static int get_year(mMonthCalendar *self)
{
	mHotPiece *year = get_yearspin_piece(self);
	if(year)
		return _c(year)->getProperty(year, NCSP_SPNRPIECE_CURPOS);
	return -1;
}

static int get_month(mMonthCalendar *self)
{
	mHotPiece * month = get_monthspin_piece(self);
	if(month)
		return _c(month)->getProperty(month, NCSP_SPNRPIECE_CURPOS) + 1;
	return -1;
}

static int get_day(mMonthCalendar* self)
{
	mHotPiece * monthgrid = get_monthgrid_piece(self);
	if(monthgrid)
		return _c(monthgrid)->getProperty(monthgrid, NCSP_MONTHPIECE_CURDAY);
    return -1;
}

static void update_day(mMonthCalendar *self, mHotPiece *monthgrid, int year, int month/*1~12*/, BOOL bredraw)
{
	if(!monthgrid)
		monthgrid = get_monthgrid_piece(self);
	if(!monthgrid)
		return;

	_c(monthgrid)->setProperty(monthgrid, NCSP_DAYGRIDPIECE_MONTH_DAYS, GetMonLen(year, month));
	_c(monthgrid)->setProperty(monthgrid, NCSP_DAYGRIDPIECE_LASTMONTH_DAYS, GetPMonLen(year, month));
	_c(monthgrid)->setProperty(monthgrid, NCSP_DAYGRIDPIECE_WEEKDAY_OF_FIRSTDAY, GetWeekDay(year, month, 1));

	if(bredraw)
		mHotPiece_update((mHotPiece*)monthgrid, (mObject*)self, TRUE);
}

static void set_year(mMonthCalendar *self, mHotPiece *year, int iyear, BOOL bupdate_monthgrid)
{
	if(!year)
		year = get_yearspin_piece(self);
	if(!year)
		return;

	if(iyear < 1970)
		iyear = 1970;
	if(iyear > 2038)
		iyear = 2038;

	if(_c(year)->setProperty(year, NCSP_SPNRPIECE_CURPOS, iyear))
	{
		if(bupdate_monthgrid)
			update_day(self, NULL, iyear, get_month(self), TRUE);
		mHotPiece_update((mHotPiece*)year, (mObject*)self, TRUE);
	}
}

static void set_month(mMonthCalendar *self, mHotPiece *month, int imonth/*1~12*/, BOOL bupdate_monthgrid)
{
	if(!month)
		month = get_monthspin_piece(self);
	if(!month)
		return;

	if(imonth < 1)
		imonth = 1;
	else if(imonth > 12)
		imonth = 12;

	if(_c(month)->setProperty(month, NCSP_SPNRPIECE_CURPOS, imonth-1))
	{
		if(bupdate_monthgrid)
			update_day(self,NULL, get_year(self), imonth, TRUE);
		mHotPiece_update((mHotPiece*)month, (mObject*)self, TRUE);
	}
}

static void set_day(mMonthCalendar *self, mHotPiece *monthgrid, int iday/*1~31*/)
{
	if(!monthgrid)
		monthgrid = get_monthgrid_piece(self);
	if(!monthgrid)
		return;

	if(iday < 1)
		iday = 1;
	else if(iday > 31)
		iday = 31;

	if(_c(monthgrid)->setProperty(monthgrid, NCSP_MONTHPIECE_CURDAY, iday))
		mHotPiece_update((mHotPiece*)monthgrid, (mObject*)self, TRUE);
}

static void set_year_month_day(mMonthCalendar *self, int iyear, int imonth, int day)
{
	mHotPiece * year = get_yearspin_piece(self);
	mHotPiece * month = get_monthspin_piece(self);
	mHotPiece * monthgrid = get_monthgrid_piece(self);
	int old_year ;
	int old_month;
	if(!year || !month || !monthgrid)
		return ;

	old_year  = _c(year)->getProperty(year, NCSP_SPNRPIECE_CURPOS);
	old_month = _c(month)->getProperty(month, NCSP_SPNRPIECE_CURPOS);

	if(old_year == iyear && old_month == imonth)
		return ;

	set_year(self, year, iyear, FALSE);

	set_month(self, month, imonth, FALSE);

	update_day(self, monthgrid, iyear, imonth, TRUE);

	if(day >= 0 && day <= 31)
		set_day(self,monthgrid, day);

}

static void set_year_month_from_localtime(mMonthCalendar * self)
{
	time_t tim;
	struct tm *ptm;

	time(&tim);
	ptm = localtime(&tim);

	if(!ptm)
		return;

	set_year_month_day(self, ptm->tm_year + 1900, ptm->tm_mon+1, ptm->tm_mday);
}

static BOOL mMonthCalendar_onYearChanged(mMonthCalendar* self, mHotPiece *sender, int event_id, DWORD param)
{
	//mHotPiece * monthgrid = get_monthgrid_piece(self);

	int month = get_month(self);
	//TODO
	update_day(self, NULL, (int)param, month, TRUE);

	mHotPiece_update((mHotPiece*)sender, (mObject*)self, TRUE);
	ncsNotifyParent((mWidget*)self, NCSN_CDR_YEAR_CHANGED);
	return FALSE;
}

static BOOL mMonthCalendar_onMonthChanged(mMonthCalendar* self, mHotPiece *sender, int event_id, DWORD param)
{
	//TODO
	int year = get_year(self);
	update_day(self, NULL, year, (int)(param+1), TRUE);

	mHotPiece_update((mHotPiece*)sender, (mObject*)self, TRUE);
	ncsNotifyParent((mWidget*)self, NCSN_CDR_MONTH_CHANGED);
	return FALSE;
}

static void next_prev_month(mMonthCalendar *self, BOOL bprev )
{
	mHotPiece *year = get_yearspin_piece(self);
	mHotPiece * month = get_monthspin_piece(self);
	int iyear, oldyear, imonth;

	if(!year || !month )
		return ;

	iyear  = _c(year)->getProperty(year, NCSP_SPNRPIECE_CURPOS);
	imonth = _c(month)->getProperty(month, NCSP_SPNRPIECE_CURPOS);
	oldyear = (int)(intptr_t)year;

	if(bprev)
	{
		imonth --;
		if(imonth < 0){
			imonth = 11;
			iyear --;
			if(iyear < 1970)
				return;
		}
	}
	else
	{
		imonth ++;
		if(imonth >= 12)
		{
			imonth = 0;
			iyear ++;
		}
	}

	//set month
	set_month(self, month, imonth+1, FALSE);

	if(iyear != oldyear)
	{
		set_year(self, year, iyear, FALSE);
		ncsNotifyParent((mWidget*)self, NCSN_CDR_YEAR_CHANGED);
	}
	ncsNotifyParent((mWidget*)self, NCSN_CDR_MONTH_CHANGED);


	//set monthgrid
	update_day(self, NULL, iyear, imonth, TRUE);

}

static BOOL mMonthCalendar_onDayChanged(mMonthCalendar *self, mHotPiece *sender, int event_id, DWORD param)
{
	switch(event_id)
	{
	case NCSN_MONTHPIECE_PREVMONTH:
		next_prev_month(self, TRUE);
		break;
	case NCSN_MONTHPIECE_NEXTMONTH:
		next_prev_month(self, FALSE);
		break;
	case NCSN_MONTHPIECE_DAYCHANGED:
		ncsNotifyParent((mWidget*)self, NCSN_CDR_DAY_CHANGED);
		break;
	}

	return FALSE;
}


static mHotPiece * create_year_spinner(mMonthCalendar* self)
{
	mNumSpinnedPiece * numsp = NEWPIECE(mNumSpinnedPiece);
	mHSpinBoxPiece *hsp = NEWPIECEEX(mHSpinBoxPiece, (DWORD)numsp);
	_c(hsp)->setProperty(hsp, NCSP_SPNRPIECE_MAXPOS, 2038);
	_c(hsp)->setProperty(hsp, NCSP_SPNRPIECE_MINPOS, 1970);
	_c(hsp)->setProperty(hsp, NCSP_SPNRPIECE_LINESTEP, 1);

	ncsAddEventListeners((mObject*)hsp, (mObject*)self, (NCS_CB_ONPIECEEVENT)mMonthCalendar_onYearChanged, event_ids);

	return (mHotPiece *)hsp;
}

static mHotPiece * create_month_spinner(mMonthCalendar *self)
{
	mListSpinnedPiece * listsp = NEWPIECE(mListSpinnedPiece);
	mHSpinBoxPiece *hsp = NEWPIECEEX(mHSpinBoxPiece, (DWORD)listsp);
	_c(listsp)->setProperty(listsp,NCSP_MLISTSPINNEDPIECE_LIST ,(DWORD)chMon_ES);
	_c(hsp)->setProperty(hsp, NCSP_SPNRPIECE_MAXPOS, 11);
	_c(hsp)->setProperty(hsp, NCSP_SPNRPIECE_MINPOS, 0);
	_c(hsp)->setProperty(hsp, NCSP_SPNRPIECE_LINESTEP, 1);

	ncsAddEventListeners((mObject*)hsp, (mObject*)self, (NCS_CB_ONPIECEEVENT)mMonthCalendar_onMonthChanged, event_ids);
	return (mHotPiece *)hsp;
}

static mObject * mMonthCalendar_createBody(mMonthCalendar *self)
{
	int event_ids[]={
		NCSN_MONTHPIECE_PREVMONTH,
		NCSN_MONTHPIECE_NEXTMONTH,
		NCSN_MONTHPIECE_DAYCHANGED,
		0
	};
	mPairPiece * pair_month, *pair_total;
	//create header of monthcalendar
	mHBoxLayoutPiece * hheader = NEWPIECEEX(mHBoxLayoutPiece, 3);
	_c(hheader)->setCellInfo(hheader, 0, MONFIELD_W , NCS_LAYOUTPIECE_ST_FIXED,FALSE);
	_c(hheader)->setCellInfo(hheader, 2, YEARFIELD_W, NCS_LAYOUTPIECE_ST_FIXED ,FALSE);
	_c(hheader)->setCell(hheader, 0, create_month_spinner(self));
	_c(hheader)->setCell(hheader, 2, create_year_spinner(self));
	_c(hheader)->setProperty(hheader, NCSP_BOXLAYOUTPIECE_MARGIN, 2);

	//create the month and week area
	pair_month = NEWPIECE(mPairPiece);
	_c(pair_month)->setProperty(pair_month, NCSP_PAIRPIECE_DIRECTION, 1);
	pair_month->first = (mHotPiece*)NEWPIECE(mWeekHeadPiece);
	pair_month->second = (mHotPiece*)NEWPIECE(mMonthPiece);
	_c(pair_month)->setProperty(pair_month, NCSP_PAIRPIECE_FIRST_SIZE, WEEKFIELD_H(self->hwnd));
	ncsAddEventListeners((mObject*) pair_month->second, (mObject*)self, (NCS_CB_ONPIECEEVENT)mMonthCalendar_onDayChanged, event_ids);

	//create total
	pair_total = NEWPIECE(mPairPiece);
	_c(pair_total)->setProperty(pair_total, NCSP_PAIRPIECE_DIRECTION, 1);
	_c(pair_total)->setProperty(pair_total, NCSP_PAIRPIECE_FIRST_SIZE, HEADER_HEIGHT);
	pair_total->first = (mHotPiece*) hheader;
	pair_total->second = (mHotPiece*) pair_month;


	return (mObject*)pair_total;
}

static BOOL mMonthCalendar_onCreate(mMonthCalendar* self, LPARAM lParam)
{
	//set local time
	set_year_month_from_localtime(self);
	return TRUE;
}


static BOOL mMonthCalendar_setProperty(mMonthCalendar *self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_CDR_YEAR:
		set_year(self, NULL, (int)value, TRUE);
		return TRUE;
	case NCSP_CDR_MONTH:
		set_month(self, NULL, (int)value, TRUE);
		return TRUE;
	case NCSP_CDR_DAY:
		set_day(self, NULL, (int)value);
		return TRUE;
	}
	return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static DWORD mMonthCalendar_getProperty(mMonthCalendar *self, int id)
{
	switch(id)
	{
	case NCSP_CDR_YEAR:
		return get_year(self);
	case NCSP_CDR_MONTH:
		return get_month(self);
	case NCSP_CDR_DAY:
		return get_day(self);
	}
	return Class(mWidget).getProperty((mWidget*)self, id);
}


static const char mmontclandar_classname[] = NCSCTRL_MONTHCALENDAR;
BEGIN_MINI_CLASS(mMonthCalendar, mWidget)
	_class->className = mmontclandar_classname;
	CLASS_METHOD_MAP(mMonthCalendar, createBody)
	CLASS_METHOD_MAP(mMonthCalendar, onCreate)
	CLASS_METHOD_MAP(mMonthCalendar, setProperty)
	CLASS_METHOD_MAP(mMonthCalendar, getProperty)
END_MINI_CLASS

#endif //_MGNCSCTRL_MONTHCALENDAR

