#
# configure.ac
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

AC_PREREQ([2.63])
AC_INIT([kure2], [2.2], [rub@informatik.uni-kiel.de])

AC_CONFIG_MACRO_DIR([m4])

# "subdir-objects": store object (.o) files in the corresponding
# subdirectories.
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects dist-bzip2])

# scrips for easier configure script
AC_DEFUN([K_APPEND], [$1="$$1 $2"])
AC_DEFUN([K_PREPEND], [$1="$2 $$1"])
AC_DEFUN([K_BACKUP], [rv_backup_$1="$$1"])
AC_DEFUN([K_RESTORE], [$1="$rv_backup_$1"])

# It is necessary to distribute the information of the sizes of 
# int, long and void*. See below.
AC_CONFIG_HEADERS([config.h])

AC_LANG([C])
AC_PROG_CC
AC_REQUIRE_CPP

# Disabled shared libaries by default. They are not supported anyway.
AC_DISABLE_SHARED
AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_MAKE_SET
AC_PROG_LEX
AC_PATH_PROG([LUAC], luac,)

AC_PATH_PROG([BISON], bison,)

# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])

# Checks for header files.
AC_CHECK_HEADERS([limits.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
# Would require implementations for missing functions.
#AC_FUNC_ALLOCA
#AC_FUNC_ERROR_AT_LINE
#AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strdup])

# GMP (Gnu Multiple Precision) Library
AM_PATH_GMP(4.0.1, have_gmp="yes")

# Lua (Scripting Language). Lua has a .pc file which can have different names
# on different platforms, e.g. "lua5.1" in ubuntu 9.04 but just "lua" in the
# Lua source package for version 5.1.4.

AC_ARG_WITH([lua-pc], 
	AS_HELP_STRING([--with-lua-pc], 
		[Name of Lua's pkg-config name @<:@default=lua@:>@]),
	[LUA_PKG_NAME="$withval"], [LUA_PKG_NAME="lua"])
PKG_CHECK_MODULES(LUA, $LUA_PKG_NAME,,, AC_MSG_ERROR([Need Lua 5.1 scripting language. See http://www.lua.org/.]))
LUA_LIBS="`pkg-config $LUA_PKG_NAME --libs-only-L` -Wl,-l:lib$LUA_PKG_NAME.a -lm"
AC_SUBST([LUA_LIBS], [$LUA_LIBS])

# GLib 2.0
PKG_CHECK_MODULES(GLIB, glib-2.0,,, AC_MSG_ERROR([Need GLib 2.0. See http://www.gtk.org/.]))

# CUDD 
AC_PATH_CUDD([2.3.1])

# The values SIZEOF_INT, SIZEOF_INT and SIZEOF_INT are set by cudd.m4.
# We need these values to create include/Kure.h from include/Kure.h.in.
AC_SUBST([SIZEOF_INT], [$ac_cv_sizeof_int])
AC_SUBST([SIZEOF_LONG], [$ac_cv_sizeof_long])
AC_SUBST([SIZEOF_VOID_P], [$ac_cv_sizeof_void_p])

CUDD_CFLAGS="$cudd_inc"
CUDD_LIBS="$cudd_ldflags $cudd_libs"

AC_SUBST([CUDD_CFLAGS])
AC_SUBST([CUDD_LIBS])


AC_CONFIG_FILES([Makefile
		 src/Makefile
		 include/KureCudd.h
		 kure2.pc])

AC_OUTPUT


echo
echo "Building Kure with prefix=$prefix, datarootdir=$datarootdir"
echo 
echo "Packages"
echo "  Cudd:           $with_cudd"
echo "   GMP:           $GMP_CFLAGS, $GMP_LIBS"
echo "   Lua:           $LUA_CFLAGS, $LUA_LIBS"
echo
echo " \$LIBS:          $LIBS"
echo " \$CFLAGS:        $CFLAGS"
echo
echo "Build with 'make' and install with 'make install'. Enjoy!"
echo
