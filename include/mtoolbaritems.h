
#ifndef MGNCS_TOOLBAR_ITEMS_H
#define MGNCS_TOOLBAR_ITEMS_H

//////////////////////////////////////////////////////
/**
 *  mPushToolItem
 *
 */
typedef struct _mPushToolItemClass mPushToolItemClass;
typedef struct _mPushToolItem mPushToolItem;

typedef struct mPushToolItemCreateInfo{
	mToolImage * toolImg;
	const char * str;
	UINT         flags;
}mPushToolItemCreateInfo;

#define mPushToolItemClassHeader(clss, clssSuper) \
	mAbstractButtonPieceClassHeader(clss, clssSuper) \
	mHotPiece * (*createContent)(clss*, mPushToolItemCreateInfo* create_info); \
	BOOL (*setCheck)(clss *, int state); \
	int  (*getCheck)(clss *);

struct _mPushToolItemClass
{
	mPushToolItemClassHeader(mPushToolItem, mAbstractButtonPiece)
};

#define mPushToolItemHeader(clss) \
	mAbstractButtonPieceHeader(clss) 

struct _mPushToolItem
{
	mPushToolItemHeader(mPushToolItem)
};

MGNCS_EXPORT extern mPushToolItemClass g_stmPushToolItemCls;


/**
 *  mMenuToolItem
 */
typedef struct _mMenuToolItemClass mMenuToolItemClass;
typedef struct _mMenuToolItem mMenuToolItem;


#define mMenuToolItemClassHeader(clss, clssSuper) \
	mPushToolItemClassHeader(clss, clssSuper) \
	BOOL (*showMenu)(clss *, mWidget *owner);

struct _mMenuToolItemClass
{
	mMenuToolItemClassHeader(mMenuToolItem, mPushToolItem)
};

#define mMenuToolItemHeader(clss) \
	mPushToolItemHeader(clss) \
	mPopMenuMgr * popmenu;

struct _mMenuToolItem
{
	mMenuToolItemHeader(mMenuToolItem)
};

MGNCS_EXPORT extern mMenuToolItemClass g_stmMenuToolItemCls;



#endif


