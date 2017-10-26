AUTOMAKE_OPTIONS=foreign

TOP_DIR=..

INCLUDES=-I$(TOP_DIR) -I$(TOP_DIR)/include -I.

SUBDIRS=databinding datasource imetest

COMMON_LADD=$(TOP_DIR)/src/.libs/libmgncs.so @APP_LIBS@ -lmgplus -lmgutils -lstdc++

noinst_PROGRAMS=static radiogroup container progressbar combobox \
				spinbox spinner mainwnd image propsheet \
				commrdrtest listbox trackbar rectangle \
				groupbox button checkbutton timer iconview sledit listview scrollview\
				scrollbar mledit toolbar menubutton \
				ledstatic animation monthcalendar\
				name2value separator colorbutton \
				bindtest \
				staticds \
				textedit \
				resdump list list_group \
				imagebutton

static_SOURCES=static.c
static_LDFLAGS=$(COMMON_LADD)

ledstatic_SOURCES=ledstatic.c
ledstatic_LDFLAGS=$(COMMON_LADD)

radiogroup_SOURCES=radiogroup.c
radiogroup_LDFLAGS=$(COMMON_LADD)

container_SOURCES=container.c
container_LDFLAGS=$(COMMON_LADD)

progressbar_SOURCES=progressbar.c
progressbar_LDFLAGS=$(COMMON_LADD)

combobox_SOURCES=combobox.c
combobox_LDFLAGS=$(COMMON_LADD)

spinbox_SOURCES=spinbox.c
spinbox_LDFLAGS=$(COMMON_LADD)

spinner_SOURCES=spinner.c
spinner_LDFLAGS=$(COMMON_LADD)

mainwnd_SOURCES=mainwnd.c
mainwnd_LDFLAGS=$(COMMON_LADD)

image_SOURCES=image.c
image_LDFLAGS=$(COMMON_LADD)

propsheet_SOURCES=propsheet.c
propsheet_LDFLAGS=$(COMMON_LADD)

commrdrtest_SOURCES=commrdrtest.c
commrdrtest_LDFLAGS=$(COMMON_LADD)

listbox_SOURCES=listbox.c
listbox_LDFLAGS=$(COMMON_LADD)

trackbar_SOURCES=trackbar.c
trackbar_LDFLAGS=$(COMMON_LADD)

rectangle_SOURCES=rectangle.c
rectangle_LDFLAGS=$(COMMON_LADD)

groupbox_SOURCES=groupbox.c
groupbox_LDFLAGS=$(COMMON_LADD)

button_SOURCES=button.c
button_LDFLAGS=$(COMMON_LADD)

menubutton_SOURCES=menubutton.c
menubutton_LDFLAGS=$(COMMON_LADD)

timer_SOURCES=timer.c
timer_LDFLAGS=$(COMMON_LADD)

iconview_SOURCES=iconview.c
iconview_LDFLAGS=$(COMMON_LADD)

listview_SOURCES=listview.c
listview_LDFLAGS=$(COMMON_LADD)

scrollview_SOURCES=scrollview.c
scrollview_LDFLAGS=$(COMMON_LADD)

sledit_SOURCES=sledit.c
sledit_LDFLAGS=$(COMMON_LADD)

mledit_SOURCES=mledit.c
mledit_LDFLAGS=$(COMMON_LADD)

scrollbar_SOURCES=scrollbar.c
scrollbar_LDFLAGS=$(COMMON_LADD)

toolbar_SOURCES=toolbar.c
toolbar_LDFLAGS=$(COMMON_LADD)

monthcalendar_SOURCES=monthcalendar.c
monthcalendar_LDFLAGS=$(COMMON_LADD)

animation_SOURCES=animation.c
animation_LDFLAGS=$(COMMON_LADD)

name2value_SOURCES=name2value.c
name2value_LDFLAGS=$(COMMON_LADD)

separator_SOURCES=separator.c
separator_LDFLAGS=$(COMMON_LADD)

checkbutton_SOURCES=checkbutton.c
checkbutton_LDFLAGS=$(COMMON_LADD)

colorbutton_SOURCES=colorbutton.c
colorbutton_LDFLAGS=$(COMMON_LADD)

bindtest_SOURCES=bindtest.c
bindtest_LDFLAGS=$(COMMON_LADD)

staticds_SOURCES=staticds.c
staticds_LDFLAGS=$(COMMON_LADD)

resdump_SOURCES=resdump.c
resdump_LDFLAGS=$(COMMON_LADD)

list_SOURCES=list.c
list_LDFLAGS=$(COMMON_LADD)

list_group_SOURCES=list_group.c
list_group_LDFLAGS=$(COMMON_LADD)

textedit_SOURCES=textedit.c
textedit_LDFLAGS=$(COMMON_LADD)

imagebutton_SOURCES=imagebutton.c
imagebutton_LDFLAGS=$(COMMON_LADD)

EXTRA_DIST=

