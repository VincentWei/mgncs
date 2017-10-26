
TOP_DIR=../..

INCLUDES=-I$(TOP_DIR)/include -I. -I.. -I$(TOP_DIR) -Iinclude

noinst_PROGRAMS=datasource

COMMON_LADD=$(TOP_DIR)/src/.libs/libmgncs.so @APP_LIBS@ -lmgplus -lmgutils -lstdc++

datasource_SOURCES=src/datasource.c src/datasource_main.c src/open_ds.c src/selectdlg.c
datasource_LDFLAGS=$(COMMON_LADD)

EXTRA_DIST= include/ncs-window-types.h include/ncs-windows.h include/resource.h

