
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mgplus/mgplus.h>

#include "mgncs.h"
#include "fashion_common.h"

#ifdef _MGNCS_RDR_FASHION

#ifdef _MGNCSCTRL_COMBOBOX
extern mComboboxRenderer fashion_combobox_renderer;
#endif

extern mWidgetRenderer fashion_widget_renderer;
extern mGroupBoxRenderer fashion_groupbox_renderer;

#ifdef _MGNCSCTRL_PROPSHEET
extern mPropSheetRenderer fashion_propsheet_renderer;
#endif

#ifdef _MGNCSCTRL_LISTVIEW
extern mListView fashion_listview_renderer;
#endif

#define RDR_ENTRY(CLASSNAME, classname) \
	{ NCSCTRL_##CLASSNAME, (mWidgetRenderer*)(void*)(&(fashion_##classname##_renderer))}


extern void fashion_init_boxpiece_renderer(void);

BOOL ncsInitFashionRenderers(void)
{
	int i;
	NCS_RDR_ENTRY entries[] = {
		RDR_ENTRY(WIDGET, widget),
		RDR_ENTRY(GROUPBOX, groupbox),
#ifdef _MGNCSCTRL_COMBOBOX
		RDR_ENTRY(COMBOBOX, combobox),
#endif
#ifdef _MGNCSCTRL_PROPSHEET
		RDR_ENTRY(PROPSHEET, propsheet),
#endif
#ifdef _MGNCSCTRL_LISTVIEW
		RDR_ENTRY(LISTVIEW, listview)
#endif
	};

	MGPlusRegisterFashionLFRDR();

	for(i = 0; i< sizeof(entries)/sizeof(NCS_RDR_ENTRY); i++)
	{
		entries[i].renderer->class_init(entries[i].renderer);
		if(entries[i].renderer->init_self)
			entries[i].renderer->init_self(entries[i].renderer);
	}

	init_fashion_common();

	fashion_init_boxpiece_renderer();

	return ncsRegisterCtrlRDRs("fashion",
		entries,
		sizeof(entries)/sizeof(NCS_RDR_ENTRY));
}

void ncsUninitFashionRenderers(void)
{
    uninit_fashion_common();
    MGPlusUnregisterFashionLFRDR();
}

#endif


