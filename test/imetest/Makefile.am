AUTOMAKE_OPTIONS = subdir-objects

TOP_DIR=../..

AM_CPPFLAGS=-I$(TOP_DIR)/include -I. -I.. -I$(TOP_DIR) -Iinclude

noinst_PROGRAMS=imetest

COMMON_LADD=@APP_LIBS@ -lstdc++

imetest_SOURCES=src/ime-view.c src/imetest.c src/imetest_main.c src/mobile-ime.c
imetest_LDADD=$(COMMON_LADD)

EXTRA_DIST= include/ncs-window-types.h include/ncs-windows.h include/resource.h include/mobile-ime.h
