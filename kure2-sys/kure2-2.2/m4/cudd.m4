dnl
dnl  Copyright (C) 2009, 2010 Stefan Bolus, University of Kiel, Germany
dnl
dnl  This program is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU General Public License as published by
dnl  the Free Software Foundation; either version 2 of the License, or
dnl  (at your option) any later version.
dnl
dnl  This program is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl  GNU General Public License for more details.
dnl
dnl  You should have received a copy of the GNU General Public License
dnl  along with this program; if not, write to the Free Software
dnl  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


dnl
dnl Used internally.
dnl
AC_DEFUN([AC_PATH_CUDD_IMPL], [dnl
	dnl <cudd_dir>/include/cudd.h uses these values to define the DdNode
	dnl structure. It is crucial, that your current values fit to the
	dnl ones, used to compile CUDD itself. Otherwise strange things will
	dnl happen.
	AC_CHECK_SIZEOF([int])
	AC_CHECK_SIZEOF([long])
	AC_CHECK_SIZEOF([void *])

	dnl The user has to specify the CUDD directory with --with-cudd=<dir>.
	dnl The test will fail otherwise. The structure of the CUDD distrubution
	dnl is somewhat uncommon. While all header files are in a central 
	dnl directory (header/), object files are not. Each object file is in a
        dnl separate sub-directory (st, util, dddmp, epd, mtr, cudd). On the
	dnl top of that there is a separate directory for the object files of
	dnl the the C++ interface (obj).
	cudd_libs=""
	cudd_inc=""
	AC_ARG_WITH([cudd], AS_HELP_STRING([--with-cudd=DIR], [CUDD (BDD-package) location]))
	if test x"$with_cudd" != x; then
	   dnl CUDD doesn't have a single directory, where the objects are stored
	   dnl in. Each object file is in its own directory. The order in important.
	   cudd_ldflags=""
   	   cudd_libs=m4_foreach([i],[cudd, mtr, epd, dddmp, st, util],[" $with_cudd/"i"/lib"i".a"])
	   cudd_libs="$cudd_libs -lm"
	   cudd_inc="-I$with_cudd/include $cudd_inc"
	else
	   AC_MSG_ERROR([Please set the location of your CUDD distribution using the --with-cudd option.])
	fi

	have_cudd=yes

	saved_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="$cudd_inc $CPPFLAGS"
	AC_CHECK_HEADERS([cudd.h cuddInt.h],,
        	         [AC_MSG_ERROR([Unable to find CUDD headers in directory $cudd_inc.])])

	dnl In the next step we query the CUDD version. The CUDD version is
	dnl defined in the file cudd.h as CUDD_VERSION. Hence, we need the 
	dnl C Preprocessor, GREP and AWK. After this check, the version is
	dnl available in $cudd_version.
	AC_LANG_PUSH([C])
	AC_REQUIRE([AC_PROG_CPP])
	AC_REQUIRE([AC_PROG_GREP])

	AC_MSG_CHECKING([for correct version of CUDD])
	AC_LANG_CONFTEST([AC_LANG_SOURCE([[#include "cudd.h"
			                   CUDD_VERSION cudd-version-key]])])
        cudd_version=`$CPP $CPPFLAGS $cudd_inc conftest.c | $GREP cudd-version-key | $GREP -o '@<:@0-9.@:>@\+'`
	AS_ECHO_N("($cudd_version) ")

	AC_LANG_POP([C])

	dnl CUDD's libutil.a in cudd-<version>/util/ cannot easily be linked
	dnl using '-L<path> -lutil' on Darwin because it appears there is
	dnl another libtutil.a in the standard search directories which is
	dnl prefered to the CUDD library. Hence, it is recommended to use
	dnl '<path>/libutil.a' instead. On the other side, we cannot use
	dnl AC_CHECK_LIB for this. D'oh.

	dnl Now check the version of the given CUDD package against the given
	dnl requested version. A bad version will make the test fail.
	AS_VERSION_COMPARE($cudd_version, $1,
		   [AC_MSG_RESULT([no]); have_cudd=no], # less
		   [AC_MSG_RESULT([yes])], [AC_MSG_RESULT([yes])])
        if test x"$have_cudd" != xyes; then
   	   AC_MSG_ERROR([You need CUDD $1+ in order to compile this package. CUDD is available from http://vlsi.colorado.edu/~fabio/CUDD/.])
	fi

	dnl Check if the CUDD libraries are available and can be linked.
	dnl AC_CHECK_LIB automatically prepends the libraries to the LIBS
	dnl variable. The given functions calls where chosen arbitrarily.
	dnl Due to dependencies among the libraries, the order of the 
	dnl tests is mandatory in some cases.
	saved_LDFLAGS="$LDFLAGS"
	saved_LIBS="$LIBS"
	LDFLAGS="$cudd_ldflags"
	LIBS="$cudd_libs"
	AC_MSG_CHECKING([for usability of CUDD])
	AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <stdio.h>
					 #include "cudd.h"],
					[DdManager * manager = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);
					 Cudd_Quit(manager);
					 return 0;])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_ERROR([Unable to link CUDD. See config.log for details.])])
	cudd_libs="$LIBS"
	LDFLAGS="$saved_LDFLAGS"
	LIBS="$saved_libs"
	CPPFLAGS="$saved_CPPFLAGS"

	AC_SUBST([cudd_inc])
	AC_SUBST([cudd_ldflags])
	AC_SUBST([cudd_libs])
]) dnl AC_DEFUN


dnl
dnl Check if CUDD is present in a given directory. The directory has to be
dnl passes with the user using the --with-cudd option. The function expects
dnl exactly one argument which is the minimum version of the CUDD version
dnl required.
dnl
AC_DEFUN([AC_PATH_CUDD],[dnl
	AC_LANG_PUSH([C])
	AC_PATH_CUDD_IMPL([$1])
	AC_LANG_POP([C])
])


dnl
dnl Check if CUDD is available and has at least the given version. Additionally,
dnl check if the CUDD C++ interface is available and can be used in the same CUDD
dnl distribution.
dnl
AC_DEFUN([AC_PATH_CUDD_OBJ],[dnl
	dnl For the following tests use C++. Ensure there is C++ compiler 
	dnl around.

	AC_LANG_PUSH([C++])

	dnl Ensure CUDD is present and at least the C part is working with our
	dnl C++ compiler.
	AC_PATH_CUDD_IMPL([$1])

	dnl Check for the CUDD C++ header file: cuddObj.hh
	saved_CPPFLAGS="$CPPFLAGS"                                                                                     
	CPPFLAGS="$cudd_inc"
	AC_CHECK_HEADERS([cuddObj.hh],,
		[AC_MSG_ERROR([Unable to find CUDD C++ headers in $cudd_inc])])

		echo "with_cudd: $with_cudd"

	dnl Check for a working static library libobj. Don't forget to add the 
	dnl obj/ directory to our LDFLAGS. The cudd directory is available in
	dnl $with_cudd.
	saved_LDFLAGS="$LDFLAGS"
	saved_LIBS="$LIBS"
	LDFLAGS="$cudd_ldflags" 
	LIBS="$with_cudd/obj/libobj.a $cudd_libs"
	AC_MSG_CHECKING([for C++ binding of CUDD package.]) 
	AC_TRY_LINK([#include "cuddObj.hh"], [Cudd cudd (1);],
		AC_MSG_RESULT([yes]),
		AC_MSG_ERROR([Unable to link CUDD C++ library.]))
	cudd_libs="$LIBS"
	LIBS="$saved_LIBS"
	LDFLAGS="$saved_LDFLAGS"
	CPPFLAGS="$saved_CPPFLAGS"
	
	AC_LANG_POP([C++])
])
