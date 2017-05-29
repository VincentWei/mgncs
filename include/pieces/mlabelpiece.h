#ifndef _MGNCS_LABELPIECE_H
#define _MGNCS_LABELPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mLabelPieceClass mLabelPieceClass;
typedef struct _mLabelPiece mLabelPiece;

#define mLabelPieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls)

struct _mLabelPieceClass
{
	mLabelPieceClassHeader(mLabelPiece, mStaticPiece)
};

MGNCS_EXPORT extern mLabelPieceClass g_stmLabelPieceCls;

#define mLabelPieceHeader(clss) \
	mStaticPieceHeader(clss) \
	const char* str; \
	char align; \
	char valign; \
	unsigned char flag; 

struct _mLabelPiece
{
	mLabelPieceHeader(mLabelPiece)
};

#define mLabelPiece_setFlag(self, bitmask) (((self)->flag)|=(bitmask))
#define mLabelPiece_clearFlag(self, bitmask) (((self)->flag)&=(~(bitmask)))
#define mLabelPiece_isFlag(self, bitmask) (((self->flag)&(bitmask)) == (bitmask))

#define mLabelPiece_setAutoWrap(self, autoWrap) ((autoWrap)?mLabelPiece_setFlag(self, 0x1):mLabelPiece_clearFlag(self, 0x1))
#define mLabelPiece_isAutoWrap(self)  mLabelPiece_isFlag(self, 0x1)

#define mLabelPiece_setPrefix(self, prefix) ((prefix)?mLabelPiece_setFlag(self, 0x2):mLabelPiece_clearFlag(self, 0x2))
#define mLabelPiece_isPrefix(self)  mLabelPiece_isFlag(self, 0x2)

#define mLabelPiece_setWordBreak(self, wordbreak) ((wordbreak)?mLabelPiece_setFlag(self, 0x4):mLabelPiece_clearFlag(self, 0x4))
#define mLabelPiece_isWordBreak(self)  mLabelPiece_isFlag(self, 0x4)

#define mLabelPiece_setSelected(self, selected) ((selected)?mLabelPiece_setFlag(self, 0x8):mLabelPiece_clearFlag(self, 0x8))
#define mLabelPiece_isSelected(self) mLabelPiece_isFlag(self, 0x8)

enum mLabelPieceProps {
	NCSP_LABELPIECE_LABEL    = LABELPIECE_PROP_BEGIN,
	NCSP_LABELPIECE_PREFIX,
	NCSP_LABELPIECE_SELECT   = PIECECOMM_PROP_SELECT,
	NCSP_LABELPIECE_ALIGN    = PIECECOMM_PROP_ALIGN,
	NCSP_LABELPIECE_VALIGN   = PIECECOMM_PROP_VALIGN, 
	NCSP_LABELPIECE_AUTOWRAP = PIECECOMM_PROP_AUTOWRAP,
	NCSP_LABELPIECE_WORDBREAK = PIECECOMM_PROP_WORDBREAK
};


#ifdef __cplusplus
}
#endif

#endif

