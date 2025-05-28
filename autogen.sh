#!/bin/sh

cat m4/*.m4 > aclocal.m4
libtoolize
aclocal
autoheader
automake --add-missing --copy
autoconf
