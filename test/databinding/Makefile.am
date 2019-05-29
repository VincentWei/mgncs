AUTOMAKE_OPTIONS = subdir-objects

TOP_DIR=../..

AM_CPPFLAGS=-I$(TOP_DIR)/include -I. -I.. -I$(TOP_DIR) -Iinclude

noinst_PROGRAMS=databinding

COMMON_LADD= @APP_LIBS@ -lstdc++

databinding_SOURCES=src/main.c src/databinding_main.c
databinding_LDADD=$(COMMON_LADD)

EXTRA_DIST= include/ncs-window-types.h include/ncs-windows.h include/resource.h
