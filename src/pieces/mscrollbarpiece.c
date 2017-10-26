
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mrdr.h"

#include "mhotpiece.h"
#include "mstaticpiece.h"
#include "mrenderablepiece.h"
#include "mscrollbarpiece.h"

#ifdef _MGNCSCTRL_SCROLLBAR

BEGIN_MINI_CLASS(mScrollBarPiece, mRenderablePiece)
END_MINI_CLASS

#endif //_MGNCSCTRL_SCROLLBAR
