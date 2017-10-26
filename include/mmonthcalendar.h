#ifdef _MGNCSCTRL_MONTHCALENDAR

#ifndef _MGUI_NCSCTRL_CDR_H
#define _MGUI_NCSCTRL_CDR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup ControlMonthCalendar mMonthCalendar
 * @{
 */

/**
 * \def NCSCTRL_MONTHCALENDAR
 * \brief the name of month calendar control
 */
#define NCSCTRL_MONTHCALENDAR NCSCLASSNAME("monthcal")

typedef struct _mMonthCalendarClass mMonthCalendarClass;
typedef struct _mMonthCalendar mMonthCalendar;
typedef struct _mMonthCalendarRenderer mMonthCalendarRenderer;

#define mMonthCalendarRendererHeader(clss, superCls) \
   mWidgetRendererHeader(clss, superCls)

struct _mMonthCalendarRenderer
{
	mMonthCalendarRendererHeader(mMonthCalendar, mWidget)
};

#define mMonthCalendarClassHeader(clss, superCls) \
	mWidgetClassHeader(clss, superCls)

/**
 * struct mMonthCalendarClass
 * \brief mMonthCalendar class, derived from \ref mWidget
 */

struct _mMonthCalendarClass
{
	mMonthCalendarClassHeader(mMonthCalendar, mWidget)
};

MGNCS_EXPORT extern mMonthCalendarClass g_stmMonthCalendarCls;

#define mMonthCalendarHeader(clss) \
	mWidgetHeader(clss)

/**
 * \struct mMonthCalender
 * \brief the member of mMonthCalendar Control derived from \ref mWidget
 */

struct _mMonthCalendar
{
	mMonthCalendarHeader(mMonthCalendar)
};

/**
 * \enum mMonthCalendarProps
 * \brief the properies id of mMonthCalendar
 */
enum mMonthCalendarProps{
	/**
	 * The current year
	 *     - Type : int 1970 ~ 2030
	 *	   - ReandWrite
	 *	   - Reset the current Year
	 */
	NCSP_CDR_YEAR = NCSP_WIDGET_MAX + 1,
	/**
	 * The current month
	 * 	- Type : int 1~12
	 * 	- ReadWrite
	 */
	NCSP_CDR_MONTH,
	/**
	 * The current day
	 * 	- Type : int 1~31
	 * 	- ReadWrite
	 */
	NCSP_CDR_DAY,
	NCSP_CDR_MAX
};

/**
 * \enum mMonthCalendarEvents
 * \brief the events of mMonthCalendar
 */
enum mMonthCalendarEvents{
	/**
	 * When year changed
	 *  - param : int , new current year
	 */
	NCSN_CDR_YEAR_CHANGED = NCSN_WIDGET_MAX + 1,
	/**
	 * When month changed
	 * 	- param : int , new current month
	 */
	NCSN_CDR_MONTH_CHANGED,
	/**
	 * When Day changed
	 * 	- param : int, new current day
	 */
	NCSN_CDR_DAY_CHANGED,
	NCSN_CDR_MAX
};

/**
 * @} end of ControlMonthCalendar
 */

#ifdef __cplusplus
}
#endif

#endif
#endif //_MGNCSCTRL_MONTHCALENDAR

