/*
 * KureError.c
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

#include "KureImpl.h"
#include <string.h>

void kure_set_error (KureError ** perr, KureErrorCode code, const char * fmt, ...)
{
	if (perr) {
		va_list ap;

		va_start(ap, fmt);
		kure_set_error_va(perr, code, fmt, ap);
		va_end (ap);
	}
}


void kure_set_error_va (KureError ** perr, KureErrorCode code, const char * fmt, va_list ap)
{
	if (perr) {
		KureError * err = kure_error_new ();

		/* Free the old error, if there is one. */
		if (*perr) {
			kure_error_destroy (*perr);
			*perr = NULL;
		}

		if (fmt) {
			gmp_vasprintf (&err->message, fmt, ap);
		}
		else {
			// Use a default message for the given code instead?
			static const struct table_t {
				KureErrorCode code;
				char * message;
			} table[] = { { KURE_ERR_INV_ARG, "Invalid argument(s)." },
						  { KURE_ERR_DIFF_CONTEXTS, "Argument belong to different Kure contexts." },
						  { KURE_ERR_NOT_SAME_DIM, "Arguments of same dimension expected." },
						  { KURE_ERR_NOT_COMPAT_DIM, "Relations have incompatible dimensions." },
						  { KURE_ERR_OUT_OF_BOUNDS, "Position out of bounds." },
					  	  { KURE_ERR_SUCCESS, "Success" },
						  { -1, NULL } };
			const struct table_t *ptr = table;
			for ( ; ptr->message != NULL ; ptr++) {
				if (code == ptr->code) err->message = strdup (ptr->message);
			}
			if (! err->message) err->message = strdup ("Unknown error.");
		}
		err->code = code;

		*perr = err;
	}
}


KureError * kure_error_new ()
{
	KureError * self = (KureError*) calloc (1, sizeof(KureError));
	self->code = KURE_ERR_SUCCESS;
	return self;
}

void kure_error_destroy (KureError * err)
{
	if (err) {
		if (err->message) free (err->message);
		free (err);
	}
}
