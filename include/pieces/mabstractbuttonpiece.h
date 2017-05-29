
#ifndef _MGUI_NCSCTRL_ABSTRACTBUTTONPIECEPIECE_H
#define _MGUI_NCSCTRL_ABSTRACTBUTTONPIECEPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mAbstractButtonPieceClass mAbstractButtonPieceClass;
typedef struct _mAbstractButtonPiece mAbstractButtonPiece;

#define mAbstractButtonPieceClassHeader(clss, superCls) \
	mContainerPieceClassHeader(clss, superCls)

struct _mAbstractButtonPieceClass
{
	mAbstractButtonPieceClassHeader(mAbstractButtonPiece, mContainerPiece)
};

MGNCS_EXPORT extern mAbstractButtonPieceClass g_stmAbstractButtonPieceCls;

#define mAbstractButtonPieceHeader(clss) \
	mContainerPieceHeader(clss)  \
	unsigned char flags; \
	unsigned char state; \
	unsigned char check_state; \
	unsigned char revert;

struct _mAbstractButtonPiece
{
	mAbstractButtonPieceHeader(mAbstractButtonPiece)
};

enum mAbstractButtonPieceStates{
	NCS_ABP_NORMAL = 0,
	NCS_ABP_HILIGHT,
	NCS_ABP_PUSHED,
	NCS_ABP_CAPTURED,
	NCS_ABP_DISABLED
};

enum mAbstractButtonPieceCheckStates {
	NCS_ABP_UNCHECKED = 0,
	NCS_ABP_HALFCHECKED ,
	NCS_ABP_CHECKED
};

#define NCSS_ABP_CHECKABLE      0x01
#define NCSS_ABP_AUTOCHECK      0x02
#define NCSS_ABP_3DCHECK        0x04
#define NCSS_ABP_DISABLE        0x08
#define NCSS_ABP_FLAT           0x10 // show button's as flat
#define NCSS_ABP_NOREPEATMSG    0x20 // disbale the auto repeat message


#define  mAbstractButtonPiece_isCheckable(self)  \
	IS_BIT_SET((self)->flags, NCSS_ABP_CHECKABLE)
#define mAbstractButtonPiece_setCheckable(self, bcheckable)  \
	((bcheckable)?SET_BIT((self)->flags, NCSS_ABP_CHECKABLE):CLEAR_BIT((self)->flags, NCSS_ABP_CHECKABLE))

#define  mAbstractButtonPiece_isAutoCheck(self)  \
	IS_BIT_SET((self)->flags, NCSS_ABP_AUTOCHECK)
#define mAbstractButtonPiece_setAutoCheck(self, bautocheck)  \
	((bautocheck)?SET_BIT((self)->flags, NCSS_ABP_AUTOCHECK):CLEAR_BIT((self)->flags, NCSS_ABP_AUTOCHECK))

#define  mAbstractButtonPiece_is3DCheck(self)  \
	IS_BIT_SET((self)->flags, NCSS_ABP_3DCHECK)
#define mAbstractButtonPiece_set3DCheck(self, b3dcheck)  \
	((b3dcheck)?SET_BIT((self)->flags, NCSS_ABP_3DCHECK):CLEAR_BIT((self)->flags, NCSS_ABP_3DCHECK))

#define  mAbstractButtonPiece_needRepeatMsg(self)  \
	!(IS_BIT_SET((self)->flags, NCSS_ABP_NOREPEATMSG))
#define mAbstractButtonPiece_setNoRepeatMsg(self, norepeat)  \
	((norepeat)?SET_BIT((self)->flags, NCSS_ABP_NOREPEATMSG):CLEAR_BIT((self)->flags, NCSS_ABP_NOREPEATMSG))

#define mAbstractButtonPiece_isEnableFlag(self)  \
	(!IS_BIT_SET((self)->flags, NCSS_ABP_DISABLE))

#define mAbstractButtonPiece_setEnable(self, benable) \
	((benable)?CLEAR_BIT((self)->flags,NCSS_ABP_DISABLE):SET_BIT((self)->flags, NCSS_ABP_DISABLE))

enum mAbstractButtonPieceProps{
	NCSP_ABP_CHECKABLE = ABP_PROP_BEGIN,
	NCSP_ABP_AUTOCHECK,
	NCSP_ABP_3DCHECK,
	NCSP_ABP_CHECKSTATE,
	NCSP_ABP_STATE,
	NCSP_ABP_DISABLE,
	NCSP_ABP_FLAT,
	NCSP_ABP_NOREPEATMSG,
};

enum mAbstractButtonPieceEvent {
	NCSN_ABP_CLICKED = ABP_EVENT_BEGIN,
	NCSN_ABP_STATE_CHANGED,
	NCSN_ABP_PUSHED,
	NCSN_ABP_DISABLED //param = 0, diabled, 1 enabled
};

#ifdef __cplusplus
}
#endif

#endif

