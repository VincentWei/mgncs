
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCS_RDR_FLAT

extern mWidgetRenderer flat_widget_renderer;
extern mComboboxRenderer flat_combobox_renderer;
extern mGroupBoxRenderer flat_groupbox_renderer;
extern mPropSheetRenderer flat_propsheet_renderer;
extern mListViewRenderer flat_listview_renderer;

#define RDR_ENTRY(CLASSNAME, classname) \
	{ NCSCTRL_##CLASSNAME, (mWidgetRenderer*)(void*)(&(flat_##classname##_renderer))}

extern void flat_init_boxpiece_renderer(void);

BOOL ncsInitFlatRenderers(void)
{
	int i;
	NCS_RDR_ENTRY entries [] = {
		RDR_ENTRY(WIDGET,widget),
		RDR_ENTRY(STATIC,widget),
		RDR_ENTRY(IMAGE,widget),
		RDR_ENTRY(GROUPBOX,groupbox),
		RDR_ENTRY(COMBOBOX, combobox),
		RDR_ENTRY(PROPSHEET, propsheet),
		RDR_ENTRY(LISTVIEW, listview),
		//TODO other render
	};

	for(i=0; i< sizeof(entries)/sizeof(NCS_RDR_ENTRY); i++)
	{
		entries[i].renderer->class_init(entries[i].renderer);
		if(entries[i].renderer->init_self)
			entries[i].renderer->init_self(entries[i].renderer);
	}

	flat_init_boxpiece_renderer();

	return ncsRegisterCtrlRDRs("flat",
		entries,
		sizeof(entries)/sizeof(NCS_RDR_ENTRY)
		);
}

#endif

