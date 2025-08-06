/*
 * Kure.c
 *
 *  Copyright (C) 2010 Stefan Bolus, University of Kiel, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "Kure.h"

int kure_num_vars (const mpz_t n)
{
	if (mpz_cmp_ui(n,1) == 0) return 0;
	else {
		/* mpz_sizeinbase cannot be used directly because it returns the position
		 * of the MSB rather than the necessary number of variables. E.g. it
		 * returns 4 for 8 (1000b=8) even if 3 variables are sufficient. */
		mpz_t tmp;
		int vars;
		mpz_init_set(tmp, n);
		mpz_sub_ui(tmp,tmp,1);
		vars = mpz_sizeinbase (tmp,2);
		mpz_clear (tmp);
		return vars;
	}
}

int kure_check_version (int major, int minor, int micro)
{
  return KURE_CHECK_VERSION (major, minor, micro);
}
