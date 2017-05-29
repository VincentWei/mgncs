#ifndef _MGNCS_STATICPIECE_H
#define _MGNCS_STATICPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mStaticPieceClass mStaticPieceClass;
typedef struct _mStaticPiece mStaticPiece;

#define mStaticPieceClassHeader(clss, superCls) \
	mHotPieceClassHeader(clss, superCls)

struct _mStaticPieceClass
{
	mStaticPieceClassHeader(mStaticPiece, mHotPiece)
};

MGNCS_EXPORT extern mStaticPieceClass g_stmStaticPieceCls;

#define mStaticPieceHeader(clss) \
	mHotPieceHeader(clss) \
	int left:16; \
	int top:16; \
	int right:16; \
	int bottom:16;

struct _mStaticPiece
{
	mStaticPieceHeader(mStaticPiece)
};

#define SetStaticPieceRect(self, prc) do{ \
	(self)->left = (short)((prc->left)&0xFFFF); \
	(self)->top = (short)((prc->top)&0xFFFF); \
	(self)->right = (short)((prc->right)&0xFFFF); \
	(self)->bottom = (short)((prc->bottom)&0xFFFF); \
}while(0)


#ifdef __cplusplus
}
#endif

#endif

