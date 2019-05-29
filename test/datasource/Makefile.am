AUTOMAKE_OPTIONS = subdir-objects

TOP_DIR=../..

AM_CPPFLAGS=-I$(TOP_DIR)/include -I. -I.. -I$(TOP_DIR) -Iinclude

noinst_PROGRAMS=datasource

COMMON_LADD=@APP_LIBS@ -lstdc++

datasource_SOURCES=src/datasource.c src/datasource_main.c src/open_ds.c src/selectdlg.c
datasource_LDADD=$(COMMON_LADD)

EXTRA_DIST= include/ncs-window-types.h include/ncs-windows.h include/resource.h

