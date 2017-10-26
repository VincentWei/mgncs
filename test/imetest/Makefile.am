
TOP_DIR=../..

INCLUDES=-I$(TOP_DIR)/include -I. -I.. -I$(TOP_DIR) -Iinclude

noinst_PROGRAMS=imetest

COMMON_LADD=$(TOP_DIR)/src/.libs/libmgncs.so @APP_LIBS@ -lmgplus -lmgutils -lstdc++

imetest_SOURCES=src/ime-view.c src/imetest.c src/imetest_main.c src/mobile-ime.c
imetest_LDFLAGS=$(COMMON_LADD)

EXTRA_DIST= include/ncs-window-types.h include/ncs-windows.h include/resource.h include/mobile-ime.h
