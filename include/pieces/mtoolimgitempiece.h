/*
 ** $Id: mtoolimgitempiece.h 1683 2017-10-26 06:52:09Z weiym $
 **
 ** mtoolimgitempiece.h: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: dongjunjie 
 **  
 ** Create date: 2010年12月24日 
 */

#ifndef _MGNCS_TOOLIMGITEMPIECE_H
#define _MGNCS_TOOLIMGITEMPIECE_H

DECLARE_OBJECT(mToolImageItemPiece)

#define mToolImageItemPieceHeader(clss) \
	mToolImagePieceHeader(clss)

#define mToolImageItemPieceClassHeader(clss, superCls) \
	mToolImagePieceClassHeader(clss, superCls)

DEFINE_OBJECT(mToolImageItemPiece, mToolImagePiece)

#endif

