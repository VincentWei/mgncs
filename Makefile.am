if MGNCS_HAVE_LIBSUFFIX
SUBDIRS=etc include src build
else
SUBDIRS=etc include src build samples
endif

DIST_SUBDIRS=samples

AUTOMAKE_OPTIONS=foreign

EXTRA_DIST=makefile.ng rules.make LICENSE README.md RELEASE-NOTES.md

libmgncsincludedir=$(includedir)/mgncs

libmgncsinclude_HEADERS = mgncsconfig.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA=mgncs.pc

