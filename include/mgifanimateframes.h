#ifndef _MGUI_NCSCTRL_GIFANIMATEFRAMES_H
#define _MGUI_NCSCTRL_GIFANIMATEFRAMES_H

#ifdef _cplusplus
extern "C"{
#endif

typedef struct _mGIFAnimateFrames mGIFAnimateFrames;
typedef struct _mGIFAnimateFramesClass mGIFAnimateFramesClass;

typedef struct _mGIFAnimateFrame{
	int off_x;
	int off_y;
	int disposal;
	unsigned int delay_time;
	BITMAP bmp;
	struct _mGIFAnimateFrame* next;
	struct _mGIFAnimateFrame* prev;
}mGIFAnimateFrame;


#define mGIFAnimateFramesHeader(clsName) \
	mAnimateFramesHeader(clsName) \
	/*RGB bk;*/	\
	mGIFAnimateFrame *frames;  \
	HDC mem_dc;

struct _mGIFAnimateFrames{
	mGIFAnimateFramesHeader(mAnimateFrames)
};

#define mGIFAnimateFramesClassHeader(clss, superCls) \
	mAnimateFramesClassHeader(clss, superCls) 

struct _mGIFAnimateFramesClass{
	mGIFAnimateFramesClassHeader(mGIFAnimateFrames, mAnimateFrames)
};

MGNCS_EXPORT extern mGIFAnimateFramesClass g_stmGIFAnimateFramesCls;

MGNCS_EXPORT mGIFAnimateFrames * ncsCreateAnimateFramesFromGIFFile(const char* gif_file);

MGNCS_EXPORT mGIFAnimateFrames * ncsCreateAnimateFramesFromGIFMem(const void* mem, int size);

#ifdef _cplusplus
}
#endif

#endif
