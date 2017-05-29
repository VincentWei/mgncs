#ifndef _MGUI_NCSCTRL_BMPARRAYANIMATEFRAMES_H
#define _MGUI_NCSCTRL_BMPARRAYANIMATEFRAMES_H

#ifdef _cplusplus
extern "C"{
#endif

typedef struct _mBmpArrayAnimateFrames mBmpArrayAnimateFrames;
typedef struct _mBmpArrayAnimateFramesClass mBmpArrayAnimateFramesClass;

typedef struct _mBmpArrayAnimateFrame{
	PBITMAP bmp;
	struct _mBmpArrayAnimateFrame* next;
	struct _mBmpArrayAnimateFrame* prev;
}mBmpArrayAnimateFrame;

#define mBmpArrayAnimateFramesHeader(clsName) \
	mAnimateFramesHeader(clsName) \
	mBmpArrayAnimateFrame *frames; \
	PRIVATE DWORD flags;

struct _mBmpArrayAnimateFrames{
	mBmpArrayAnimateFramesHeader(mAnimateFrames)
};

#define mBmpArrayAnimateFramesClassHeader(clss, superCls) \
	mAnimateFramesClassHeader(clss, superCls) \

struct _mBmpArrayAnimateFramesClass{
	mBmpArrayAnimateFramesClassHeader(mBmpArrayAnimateFrames, mAnimateFrames)
};

MGNCS_EXPORT extern mBmpArrayAnimateFramesClass g_stmBmpArrayAnimateFramesCls;

MGNCS_EXPORT mBmpArrayAnimateFrames * ncsCreateAnimateFramesFromDIR(const char* dir);

MGNCS_EXPORT mBmpArrayAnimateFrames * ncsCreateAnimateFramesFromImageList(const char ** file_list, int count);

MGNCS_EXPORT mBmpArrayAnimateFrames * ncsCreateAnimateFramesFromBmpArray(PBITMAP * bmps, int count, BOOL auto_delete);

#define ncsCreateAnimateFramesFromMEM(bmps, count) \
	ncsCreateAnimateFramesFromBmpArray(bmps, count, FALSE)


#ifdef _cplusplus
}
#endif

#endif
