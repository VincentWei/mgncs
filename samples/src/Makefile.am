AUTOMAKE_OPTIONS=foreign

TOP_DIR=../..

INCLUDES=-I$(TOP_DIR) -I$(TOP_DIR)/include -I.

SUBDIRS=res

COMMON_LADD= -lmgncs -lmgplus -lmgutils $(APP_LIBS) -lstdc++

noinst_PROGRAMS=helloworld helloworld_domodal helloworld_dlgbox \
    wnd_template event_listener data_source data_binding \
    button checkbutton radiogroup menubutton colorbutton \
	spinner spinbox progressbar resmgr edit panel container \
	propsheet scrollview listview iconview listbox trackbar \
	scrollbar timer combobox monthcalender

resmgr_SOURCES=resmgr.c resmgr_main.c resource.h
resmgr_LDFLAGS=$(COMMON_LADD)

monthcalender_SOURCES=monthcalender.c
monthcalender_LDFLAGS=$(COMMON_LADD)

helloworld_SOURCES=helloworld.c
helloworld_LDFLAGS=$(COMMON_LADD)

helloworld_domodal_SOURCES=helloworld_domodal.c
helloworld_domodal_LDFLAGS=$(COMMON_LADD)

helloworld_dlgbox_SOURCES=helloworld_dlgbox.c
helloworld_dlgbox_LDFLAGS=$(COMMON_LADD)

wnd_template_SOURCES=wnd_template.c
wnd_template_LDFLAGS=$(COMMON_LADD)

event_listener_SOURCES=event_listener.c
event_listener_LDFLAGS=$(COMMON_LADD)

data_source_SOURCES=data_source.c
data_source_LDFLAGS=$(COMMON_LADD)

data_binding_SOURCES=data_binding.c
data_binding_LDFLAGS=$(COMMON_LADD)

checkbutton_SOURCES=checkbutton.c
checkbutton_LDFLAGS=$(COMMON_LADD)


radiogroup_SOURCES=radiogroup.c
radiogroup_LDFLAGS=$(COMMON_LADD)

button_SOURCES=button.c
button_LDFLAGS=$(COMMON_LADD)

menubutton_SOURCES=menubutton.c
menubutton_LDFLAGS=$(COMMON_LADD)

colorbutton_SOURCES=colorbutton.c
colorbutton_LDFLAGS=$(COMMON_LADD)

spinner_SOURCES=spinner.c
spinner_LDFLAGS=$(COMMON_LADD)

spinbox_SOURCES=spinbox.c
spinbox_LDFLAGS=$(COMMON_LADD)

progressbar_SOURCES=progressbar.c
progressbar_LDFLAGS=$(COMMON_LADD)

panel_SOURCES=panel.c
panel_LDFLAGS=$(COMMON_LADD)

edit_SOURCES=edit.c
edit_LDFLAGS=$(COMMON_LADD)

container_SOURCES=container.c
container_LDFLAGS=$(COMMON_LADD)

propsheet_SOURCES=propsheet.c
propsheet_LDFLAGS=$(COMMON_LADD)

scrollview_SOURCES=scrollview.c
scrollview_LDFLAGS=$(COMMON_LADD)

listview_SOURCES=listview.c
listview_LDFLAGS=$(COMMON_LADD)

iconview_SOURCES=iconview.c
iconview_LDFLAGS=$(COMMON_LADD)

listbox_SOURCES=listbox.c
listbox_LDFLAGS=$(COMMON_LADD)

trackbar_SOURCES=trackbar.c
trackbar_LDFLAGS=$(COMMON_LADD)

scrollbar_SOURCES=scrollbar.c
scrollbar_LDFLAGS=$(COMMON_LADD)

timer_SOURCES=timer.c
timer_LDFLAGS=$(COMMON_LADD)

combobox_SOURCES=combobox.c
combobox_LDFLAGS=$(COMMON_LADD)

EXTRA_DIST=resmgr_demo.res ncs-window-types.h ncs-windows.h

