
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

extern mComboboxRenderer fashion_combobox_renderer;
extern mWidgetRenderer fashion_widget_renderer;
extern mGroupBoxRenderer fashion_groupbox_renderer;
extern mPropSheetRenderer fashion_propsheet_renderer;
extern mListView fashion_listview_renderer;

#define RDR_ENTRY(CLASSNAME, classname) \
	{ NCSCTRL_##CLASSNAME, (mWidgetRenderer*)(void*)(&(fashion_##classname##_renderer))}


extern void fashion_init_boxpiece_renderer(void);

BOOL ncsInitFashionRenderers(void)
{
	int i;
	NCS_RDR_ENTRY entries[] = {
		RDR_ENTRY(WIDGET, widget),
		RDR_ENTRY(GROUPBOX, groupbox),
		RDR_ENTRY(COMBOBOX, combobox),
		RDR_ENTRY(PROPSHEET, propsheet),
		RDR_ENTRY(LISTVIEW, listview)
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
}

#endif


