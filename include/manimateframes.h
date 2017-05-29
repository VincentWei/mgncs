#ifndef _MGUI_NCSCTRL_ANIMATEFRAMES_H
#define _MGUI_NCSCTRL_ANIMATEFRAMES_H

#ifdef _cplusplus
extern "C"{
#endif

#define NCSCTRL_ANIMATEFRAMES NCSCLASSNAME("animateframes")

typedef struct _mAnimateFrames mAnimateFrames;
typedef struct _mAnimateFramesClass mAnimateFramesClass;

#define mAnimateFramesHeader(clss) \
	mObjectHeader(clss) \
	int max_width; \
	int max_height; \
	int nr_frames; \
	void * cur_frame;

struct _mAnimateFrames{
	mAnimateFramesHeader(mAnimateFrames)
};

#define NCSR_ANIMATEFRAME_OK 0
#define NCSR_ANIMATEFRAME_LASTFRAME 1
#define NCSR_ANIMATEFRAME_FAILED -1

#define mAnimateFramesClassHeader(clss, superCls) \
	mObjectClassHeader(clss, superCls) \
	BOOL (*getMaxFrameSize)(mAnimateFrames* self, int *pwidth, int *pheight); \
	int (*drawFrame)(mAnimateFrames* self, HDC hdc, mObject *owner, RECT *pRect, int align, int valign, BOOL bScale); \
	int (*nextFrame)(mAnimateFrames* self);

struct _mAnimateFramesClass{
	mAnimateFramesClassHeader(mAnimateFrames, mObject)
};

MGNCS_EXPORT extern mAnimateFramesClass g_stmAnimateFramesCls;

#ifdef _cplusplus
}
#endif

#endif
