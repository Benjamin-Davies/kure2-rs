#
# Makefile.am
#
#  Copyright (C) 2010,2011,2012 Stefan Bolus, University of Kiel, Germany
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

ACLOCAL_AMFLAGS = -I m4 --install

SUBDIRS = src

pkgconfigdir = $(libdir)/pkgconfig

# Distribute the pkg-config .pc file.
pkgconfig_DATA = kure2.pc

dist_pkgdata_DATA = 

programdir = $(pkgdatadir)/programs

# Distribute Example programs.
dist_program_DATA = data/programs/ApproxAlg.prog \
		data/programs/Aux.prog \
		data/programs/Cyclefree.prog \
		data/programs/DedekindCuts.prog \
		data/programs/DFS.prog \
		data/programs/Graph.prog \
		data/programs/KernelCyclefree.prog \
		data/programs/KernelEnumerate.prog \
		data/programs/Misc.prog \
		data/programs/OrderLattice.prog \
		data/programs/PetriNets.prog \
		data/programs/RecursiveProg.prog \
		data/programs/ReducedGraph.prog \
		data/programs/SpannTree.prog \
		data/programs/TransClosure.prog \
		data/programs/VertexBase.prog 

EXTRA_DIST = $(top_srcdir)/CHANGES $(top_srcdir)/README \
	$(top_srcdir)/LICENSE \
	$(top_srcdir)/m4/*.m4 $(top_srcdir)/configure

# Doc files for the distribution
EXTRA_DIST += $(top_srcdir)/doc/Makefile \
		$(top_srcdir)/doc/doxygen/Makefile \
        $(top_srcdir)/doc/doxygen/index.html \
        $(top_srcdir)/doc/doxygen/index.txt \
        $(top_srcdir)/doc/doxygen/Doxyfile

# Examples
EXTRA_DIST += $(top_srcdir)/examples/basics.c \
	$(top_srcdir)/examples/dsl.c \
	$(top_srcdir)/examples/Hamilton.prog \
	$(top_srcdir)/examples/dsl_to_lua.c \
	$(top_srcdir)/examples/Makefile \
	$(top_srcdir)/examples/README \
	$(top_srcdir)/examples/shell/Makefile \
	$(top_srcdir)/examples/shell/shell.c

# Check
EXTRA_DIST += $(top_srcdir)/check/main.c \
	$(top_srcdir)/check/Mat.c \
	$(top_srcdir)/check/Mat.h \
	$(top_srcdir)/check/Makefile
