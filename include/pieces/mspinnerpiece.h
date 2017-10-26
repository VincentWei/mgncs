#ifdef _MGNCSCTRL_SPINNER

#ifndef _MGUI_NCSCTRL_SPINNERPIECE_H
#define _MGUI_NCSCTRL_SPINNERPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mSpinnerPieceClass mSpinnerPieceClass;
typedef struct _mSpinnerPiece mSpinnerPiece;

#define mSpinnerPieceClassHeader(clss, superCls) \
	mContainerPieceClassHeader(clss, superCls)  \
	int (*lineStep)(clss*, BOOL bPrev); \
/**protected */ \
	mHotPiece *(*getIncPiece)(clss *); \
	mHotPiece *(*getDecPiece)(clss *); \
	void (*onPosChanged)(clss *);

struct _mSpinnerPieceClass
{
	mSpinnerPieceClassHeader(mSpinnerPiece, mContainerPiece)
};

MGNCS_EXPORT extern mSpinnerPieceClass g_stmSpinnerPieceCls;

#define mSpinnerPieceHeader(clss) \
	mContainerPieceHeader(clss)  \
	int max; \
	int min; \
	int cur_pos;  \
	int line_step; \
    int loop_flag;

struct _mSpinnerPiece
{
	mSpinnerPieceHeader(mSpinnerPiece)
};

enum mSpinnerPieceProps{
	NCSP_SPNRPIECE_MAXPOS   = PIECECOMM_PROP_MAX,
	NCSP_SPNRPIECE_MINPOS   = PIECECOMM_PROP_MIN,
	NCSP_SPNRPIECE_LINESTEP = PIECECOMM_PROP_LINESTEP,
	NCSP_SPNRPIECE_CURPOS   = PIECECOMM_PROP_POS,
	NCSP_SPNRPIECE_LOOP     = SPINBOXPIECE_PROP_BEGIN
};

enum mSpinnerPieceEvents{
	NCSN_SPNRPIECE_INC = SPINBOXPIECE_EVENT_BEGIN,
	NCSN_SPNRPIECE_DEC,
	NCSN_SPNRPIECE_REACHMIN,
	NCSN_SPNRPIECE_REACHMAX,
	NCSN_SPNRPIECE_MAX
};

#ifdef __cplusplus
}
#endif

#endif
#endif //_MGNCSCTRL_SPINNER

