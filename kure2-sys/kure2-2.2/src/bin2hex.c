/*
 * bin2hex.c
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

#include <stdio.h>

int main (int argc, char ** argv)
{
	unsigned char buf[BUFSIZ];
	size_t n;
	char is_first = 1;
	FILE * fp = fdopen (fileno(stdin), "rb");

	while (! feof (fp)) {
		size_t i;
		n = fread(buf, 1, BUFSIZ, fp);
		for (i = 0 ; i < n ; i ++) {
			if (! is_first)	putchar (',');
			else is_first = 0;

			printf ("0x%x", buf[i]);
		}
	}

	return 0;
}
