
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

extern mWidgetRenderer classic_widget_renderer;
extern mComboboxRenderer classic_combobox_renderer;
extern mGroupBoxRenderer classic_groupbox_renderer;
extern mPropSheetRenderer classic_propsheet_renderer;
extern mListViewRenderer classic_listview_renderer;

#define RDR_ENTRY(CLASSNAME, classname) \
	{ NCSCTRL_##CLASSNAME, (mWidgetRenderer*)(void*)(&(classic_##classname##_renderer))}

extern void classic_init_boxpiece_renderer(void);

BOOL ncsInitClassicRenderers(void)
{
	int i;
	NCS_RDR_ENTRY entries [] = {
		RDR_ENTRY(WIDGET,widget),
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

	classic_init_boxpiece_renderer();

	return ncsRegisterCtrlRDRs("classic",
		entries,
		sizeof(entries)/sizeof(NCS_RDR_ENTRY)
		);
}


