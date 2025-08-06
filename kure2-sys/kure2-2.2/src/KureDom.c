/*
 * KureDom.c
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

static KureDom * _dp_new_copy (const KureDom * dom)
{
	KureDirectProduct * self = (KureDirectProduct*) dom;
	return kure_direct_product_new(self->first, self->second);
}

static void _dp_destroy (KureDom * dom)
{
	KureDirectProduct * self = (KureDirectProduct*) dom;
	mpz_clear (self->first);
	mpz_clear (self->second);
	free (self);
}

static int _dp_get_comp_count (const KureDom * dom)
{
	return 2; /* Implementation detail. */
}

static Kure_bool _dp_get_comp (const KureDom * dom, mpz_t size, int i)
{
	KureDirectProduct * self = (KureDirectProduct*) dom;
	if (0 == i)	mpz_set (size, self->first);
	else if (1 == i) mpz_set (size, self->second);
	else return FALSE;
	return TRUE;
}

static KureDom * _ds_new_copy (const KureDom * dom)
{
	KureDirectSum * self = (KureDirectSum*) dom;
	return kure_direct_sum_new(self->first, self->second);
}

static void _ds_destroy (KureDom * dom)
{
	KureDirectSum * self = (KureDirectSum*) dom;
	mpz_clear (self->first);
	mpz_clear (self->second);
	free (self);
}

static int _ds_get_comp_count (const KureDom * dom)
{
	return 2; /* Implementation detail. */
}

static Kure_bool _ds_get_comp (const KureDom * dom, mpz_t size, int i)
{
	KureDirectSum * self = (KureDirectSum*) dom;
	if (0 == i)	mpz_set (size, self->first);
	else if (1 == i) mpz_set (size, self->second);
	else return FALSE;
	return TRUE;
}

static KureDomClass _direct_product_class = { "Direct Product", _dp_new_copy, _dp_destroy,
		_dp_get_comp, _dp_get_comp_count };
static KureDomClass _direct_sum_class = { "Direct Sum", _ds_new_copy, _ds_destroy,
		_ds_get_comp, _ds_get_comp_count };

KureDom * kure_direct_product_new (mpz_t n, mpz_t m)
{
	if (mpz_cmp_si(n,0) <= 0 || mpz_cmp_si(m,0) <= 0) return NULL;
	else {
		KureDirectProduct * self = KURE_NEW0(KureDirectProduct,1);
		self->c = &_direct_product_class;
		mpz_set (self->first, n);
		mpz_set (self->second, m);
		return (KureDom*) self;
	}
}\

const KureDomClass * kure_dom_get_class (const KureDom * self) { return self->c; }
const KureDomClass * kure_direct_product_get_class () { return &_direct_product_class; }
const KureDomClass * kure_direct_sum_get_class () { return &_direct_sum_class; }

KureDom * kure_direct_product_new_si (int n, int m)
{
	KureDom * ret;
	mpz_t _n, _m;
	mpz_init_set_si (_n, n);
	mpz_init_set_si (_m, m);
	ret = kure_direct_product_new (_n,_m);
	mpz_clear (_n);
	mpz_clear (_m);
	return ret;
}

KureDom * kure_direct_sum_new (mpz_t n, mpz_t m)
{
	if (mpz_cmp_si(n,0) <= 0 || mpz_cmp_si(m,0) <= 0) return NULL;
	else {
		KureDirectSum * self = KURE_NEW0(KureDirectSum,1);
		self->c = &_direct_sum_class;
		mpz_set (self->first, n);
		mpz_set (self->second, m);
		return (KureDom*) self;
	}
}

KureDom * kure_direct_sum_new_si (int n, int m)
{
	KureDom * ret;
	mpz_t _n, _m;
	mpz_init_set_si (_n, n);
	mpz_init_set_si (_m, m);
	ret = kure_direct_sum_new (_n,_m);
	mpz_clear (_n);
	mpz_clear (_m);
	return ret;
}

const char * kure_dom_get_name (const KureDom * self) { return self->c->name; }

KureDom * kure_dom_new_copy (const KureDom * self)
{
	if (self) return self->c->new_copy(self); else return NULL;
}

void kure_dom_destroy (KureDom * self) {
	if (self) return self->c->destroy(self);
}

int kure_dom_get_comp_count (const KureDom * self) {
	if (self) return self->c->get_comp_count (self); else return -1;
}

Kure_bool kure_dom_get_comp (const KureDom * self, mpz_t size, int i) {
	if (self) return self->c->get_comp (self, size, i); else return FALSE;
}
