
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

#ifdef _MGNCSCTRL_COMBOBOX
extern mComboboxRenderer flat_combobox_renderer;
#endif

extern mGroupBoxRenderer flat_groupbox_renderer;

#ifdef _MGNCSCTRL_PROPSHEET
extern mPropSheetRenderer flat_propsheet_renderer;
#endif

#ifdef _MGNCSCTRL_LISTVIEW
extern mListViewRenderer flat_listview_renderer;
#endif //_MGNCSCTRL_LISTVIEW

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
#ifdef _MGNCSCTRL_COMBOBOX
		RDR_ENTRY(COMBOBOX, combobox),
#endif
#ifdef _MGNCSCTRL_PROPSHEET
		RDR_ENTRY(PROPSHEET, propsheet),
#endif
#ifdef _MGNCSCTRL_LISTVIEW
		RDR_ENTRY(LISTVIEW, listview),
#endif //_MGNCSCTRL_LISTVIEW
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

