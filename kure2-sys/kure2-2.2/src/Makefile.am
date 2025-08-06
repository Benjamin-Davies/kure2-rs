#
# src/Makefile.am
#
#  Copyright (C) 2010 Stefan Bolus, University of Kiel, Germany
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#

# Note: We have to call (f)lex and bison on our own, because we have
#       a non-Yacc-compatible GLR parser which cannot be compiled
#       using ylwrap.

SOURCES_BDD = bdd/bdd.c \
	bdd/bddCompose.c \
	bdd/bddDivers.c \
	bdd/bddFind.c \
	bdd/bddMinterm.c \
	bdd/bddRandom.c \
	bdd/bddRelation.c \
	bdd/bddShift.c \
	bdd/bddMinSets.c

SOURCES_REL = relation/relationBaseops.c \
	relation/relationBits.c \
	relation/relationCardinality.c \
	relation/relationClosures.c \
	relation/relationColumns.c \
	relation/relationCompose.c \
	relation/relationDomains.c \
	relation/relationPowersets.c \
	relation/relationRandom.c \
	relation/relationResidue.c \
	relation/relationTests.c \
	relation/relationVectors.c


SOURCES_KURE = KureLang.c \
	KureContext.c \
	KureError.c \
	KureLuaBinding.c \
	KureRel.c \
	KureDom.c \
	Kure.c

SOURCES_LANG = parser.c parser.h lexer.c lexer.h kure_compat.c 

lib_LTLIBRARIES = libkure2.la
libkure2_la_SOURCES = $(SOURCES_BDD) $(SOURCES_REL) $(SOURCES_KURE) \
	$(SOURCES_LANG) $(top_srcdir)/include/KureImpl.h 
pkginclude_HEADERS = $(top_srcdir)/include/Kure.h $(top_builddir)/include/KureCudd.h 
libkure2_la_LDFLAGS = 
libkure2_la_LIBADD = $(LIBOBJS)

AM_CPPFLAGS = -I. -I$(top_srcdir) -I$(top_srcdir)/include $(CUDD_CFLAGS) $(LUA_CFLAGS) \
	$(GMP_CFLAGS) $(GLIB_CFLAGS) -I$(top_builddir)/include
AM_CFLAGS = -g

LIBS =

# bin2hex is used to embed pre-compiled Lua code into a C file.
noinst_PROGRAMS = bin2hex
bin2hex_SOURCES = bin2hex.c
bin2hex_LDADD =

BUILT_SOURCES = kure_compat.luac kure_compat.c parser.c parser.h lexer.c

dist_noinst_SCRIPTS = kure_compat.sh

kure_compat.luac: kure_compat.lua
	$(LUAC) -o kure_compat.luac $(srcdir)/kure_compat.lua

kure_compat.c: bin2hex kure_compat.sh kure_compat.luac
	$(srcdir)/kure_compat.sh kure_compat.luac g_kure_compat kure_compat.c

# Tell bison also to create a header file for the parser. Even if an 
# appropriate option is given in the file, this is necessary for automake
# to keep track of the dependencies.
AM_YFLAGS = -d

parser.c: $(srcdir)/parser.y
	$(BISON) $(srcdir)/parser.y

parser.h: parser.c

lexer.c: $(srcdir)/lexer.l
	$(LEX) $(srcdir)/lexer.l

EXTRA_DIST = parser.y lexer.l kure_compat.lua

CLEANFILES =
DISTCLEANFILES = $(CLEANFILES) 
