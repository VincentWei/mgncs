
#ifndef MOBILE_IME_H
#define MOBILE_IME_H

#define MGPTI_IME   1
#define PINYIN_IME   2

#define mMobileIMManagerClassHeader(clss, super)      \
	mIMManagerClassHeader(clss, super)

#define mMobileIMManagerHeader(clss)                         \
	mIMManagerHeader(clss)

DECLARE_CLASS(mMobileIMManager, mIMManager)


/*#define mMobileIMEClassHeader(clss, super)         \
	mIMEClassHeader(clss, super)

#define mMobileIMEHeader(clss)                             \
	mIMEHeader(clss)

DECLARE_CLASS(mMobileIME, mIME)
*/

#endif
