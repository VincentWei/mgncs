
TOP_DIR=../..

INCLUDES=-I$(TOP_DIR)/include -I. -I.. -I$(TOP_DIR) -Iinclude

noinst_PROGRAMS=databinding

COMMON_LADD= $(TOP_DIR)/src/.libs/libmgncs.so @APP_LIBS@ -lmgplus -lmgutils -lstdc++

databinding_SOURCES=src/main.c src/databinding_main.c
databinding_LDFLAGS=$(COMMON_LADD)

EXTRA_DIST= include/ncs-window-types.h include/ncs-windows.h include/resource.h
