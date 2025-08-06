/*
 * KureRel.c
 *
 *  Copyright (C) 2010,2011,2012 Stefan Bolus, University of Kiel, Germany
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

/* Cudd's util.h defines MAX and MIN but doesn't check for previously defined
 * macros of that name which causes a compiler warning. This is a workaround.
 */

#include "KureImpl.h"

/* dddmp.h would redefine MAX and MIN */
#undef MIN
#undef MAX
#include <dddmp.h>
#include <errno.h>
#include <string.h> // strerror

#ifndef MAX
#  define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef MIN
#  define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#define _KURE_CHECK_VALID3(a,b,c) if (!(a) || !(b) || !(c)) { return FALSE; }
#define _KURE_CHECK_VALID2(a,b) if (!(a) || !(b)) { return FALSE; }
#define _KURE_CHECK_VALID1(a) if (!(a)) { return FALSE; }

#define _KURE_CHECK_HOMOGEN(rel) if ( mpz_cmp((rel)->rows,(rel)->cols)!=0) { \
				kure_context_set_error ((rel)->context, KURE_ERR_INV_ARG, \
						"Homogeneous relation expected. Got dimension (%Zd,%Zd).", \
						(rel)->rows,(rel)->cols); return FALSE; }

#define _KURE_CHECK_SAME_CONTEXT2(a,b) if ((a)->context != (b)->context) { \
	kure_context_set_error (a->context, KURE_ERR_DIFF_CONTEXTS, NULL); return FALSE; }

#define _KURE_CHECK_SAME_CONTEXT3(a,b,c) if ((a)->context != (b)->context \
		|| (b)->context != (c)->context) { \
	kure_context_set_error (a->context, KURE_ERR_DIFF_CONTEXTS, NULL); return FALSE; }

#define _KURE_CHECK_SAME_DIM2(a,b) if ( !kure_rel_same_dim((a),(b))) { \
	kure_context_set_error ((a)->context, KURE_ERR_NOT_SAME_DIM, NULL); return FALSE; }

#define _KURE_CHECK_COMPAT_DIM(d1,d2) if ( !gmp_equal((d1), (d2))) { \
	kure_context_set_error(rop->context, KURE_ERR_NOT_COMPAT_DIM, \
		"Relations are not compatible with mult. %Zd != %Zd\n", (d1),(d2)); \
		return FALSE; }

#define _KURE_CHECK_IS_IN(rel,row,col) if (mpz_cmp_ui((row),0)<0 \
	|| mpz_cmp_ui((col),0)<0 || !gmp_less((row),(rel)->rows) \
			|| !gmp_less((col),(rel)->cols)) { \
				kure_context_set_error ((rel)->context, KURE_ERR_OUT_OF_BOUNDS, \
						"The given 0-indexed bit (row,col)=(%Zd,%Zd) was out of bounds (%Zd,%Zd).", \
						(row),(col),(rel)->rows,(rel)->cols); return FALSE; }

#define _KURE_CHECK_IS_IN_SI(rel,row,col) if (row<0 || col < 0 \
	|| mpz_cmp_si((rel)->rows,row)<=0 || mpz_cmp_si((rel)->cols,col)<=0) { \
				kure_context_set_error ((rel)->context, KURE_ERR_OUT_OF_BOUNDS, \
						"The given 0-indexed bit (row,col)=(%d,%d) was out of bounds (%Zd,%Zd).", \
						(row),(col),(rel)->rows,(rel)->cols); return FALSE; }

#define _KURE_CHECK_POSITIVE(c,n) if ( mpz_cmp_si(n, 0) <= 0) { \
	kure_context_set_error((c), KURE_ERR_INV_ARG, \
		"Value greater 0 expected. Got %Zd\n", n); \
		return FALSE; }

#define _KURE_CHECK_POSITIVE_SI(c,n) if (n <= 0) { \
	kure_context_set_error((c), KURE_ERR_INV_ARG, \
		"Value greater 0 expected. Got %Zd\n", n); \
		return FALSE; }

#define _KURE_CHECK_ROWS_FITS_SI(a) if ( !mpz_fits_sint_p ((a)->rows)) { \
		kure_context_set_error(rop->context, KURE_ERR_INV_ARG, \
			"Relation has too much rows. More than %d\n", INT_MAX); \
			return FALSE; }

#define _KURE_CHECK_PROBABILITY(prob) if ((prob) < 0 || (prob) > 1) { \
	kure_context_set_error (self->context, KURE_ERR_INV_ARG, \
		"Probability has to be in [0,1]."); return FALSE; }

#define _KURE_CHECK_NON_EMPTY(rel) if (test_on_empty((KureRel*)rel)) { \
	kure_context_set_error ((rel)->context, KURE_ERR_INV_ARG, \
		"Non-empty relation expected."); return FALSE; }

#define _KURE_CHECK_IS_ROW_IN(rel, row) if ( mpz_cmp(rel->rows, row) <= 0) { \
	kure_context_set_error ((rel)->context, KURE_ERR_OUT_OF_BOUNDS, \
			"Row is out of bounds."); return FALSE; }

#define _KURE_CHECK_DIM(c,rows,cols) if (mpz_cmp_si(rows,0) <= 0 \
	|| mpz_cmp_si(cols,0) <= 0) { \
		kure_context_set_error ((c), KURE_ERR_INV_ARG, \
				"Invalid dimension (rows, cols)=(%Zd,%Zd).", rows, cols); return FALSE;	}

#define _KURE_CHECK_DIM_SI(c,rows,cols) if (rows <= 0 || cols <= 0) { \
	kure_context_set_error ((c), KURE_ERR_INV_ARG, \
			"Invalid dimension (rows,cols)=(%d,%d).", rows, cols); return FALSE; }

#define _KURE_CHECK_FITS_SI(rel) if ( !kure_rel_fits_si(rel)) { \
	kure_context_set_error ((rel)->context, KURE_ERR_INV_ARG, \
			"Relation has more than %d rows and/or columns.", INT_MAX); return FALSE; }

#define _KURE_CHECK_SIZE_FITS_SI(rel) if ( !kure_rel_prod_fits_si(rel)) { \
	kure_context_set_error ((rel)->context, KURE_ERR_INV_ARG, \
			"Relation's size is too big. More than %d bits.", INT_MAX); return FALSE; }

#define _KURE_CHECK_HAS_ONE_COL(rel) if (0 != mpz_cmp_si(rel->cols,1)) { \
	kure_context_set_error(rop->context, KURE_ERR_INV_ARG, \
		"Relation has to have just one column instead of %Zd.\n", rel->cols); \
		return FALSE; }

/*!
 * Compares op1 to base^exp and returns the result using \ref mpz_cmp.
 *
 * \author stefan
 * \date Aug 14, 2012
 */
static int _mpz_cmp_ui_pow_ui (const mpz_t op1, unsigned long base, unsigned long exp)
{
	int result;
	mpz_t tmp;
	mpz_init (tmp);
	mpz_ui_pow_ui (tmp, base, exp);
	result = mpz_cmp (op1, tmp);
	mpz_clear (tmp);
	return result;
}

#define _KURE_CHECK_HAS_2_POWER_N_ROWS(rel) \
		if (0 != _mpz_cmp_ui_pow_ui (rel->rows, 2, kure_rel_get_vars_rows(rel))) { \
			kure_context_set_error(rop->context, KURE_ERR_INV_ARG, \
					"Relation has to have 2^%d rows instead of %Zd.\n", kure_rel_get_vars_rows(rel), rel->rows); \
		return FALSE; }


KureRel * kure_rel_new_with_size (KureContext * context, const mpz_t rows, const mpz_t cols)
{
	if (context) {
		KureRel * self = (KureRel*) calloc(1, sizeof(KureRel));
		self->context = context;
		self->bdd = Cudd_Not(Cudd_ReadOne(context->manager));
		Cudd_Ref(self->bdd);
		mpz_init_set(self->rows, rows);
		mpz_init_set(self->cols, cols);

		return self;
	}
	else return NULL;
}

KureRel * kure_rel_new_with_size_si (KureContext * context, int rows, int cols)
{
	KureRel * ret;
	mpz_t _rows, _cols;
	mpz_init_set_si (_rows,rows);
	mpz_init_set_si (_cols,cols);
	ret = kure_rel_new_with_size (context, _rows, _cols);
	mpz_clear(_rows);
	mpz_clear(_cols);
	return ret;
}

KureRel * kure_rel_new_with_proto (const KureRel * proto) {
	return kure_rel_new_with_size (proto->context, proto->rows, proto->cols); }

KureRel * kure_rel_new_with_rows (KureContext * context, const mpz_t rows)
{
	KureRel * ret;
	mpz_t one; mpz_init_set_si (one, 1);
	ret = kure_rel_new_with_size (context, rows, one);
	mpz_clear (one); return ret;
}

KureRel * kure_rel_new_with_cols (KureContext * context, const mpz_t cols)
{
	KureRel * ret;
	mpz_t one; mpz_init_set_si (one, 1);
	ret = kure_rel_new_with_size (context, one, cols);
	mpz_clear (one); return ret;
}

/*!
 * Creates an empty new relation with 1 row an 1 column. Returns NULL
 * on error.
 */
KureRel * kure_rel_new (KureContext * context)
{
	return kure_rel_new_with_size_si (context, 1,1);
}

/*!
 * Copy constructor.
 */
KureRel * kure_rel_new_copy (const KureRel * that)
{
	return kure_rel_new_from_bdd (that->context, that->bdd,
			that->rows, that->cols);
}

Kure_success kure_rel_assign_from_string (KureRel * self, const char * s,
		int rows, int cols,	const char one_ch, const char zero_ch)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_DIM_SI(self->context, rows, cols)
	else if (NULL == s) {
		kure_context_set_error(self->context, KURE_ERR_INV_ARG, NULL); return FALSE;
	}
	else {
		DdManager * manager = kure_context_get_manager (self->context);
		const char * ptr = s;
		int i,j;

		kure_rel_set_size_si (self, rows, cols);

		/* Clear the relation. */
		Cudd_RecursiveDeref (manager, self->bdd);
		self->bdd = Cudd_Not(Cudd_ReadOne(manager));
		Cudd_Ref(self->bdd);

		for (i = 0 ; i < rows; i ++) {
			for (j = 0 ; j < cols ;) {
				char ch = *ptr++;
				if (one_ch == ch) {
					set_rel_bit (self, i,j);
					j++;
				}
				else if (zero_ch == ch) j++;
				else if ('\0' == ch) goto done;
				else { /* Irrelevant character, e.g.
					    * newline, carriage return, etc. */	}
			}
		}

done:	;
		return TRUE;
	}
}

KureRel * kure_rel_new_from_string (KureContext * context, const char * s,
					int rows, int cols,	const char one_ch, const char zero_ch)
{
	if ( !context) return NULL;
	else {
		KureRel * rel = kure_rel_new(context);
		if ( !rel) return NULL;
		else {
			Kure_success success
				= kure_rel_assign_from_string (rel, s, rows, cols, one_ch, zero_ch);
			if ( !success) {
				kure_rel_destroy(rel);
				return NULL;
			}
			else return rel;
		}
	}
}

KureRel * kure_rel_new_from_bdd (KureContext * context, DdNode * node,
		const mpz_t rows, const mpz_t cols)
{
	if (!node) return NULL;
	else {
		KureRel * self = NULL;

		Cudd_Ref(node);
		self = kure_rel_new_with_size(context, rows, cols);
		if (self) {
			/* At this point, self->bdd is always the constant zero. Do not
			 * dereference that node. */
			assert (Cudd_IsConstant(self->bdd));
			self->bdd = node;
		}
		else /* error */ {
			/* If the node has reference count 1, it had 0 just before we were
			 * called. Something different from Cudd_Deref would recursively
			 * destroy the node, but we are not responsible for that. */
			Cudd_Deref (node);
		}

		return self;
	}
}

KureRel * kure_rel_new_from_bdd_si (KureContext * context, DdNode * node, int rows, int cols)
{
	KureRel * ret;
	mpz_t _rows, _cols;
	mpz_init_set_si (_rows, rows);
	mpz_init_set_si (_cols, cols);
	ret = kure_rel_new_from_bdd (context, node, _rows, _cols);
	mpz_clear (_rows);
	mpz_clear (_cols);
	return ret;
}


void kure_rel_destroy (KureRel * self)
{
	if (self) {
		Cudd_RecursiveDeref(self->context->manager, self->bdd);
		mpz_clear(self->rows);
		mpz_clear(self->cols);
		free (self);
	}
}


Kure_success kure_rel_set_size (KureRel * self, const mpz_t rows, const mpz_t cols)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_DIM_SI(self->context, rows, cols)
	else {
		mpz_set (self->rows, rows);
		mpz_set (self->cols, cols);

		return TRUE;
	}
}

Kure_success kure_rel_set_size_si (KureRel * self, int rows, int cols)
{
	Kure_success ret;
	mpz_t _rows, _cols;
	mpz_init_set_si (_rows,rows);
	mpz_init_set_si (_cols,cols);
	ret = kure_rel_set_size (self, _rows, _cols);
	mpz_clear(_rows);
	mpz_clear(_cols);
	return ret;
}


/*!
 * Returns TRUE if the number of rows and columns fits into a signed integer
 * each.
 */
Kure_bool kure_rel_fits_si (const KureRel * self)
{
	return mpz_fits_sint_p (self->rows) && mpz_fits_sint_p(self->cols);
}

/*!
 * Returns TRUE if the product of rows and columns fits into a signed integer.
 * This is a stronger property than \ref kure_rel_fits_si offers and important
 * to allocate memory for the bits of a relation.
 */
Kure_bool kure_rel_prod_fits_si (const KureRel * self)
{
	Kure_bool ret;
	mpz_t t;
	mpz_init_set (t, self->rows);
	mpz_mul (t,t,self->cols);
	ret = mpz_fits_sint_p (t);
	mpz_clear (t);
	return ret;
}

/*!
 * Returns TRUE if the number of rows fits into a signed integer.
 */
Kure_bool 		kure_rel_rows_fits_si (const KureRel * self) {
	return mpz_fits_sint_p (self->rows);
}

/*!
 * Returns TRUE if the number of cols fits into a signed integer.
 */
Kure_bool 		kure_rel_cols_fits_si (const KureRel * self) {
	return mpz_fits_sint_p(self->cols);
}


/*!
 * Stores the number of rows in the given argument which must be
 * initialized.
 */
void kure_rel_get_rows (const KureRel * self, mpz_t rows)
{
	mpz_set (rows, self->rows);
}

/*!
 * Same as \refkure_rel_get_rows but now for the columns.
 */
void kure_rel_get_cols (const KureRel * self, mpz_t cols)
{
	mpz_set (cols, self->cols);
}


/*!
 * Returns the number of rows as signed integer. You have to ensure yourself,
 * that the number fits into a signed int. If it does not you could get strange
 * results.
 */
int kure_rel_get_rows_si (const KureRel * self)
{
	return mpz_get_si (self->rows);
}

/*!
 * Same as \ref kure_rel_get_cols_si but now for the columns.
 */
int kure_rel_get_cols_si (const KureRel * self)
{
	return mpz_get_si (self->cols);
}


/*!
 * Returns TRUE if both relations have the same number of rows and columns,
 * respectively.
 */
Kure_bool kure_rel_same_dim(const KureRel * a, const KureRel * b)
{
	if (a == b) return TRUE;
	else return (mpz_cmp (a->cols, b->cols) == 0)
			&& (mpz_cmp(a->rows, b->rows) == 0);
}

KureContext * kure_rel_get_context (KureRel * self) { return self->context; }

int	kure_rel_get_vars_rows (const KureRel* self)  { return Kure_num_vars (self->rows); }
int	kure_rel_get_vars_cols (const KureRel * self) { return Kure_num_vars (self->cols); }

static Kure_success _kure_rel_to_string_wrapper (KureRel * rel, char one_ch, char zero_ch, char ** pstr)
{
	_KURE_CHECK_VALID1(rel)
	else _KURE_CHECK_SIZE_FITS_SI(rel)
	else {
		int rows = kure_rel_get_rows_si (rel), cols = kure_rel_get_cols_si (rel);
		char * s = (char*) malloc (rows * (size_t)cols + (size_t)rows/*\n*/ + 1/*\0*/);
		char * p = s;
		int i,j;
		int vars_rows = kure_rel_get_vars_rows(rel),
				vars_cols = kure_rel_get_vars_cols(rel);

		for (i = 0 ; i < rows ; ++i) {
			for (j = 0 ; j < cols ; ++j, p) {
				*p++ = (kure_get_bit_fast_si(rel, i,j, vars_rows, vars_cols)
						? one_ch : zero_ch);
			}
			*p++ = '\n';
		}

		*p = '\0';
		*pstr = s;
		return TRUE;
	}
}

DdNode * kure_rel_get_bdd (const KureRel * self) { return self->bdd; }
void kure_rel_set_bdd (KureRel * self, DdNode * node)
{
	Cudd_Ref (node);
	Cudd_IterDerefBdd (self->context->manager, self->bdd);

	self->bdd = node;
}

char * kure_rel_to_string (KureRel * rel, char one_ch, char zero_ch)
{
	char * s = NULL;
	Kure_success success = _kure_rel_to_string_wrapper(rel, one_ch, zero_ch, &s);
	if (success) return s;
	else return NULL;
}

Kure_success kure_rel_read_from_dddmp_stream (KureRel * self, FILE * fp,
		const mpz_t rows, const mpz_t cols)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_DIM(self->context, rows, cols)
	else if ( !fp) {
		kure_context_set_error(self->context, KURE_ERR_INV_ARG, "Given stream is NULL.");
		return FALSE;
	}
	else {
		KureContext * context = self->context;
		DdManager * manager = kure_context_get_manager(context);
		DdNode * bdd = Dddmp_cuddBddLoad(manager, DDDMP_VAR_MATCHIDS, NULL,
				NULL, NULL, DDDMP_MODE_DEFAULT, NULL, fp);

		kure_rel_set_bdd (self, bdd);
		kure_rel_set_size (self, rows, cols);
		return TRUE;
	}
}

Kure_success kure_rel_read_from_dddmp_file (KureRel * self, const char * file,
		const mpz_t rows, const mpz_t cols)
{
	_KURE_CHECK_VALID1(self)
	else if (!file) {
		kure_context_set_error(self->context, KURE_ERR_INV_ARG,
				"Given filename is NULL.");
		return FALSE;
	}
	else {
		FILE * fp = fopen (file, "rb");
		if ( !fp) {
			kure_context_set_error(self->context, KURE_ERR_INV_ARG,
					"Unable to open file \"%s\" for reading. Reason: %s",
					file, strerror(errno));
			return FALSE;
		}
		else {
			Kure_success ret = kure_rel_read_from_dddmp_stream (self, fp, rows, cols);
			fclose (fp);
			return ret;
		}
	}
}

Kure_success kure_rel_write_to_dddmp_stream (const KureRel * self, FILE * fp)
{
	_KURE_CHECK_VALID1(self)
	else if ( !fp) {
		kure_context_set_error(self->context, KURE_ERR_INV_ARG,
				"Given stream is NULL.");
		return FALSE;
	}
	else {
		KureContext * context = self->context;
		DdManager * manager = kure_context_get_manager(context);
		int success = Dddmp_cuddBddStore (manager, NULL, kure_rel_get_bdd(self),
				NULL, NULL, DDDMP_MODE_DEFAULT, (Dddmp_VarInfoType) NULL,
                NULL, fp);
        if ( !success) {
        	kure_context_set_error(context, 0, "Dddmp_cuddBddStore failed. "
        			"Reason unknown.");
        }

        return success;
	}
}

Kure_success kure_rel_write_to_dddmp_file (const KureRel * self, const char * file)
{
	_KURE_CHECK_VALID1(self)
	else if ( !file) {
		kure_context_set_error(self->context, KURE_ERR_INV_ARG,
				"Given filename is NULL.");
		return FALSE;
	}
	else {
		FILE * fp = fopen (file, "wb");
		if ( !fp) {
			kure_context_set_error(self->context, KURE_ERR_INV_ARG,
					"Unable to open file \"%s\" for writing. Reason: %s",
					file, strerror(errno));
			return FALSE;
		}
		else {
			Kure_success ret = kure_rel_write_to_dddmp_stream (self, fp);
			fclose (fp);
			return ret;
		}
	}
}

/*******************************************************************************
 *                               Basic Relations                               *
 *                                                                             *
 *                              Thu, 11 Mar 2010                               *
 ******************************************************************************/


Kure_success kure_null(KureRel * self) {	return kure_O(self); }
Kure_success kure_O (KureRel * self)
{
	if (self) { null_rel(self); return TRUE; }
	else return FALSE;
}

Kure_success kure_all (KureRel * self) { return kure_L(self); }
Kure_success kure_L (KureRel * self)
{
	if (self) { L_rel(self); return TRUE; }
	else return FALSE;
}

Kure_success kure_identity (KureRel * self) { return kure_I(self); }
Kure_success kure_I (KureRel * self)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_HOMOGEN(self)
	else {
		eins_rel(self); return TRUE;
	}
}


/*******************************************************************************
 *                              Basic Operations                               *
 *                                                                             *
 *                              Thu, 11 Mar 2010                               *
 ******************************************************************************/



Kure_success kure_and (KureRel * rop, const KureRel * arg1, const KureRel * arg2)
{
	_KURE_CHECK_VALID3(rop,arg1,arg2)
	else _KURE_CHECK_SAME_CONTEXT3(rop,arg1,arg2)
	else _KURE_CHECK_SAME_DIM2(arg1,arg2)
	else {
		und_rel((KureRel*)arg1, (KureRel*)arg2, rop);
		return TRUE;
	}
}

Kure_success kure_or (KureRel * rop, const KureRel * arg1, const KureRel * arg2)
{
	_KURE_CHECK_VALID3(rop,arg1,arg2)
	else _KURE_CHECK_SAME_CONTEXT3(rop,arg1,arg2)
	else _KURE_CHECK_SAME_DIM2(arg1,arg2)
	else {
		oder_rel((KureRel*)arg1, (KureRel*)arg2, rop);
		return TRUE;
	}
}

Kure_success kure_transpose (KureRel * rop, const KureRel * arg)
{
	_KURE_CHECK_VALID2(rop,arg)
	else _KURE_CHECK_SAME_CONTEXT2(rop,arg)
	else {
		transp_rel((KureRel*)arg, rop);
		return TRUE;
	}
}

Kure_success kure_complement (KureRel * rop, const KureRel * arg)
{
	_KURE_CHECK_VALID2(rop,arg)
	else _KURE_CHECK_SAME_CONTEXT2(rop,arg)
	else {
		kompl_rel((KureRel*)arg, rop);
		return TRUE;
	}
}

Kure_success kure_mult (KureRel * rop, const KureRel * arg1, const KureRel * arg2)
{
	_KURE_CHECK_VALID3(rop,arg1,arg2)
	else _KURE_CHECK_SAME_CONTEXT3(rop,arg1,arg2)
	else _KURE_CHECK_COMPAT_DIM(arg1->cols, arg2->rows)
	else {
		mult_rel((KureRel*)arg1, (KureRel*)arg2, rop);
		return TRUE;
	}
}

Kure_success kure_mult_transp_norm (KureRel * rop, const KureRel * arg1, const KureRel * arg2)
{
	_KURE_CHECK_VALID3(rop,arg1,arg2)
	else _KURE_CHECK_SAME_CONTEXT3(rop,arg1,arg2)
	else _KURE_CHECK_COMPAT_DIM(arg1->rows, arg2->rows)
	else {
		mult_rel_transp_norm((KureRel*) arg1, (KureRel*) arg2, rop);
		return TRUE;
	}
}

Kure_success kure_mult_norm_transp (KureRel * rop, const KureRel * arg1, const KureRel * arg2)
{
	_KURE_CHECK_VALID3(rop,arg1,arg2)
	else _KURE_CHECK_SAME_CONTEXT3(rop,arg1,arg2)
	else _KURE_CHECK_COMPAT_DIM(arg1->cols, arg2->cols)
	else {
		mult_rel_norm_transp((KureRel*)arg1, (KureRel*)arg2, rop);
		return TRUE;
	}
}

Kure_success kure_mult_transp_transp (KureRel * rop, const KureRel * arg1, const KureRel * arg2)
{
	_KURE_CHECK_VALID3(rop,arg1,arg2)
	else _KURE_CHECK_SAME_CONTEXT3(rop,arg1,arg2)
	else _KURE_CHECK_COMPAT_DIM(arg1->rows, arg2->cols)
	else {
		mult_rel_transp_transp((KureRel*)arg1, (KureRel*)arg2, rop);
		return TRUE;
	}
}


/*******************************************************************************
 *                                 Bit Access                                  *
 *                                                                             *
 *                              Thu, 11 Mar 2010                               *
 ******************************************************************************/

Kure_success kure_set_bit (KureRel * self, Kure_bool yesno, mpz_t row, mpz_t col)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_IS_IN(self,row,col)
	else {
		if (yesno) mp_set_rel_bit(self,row,col);
		else mp_clear_rel_bit(self,row,col);
		return TRUE;
	}
}

Kure_success kure_set_bit_si (KureRel * self, Kure_bool yesno, int row, int col)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_IS_IN_SI(self,row,col)
	else {
		if (yesno) set_rel_bit(self,row,col);
		else clear_rel_bit(self,row,col);
		return TRUE;
	}
}

// We need this in order to use our check macros.
static KURE_INLINE Kure_success
_kure_get_bit_swapped (KureRel * self, mpz_t row, mpz_t col, Kure_bool *pval)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_IS_IN(self,row,col)
	else {
		*pval = mp_get_rel_bit(self, row, col);
		return TRUE;
	}
}

Kure_bool kure_get_bit (KureRel * self, mpz_t row, mpz_t col, Kure_success * psuccess)
{
	Kure_bool val;
	Kure_success s = _kure_get_bit_swapped (self, row, col, &val);
	if (psuccess) *psuccess = s;
	return val;
}

Kure_bool kure_get_bit_fast (KureRel * self, mpz_t row, mpz_t col, int vars_rows, int vars_cols)
{
	/* To check vars_rows and vars_cols would not make any sense here, because
	 * their idea is to speed up things. */
	return mp_get_rel_bit_full (self, row, col, vars_rows, vars_cols);
}

// We need this in order to use our check macros.
static KURE_INLINE Kure_success
_kure_get_bit_si_swapped (KureRel * self, int row, int col, Kure_bool *pval)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_IS_IN_SI(self,row,col)
	else {
		*pval = get_rel_bit(self, row, col);
		return TRUE;
	}
}

Kure_bool kure_get_bit_si (KureRel * self, int row, int col, Kure_success * psuccess)
{
	Kure_bool val;
	Kure_success s = _kure_get_bit_si_swapped (self, row, col, &val);
	if (psuccess) *psuccess = s;
	return val;
}

Kure_bool kure_get_bit_fast_si (KureRel * self, int row, int col, int vars_rows, int vars_cols)
{
	/* To check vars_rows and vars_cols would not make any sense here, because
	 * their idea is to speed up things. */
	return get_rel_bit_full (self, row, col, vars_rows, vars_cols);
}


/*******************************************************************************
 *                                    Rows                                     *
 *                                                                             *
 *                              Tue, 23 Mar 2010                               *
 ******************************************************************************/

Kure_success kure_set_row (KureRel * self, Kure_bool yesno, mpz_t row)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_IS_ROW_IN(self, row)
	else {
		set_rel_line(self, row); return TRUE;
	}
}

Kure_success kure_set_row_si (KureRel * self, Kure_bool yesno, int row)
{
	Kure_success ret;
	mpz_t _row;
	mpz_init_set_si (_row, row);
	ret = kure_set_row(self, yesno, _row);
	mpz_clear (_row);
	return ret;
}


/*******************************************************************************
 *                                 Cardinality                                 *
 *                                                                             *
 *                              Tue, 23 Mar 2010                               *
 ******************************************************************************/

Kure_success kure_less_card (KureRel * rop, const KureRel * arg)
{
	_KURE_CHECK_VALID2(rop, arg)
	else _KURE_CHECK_SAME_CONTEXT2(rop, arg)
	else _KURE_CHECK_ROWS_FITS_SI(arg)
	else {
		lesscard_rel ((KureRel*)arg, rop); return TRUE;
	}
}

Kure_success kure_subsetvec_rel (KureRel * rop, const KureRel * pot, const KureRel * vec)
{
	_KURE_CHECK_VALID3(rop, pot, vec)
	else _KURE_CHECK_SAME_CONTEXT3(rop, pot, vec)
	else _KURE_CHECK_ROWS_FITS_SI(vec)
	else if ( !gmp_is_pow2(pot->rows)) {
		kure_context_set_error (rop->context, KURE_ERR_INV_ARG,
				"Number of columns for `pot` must be a power of 2.");
		return FALSE;
	}
	else if ( mpz_cmp_si(vec->rows, Kure_num_vars(pot->rows)) > 0) {
		kure_context_set_error (rop->context, KURE_ERR_INV_ARG,
				"log2(pot.rows) must be greater or equal to vec.rows.");
		return FALSE;
	}
	else {
		subsetvector_rel ((KureRel*)pot, (KureRel*)vec, rop); return TRUE;
	}
}

#define _CARD_OP(name,op) \
Kure_bool kure_is_card##name (const KureRel * a, const KureRel * b, Kure_success *psuccess) { \
	Kure_bool ret = FALSE; \
	Kure_success success = TRUE; \
	mpz_t x,y; \
	mpz_init (x); mpz_init (y); \
	success = (kure_get_entries(a, x) && kure_get_entries(b,y)); \
	if (success) \
		ret = (mpz_cmp(x,y) op 0); \
	mpz_clear (x); mpz_clear (y); \
	if (psuccess) *psuccess = success; \
	return ret; }

_CARD_OP(lt,<)
_CARD_OP(leq,<=)
_CARD_OP(eq,==)


/*******************************************************************************
 *                                Count Entries                                *
 *                                                                             *
 *                              Tue, 23 Mar 2010                               *
 ******************************************************************************/

Kure_success kure_get_entries (const KureRel * self, mpz_t entries)
{
	_KURE_CHECK_VALID1(self)
	else {
		rvRelationCountEntries((KureRel*)self, entries); return TRUE;
	}
}

int	kure_get_entries_si (const KureRel * self, Kure_success * psuccess)
{
	int ret;
	mpz_t ents;
	Kure_success success;

	mpz_init (ents);
	success = kure_get_entries (self, ents);
	if (! success) {
	}
	else if ( !mpz_fits_sint_p (ents)) {
		kure_context_set_error (self->context, KURE_ERR_NUM_TOO_BIG,
			"Too much entries. Returned value may be wrong!");
		success = FALSE;
	}
	else {
		ret = MAX(0, mpz_get_si (ents));
		success = TRUE;
	}

	if (psuccess) *psuccess = success;
	mpz_clear (ents);
	return ret;
}

Kure_success kure_vec_get_entries (const KureRel * self, mpz_t entries)
{
	_KURE_CHECK_VALID1(self)
	else {
		rvRelationCountColumns((KureRel*)self, entries); return TRUE;
	}
}

int	kure_vec_get_entries_si (const KureRel * self, Kure_success * psuccess)
{
	int ret;
	mpz_t ents;
	Kure_success success;

	mpz_init (ents);
	success = kure_vec_get_entries (self, ents);
	if (! success) {
	}
	else if ( !mpz_fits_sint_p (ents)) {
		kure_context_set_error (self->context, KURE_ERR_NUM_TOO_BIG,
			"Too much entries. Returned value may be wrong!");
		success = FALSE;
	}
	else {
		ret = MAX(0, mpz_get_si (ents));
		success = TRUE;
	}

	if (psuccess) *psuccess = success;
	mpz_clear (ents);
	return ret;
}

/*******************************************************************************
 *                                    Hulls                                    *
 *               (Transitive, symmetric and reflexive closure.)                *
 *                                                                             *
 *                              Tue, 23 Mar 2010                               *
 ******************************************************************************/

#define HULL_ALG(kure_name, int_name) \
	Kure_success kure_name (KureRel * rop, const KureRel * arg) { \
		_KURE_CHECK_VALID2(rop, arg) \
		else _KURE_CHECK_SAME_CONTEXT2(rop, arg) \
		else _KURE_CHECK_HOMOGEN(arg) \
		else { \
			/* Different argument order. */ \
			int_name ((KureRel*)arg, rop); return TRUE; \
		} }

HULL_ALG(kure_trans_hull, transh_rel)
HULL_ALG(kure_symm_hull, symmh_rel)
HULL_ALG(kure_refl_hull, reflh_rel)

#undef HULL_ALG


/*******************************************************************************
 *                                Special Sets                                 *
 *                                                                             *
 *                              Tue, 23 Mar 2010                               *
 ******************************************************************************/

Kure_success kure_membership (KureRel * rop, int n)
{
	_KURE_CHECK_VALID1(rop)
	else _KURE_CHECK_POSITIVE_SI(rop->context, n)
	else {
		epsi_rel (rop, n); return TRUE;
	}
}

Kure_success kure_partial_funcs (KureRel * rop, int size1, mpz_t size2)
{
	_KURE_CHECK_VALID1(rop)
	else _KURE_CHECK_POSITIVE_SI(rop->context, size1)
	else _KURE_CHECK_POSITIVE(rop->context, size2)
	else {
		paf_rel (rop, size1, size2); return TRUE;
	}
}

Kure_success kure_partial_funcs_si (KureRel * rop, int size1, int size2)
{
	Kure_success ret;
	mpz_t _size2;
	mpz_init_set_si (_size2, size2);
	ret = kure_partial_funcs (rop, size1, _size2);
	mpz_clear (_size2);
	return ret;
}

Kure_success kure_total_funcs (KureRel * rop, int size1, mpz_t size2)
{
	_KURE_CHECK_VALID1(rop)
	else _KURE_CHECK_POSITIVE_SI(rop->context, size1)
	else _KURE_CHECK_POSITIVE(rop->context, size2)
	else {
		totf_rel (rop, size1, size2); return TRUE;
	}
}

Kure_success kure_total_funcs_si (KureRel * rop, int size1, int size2)
{
	Kure_success ret;
	mpz_t _size2;
	mpz_init_set_si (_size2, size2);
	ret = kure_total_funcs (rop, size1, _size2);
	mpz_clear (_size2);
	return ret;
}


/*******************************************************************************
 *                                   Random                                    *
 *         (Random relations, graphs without cycles and permutations.)         *
 *                                                                             *
 *                              Tue, 23 Mar 2010                               *
 ******************************************************************************/

/****
 * Random relations:
 */

Kure_success kure_random (KureRel * self, float prob,
		KureRandomFunc random_func, void * user_data)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_PROBABILITY(prob)
	else {
		random_rel (self, prob, random_func, user_data);
		return TRUE;
	}
}

Kure_success kure_random_simple (KureRel * self, float prob)
{
	_KURE_CHECK_VALID1(self)
	else {
		KureContext * context = self->context;
		return kure_random (self, prob, context->random_func, context->random_udata);
	}
}

Kure_success kure_random_full (KureRel * self, mpz_t rows, mpz_t cols,
		float prob, KureRandomFunc random_func, void * user_data)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_PROBABILITY(prob)
	else _KURE_CHECK_POSITIVE(self->context, rows)
	else _KURE_CHECK_POSITIVE(self->context, cols)
	else {
		random_rel_full (self, rows, cols, prob, random_func, user_data);
		return TRUE;
	}
}

Kure_success kure_random_full_si (KureRel * self, int rows, int cols,
		float prob, KureRandomFunc random_func, void * user_data)
{
	Kure_success ret;
	mpz_t _rows, _cols;
	mpz_init_set_si (_rows, rows);
	mpz_init_set_si (_cols, cols);
	ret = kure_random_full (self, _rows, _cols, prob, random_func, user_data);
	mpz_clear (_rows);
	mpz_clear (_cols);
	return ret;
}


/****
 * Random graphs without cycles:
 */

Kure_success kure_random_no_cycles_simple (KureRel * self, float prob)
{
	_KURE_CHECK_VALID1(self)
	else {
		KureContext * context = self->context;
		return kure_random_no_cycles (self, prob, context->random_func,
				context->random_udata);
	}
}

Kure_success kure_random_no_cycles (KureRel * self, float prob,
		KureRandomFunc random_func, void * user_data)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_PROBABILITY(prob)
	else _KURE_CHECK_HOMOGEN(self)
	else {
		rvRelationRandomCF (self, prob, random_func, user_data); return TRUE;
	}
}

Kure_success kure_random_no_cycles_full (KureRel * self, mpz_t size,
		float prob, KureRandomFunc random_func, void * user_data)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_PROBABILITY(prob)
	else _KURE_CHECK_POSITIVE(self->context, size)
	else {
		rvRelationRandomCF_full (self, size, prob, random_func, user_data);
		return TRUE;
	}
}

Kure_success kure_random_no_cycles_full_si (KureRel * self, int size,
		float prob, KureRandomFunc random_func, void * user_data)
{
	Kure_success ret;
	mpz_t _size;
	mpz_init_set_si (_size, size);
	ret = kure_random_no_cycles_full (self, _size, prob, random_func, user_data);
	mpz_clear (_size);
	return ret;
}


/****
 * Random permutations:
 */

Kure_success kure_random_perm (KureRel * self, KureRandomFunc random_func,
		void * user_data)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_HOMOGEN(self)
	else {
		rvRelationRandomPerm (self, random_func, user_data); return TRUE;
	}
}

Kure_success kure_random_perm_simple (KureRel * self)
{
	_KURE_CHECK_VALID1(self)
	else {
		KureContext * context = self->context;
		return kure_random_perm (self, context->random_func,
				context->random_udata);
	}
}

Kure_success kure_random_perm_full (KureRel * self, mpz_t size,
		KureRandomFunc random_func, void * user_data)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_POSITIVE(self->context, size)
	else {
		rvRelationRandomPerm_full (self, size, random_func, user_data);
		return TRUE;
	}
}

Kure_success kure_random_perm_full_si (KureRel * self, int size,
		KureRandomFunc random_func, void * user_data)
{
	Kure_success ret;
	mpz_t _size;
	mpz_init_set_si (_size, size);
	ret = kure_random_perm_full (self, _size, random_func, user_data);
	mpz_clear (_size);
	return ret;
}


/*******************************************************************************
 *                       Residue and Symmetric Quotient                        *
 *                                                                             *
 *                              Tue, 23 Mar 2010                               *
 ******************************************************************************/

Kure_success kure_left_residue (KureRel * rop, const KureRel * S, const KureRel * R)
{
	_KURE_CHECK_VALID3(rop, R, S)
	else _KURE_CHECK_SAME_CONTEXT3(rop, R, S)
	else _KURE_CHECK_COMPAT_DIM(S->cols, R->cols)
	else {
		res_li_rel ((KureRel*)S, (KureRel*)R, rop); return TRUE;
	}
}

Kure_success kure_right_residue (KureRel * rop, const KureRel * R, const KureRel * S)
{
	_KURE_CHECK_VALID3(rop, R, S)
	else _KURE_CHECK_SAME_CONTEXT3(rop, R, S)
	else _KURE_CHECK_COMPAT_DIM(R->rows, S->rows)
	else {
		res_re_rel((KureRel*)R, (KureRel*)S, rop); return TRUE;
	}
}

Kure_success kure_symm_quotient (KureRel * rop, const KureRel * R, const KureRel * S)
{
	_KURE_CHECK_VALID3(rop, R, S)
	else _KURE_CHECK_SAME_CONTEXT3(rop, R, S)
	else _KURE_CHECK_COMPAT_DIM(R->rows, S->rows)
	else {
		syq_rel((KureRel*)R, (KureRel*)S, rop); return TRUE;
	}
}

/*******************************************************************************
 *                                    Tests                                    *
 *                       (Empty, Univalent, Vector, ...)                       *
 *                                                                             *
 *                              Tue, 23 Mar 2010                               *
 ******************************************************************************/

static Kure_success _kure_is_row_complete_si_swapped (const KureRel * self, int x, int y,
					KureDirection dir, Kure_bool * pret)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_IS_IN_SI(self, y, x)
	else {
		*pret = test_line_complete ((KureRel*)self, x,y,dir);
		return TRUE;
	}
}

Kure_bool kure_is_row_complete_si (const KureRel * self, int x, int y,
					KureDirection dir, Kure_success * psuccess)
{
	Kure_bool ret;
	Kure_success success = _kure_is_row_complete_si_swapped(self, x,y,dir,&ret);
	if (psuccess) *psuccess = success;
	return ret;
}

static Kure_success _kure_is_empty_swapped (const KureRel * self, Kure_bool * pret)
{
	_KURE_CHECK_VALID1(self)
	else {
		*pret = test_on_empty ((KureRel*)self); return TRUE;
	}
}

Kure_bool kure_is_empty (const KureRel * self, Kure_success * psuccess)
{
	Kure_bool ret;
	Kure_success success = _kure_is_empty_swapped (self, &ret);
	if (psuccess) *psuccess = success;
	return ret;
}

static Kure_success _kure_is_univalent_swapped (const KureRel * self, Kure_bool * pret)
{
	_KURE_CHECK_VALID1(self)
	else {
		*pret = test_on_eind ((KureRel*)self); return TRUE;
	}
}

Kure_bool kure_is_univalent (const KureRel * self, Kure_success * psuccess)
{
	Kure_bool ret;
	Kure_success success = _kure_is_univalent_swapped (self, &ret);
	if (psuccess) *psuccess = success;
	return ret;
}

static Kure_success _kure_is_vector_swapped (const KureRel * self, Kure_bool * pret)
{
	_KURE_CHECK_VALID1(self)
	else {
		*pret = line_completeness ((KureRel*)self); return TRUE;
	}
}

Kure_bool kure_is_vector (const KureRel * self, Kure_success * psuccess)
{
	Kure_bool ret;
	Kure_success success = _kure_is_vector_swapped (self, &ret);
	if (psuccess) *psuccess = success;
	return ret;
}

Kure_bool kure_is_hom (const KureRel * self, Kure_success * psuccess)
{
	if (! self) { if (psuccess) *psuccess = FALSE; return FALSE; }
	else {
		if (psuccess) *psuccess = TRUE;
		return 0 == mpz_cmp (self->cols, self->rows);
	}
}

/*******************************************************************************
 *                                 Comparison                                  *
 *                                                                             *
 *                              Tue, 23 Mar 2010                               *
 ******************************************************************************/

Kure_bool kure_equals (const KureRel * a, const KureRel * b, Kure_success * psuccess)
{
	_KURE_CHECK_VALID2(a,b)
	else _KURE_CHECK_SAME_CONTEXT2(a,b)
	else {
		if (psuccess) *psuccess = TRUE;
		return test_on_equal ((KureRel*)a, (KureRel*)b);
	}
}

Kure_bool kure_includes (const KureRel * a, const KureRel * b, Kure_success * psuccess)
{
	_KURE_CHECK_VALID2(a,b)
	else _KURE_CHECK_SAME_CONTEXT2(a,b)
	else {
		if (psuccess) *psuccess = TRUE;
		return test_on_inclusion ((KureRel*)a, (KureRel*)b);
	}
}


/*******************************************************************************
 *                           Iteration with vectors                            *
 *                                                                             *
 *                              Tue, 23 Mar 2010                               *
 ******************************************************************************/

Kure_success kure_vec_begin (KureRel * self)
{
	_KURE_CHECK_VALID1(self)
	else {
		init_vector (self, self->rows, self->cols);
		return TRUE;
	}
}

Kure_success kure_vec_begin_full (KureRel * self, mpz_t rows, mpz_t cols)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_POSITIVE(self->context, rows)
	else _KURE_CHECK_POSITIVE(self->context, cols)
	else {
		init_vector (self, rows, cols);
		return TRUE;
	}
}

Kure_success kure_vec_begin_full_si (KureRel * self, int rows, int cols)
{
	Kure_success ret;
	mpz_t _rows, _cols;
	mpz_init_set_si (_rows, rows);
	mpz_init_set_si (_cols, cols);
	ret = kure_vec_begin_full (self, _rows, _cols);
	mpz_clear (_rows);
	mpz_clear (_cols);
	return ret;
}

Kure_success kure_vec_next (KureRel * rop, KureRel * arg)
{
	_KURE_CHECK_VALID2(rop, arg)
	else _KURE_CHECK_SAME_CONTEXT2(rop, arg)
	else {
		next_vector (arg, rop);
		return TRUE;
	}
}


/*******************************************************************************
 *                              Special Relations                              *
 *                                                                             *
 *                              Tue, 23 Mar 2010                               *
 ******************************************************************************/

Kure_success kure_successors (KureRel * self, mpz_t size)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_POSITIVE(self->context, size)
	else {
		succ_vector (self, size); return TRUE;
	}
}

Kure_success kure_successors_si (KureRel * self, int size)
{
	Kure_success ret;
	mpz_t _size;
	mpz_init_set_si (_size, size);
	ret = kure_successors (self, _size);
	mpz_clear (_size);
	return ret;
}

Kure_success kure_domain (KureRel * rop, const KureRel * arg)
{
	_KURE_CHECK_VALID2(rop, arg)
	else _KURE_CHECK_SAME_CONTEXT2(rop, arg)
	else {
		dom_rel ((KureRel*)arg, rop); return TRUE;
	}
}
#include <assert.h>
Kure_success kure_vec_point (KureRel * rop, const KureRel * arg)
{
	_KURE_CHECK_VALID2(rop, arg)
	else _KURE_CHECK_SAME_CONTEXT2(rop, arg)
	else _KURE_CHECK_NON_EMPTY(arg)
	else {
		point ((KureRel*)arg, rop); return TRUE;
	}
}

Kure_success kure_atom (KureRel * rop, const KureRel * arg)
{
	_KURE_CHECK_VALID2(rop, arg)
	else _KURE_CHECK_SAME_CONTEXT2(rop, arg)
	else _KURE_CHECK_NON_EMPTY(arg)
	else {
		atom ((KureRel*)arg, rop); return TRUE;
	}
}

Kure_success kure_vec_inj (KureRel * rop, const KureRel * arg)
{
	_KURE_CHECK_VALID2(rop, arg)
	else _KURE_CHECK_SAME_CONTEXT2(rop, arg)
	else _KURE_CHECK_NON_EMPTY(arg)
	else {
		inj_vector ((KureRel*)arg, rop); return TRUE;
	}
}


/*******************************************************************************
 *      Direct Sum (+), Tupling ([,]) and projection/injection Relations       *
 *                                                                             *
 *                              Tue, 23 Mar 2010                               *
 ******************************************************************************/

Kure_success kure_tupling (KureRel * rop, const KureRel * arg1, const KureRel * arg2)
{
	_KURE_CHECK_VALID3(rop, arg1, arg2)
	else _KURE_CHECK_SAME_CONTEXT3(rop, arg1, arg2)
	else _KURE_CHECK_COMPAT_DIM(arg1->rows, arg2->rows)
	else {
		tup_P_rel ((KureRel*)arg1, (KureRel*)arg2, rop); return TRUE;
	}
}

Kure_success kure_right_tupling (KureRel * rop, const KureRel * arg1, const KureRel * arg2)
{
	_KURE_CHECK_VALID3(rop, arg1, arg2)
	else _KURE_CHECK_SAME_CONTEXT3(rop, arg1, arg2)
	else _KURE_CHECK_COMPAT_DIM(arg1->rows, arg2->rows)
	else {
		tup_P_rel ((KureRel*)arg1, (KureRel*)arg2, rop); return TRUE;
	}
}

Kure_success kure_left_tupling (KureRel * rop, const KureRel * arg1, const KureRel * arg2)
{
	_KURE_CHECK_VALID3(rop, arg1, arg2)
	else _KURE_CHECK_SAME_CONTEXT3(rop, arg1, arg2)
	else _KURE_CHECK_COMPAT_DIM(arg1->cols, arg2->cols)
	else {
		tupling_left_rel ((KureRel*)arg1, (KureRel*)arg2, rop); return TRUE;
	}
}

Kure_success kure_direct_sum (KureRel * rop, const KureRel * arg1, const KureRel * arg2)
{
	_KURE_CHECK_VALID3(rop, arg1, arg2)
	else _KURE_CHECK_SAME_CONTEXT3(rop, arg1, arg2)
	else if (mpz_cmp(arg1->cols, arg2->cols) != 0) {
		kure_context_set_error (rop->context, KURE_ERR_INV_ARG,
				"Argument must have the same number of columns.");
		return FALSE;
	}
	else {
		tup_S_rel ((KureRel*)arg1, (KureRel*)arg2, rop); return TRUE;
	}
}

Kure_success kure_proj_1 (KureRel * self, mpz_t rows, mpz_t cols)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_POSITIVE(self->context, rows)
	else _KURE_CHECK_POSITIVE(self->context, cols)
	else {
		pro1_rel (self, rows, cols); return TRUE;
	}
}

Kure_success kure_proj_1_si (KureRel * self, int rows, int cols)
{
	Kure_success ret;
	mpz_t _rows, _cols;
	mpz_init_set_si (_rows, rows);
	mpz_init_set_si (_cols, cols);
	ret = kure_proj_1 (self, _rows, _cols);
	mpz_clear (_rows);
	mpz_clear (_cols);
	return ret;
}

Kure_success kure_proj_1_dom (KureRel * self, const KureDom * dom)
{
	_KURE_CHECK_VALID1(self)
	else if (!KURE_DOM_IS_DIRECT_PRODUCT(dom)) {
		kure_context_set_error (self->context, KURE_ERR_INV_ARG,
				"Direct Product expected. Got %s instead.", kure_dom_get_name(dom));
				return FALSE;
	}
	else {
		KureDirectProduct * p = (KureDirectProduct*) dom;
		return kure_proj_1 (self, p->first, p->second);
	}
}

Kure_success kure_proj_2 (KureRel * self, mpz_t rows, mpz_t cols)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_POSITIVE(self->context, rows)
	else _KURE_CHECK_POSITIVE(self->context, cols)
	else {
		pro2_rel (self, rows, cols); return TRUE;
	}
}

Kure_success kure_proj_2_si (KureRel * self, int rows, int cols)
{
	Kure_success ret;
	mpz_t _rows, _cols;
	mpz_init_set_si (_rows, rows);
	mpz_init_set_si (_cols, cols);
	ret = kure_proj_2 (self, _rows, _cols);
	mpz_clear (_rows);
	mpz_clear (_cols);
	return ret;
}

Kure_success kure_proj_2_dom (KureRel * self, const KureDom * dom)
{
	_KURE_CHECK_VALID1(self)
	else if (!KURE_DOM_IS_DIRECT_PRODUCT(dom)) {
		kure_context_set_error (self->context, KURE_ERR_INV_ARG,
				"Direct Product expected. Got %s instead.", kure_dom_get_name(dom));
				return FALSE;
	}
	else {
		KureDirectProduct * p = (KureDirectProduct*) dom;
		return kure_proj_2 (self, p->first, p->second);
	}
}

Kure_success kure_inj_1 (KureRel * self, mpz_t rows, mpz_t cols)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_POSITIVE(self->context, rows)
	else _KURE_CHECK_POSITIVE(self->context, cols)
	else {
		inj1_rel (self, rows, cols); return TRUE;
	}
}

Kure_success kure_inj_1_si (KureRel * self, int rows, int cols)
{
	Kure_success ret;
	mpz_t _rows, _cols;
	mpz_init_set_si (_rows, rows);
	mpz_init_set_si (_cols, cols);
	ret = kure_inj_1 (self, _rows, _cols);
	mpz_clear (_rows);
	mpz_clear (_cols);
	return ret;
}

Kure_success kure_inj_1_dom (KureRel * self, const KureDom * dom)
{
	_KURE_CHECK_VALID1(self)
	else if (!KURE_DOM_IS_DIRECT_SUM(dom)) {
		kure_context_set_error (self->context, KURE_ERR_INV_ARG,
				"Direct Sum expected. Got %s instead.", kure_dom_get_name(dom));
				return FALSE;
	}
	else {
		KureDirectSum * p = (KureDirectSum*) dom;
		return kure_inj_1 (self, p->first, p->second);
	}
}

Kure_success kure_inj_2 (KureRel * self, mpz_t rows, mpz_t cols)
{
	_KURE_CHECK_VALID1(self)
	else _KURE_CHECK_POSITIVE(self->context, rows)
	else _KURE_CHECK_POSITIVE(self->context, cols)
	else {
		inj2_rel (self, rows, cols); return TRUE;
	}
}

Kure_success kure_inj_2_si (KureRel * self, int rows, int cols)
{
	Kure_success ret;
	mpz_t _rows, _cols;
	mpz_init_set_si (_rows, rows);
	mpz_init_set_si (_cols, cols);
	ret = kure_inj_2 (self, _rows, _cols);
	mpz_clear (_rows);
	mpz_clear (_cols);
	return ret;
}

Kure_success kure_inj_2_dom (KureRel * self, const KureDom * dom)
{
	_KURE_CHECK_VALID1(self)
	else if (!KURE_DOM_IS_DIRECT_SUM(dom)) {
		kure_context_set_error (self->context, KURE_ERR_INV_ARG,
				"Direct Sum expected. Got %s instead.", kure_dom_get_name(dom));
				return FALSE;
	}
	else {
		KureDirectSum * p = (KureDirectSum*) dom;
		return kure_inj_2 (self, p->first, p->second);
	}
}

/*******************************************************************************
 *                            Product and Sum Order                            *
 *                                                                             *
 *                              Tue, 23 Mar 2010                               *
 ******************************************************************************/

Kure_success kure_product_order (KureRel * rop, const KureRel * arg1, const KureRel * arg2)
{
	_KURE_CHECK_VALID3(rop, arg1, arg2)
	else _KURE_CHECK_HOMOGEN(arg1)
	else _KURE_CHECK_HOMOGEN(arg2)
	else _KURE_CHECK_SAME_CONTEXT3(rop, arg1, arg2)
	else {
		KureContext * c = kure_rel_get_context(rop);
		KureRel * pi = NULL;
		Kure_success success = TRUE;

		pi = kure_rel_new (c);
		success = kure_proj_1 (pi, ((KureRel*)arg1)->rows, ((KureRel*)arg2)->rows);
		if (success) {
			KureRel * X = kure_rel_new (c);
			success = kure_mult_norm_transp (X, arg1, pi);
			success = success && kure_mult (X, pi, X);
			if (success) {
				KureRel * rho = kure_rel_new (c);
				success = kure_proj_2 (rho, ((KureRel*)arg1)->rows, ((KureRel*)arg2)->rows);
				if (success) {
					KureRel * Y = kure_rel_new (c);
					success = kure_mult_norm_transp (Y, arg2, rho);
					success = success && kure_mult (Y, rho, Y);
					success = success && kure_and (rop, X,Y);
					kure_rel_destroy(Y);
				}
				kure_rel_destroy(rho);
			}
			kure_rel_destroy (X);
		}
		kure_rel_destroy(pi);
		return success;
	}
}

Kure_success kure_sum_order (KureRel * rop, const KureRel * arg1, const KureRel * arg2)
{
	_KURE_CHECK_VALID3(rop, arg1, arg2)
	else _KURE_CHECK_HOMOGEN(arg1)
	else _KURE_CHECK_HOMOGEN(arg2)
	else _KURE_CHECK_SAME_CONTEXT3(rop, arg1, arg2)
	else {
		KureContext * c = kure_rel_get_context(rop);
		KureRel * iota = NULL;
		Kure_success success = TRUE;

		iota = kure_rel_new (c);
		success = kure_inj_1 (iota, ((KureRel*)arg1)->rows, ((KureRel*)arg2)->rows);
		if (success) {
			KureRel * X = kure_rel_new (c);
			success = kure_mult_transp_norm (X, iota, arg1);
			success = success && kure_mult (X, X, iota);
			if (success) {
				KureRel * kappa = kure_rel_new (c);
				success = kure_inj_2 (kappa, ((KureRel*)arg1)->rows, ((KureRel*)arg2)->rows);
				if (success) {
					KureRel * Y = kure_rel_new (c);
					success = kure_mult_transp_norm (Y, kappa, arg2);
					success = success && kure_mult (Y, Y, kappa);
					success = success && kure_or (rop, X,Y);
					kure_rel_destroy(Y);
				}
				kure_rel_destroy(kappa);
			}
			kure_rel_destroy (X);
		}
		kure_rel_destroy(iota);
		return success;
	}
}


/*******************************************************************************
 *                                Minimum sets                                 *
 *                                                                             *
 *                              Mon, 06 Aug 2012                               *
 ******************************************************************************/

Kure_success kure_minsets_upset (KureRel * rop, const KureRel * arg)
{
	_KURE_CHECK_VALID2(rop,arg)
	else _KURE_CHECK_SAME_CONTEXT2(rop,arg)
	else _KURE_CHECK_HAS_ONE_COL(arg)
	else _KURE_CHECK_HAS_2_POWER_N_ROWS(arg)
	else {


		DdManager * manager = arg->context->manager;
		size_t nvars = kure_rel_get_vars_cols(arg) + kure_rel_get_vars_rows(arg);
		DdNode * res = minsets_upset (manager, nvars, arg->bdd);

		if (res) {
			Cudd_Ref (res);
			Cudd_RecursiveDeref(manager, rop->bdd);
			rop->bdd = res;
			if (arg != rop) {
				kure_rel_set_size (rop, arg->rows, arg->cols);
			}
		}
		return TRUE;
	}
}


Kure_success kure_minsets (KureRel * rop, const KureRel * arg)
{
	_KURE_CHECK_VALID2(rop,arg)
	else _KURE_CHECK_SAME_CONTEXT2(rop,arg)
	else _KURE_CHECK_HAS_ONE_COL(arg)
	else _KURE_CHECK_HAS_2_POWER_N_ROWS(arg)
	else {
		DdManager * manager = arg->context->manager;
		size_t nvars = kure_rel_get_vars_cols(arg) + kure_rel_get_vars_rows(arg);
		DdNode * res = minsets (manager, nvars, arg->bdd);

		if (res) {
			Cudd_Ref (res);
			Cudd_RecursiveDeref(manager, rop->bdd);
			rop->bdd = res;
			if (arg != rop) {
				kure_rel_set_size (rop, arg->rows, arg->cols);
			}
		}
		return TRUE;
	}
}


Kure_success kure_maxsets_downset (KureRel * rop, const KureRel * arg)
{
	_KURE_CHECK_VALID2(rop,arg)
	else _KURE_CHECK_SAME_CONTEXT2(rop,arg)
	else _KURE_CHECK_HAS_ONE_COL(arg)
	else _KURE_CHECK_HAS_2_POWER_N_ROWS(arg)
	else {
		DdManager * manager = arg->context->manager;
		size_t nvars = kure_rel_get_vars_cols(arg) + kure_rel_get_vars_rows(arg);
		DdNode * res = maxsets_downset (manager, nvars, arg->bdd);

		if (res) {
			Cudd_Ref (res);
			Cudd_RecursiveDeref(manager, rop->bdd);
			rop->bdd = res;
			if (arg != rop) {
				kure_rel_set_size (rop, arg->rows, arg->cols);
			}
		}
		return TRUE;
	}
}

Kure_success kure_maxsets (KureRel * rop, const KureRel * arg)
{
	_KURE_CHECK_VALID2(rop,arg)
	else _KURE_CHECK_SAME_CONTEXT2(rop,arg)
	else _KURE_CHECK_HAS_ONE_COL(arg)
	else _KURE_CHECK_HAS_2_POWER_N_ROWS(arg)
	else {
		DdManager * manager = arg->context->manager;
		size_t nvars = kure_rel_get_vars_cols(arg) + kure_rel_get_vars_rows(arg);
		DdNode * res = maxsets (manager, nvars, arg->bdd);

		if (res) {
			Cudd_Ref (res);
			Cudd_RecursiveDeref(manager, rop->bdd);
			rop->bdd = res;
			if (arg != rop) {
				kure_rel_set_size (rop, arg->rows, arg->cols);
			}
		}
		return TRUE;
	}
}
