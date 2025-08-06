/*
 * KureContext.c
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
#include <stdlib.h>

KureContext * kure_context_new ()
{
	DdManager * manager = Cudd_Init (0, 0, CUDD_UNIQUE_SLOTS,
			CUDD_CACHE_SLOTS, 0);
	return kure_context_new_with_manager(manager);
}

static float _random_wrap (void * dummy)
{
#if defined (__SVR4) && defined (__sun)
	/* On SunOS RAND_MAX is 32767, while random() returns number in range
	 * [0,2**31-1]. */
	return (float)(random() / (double)((1ul << 31)-1));
#else
	return (float)(random() / (double)RAND_MAX);
#endif
}

KureContext * kure_context_new_with_manager (DdManager * manager)
{
	KureContext * self = (KureContext*) calloc(1, sizeof(KureContext));
	self->manager = manager;
	self->refs    = 0;

	/* Disable automatic reordering. */
	Cudd_AutodynDisable (self->manager);

	/* Set the default random number generator. */
	self->random_func = _random_wrap;
	self->random_udata = NULL;

	return self;
}

void kure_context_destroy (KureContext * self)
{
	if (self) {
#ifdef KURE_VERBOSE
		if (kure_context_get_refs(self) > 0)
			fprintf (stderr, "kure_context_destroy: The given context is references somewhere.\n");
		else {
			fprintf (stderr, "kure_context_destroy: Destroying context.\n");
		}
#endif

		Cudd_CheckZeroRef (self->manager);
		Cudd_Quit(self->manager);
		free (self);
	}
}

DdManager *	kure_context_get_manager (KureContext * self) {
	return self->manager; }

void kure_context_set_error (KureContext * context, KureErrorCode code, const char * fmt, ...)
{
	va_list ap;
	va_start (ap, fmt);
	kure_set_error_va (&context->error, code, fmt, ap);
	va_end (ap);
}

KureError * kure_context_get_error (KureContext * self) { return self->error; }

void kure_context_ref (KureContext * self) { if(self) self->refs++; }
void kure_context_deref (KureContext * self)
{
	if (self)
	{
		self->refs--;
		if	(self->refs <= 0) {
			kure_context_destroy (self);
		}
	}
}
int kure_context_get_refs (KureContext * self) { return self ? self->refs : 0; }

KureRandomFunc kure_context_get_random_func (KureContext * self)
{
	return self->random_func;
}

void * kure_context_get_random_udata (KureContext * self)
{
	return self->random_udata;
}

void kure_context_set_random_func (KureContext * self,
					KureRandomFunc random_func, void * udata)
{
	if (NULL == random_func) /* reset */ {
		self->random_func  = _random_wrap;
		self->random_udata = NULL;
	}
	else {
		self->random_func  = random_func;
		self->random_udata = udata;
	}
}

float kure_context_random (KureContext * context)
{
	return context->random_func (context->random_udata);
}
