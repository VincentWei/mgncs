SUBDIRS=etc include src build

AUTOMAKE_OPTIONS=foreign

EXTRA_DIST=makefile.ng rules.make LICENSE README.md RELEASE-NOTES.md

libmgncsincludedir=$(includedir)/mgncs

libmgncsinclude_HEADERS = mgncsconfig.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA=mgncs.pc

docs:
	doxygen Doxyfile
