#ifdef _MGNCSCTRL_LEDLABEL

#ifndef _MGUI_NCSCTRL_LEDLABELPIECE_H
#define _MGUI_NCSCTRL_LEDLABELPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mLedstaticPieceClass mLedstaticPieceClass;
typedef struct _mLedstaticPiece mLedstaticPiece;

#define mLedstaticPieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls)

struct _mLedstaticPieceClass
{
	mLedstaticPieceClassHeader(mLedstaticPiece, mStaticPiece)
};

MGNCS_EXPORT extern mLedstaticPieceClass g_stmLedstaticPieceCls;

#define mLedstaticPieceHeader(clss) \
	mStaticPieceHeader(clss)   \
	char *text;		\
	unsigned int width:16;		\
	unsigned int height:16;	\
	DWORD color;	\
	unsigned int format;

struct _mLedstaticPiece
{
	mLedstaticPieceHeader(mLedstaticPiece)
};

#define mLedstaticPiece_setFlag(self, bitmask) ((self)->format = (((bitmask) << 8) | (self->format & 0xFFFF00FF)))
#define mLedstaticPiece_clearFlag(self, bitmask) (((self)->format) &= (~(bitmask << 8)))
#define mLedstaticPiece_isFlag(self, bitmask) (((self->format) & (bitmask << 8)))

#define mLedstaticPiece_setAutoWrap(self, autoWrap) ((autoWrap)?mLedstaticPiece_setFlag(self, 0x1):mLedstaticPiece_clearFlag(self, 0x1))
#define mLedstaticPiece_isAutoWrap(self)  mLedstaticPiece_isFlag(self, 0x1)

enum mLedstaticPieceProps {
	NCSP_LEDLBLPIECE_TEXT = LEDLBLPIECE_PROP_BEGIN,
	NCSP_LEDLBLPIECE_COLOR,
	NCSP_LEDLBLPIECE_ALIGN = PIECECOMM_PROP_ALIGN,
	NCSP_LEDLBLPIECE_VALIGN = PIECECOMM_PROP_VALIGN,
	NCSP_LEDLBLPIECE_AUTOWRAP = PIECECOMM_PROP_AUTOWRAP,
	NCSP_LEDLBLPIECE_WIDTH,
	NCSP_LEDLBLPIECE_HEIGHT,
	NCSP_LEDLBLPIECE_GAP,
};

#ifdef __cplusplus
}
#endif

#endif
#endif //_MGNCSCTRL_LEDLABEL
