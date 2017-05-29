#ifndef _MGUI_NCS_CTRLMEMANIMATEFRAMES_H
#define _MGUI_NCS_CTRLMEMANIMATEFRAMES_H

#ifdef _cplusplus
extern "C"{
#endif

typedef struct _mMemAnimateFrames mMemAnimateFrames;
typedef struct _mMemAnimateFramesClass mMemAnimateFramesClass;

typedef struct _mMemAnimateFrame{
	PBITMAP pbmp;
	struct _mMemAnimateFrame* next;
	struct _mMemAnimateFrame* prev;
}mMemAnimateFrame;

#define mMemAnimateFramesHeader(clsName) \
	mAnimateFramesHeader(clsName) \
	mMemAnimateFrame *frames;

struct _mMemAnimateFrames{
	mMemAnimateFramesHeader(mAnimateFrames)
};

#define mMemAnimateFramesClassHeader(clss, superCls) \
	mAnimateFramesClassHeader(clss, superCls) \

struct _mMemAnimateFramesClass{
	mMemAnimateFramesClassHeader(mMemAnimateFrames, mAnimateFrames)
};

MGNCS_EXPORT extern mMemAnimateFramesClass g_stmMemAnimateFramesCls;

//MGNCS_EXPORT mMemAnimateFrames * ncsCreateAnimateFramesFromMEM(PBITMAP * bmps, int count);

#ifdef _cplusplus
}
#endif

#endif
