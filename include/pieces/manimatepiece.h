#ifdef _MGNCSCTRL_ANIMATE
#ifndef _MGNCS_ANIMATEPIECE_H
#define _MGNCS_ANIMATEPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mAnimatePiece mAnimatePiece;
typedef struct _mAnimatePieceClass mAnimatePieceClass;

#define mAnimatePieceHeader(clss) \
	mStaticPieceHeader(clss) \
	unsigned char flags;	\
	unsigned char align:4;  \
	unsigned char valign:4; \
	mAnimateFrames *frame;

struct _mAnimatePiece
{
	mAnimatePieceHeader(mAnimatePiece)
};

#define mAnimatePieceClassHeader(clss, superCls) \
	mStaticPieceClassHeader(clss, superCls) \
	void (*reset)(mAnimatePiece *self); \
	void (*autofit)(mAnimatePiece *self, int *width, int *height); \

struct _mAnimatePieceClass
{
	mAnimatePieceClassHeader(mAnimatePiece, mStaticPiece)
};

MGNCS_EXPORT extern mAnimatePieceClass g_stmAnimatePieceCls;

enum mAnimatePieceProps {
	NCSP_ANIMATEPIECE_GIFFILE = ANIMATEPIECE_PROP_BEGIN,
	NCSP_ANIMATEPIECE_DIR,
	NCSP_ANIMATEPIECE_MEM,
	NCSP_ANIMATEPIECE_AUTOLOOP,
	NCSP_ANIMATEPIECE_SCALE,
	NCSP_ANIMATEPIECE_AUTOFIT,
	NCSP_ANIMATEPIECE_AUTOPLAY,
	NCSP_ANIMATEPIECE_BMPARRAY,
	NCSP_ANIMATEPIECE_AFRAME,
	NCSP_ANIMATEPIECE_ALIGN = PIECECOMM_PROP_ALIGN,
	NCSP_ANIMATEPIECE_VALIGN = PIECECOMM_PROP_VALIGN
};

#define NCSS_AF_AUTOLOOP 0x01
#define NCSS_AF_PLAY 0x02
#define NCSS_AF_SCALE 0x04
#define NCSS_AF_AUTOFIT 0x08
#define NCSS_AF_AUTOPLAY 0x10

#define mAnimatePiece_setAutoloop(self, bautoloop) \
	((bautoloop)?SET_BIT((self)->flags, NCSS_AF_AUTOLOOP):CLEAR_BIT((self)->flags, NCSS_AF_AUTOLOOP))

#define mAnimatePiece_isAutoloop(self)	\
	IS_BIT_SET((self)->flags, NCSS_AF_AUTOLOOP)

#define mAnimatePiece_setScale(self, bscale) \
	((bscale)?SET_BIT((self)->flags, NCSS_AF_SCALE):CLEAR_BIT((self)->flags, NCSS_AF_SCALE))

#define mAnimatePiece_isScale(self)	\
	IS_BIT_SET((self)->flags, NCSS_AF_SCALE)

#define mAnimatePiece_setAutofit(self, bautofit) \
	((bautofit)?SET_BIT((self)->flags, NCSS_AF_AUTOFIT):CLEAR_BIT((self)->flags, NCSS_AF_AUTOFIT))

#define mAnimatePiece_isAutofit(self)	\
	IS_BIT_SET((self)->flags, NCSS_AF_AUTOFIT)

#define mAnimatePiece_setPlay(self, bplay) \
	((bplay)?SET_BIT((self)->flags, NCSS_AF_PLAY):CLEAR_BIT((self)->flags, NCSS_AF_PLAY))

#define mAnimatePiece_isPlay(self)	\
	IS_BIT_SET((self)->flags, NCSS_AF_PLAY)

#define mAnimatePiece_setAutoplay(self, bautoplay) \
	((bautoplay)?SET_BIT((self)->flags, NCSS_AF_AUTOPLAY):CLEAR_BIT((self)->flags, NCSS_AF_AUTOPLAY))

#define mAnimatePiece_isAutoplay(self)	\
	IS_BIT_SET((self)->flags, NCSS_AF_AUTOPLAY)

#ifdef __cplusplus
}
#endif

#endif
#endif		//_MGNCSCTRL_ANIMATE

