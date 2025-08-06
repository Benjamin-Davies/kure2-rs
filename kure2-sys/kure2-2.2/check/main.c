/*
 * main.c
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

#include "Kure.h"
#include <stdlib.h> // random
#include <time.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <glib.h>
//#include "utils.c"
#include "Mat.h"

void dump_kure_rel (FILE * os, KureRel * rel, char * prefix)
{
	int i,j;

	assert (kure_rel_fits_si(rel));
	for (i = 0 ; i < kure_rel_get_rows_si(rel) ; ++i) {
		fprintf (os, "%s", prefix);
		for (j = 0 ; j < kure_rel_get_cols_si(rel) ; ++j) {
			fputc (kure_get_bit_si (rel, i,j, NULL)?'1':'.', os);
		}
		fputc ('\n', os);
	}
}


void dump_kure_rel_tranposed (FILE * os, KureRel * rel, char * prefix)
{
	int i,j;

	assert (kure_rel_fits_si(rel));
	for (j = 0 ; j < kure_rel_get_cols_si(rel) ; ++j) {
		fprintf (os, "%s", prefix);
		for (i = 0 ; i < kure_rel_get_rows_si(rel) ; ++i) {
			fputc (kure_get_bit_si (rel, i,j, NULL)?'1':'.', os);
		}
		fputc ('\n', os);
	}
}


void dump_mat (FILE * os, Mat * mat, char * prefix)
{
	int i,j;

	for (i = 0 ; i < mat->rows ; ++i) {
		fprintf (os, "%s", prefix);
		for (j = 0 ; j < mat->cols ; ++j) {
			fputc (mat_getbit(mat,i,j)?'1':'.', os);
		}
		fputc ('\n', os);
	}
}

float random_func(void * dummy)
{
	float b = random() / (float) RAND_MAX;
	return b;
}

static char _kure_get_bit_aux (int row, int col, void * user_data)
{
	Kure_success success;
	char ret = kure_get_bit_si((KureRel*)user_data, row, col, &success);
	assert (success);
	return ret;
}

void _kure_set_bit_aux (int row, int col, char yesno, void * user_data)
{ assert (kure_set_bit_si((KureRel*)user_data,yesno,row,col)); }

void mat_to_kure (KureRel * rel, const Mat * mat)
{
	kure_rel_set_size_si (rel, mat->rows, mat->cols);
	mat_get_by_func(mat,rel,_kure_set_bit_aux);
}

void kure_to_mat (Mat * mat, const KureRel * rel)
{
	assert (kure_rel_fits_si(rel));
	mat_resize(mat, kure_rel_get_rows_si(rel), kure_rel_get_cols_si(rel));
	mat_set_by_func(mat,(void*)rel,_kure_get_bit_aux);
}

char cmp_kure_mat (const KureRel * rel, const Mat * mat)
{ return mat_cmp(mat,(void*)rel,_kure_get_bit_aux); }

FILE * os;

void check_result (char success, const char * fmt, ...)
{
	va_list ap;
	va_start (ap, fmt);
	vfprintf (os, fmt, ap);
	if (success) fprintf (os, " ... okay\n");
	else fprintf (os, " ... FAILED\n");
	va_end (ap);
}

void check_result_equal (KureRel * rel, Mat * mat, const char * fmt, ...)
{
	char success = cmp_kure_mat(rel, mat);

	va_list ap;
	va_start (ap, fmt);
	vfprintf (os, fmt, ap);
	if (success) fprintf (os, " ... okay\n");
	else {
		fprintf (os, " ... FAILED\n");
		fprintf (os, "\tResults were (KureRel first) %d x %d:\n",
				kure_rel_get_rows_si(rel), kure_rel_get_cols_si(rel));
		dump_kure_rel (os, rel, "\t\t");
		fprintf (os, "\tMat:\n");
		dump_mat (os, mat, "\t\t");
	}

#if 0
	fprintf (os, "\tResults was:\n");
	dump_kure_rel (os, rel, "\t\t");
#endif

	va_end (ap);
}


void check_kure_result_equal (KureRel * R, KureRel * S, const char * fmt, ...)
{
	Kure_bool success = kure_equals (R,S,NULL);

	va_list ap;
	va_start (ap, fmt);
	vfprintf (os, fmt, ap);
	if (success) fprintf (os, " ... okay\n");
	else {
		fprintf (os, " ... FAILED\n");
		fprintf (os, "   Results were %d x %d:\n", kure_rel_get_rows_si(R), kure_rel_get_cols_si(R));
		dump_kure_rel (os, R, "\t\t");
		fprintf (os, "   and %d x %d\n", kure_rel_get_rows_si(S), kure_rel_get_cols_si(S));
		dump_kure_rel (os, S, "\t\t");
	}

#if 0
	fprintf (os, "\tResults was:\n");
	dump_kure_rel (os, R, "\t\t");
#endif

	va_end (ap);
}

// R includes S?
void check_kure_result_includes (KureRel * R, KureRel * S, const char * fmt, ...)
{
	// Note the exchanged paramters for the Kure call.
	Kure_bool success = kure_includes (S, R, NULL);

	va_list ap;
	va_start (ap, fmt);
	vfprintf (os, fmt, ap);
	if (success) fprintf (os, " ... okay\n");
	else {
		fprintf (os, " ... FAILED\n");
		fprintf (os, "\tResults were %d x %d:\n", kure_rel_get_rows_si(R), kure_rel_get_cols_si(R));
		dump_kure_rel (os, R, "\t\t");
		fprintf (os, "\tand %d x %d\n", kure_rel_get_rows_si(S), kure_rel_get_cols_si(S));
		dump_kure_rel (os, S, "\t\t");
	}

#if 0
	fprintf (os, "\tResults was:\n");
	dump_kure_rel (os, R, "\t\t");
#endif

	va_end (ap);
}

void check_kure_result_is_vector (KureRel * R, const char * fmt, ...)
{
	// Note the exchanged paramters for the Kure call.
	Kure_bool success = kure_is_vector (R, NULL);

	va_list ap;
	va_start (ap, fmt);
	vfprintf (os, fmt, ap);
	if (success) fprintf (os, " ... okay\n");
	else {
		fprintf (os, " ... FAILED\n");
		fprintf (os, "\tResult was %d x %d:\n", kure_rel_get_rows_si(R), kure_rel_get_cols_si(R));
		dump_kure_rel (os, R, "\t\t");
	}

#if 0
	fprintf (os, "\tResults was:\n");
	dump_kure_rel (os, R, "\t\t");
#endif

	va_end (ap);
}

typedef struct {
	int rows, cols;
} Dim;

Dim sizes [] = { { 1,1 }, { 2,2 }, { 3,3 },
		{ 7,7 }, {8,8}, {16,16}, {32,32},
		{ 64, 64}, { 511, 511 }, { 513, 513 }, { 1,2 }, { 2,1 },
		{ 1,7 }, {7,1}, { 1,9 }, { 9,2 }, { 0,0 }};

void check_basic_rel (KureContext * c, const char * name,
		Kure_success (*kure_fun) (KureRel*),
		void (*mat_fun)(Mat*))
{
	Dim *p;

	for (p = sizes ; p->rows > 0 ; ++ p) {
		KureRel * rel = kure_rel_new_with_size_si (c,p->rows,p->cols);
		Mat * mat = mat_new (p->rows, p->cols);
		Kure_success success = kure_fun (rel);
		if ( !success) {
			KureError * err = kure_context_get_error (c);

			printf ("Error in check_basic_rel. Reason: %s\n", err?err->message:"Unknown");

		}
		mat_fun (mat);

		check_result_equal (rel, mat, "Checking %s for dim. %d x %d",
				name, p->rows, p->cols);

		kure_rel_destroy(rel);
		mat_destroy (mat);
	}
}

void check_identity (KureContext * c)
{
	KureRel * R = kure_rel_new (c);
	Mat * mR = mat_new (1,1);
	int i;

	for (i = 1 ; i <= 10 ; i ++) {
		kure_rel_set_size_si (R, i,i);
		kure_I(R);
		kure_to_mat(mR, R);
		check_result_equal (R, mR, "Checking kure_I for %d x %d", i,i);
	}

	kure_rel_destroy(R);
	mat_destroy(mR);
}

void check_basic_rels (KureContext * c)
{
	fprintf (os, "Checking basic functions ...\n");
	check_basic_rel (c, "Null (O)", kure_O, mat_O);
	check_basic_rel (c, "All (L)", kure_L, mat_L);
	check_identity (c);
}

void check_unary_op (KureContext * c, const char * name,
		Kure_success (*kure_fun) (KureRel*,const KureRel*),
		void (*mat_fun)(Mat*,const Mat*))
{
	Dim * p;

	for (p = sizes ; p->rows > 0 ; ++ p) {
		KureRel * rel = kure_rel_new_with_size_si (c,p->rows,p->cols);
		Mat * mat = mat_new (p->rows, p->cols);
		assert (kure_fun (rel, rel));
		mat_fun (mat,mat);

		check_result (cmp_kure_mat(rel,mat), "Checking %s for dim. %d x %d",
				name, p->rows, p->cols);

		kure_rel_destroy(rel);
		mat_destroy (mat);
	}
}

void check_unary_ops (KureContext * c)
{
	fprintf (os, "Checking unary operations ...\n");
	check_unary_op (c, "transpose", kure_transpose, mat_transpose);
	check_unary_op (c, "complement", kure_complement, mat_negate);
}

typedef struct rel_str_t {
	const char * s;
	int rows,cols;
} rel_str_t;


char check_binary_by_string_example (KureContext * c, const char * name,
		const rel_str_t * a, const rel_str_t * b,
		Kure_success (*kure_fun)(KureRel*,const KureRel*,const KureRel*),
		void (*mat_fun)(Mat*,const Mat*,const Mat*))
{
	KureRel * R = kure_rel_new (c), *S = kure_rel_new (c), *X = kure_rel_new (c);
	Mat * mR = mat_new (1,1), *mS = mat_new(1,1), *mX = mat_new(1,1);
	Kure_success success;

	kure_rel_assign_from_string (R, a->s, a->rows, a->cols, '1', '.');
	kure_rel_assign_from_string (S, b->s, b->rows, b->cols, '1', '.');

	kure_to_mat (mR, R); kure_to_mat (mS, S);

	success = kure_fun (X, R, S);
	if ( !success) {
		fprintf (stderr, "In check_binary_by_string_example: \"%s\" did not succeed for relations:\n"
				"\t%s (%d x %d)\n"
				"\t%s (%d x %d)\n", name, a->s, a->rows, a->cols,
				b->s, b->rows, b->cols);
		return 0;
	}
	else {
		mat_fun (mX, mR, mS);

		check_result_equal (X,mX, "Checking %s", name);

		kure_rel_destroy(R);
		kure_rel_destroy(S);
		kure_rel_destroy(X);
		mat_destroy (mR);
		mat_destroy (mS);
		mat_destroy (mX);
		return 1; // TODO: no meaning
	}
}

char check_binary_by_string_example2 (KureContext * c, const char * name,
		const char * a_s, int a_rows, int a_cols,
		const char * b_s, int b_rows, int b_cols,
		Kure_success (*kure_fun)(KureRel*,const KureRel*,const KureRel*),
		void (*mat_fun)(Mat*,const Mat*,const Mat*))
{
	rel_str_t a = {a_s,a_rows,a_cols}, b = {b_s,b_rows,b_cols};
	return check_binary_by_string_example (c,name,&a,&b,kure_fun,mat_fun);
}

void check_random_relation_2 (KureContext * context, const char * test_name,
		int rows1, int cols1, float prob1, int rows2, int cols2, float prob2,
		Kure_success (*kure_fun)(KureRel*,const KureRel*,KureRel*),
		void (*mat_fun)(Mat*,const Mat*,const Mat*))
{
	KureRel *R = kure_rel_new_with_size_si (context, rows1, cols1),
			*S = kure_rel_new_with_size_si(context, rows2, cols2),
			*X = kure_rel_new (context);
	Kure_success success;

	kure_random_simple (R, prob1);
	kure_random_simple (S, prob2);

	success = kure_fun (X,R,S);
	if (!success) {
		fprintf (stderr, "In check_random_relation_2: \"%s\" did not succeed!", test_name);
		abort();
	}
	else {
		Mat *mR = mat_new (rows1, cols1), *mS = mat_new(rows2, cols2), *mX = mat_new(1,1);

		kure_to_mat (mR, R);
		kure_to_mat (mS, S);

		mat_fun (mX, mR, mS);

		check_result_equal (X,mX, "Checking %s for rels %2dx%2d and %2dx%2d", test_name,
				rows1,cols1,rows2,cols2);

		mat_destroy (mR);
		mat_destroy (mS);
		mat_destroy (mX);

		kure_rel_destroy(R);
		kure_rel_destroy(S);
		kure_rel_destroy(X);
	}
}

void check_mult_norm_transp_random (KureContext * c, int rows1, int cols, int rows2,
		float p, int iters)
{
	int i;
	KureRel * R = kure_rel_new_with_size_si(c, rows1, cols),
			*S = kure_rel_new_with_size_si(c, rows2, cols), *X = kure_rel_new(c);
	Mat * mR = mat_new (rows1, cols), *mS = mat_new (rows2, cols), *mX = mat_new(1,1);

	for (i = 0 ; i < iters ; ++i) {
		Kure_success success;

		kure_random_simple (R, p);
		kure_random_simple (S, p);

		kure_to_mat (mR, R); kure_to_mat (mS, S);

		success = kure_mult_norm_transp (X,R,S);
		if ( !success) {
			fprintf (stderr, "In check_mult_norm_transp_random: Did not succeed.\n");
			return;
		}

		mat_mult_norm_transp (mX, mR, mS);
		check_result_equal (X, mX, "Checking kure_mult_norm_transp");
	}

	kure_rel_destroy (R);
	kure_rel_destroy (S);

	mat_destroy (mR);
	mat_destroy (mS);
}

void check_mult_norm_transp (KureContext * c)
{
	int iters = 100;

	check_mult_norm_transp_random (c,10,1,15,0.0,1);
	check_mult_norm_transp_random (c,10,1,15,0.1,iters);
	check_mult_norm_transp_random (c,10,1,15,0.2,iters);
	check_mult_norm_transp_random (c,10,1,15,0.3,iters);
	check_mult_norm_transp_random (c,10,1,15,1.0,1);

	check_mult_norm_transp_random (c,10,20,1,0.0,1);
	check_mult_norm_transp_random (c,10,20,1,0.1,iters);
	check_mult_norm_transp_random (c,10,20,1,0.2,iters);
	check_mult_norm_transp_random (c,10,20,1,0.3,iters);
	check_mult_norm_transp_random (c,10,20,1,1.0,1);

	check_mult_norm_transp_random (c,1,20,15,0.0,1);
	check_mult_norm_transp_random (c,1,20,15,0.1,iters);
	check_mult_norm_transp_random (c,1,20,15,0.2,iters);
	check_mult_norm_transp_random (c,1,20,15,0.3,iters);
	check_mult_norm_transp_random (c,1,20,15,1.0,1);

	check_mult_norm_transp_random (c,10,20,15,0.0,1);
	check_mult_norm_transp_random (c,10,20,15,0.1,iters);
	check_mult_norm_transp_random (c,10,20,15,0.2,iters);
	check_mult_norm_transp_random (c,10,20,15,0.3,iters);
	check_mult_norm_transp_random (c,10,20,15,1.0,1);

	check_mult_norm_transp_random (c,4,6,9,0.0,1);
	check_mult_norm_transp_random (c,4,6,9,0.1,iters);
	check_mult_norm_transp_random (c,4,6,9,0.3,iters);
	check_mult_norm_transp_random (c,4,6,9,0.5,iters);

	check_mult_norm_transp_random (c,10,10,10,0.1,iters);
	check_mult_norm_transp_random (c,10,10,10,0.25,iters);
	check_mult_norm_transp_random (c,10,10,10,0.5,iters);
	check_mult_norm_transp_random (c,10,10,10,0.75,iters);

	/* This is handled differently inside Kure. */
	check_mult_norm_transp_random (c,20,1,10,0.3,iters);
}

// R^*S
void check_mult_transp_norm_random (KureContext * c, int rows, int cols1, int cols2,
		float p, int iters)
{
	int i;
	KureRel * R = kure_rel_new_with_size_si(c, rows, cols1),
			*S = kure_rel_new_with_size_si(c, rows, cols2), *X = kure_rel_new(c);
	Mat * mR = mat_new (rows, cols1), *mS = mat_new (rows, cols2), *mX = mat_new(1,1);

	for (i = 0 ; i < iters ; ++i) {
		Kure_success success;

		kure_random_simple (R, p);
		kure_random_simple (S, p);

		kure_to_mat (mR, R); kure_to_mat (mS, S);

		success = kure_mult_transp_norm (X,R,S);
		if ( !success) {
			fprintf (stderr, "In check_mult_transp_norm_random: Did not succeed.\n");
			return;
		}

		mat_mult_transp_norm (mX, mR, mS);
		check_result_equal (X, mX, "Checking kure_mult_transp_norm");
	}

	kure_rel_destroy (R);
	kure_rel_destroy (S);

	mat_destroy (mR);
	mat_destroy (mS);
}

void check_mult_transp_norm (KureContext * c)
{
	int iters = 100;

	check_mult_transp_norm_random (c,10,1,15,0.0,1);
	check_mult_transp_norm_random (c,10,1,15,0.1,iters);
	check_mult_transp_norm_random (c,10,1,15,0.2,iters);
	check_mult_transp_norm_random (c,10,1,15,0.3,iters);
	check_mult_transp_norm_random (c,10,1,15,1.0,1);

	check_mult_transp_norm_random (c,10,20,1,0.0,1);
	check_mult_transp_norm_random (c,10,20,1,0.1,iters);
	check_mult_transp_norm_random (c,10,20,1,0.2,iters);
	check_mult_transp_norm_random (c,10,20,1,0.3,iters);
	check_mult_transp_norm_random (c,10,20,1,1.0,1);

	check_mult_transp_norm_random (c,1,20,15,0.0,1);
	check_mult_transp_norm_random (c,1,20,15,0.1,iters);
	check_mult_transp_norm_random (c,1,20,15,0.2,iters);
	check_mult_transp_norm_random (c,1,20,15,0.3,iters);
	check_mult_transp_norm_random (c,1,20,15,1.0,1);

	check_mult_transp_norm_random (c,10,20,15,0.0,1);
	check_mult_transp_norm_random (c,10,20,15,0.1,iters);
	check_mult_transp_norm_random (c,10,20,15,0.2,iters);
	check_mult_transp_norm_random (c,10,20,15,0.3,iters);
	check_mult_transp_norm_random (c,10,20,15,1.0,1);

	check_mult_transp_norm_random (c,4,6,9,0.0,1);
	check_mult_transp_norm_random (c,4,6,9,0.1,iters);
	check_mult_transp_norm_random (c,4,6,9,0.3,iters);
	check_mult_transp_norm_random (c,4,6,9,0.5,iters);

	check_mult_transp_norm_random (c,10,10,10,0.1,iters);
	check_mult_transp_norm_random (c,10,10,10,0.25,iters);
	check_mult_transp_norm_random (c,10,10,10,0.5,iters);
	check_mult_transp_norm_random (c,10,10,10,0.75,iters);

	check_mult_transp_norm_random (c,20,1,10,0.3,iters);
}

// R^*S^, rows1=cols2
void check_mult_transp_transp_random (KureContext * c, int rows1, int cols1, int rows2,
		float p, int iters)
{
	int i;
	KureRel * R = kure_rel_new_with_size_si(c, rows1, cols1),
			*S = kure_rel_new_with_size_si(c, rows2, rows1), *X = kure_rel_new(c);
	Mat * mR = mat_new (rows1, cols1), *mS = mat_new (rows2, rows1), *mX = mat_new(1,1);

	for (i = 0 ; i < iters ; ++i) {
		Kure_success success;

		kure_random_simple (R, p);
		kure_random_simple (S, p);

		kure_to_mat (mR, R); kure_to_mat (mS, S);

		success = kure_mult_transp_transp (X,R,S);
		if ( !success) {
			fprintf (stderr, "In check_mult_transp_transp_random: Did not succeed.\n");
			return;
		}

		mat_mult_transp_transp (mX, mR, mS);
		check_result_equal (X, mX, "Checking kure_mult_transp_transp");
	}

	kure_rel_destroy (R);
	kure_rel_destroy (S);

	mat_destroy (mR);
	mat_destroy (mS);
}

void check_mult_transp_transp (KureContext * c)
{
	int iters = 100;

	check_mult_transp_transp_random (c,1,1,1,0.5,1);

	check_mult_transp_transp_random (c,1,20,1,0.0,1);
	check_mult_transp_transp_random (c,1,20,1,0.1,iters);
	check_mult_transp_transp_random (c,1,20,1,0.2,iters);
	check_mult_transp_transp_random (c,1,20,1,0.3,iters);
	check_mult_transp_transp_random (c,1,20,1,1.0,1);

	check_mult_transp_transp_random (c,1,20,15,0.0,1);
	check_mult_transp_transp_random (c,1,20,15,0.1,iters);
	check_mult_transp_transp_random (c,1,20,15,0.2,iters);
	check_mult_transp_transp_random (c,1,20,15,0.3,iters);
	check_mult_transp_transp_random (c,1,20,15,1.0,1);

	check_mult_transp_transp_random (c,10,20,15,0.0,1);
	check_mult_transp_transp_random (c,10,20,15,0.1,iters);
	check_mult_transp_transp_random (c,10,20,15,0.2,iters);
	check_mult_transp_transp_random (c,10,20,15,0.3,iters);
	check_mult_transp_transp_random (c,10,20,15,1.0,1);

	check_mult_transp_transp_random (c,4,6,9,0.0,1);
	check_mult_transp_transp_random (c,4,6,9,0.1,iters);
	check_mult_transp_transp_random (c,4,6,9,0.3,iters);
	check_mult_transp_transp_random (c,4,6,9,0.5,iters);

	check_mult_transp_transp_random (c,10,10,10,0.1,iters);
	check_mult_transp_transp_random (c,10,10,10,0.25,iters);
	check_mult_transp_transp_random (c,10,10,10,0.5,iters);
	check_mult_transp_transp_random (c,10,10,10,0.75,iters);

	check_mult_transp_transp_random (c,20,1,10,0.3,iters);
}

void check_projection (KureContext * c, const char * name,
		int size1, int size2,
		Kure_success (*kure_fun)(KureRel*,int,int),
		void (*mat_fun)(Mat*,int,int))
{
	KureRel * R = kure_rel_new (c);
	Mat * mR = mat_new (1,1);
	Kure_success success;

	success = kure_fun(R,size1,size2);
	if ( !success) {
		fprintf (stderr, "In check_projection: \"%s\" for sizes %d / %d did not succeed.\n",
				name, size1, size2);
		return;
	}
	mat_fun (mR,size1,size2);

	check_result_equal (R, mR, "Checking %s for %d / %d", name, size1, size2);

	mat_destroy(mR);
	kure_rel_destroy(R);
}

void check_projections (KureContext * c)
{
	struct { int size1, size2; } sizes [] = {
			{1,1},
			{10,10},
			{64,64},
			{1,2},
			{2,1},
			{10,20},
			{20,10},
			{1,64},
			{2,64},
			{64,1},
			{64,2},
			{31,79},
			{12,96},
			{14,63},
			{69,44},
			{-1,-1} }, *iter;

	for (iter = sizes ; iter->size1 > 0 ; ++iter) {
		check_projection (c, "kure_proj_1_si", iter->size1, iter->size2,
				kure_proj_1_si, mat_p_1);
	}

	for (iter = sizes ; iter->size1 > 0 ; ++iter) {
		check_projection (c, "kure_proj_2_si", iter->size1, iter->size2,
				kure_proj_2_si, mat_p_2);
	}
}

void check_injection (KureContext * c, const char * name,
		int size1, int size2,
		Kure_success (*kure_fun)(KureRel*,int,int),
		void (*mat_fun)(Mat*,int,int))
{
	KureRel * R = kure_rel_new (c);
	Mat * mR = mat_new (1,1);
	Kure_success success;

	success = kure_fun(R,size1,size2);
	if ( !success) {
		fprintf (stderr, "In check_injection: \"%s\" for sizes %d / %d did not succeed.\n",
				name, size1, size2);
		return;
	}
	mat_fun (mR,size1,size2);

	check_result_equal (R, mR, "Checking %s for %d / %d", name, size1, size2);

	mat_destroy(mR);
	kure_rel_destroy(R);
}

void check_injections (KureContext * c)
{
	struct { int size1, size2; } sizes [] = {
			{1,1},
			{10,10},
			{64,64},
			{1,2},
			{2,1},
			{10,20},
			{20,10},
			{1,64},
			{2,64},
			{64,1},
			{64,2},
			{31,79},
			{12,96},
			{14,63},
			{69,44},
			{-1,-1} }, *iter;

	for (iter = sizes ; iter->size1 > 0 ; ++iter) {
		check_injection (c, "kure_inj_1_si", iter->size1, iter->size2,
				kure_inj_1_si, mat_i_1);
	}

	for (iter = sizes ; iter->size1 > 0 ; ++iter) {
		check_projection (c, "kure_inj_2_si", iter->size1, iter->size2,
				kure_inj_2_si, mat_i_2);
	}
}

void check_right_residue (KureContext * c)
{
	/* (R\S)_vw <=> \forall u: R_uv -> S_uw
	 *          <=> the v-th col. of R is a subvec. of the w-th col. of S.
	 */

	check_binary_by_string_example2 (c, "Right Residue (R\\S)",
			"1",
			1, 1,
			"1",
			1, 1,
			kure_right_residue, mat_right_residue);

	check_binary_by_string_example2 (c, "Right Residue (R\\S)",
			"1..."
			"1.1."
			"....",
			3, 4,
			"1.1.."
			"1.1.."
			".1...",
			3, 5,
			kure_right_residue, mat_right_residue);

	check_binary_by_string_example2 (c, "Right Residue (R\\S)",
			".1.."
			"1.11"
			".111",
			3, 4,
			"1.1."
			"1111"
			".1..",
			3, 1,
			kure_right_residue, mat_right_residue);

	check_binary_by_string_example2 (c, "Right Residue (R\\S)",
			".1.1"
			"1.1."
			".1.1",
			3, 4,
			"1.1."
			"1.1."
			".11.",
			3, 4,
			kure_right_residue, mat_right_residue);
}

void check_left_residue (KureContext * c)
{
	/* (S/R)_wu <=> \forall v: R_uv -> S_wv
	 * 	        <=> the u-th row of R is subvec of the w-th row. S
	 */

	// (S/R) = -(-S*R^)

	/* S is the first, R the second argument. Relations must have same number
	 * of columns. */

	check_binary_by_string_example2 (c, "Left Residue (S/R)",
			"1",
			1, 1,
			"1",
			1, 1,
			kure_left_residue, mat_left_residue);

	check_binary_by_string_example2 (c, "Left Residue (S/R)",
			"1..."
			"11.."
			".111"
			"...1"
			"..1."
			"....",
			6, 4,
			"1..1"
			".11."
			"..1."
			".1.1"
			"....",
			5, 4,
			kure_left_residue, mat_left_residue);

	check_binary_by_string_example2 (c, "Left Residue (S/R)",
			"1.11"
			"11.1"
			".111"
			"1..1"
			"..1."
			"...1",
			6, 4,
			"11.1"
			".111"
			"111."
			".1.1"
			"..1."
			"...."
			"1...",
			7, 4,
			kure_left_residue, mat_left_residue);
}

void check_symm_quot (KureContext * c)
{
	/* syq(R,S)_vw <=> \forall u: R_uv <-> S_uw
	 *             <=> R's v-th col is identical to S's w-th col.
	 */

	// syq(R,S) = (R\S) & (R^/S^)

	check_binary_by_string_example2 (c, "Symmetric Quotient syq(R,S)",
			"1",
			1, 1,
			"1",
			1, 1,
			kure_symm_quotient, mat_symm_quotient);

	check_binary_by_string_example2 (c, "Symmetric Quotient syq(R,S)",
			"1..."
			"1.1."
			"....",
			3, 4,
			"1.1.."
			"1.1.."
			".1...",
			3, 5,
			kure_symm_quotient, mat_symm_quotient);

	check_binary_by_string_example2 (c, "Symmetric Quotient syq(R,S)",
			"11.."
			"1.1."
			".1..",
			3, 4,
			"11.."
			"1.1."
			".1..",
			3, 4,
			kure_symm_quotient, mat_symm_quotient);

	check_binary_by_string_example2 (c, "Symmetric Quotient syq(R,S)",
			"1..."
			"1..."
			"....",
			3, 4,
			"1111"
			"...."
			"....",
			3, 4,
			kure_symm_quotient, mat_symm_quotient);
}

void check_tupling (KureContext * c)
{
	/* [R,S]_a<x,y> <=> R_ax & S_ay */
	// Must have the same number of rows.

	// [R,S] = R*\pi1^ & S*\pi2^

	check_binary_by_string_example2 (c, "Tupling [R,S]",
				"1",
				1, 1,
				"1",
				1, 1,
				kure_tupling, mat_tupling);

	check_binary_by_string_example2 (c, "Tupling [R,S]",
				"1..."
				".1.."
				"..1."
				"...1",
				4,4,
				"...1"
				"..1."
				".1.."
				"1...",
				4,4,
				kure_tupling, mat_tupling);

	check_binary_by_string_example2 (c, "Tupling [R,S]",
				"1..."
				".1.."
				"..1."
				"...1",
				4,4,
				".....1"
				"....1."
				"...1.."
				"..1...",
				4,6,
				kure_tupling, mat_tupling);
	check_binary_by_string_example2 (c, "Tupling [R,S]",
			".1...1"
			"..1.11"
			"...1.1"
			"..1..1",
			4,6,
			".1.1"
			".111"
			"..1."
			".1.1",
			4,4,
			kure_tupling, mat_tupling);
}

void check_right_tupling (KureContext * c)
{
	/* is identical to tupling, so no further checks are necessary. */
}

void check_left_tupling (KureContext * c)
{
	/* [R,S]_<a,b>x <=> R_ax & S_bx */
	// Must have the same number of columns.

	// [|R,S] = pi*R* & rho*S

	const int cols[] = {1,2,3,4,10,20,-1};
	const float probs [] = {0.0,0.1,0.25,0.5,0.75,1,-1};
	const int iterCount = 200;

	int *colsPtr;
	const int maxRows = 20;

	for (colsPtr = cols ; *colsPtr > 0 ; ++colsPtr) {
		int cols = *colsPtr;
		float *probPtr;

		for (probPtr = probs ; *probPtr >= 0 ; ++probPtr) {
			float prob = *probPtr;
			int i;

			for (i = 0 ; i < iterCount ; ++i) {
				int rows1 = (random() % maxRows)+1, rows2 = (random() % maxRows)+1;

				check_random_relation_2 (c, "left tupling [|R,S]",
						rows1, cols, prob, rows2, cols, prob,
						kure_left_tupling, mat_left_tupling);
			}
		}
	}
}

void check_direct_sum (KureContext * c)
{
	// Must have the same number of cols.

	check_binary_by_string_example2 (c, "Direct Sum R+S",
				"1",
				1, 1,
				"1",
				1, 1,
				kure_direct_sum, mat_direct_sum);

	check_binary_by_string_example2 (c, "Direct Sum R+S",
				"1"
				"1",
				2, 1,
				"."
				"."
				".",
				3, 1,
				kure_direct_sum, mat_direct_sum);

	check_binary_by_string_example2 (c, "Direct Sum R+S",
				"1."
				".1",
				2, 2,
				".1"
				".1"
				".1",
				3, 2,
				kure_direct_sum, mat_direct_sum);

	check_binary_by_string_example2 (c, "Direct Sum R+S",
				"111"
				"111"
				"111",
				3, 3,
				"..."
				"..."
				"..."
				"..."
				"...",
				5, 3,
				kure_direct_sum, mat_direct_sum);

	check_binary_by_string_example2 (c, "Direct Sum R+S",
				"1111"
				"1111"
				"1111"
				"1111",
				4, 4,
				"...",
				1, 4,
				kure_direct_sum, mat_direct_sum);
}

void check_epsi (KureContext * c)
{
	int i;
	KureRel * R = kure_rel_new (c);
	Mat * mR = mat_new (1,1);

	for (i = 1 ; i < 13 ; ++i) {
		Kure_success success = kure_membership (R, i);
		if ( !success) {
			fprintf (stderr, "In check_epsi: Did not succeed for n=%d.\n",i);
			return;
		}

		mat_epsi (mR, i);
		check_result_equal (R, mR, "Checking Membership Rel. for n=%d", i);
	}

	mat_destroy(mR);
	kure_rel_destroy(R);
}

void check_less_card (KureContext * c)
{
	int i;
	KureRel * R = kure_rel_new (c);
	Mat * mR = mat_new (1,1);
	KureRel * n = kure_rel_new (c);

	for (i = 1 ; i < 11 ; ++i) {
		kure_rel_set_size_si (n, i, 1);
		Kure_success success = kure_less_card (R, n);
		if ( !success) {
			fprintf (stderr, "In check_less_card: Did not succeed for n=%d.\n",i);
			return;
		}

		mat_less_card (mR, i);
		check_result_equal (R, mR, "Checking 'Less-or-Equal Cardinality' Rel. for n=%d", i);
	}

	mat_destroy(mR);
	kure_rel_destroy(R);
	kure_rel_destroy(n);
}

void check_vec_begin (KureContext * c)
{
	KureRel * v = kure_rel_new (c), *ref = kure_rel_new(c);

	/* kure_vec_begin */
	kure_rel_set_size_si (v, 7,1);
	kure_vec_begin(v);
	kure_rel_assign_from_string (ref, "1......", 7, 1, '1', '.');
	check_kure_result_equal (v,ref, "Checking kure_vec_begin with cols=1");

	kure_rel_set_size_si (v, 5, 3);
	kure_vec_begin (v);
	kure_rel_assign_from_string (ref,
			"111"
			"..."
			"..."
			"..."
			"...", 5, 3, '1', '.');
	check_kure_result_equal (v,ref, "Checking kure_vec_begin with cols=3");

	/* kure_vec_begin_full_si */
	kure_vec_begin_full_si (v, 9,1);
	kure_rel_assign_from_string (ref, "1........", 9, 1, '1', '.');
	check_kure_result_equal (v,ref, "Checking kure_vec_begin_full_si with cols=1");

	kure_vec_begin_full_si (v, 7, 3);
	kure_rel_assign_from_string (ref,
			"111"
			"..."
			"..."
			"..."
			"..."
			"..."
			"...", 7, 3, '1', '.');
	check_kure_result_equal (v,ref, "Checking kure_vec_begin_full_si with cols=3");

	kure_rel_destroy (v);
	kure_rel_destroy (ref);
}

void check_vec_next (KureContext * c)
{
	int n = 10;
	KureRel * v = kure_rel_new_with_size_si (c, n, 1);
	Mat * mv = mat_new (n, 1);
	int i;

	kure_vec_begin (v);
	mat_vec_begin (mv);
	check_result_equal (v,mv,"Checking if kure_vec_begin is valid in check_vec_next (cols=1)");

	for (i = 0 ; i < n ; i ++) {
		Kure_success success = kure_vec_next (v, v);
		if ( !success) {
			fprintf (stderr, "In In check_vec_next: kure_vec_next failed in iteration %d.\n", i);
			assert (success);
		}
		mat_vec_next (mv);

		check_result_equal (v,mv,"Checking kure_vec_next in iteration %d (cols=1)", i);
	}

	n = 10;
	kure_vec_begin_full_si (v, n, 3);
	mat_resize (mv, n, 3);
	mat_vec_begin (mv);
	check_result_equal (v,mv,"Checking if kure_vec_begin is valid in check_vec_next (cols=3)");

	for (i = 0 ; i < n ; i ++) {
		Kure_success success = kure_vec_next (v, v);
		if ( !success) {
			fprintf (stderr, "In In check_vec_next: kure_vec_next failed in iteration %d.\n", i);
			assert (success);
		}
		mat_vec_next (mv);

		check_result_equal (v,mv,"Checking kure_vec_next in iteration %d (cols=3)", i);
	}
}

void check_vec_point (KureContext * c)
{
	KureRel * v = kure_rel_new (c), *w = kure_rel_new (c), *copy = NULL;
	Kure_success success;

	// First only.
	kure_rel_assign_from_string (v, "1.", 2, 1, '1', '.');
	success = kure_vec_point (w, v);
	if ( !success) {
		fprintf (stderr, "In check_vec_point. kure_vec_point failed. I do not know why!\n");
	}
	else {
		check_kure_result_equal (v, w, "Checking kure_vec_point with for vector '1.' (2 rows)");
	}

	// Last only.
	kure_rel_assign_from_string (v, ".1", 2, 1, '1', '.');
	success = kure_vec_point (w, v);
	if ( !success) {
		fprintf (stderr, "In check_vec_point. kure_vec_point failed. I do not know why!\n");
	}
	else {
		check_kure_result_equal (v, w, "Checking kure_vec_point with for vector '1.' (2 rows)");
	}

	// Line complete easy case, first only.
	kure_rel_assign_from_string (v, "11..", 2, 2, '1', '.');
	success = kure_vec_point (w, v);
	if ( !success) {
		fprintf (stderr, "In check_vec_point. kure_vec_point failed. I do not know why!\n");
	}
	else {
		check_kure_result_equal (v, w, "Checking kure_vec_point with for vector '11 ..' (2 rows)");
	}

	// Line complete easy case, last only.
	kure_rel_assign_from_string (v, "..11", 2, 2, '1', '.');
	success = kure_vec_point (w, v);
	if ( !success) {
		fprintf (stderr, "In check_vec_point. kure_vec_point failed. I do not know why!\n");
	}
	else {
		check_kure_result_equal (v, w, "Checking kure_vec_point with for vector '.. 11' (2 rows)");
	}

	// Equal arguments for kure_vec_point
	kure_rel_assign_from_string (v, ".1.", 3, 1, '1', '.');
	copy = kure_rel_new_copy(v);
	success = kure_vec_point (v, v);
	if ( !success) {
		fprintf (stderr, "In check_vec_point. kure_vec_point failed. I do not know why!\n");
	}
	else {
		check_kure_result_equal (v, copy, "Checking kure_vec_point with for vector '.1.' with equal arguments.");
	}
	kure_rel_destroy(copy);
	copy = NULL;

	// One out of three
	kure_rel_assign_from_string (v, "111", 3, 1, '1', '.');
	success = kure_vec_point (w, v);
	if ( !success) {
		fprintf (stderr, "In check_vec_point. kure_vec_point failed. I do not know why!\n");
	}
	else {
		check_kure_result_is_vector (v, "Checking kure_vec_point with for vector '111'");
		check_kure_result_includes (v, w, "Checking kure_vec_point with for vector '111'");
	}

	// Even harder
	kure_rel_assign_from_string (v, "..1.1..1..1", 11, 1, '1', '.');
	success = kure_vec_point (w, v);
	if ( !success) {
		fprintf (stderr, "In check_vec_point. kure_vec_point failed. I do not know why!\n");
	}
	else {
		check_kure_result_is_vector (v, "Checking kure_vec_point with for vector '..1.1..1..1'");
		check_kure_result_includes (v, w, "Checking kure_vec_point with for vector '..1.1..1..1'");
	}

	// Even harder with more than one column
	kure_rel_assign_from_string (v,
			".."
			".."
			"11"
			".."
			"11"
			".."
			".."
			"11"
			".."
			".."
			"11", 11, 2, '1', '.');
	success = kure_vec_point (w, v);
	if ( !success) {
		fprintf (stderr, "In check_vec_point. kure_vec_point failed. I do not know why!\n");
	}
	else {
		check_kure_result_is_vector (v, "Checking kure_vec_point with for a complex vector");
		check_kure_result_includes (v, w, "Checking kure_vec_point with for a complex vector");
	}

	kure_rel_destroy(w);
	kure_rel_destroy(v);
}

void check_atom (KureContext * c)
{
	int rows, cols;
	float p;
	int iters = 100, i;
	KureRel * R = kure_rel_new (c);
	KureRel * atom = kure_rel_new (c);

	for (i = 0 ; i < iters ; ++i) {
		rows = 1+(int)(kure_context_random(c) * 99);
		cols = 1+(int)(kure_context_random(c) * 99);
		p = kure_context_random (c) + 0.001 /* avoid 0, but be aware of values >1.0 */;

		kure_rel_set_size_si (R, rows, cols);
		kure_random_simple (R, (p>1.0)?1.0:p);

		if (kure_is_empty(R, NULL))
			continue;
		else {
			kure_atom(atom, R);

			// The dimension is bounded so this is safe.
			check_result (kure_get_entries_si(atom, NULL)==1, "Checking #entries for kure_atom and random "
					"relation %d x %d, prob=%.2f", rows, cols, p);
			check_kure_result_includes (R, atom, "Checking if the Rel. returned by kure_atom really is "
					"an atom for random relation %d x %d, prob=%.4f", rows, cols, p);
		}
	}

	kure_rel_destroy(R);
	kure_rel_destroy(atom);
}

static char _is_perm (KureRel * P)
{
	int n = kure_rel_get_rows_si (P);
	int i,j;
	int vars_rows = kure_rel_get_vars_rows (P);

	if ( !kure_is_hom(P, NULL))
		return 0;

	for (i = 0 ; i < n ; ++i) {
		int ents = 0;

		for (j = 0 ; j < n ; ++j)
			if (kure_get_bit_fast_si (P, i,j,vars_rows,vars_rows))
				ents ++;

		if (ents != 1) return 0;
	}
	return 1;
}

void check_perm_simple (KureContext * c)
{
	int rows;
	int iters = 10, i;
	KureRel * R = kure_rel_new (c);

	for (i = 0 ; i <= iters ; ++i) {
		if (i == 0)
			rows = 1;
		else
			rows = 1+(int)(kure_context_random(c) * 99);

		kure_rel_set_size_si (R, rows, rows);
		kure_random_perm_simple (R);

		check_result (_is_perm(R), "Checking kure_perm_simple for n=%d", rows);
	}

	kure_rel_destroy(R);
}

void check_random_simple (KureContext * c)
{
	int rows, cols;
	float p;
	int iters = 100, i, tests = 50, k;
	double diff_allowed = 0.1 /* quite generous */;
	KureRel * R = kure_rel_new (c);

	for (i = 0 ; i < iters ; ++i) {
		long ents = 0;

		rows = 1+(int)(kure_context_random(c) * 99);
		cols = 1+(int)(kure_context_random(c) * 99);
		p = kure_context_random (c) + 0.001 /* avoid 0, but be aware of values >1.0 */;

		kure_rel_set_size_si (R, rows, cols);

		for (k = 0 ; k < tests ; ++k) {
			kure_random_simple (R, (p>1.0)?1.0:p);
			ents += kure_get_entries_si (R, NULL);
		}

		//printf ("ents: %ld, density: %lf\n", ents, ents / (double)(rows*cols*tests));
		check_result (fabs(p - ents / (double)(rows*cols*tests)) <= diff_allowed,
				"Checking kure_random_simple for %d x %d and prob. %.3f",
				rows, cols, p);
	}

	kure_rel_destroy(R);
}

void check_hull (KureContext * c, const char * name,
		Kure_success (*kure_fun) (KureRel*,const KureRel*),
		void (*mat_fun)(Mat*,const Mat*))
{
	int rows;
	int iters = 25, i;
	KureRel * R = kure_rel_new (c), *S = kure_rel_new (c);
	float p;
	Mat * mR = mat_new (1,1);

	/* input rel. = output rel. */
	for (i = 0 ; i <= iters ; ++i) {
		if (i == 0)
			rows = 1;
		else
			rows = 1+(int)(kure_context_random(c) * 99);
		p = kure_context_random (c) + 0.001 /* avoid 0, but be aware of values >1.0 */;

		kure_rel_set_size_si (R, rows, rows);
		kure_random_simple (R, p);

		kure_to_mat (mR, R);
		mat_fun (mR, mR);
		kure_fun (R,R);

		check_result_equal (R,mR, "Checking %s for inp. rel = outp. rel.and dim. "
				"%d x %d, prob. %.3f", name, rows, rows, p);
	}

	/* input rel. != output rel. */
	for (i = 0 ; i <= iters ; ++i) {
		if (i == 0)
			rows = 1;
		else
			rows = 1+(int)(kure_context_random(c) * 99);
		p = kure_context_random (c) + 0.001 /* avoid 0, but be aware of values >1.0 */;

		kure_rel_set_size_si (R, rows, rows);
		kure_random_simple (R, p);

		kure_to_mat (mR, R);
		mat_fun (mR, mR);
		kure_fun (S, R);

		check_result_equal (S,mR, "Checking %s for inp. rel != outp. rel. and dim. "
				"%d x %d, prob. %.3f", name, rows, rows, p);
	}

	kure_rel_destroy(R);
	kure_rel_destroy(S);
	mat_destroy(mR);
}

void check_refl_hull (KureContext * c)
{
	check_hull (c, "kure_refl_hull", kure_refl_hull, mat_refl_hull);
}

void check_symm_hull (KureContext * c)
{
	check_hull (c, "kure_summ_hull", kure_symm_hull, mat_symm_hull);
}

void check_trans_hull (KureContext * c)
{
	check_hull (c, "kure_trans_hull", kure_trans_hull, mat_trans_hull);
}

void check_set_row (KureContext * c)
{
	int sizes[] = {1,2,3,4,8,16,17,21,31,33,-1}, *sizes_ptr;
	int cols[] = {1,2,3,5,10,-1}, *cols_ptr;
	float probs[] = {0.01, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, -1.0}, *probs_ptr;
	int iters = 10, o;
	KureRel * v = kure_rel_new (c);

	for (cols_ptr = cols ; *cols_ptr > 0 ; cols_ptr ++) {
		for (sizes_ptr = sizes ; *sizes_ptr > 0 ; sizes_ptr ++) {
			for (probs_ptr = probs ; *probs_ptr >= .0f; probs_ptr++) {
				for (o = 0 ; o < iters ; o ++) {
					int rows = *sizes_ptr;
					float p = *probs_ptr;
					int k;

					kure_rel_set_size_si (v, rows, *cols_ptr);
					kure_O (v);

					for (k = 0 ; k < rows ; k ++)
						if (kure_context_random(c) <= p)
							kure_set_row_si (v, 1, k);

					check_result (kure_is_vector(v, NULL), "Checking kure_set_row_si for "
							"rows=%d, cols=%d, prob=%.3f", rows, *cols_ptr, p);
				}
			}
		}
	}

	kure_rel_destroy (v);
}

void check_vec_inj (KureContext * c)
{
	int rows;
	int iters = 100, i, k;
	KureRel * v = kure_rel_new (c), *R = kure_rel_new (c);
	float p;
	Mat * mR = mat_new (1,1), *mv = mat_new (1,1);
	int max_rows = 10;

	srand (time(NULL));

	/* 1 columns */
	for (i = 0 ; i <= iters ; ++i) {
		if (i == 0)
			rows = 1;
		else
			rows = 1+(int)(kure_context_random(c) * (max_rows-1));
		p = kure_context_random (c) + 0.001 /* avoid 0, but be aware of values >1.0 */;

		kure_rel_set_size_si (v, rows, 1);
		kure_random_simple (v, p);

		if (kure_is_empty (v, NULL))
			continue;

		kure_to_mat (mv, v);
		kure_vec_inj (R, v);
		mat_inj(mR, mv);

		check_result_equal (R,mR, "Checking kure_vec_inj for 1 col., %d rows and "
				"prob. %.3f", rows, p);
	}

	/* 3 columns */
	for (i = 0 ; i <= iters ; ++i) {
		if (i == 0)
			rows = 1;
		else
			rows = 1+(int)(kure_context_random(c) * (max_rows-1));
		p = kure_context_random (c) + 0.001 /* avoid 0, but be aware of values >1.0 */;

		kure_rel_set_size_si (v, rows, 3);
		kure_O(v);

		/* kure_random_simple cannot be used here anymore. */
		for (k = 0 ; k < rows ; k ++)
			if (kure_context_random(c) <= p)
				kure_set_row_si (v, 1, k);

		if (kure_is_empty (v, NULL))
			continue;

		kure_to_mat (mv, v);

#if 0
		fprintf (stderr, "Kure v:\n");
		dump_kure_rel(stderr, v, "\t");
		fprintf (stderr, "Mat v:\n");
		dump_mat(stderr, mv, "\t");
#endif

		kure_vec_inj (R, v);
		mat_inj(mR, mv);

		check_result_equal (R,mR, "Checking kure_vec_inj for 3 col., %d rows and "
				"prob. %.3f", rows, p);
	}

	kure_rel_destroy(v);
	kure_rel_destroy(R);
	mat_destroy(mR);
	mat_destroy(mv);
}

void check_is_univalent (KureContext * c)
{
	KureRel * R = kure_rel_new (c);
	Mat * mR = mat_new (1,1);

	int rows, cols;
	float p;
	int iters = 100, i;
	int max_rows = 100, max_cols = 100;
	char passed;

	/* Test some random relations which are not univalent in most cases. */
	for (i = 0 ; i < iters ; ++i) {
		rows = 1+(int)(kure_context_random(c) * (max_rows-1));
		cols = 1+(int)(kure_context_random(c) * (max_cols-1));
		p = kure_context_random (c) + 0.001 /* avoid 0, but be aware of values >1.0 */;

		kure_rel_set_size_si (R, rows, cols);
		kure_random_simple (R, (p>1.0)?1.0:p);

		kure_to_mat (mR, R);

		passed = (!!kure_is_univalent(R,NULL) == !!mat_is_univalent(mR));
		check_result (passed,
				"Checking kure_is_univalent for random relation %d x %d and prob. %.3f",
				rows, cols, p);
		if ( !passed) {
			fprintf (os, "\tmat says: is%s univalent\n", mat_is_univalent(mR)?"":" NOT");
			fprintf (os, "\tKure says: is%s univalent\n", kure_is_univalent(R,NULL)?"":" NOT");
			fprintf (os, "\tRelation was %d x %d:\n",
					kure_rel_get_rows_si(R), kure_rel_get_cols_si(R));
			dump_kure_rel (os, R, "\t\t");
		}
	}

	/* Test some positive examples. */
	for (i = 1 ; i <= 10 ; i ++) {
		kure_rel_set_size_si (R, i, i);
		kure_random_perm_simple (R);

		kure_to_mat (mR, R);

		passed = (!!kure_is_univalent(R,NULL) == !!mat_is_univalent(mR));
		check_result (passed,
				"Checking kure_is_univalent for random permutation %d x %d",i,i);
		if ( !passed) {
			fprintf (os, "\tmat says: is%s univalent\n", mat_is_univalent(mR)?"":" NOT");
			fprintf (os, "\tKure says: is%s univalent\n", kure_is_univalent(R,NULL)?"":" NOT");
			fprintf (os, "\tRelation was %d x %d:\n",
					kure_rel_get_rows_si(R), kure_rel_get_cols_si(R));
			dump_kure_rel (os, R, "\t\t");
		}
	}

	kure_rel_destroy(R);
	mat_destroy(mR);
}

static char _has_cycles (KureRel * R)
{
	KureRel * H = kure_rel_new (kure_rel_get_context(R));
	Mat * mat = mat_new (1,1);
	int i, n = kure_rel_get_rows_si (R);
	char ret = 0;

	assert (kure_is_hom (R, NULL));

	kure_trans_hull (H, R);
	kure_to_mat (mat, H);

	for (i = 0 ; i < n && !ret; ++i)
		if (mat_getbit (mat, i,i))
			ret = 1;

	kure_rel_destroy(H);
	mat_destroy(mat);
	return ret;
}

void check_random_no_cycles (KureContext * c)
{
	int n = 20, rows;
	float probs[] = {.0, 0.1, 0.2, 0.4, 0.6, 0.8, -1.0}, *probs_iter;
	int iters = 10, run;
	KureRel * R = kure_rel_new (c);

	// i=1 is a special case.
	for (rows = 1 ; rows <= n ; ++rows) {
		for (probs_iter = probs ; *probs_iter >= 0 ; ++probs_iter) {
			for (run = 0 ; run < iters ; ++run) {
				int k;
				int tests = 100;
				long ents = 0;
				int succeeded_tests = 0;

				for (k = 0 ; k < tests ; k ++) {
					Kure_success success;

					kure_rel_set_size_si (R, rows, rows);
					success = kure_random_no_cycles_simple (R, *probs_iter);
					if (success) {
						if (_has_cycles(R)) {
							check_result ( 0, "Checking kure_random_no_cycles_simple with"
									"for %d x %d and prob. %.3f (%d entries) for cycles", rows, rows, *probs_iter,
									kure_get_entries_si(R, NULL));
						}

						ents += kure_get_entries_si (R, NULL);
						succeeded_tests ++;
					}
				}

				if (succeeded_tests > 0) {
					double avg_density = ents / (double)(rows*rows*succeeded_tests);
					double diff_allowed = *probs_iter / 1.5;

					check_result (fabs(*probs_iter - avg_density) <= diff_allowed,
							"Checking kure_random_no_cycles_simple for %d x %d and prob. %.3f "
							"(avg. density: %0.3lf, diff. allowed: %.2lf)",
							rows, rows, *probs_iter, avg_density, diff_allowed);
				}
			}
		}
	}

	kure_rel_destroy (R);
}

struct tree_opt_testcase_t
{
	const char * dsl;
	const char * to_return;
};

Kure_bool _on_function_tree_opt (void * user_data, const char * code, const char * lua_code)
{
	struct tree_opt_testcase_t * test = (struct tree_opt_testcase_t*) user_data;
	char * ret_begin = strstr (lua_code, "return");

	/* find the return part ... */
	if ( !ret_begin) {
		fprintf (os, "\tMissing 'return' statement in generated Lua code: \"%s\"\n", lua_code);
		return FALSE;
	}
	else {
		if (strstr (ret_begin+6, "return")) {
			fprintf (os, "\tMultiple 'return' statements in generated Lua code: \"%s\"\n", lua_code);
			return FALSE;
		}
		else {
			ret_begin += 6 /*"return"*/;
			ret_begin += strspn (ret_begin, " \n\r\t");
			if (strncmp (test->to_return, ret_begin, strlen(test->to_return)) == 0) {
				return TRUE;
			}
			else {
				fprintf (os, "\tReturn-statement and presumed code differ:\n"
						"\t\t\"%s\" vs.\n"
						"\t\t\"%s\" (presumed)\n", ret_begin, test->to_return);
				return FALSE;
			}
		}
	}
}

void check_dsl_check_tree_optimization (KureContext * c)
{
	/* Checks application of mult. in the presence of transposition and if
	 * the parser can handle these situations. We only check if the parser
	 * generates the correct code. We don't check the Kure C routines. This
	 * has to be done somewhere else. */

	static struct tree_opt_testcase_t tests [] = { { "a(X) = X^*X^.", 		"kure.mult_transp_transp(X,X)" },
				   { "b(X) = X^*X.", 		"kure.mult_transp_norm(X,X)" },
				   { "c(X) = X*X^.", 		"kure.mult_norm_transp(X,X)" },
				   { "d(X) = X*X.", 		"kure.mult(X,X)" },
				   { "e(X) = X^^.", 		"X" },
				   { "f(X) = (X^)^.", 		"(X)" },
				   { "g(X) = X*(X)^.", 		"kure.mult_norm_transp(X,(X))" },
				   { "h(X) = X*(X^).", 		"kure.mult_norm_transp(X,(X))" },
				   { "i(X) = (-X^)^.", 		"(kure.complement(X))" },
				   { "j(X) = X*(X^^*X^*X^).", 		"kure.mult(X,(kure.mult_norm_transp(kure.mult_transp_transp(kure.transpose(X),X),X)))" },
				   { "k(X) = X^^^^^.", 		"kure.transpose(X)" },
				   { "l(X) = -(-(X^))^.", 	"kure.transpose(kure.complement((kure.complement((kure.transpose(X))))))" },
				   { "left_res(S,R) = -(-S*R^).", 	"kure.complement((kure.mult_norm_transp(kure.complement(S),R)))" },
				   { "right_res(R,S) = -(R^*-S).", 	"kure.complement((kure.mult_transp_norm(R,kure.complement(S))))" },
				   {NULL, NULL}}, *iter = NULL;

	lua_State * l = kure_lua_new(c);

	KureParserObserver o = {0};
	o.onFunction = _on_function_tree_opt;

	for ( iter = tests ; iter->dsl ; ++iter) {
		KureError *err = NULL;
		Kure_success success;

		o.object = iter;
		success = kure_lang_parse (iter->dsl, &o, &err);
		check_result (success, "Checking if \"%s\" can be parsed", iter->dsl);
		if ( !success) {
			fprintf (os, "\tReason: %s\n", err->message);
			kure_error_destroy (err);
		}
	}

	kure_lua_destroy (l);
}


void check_dsl (KureContext * c)
{
	check_dsl_check_tree_optimization (c);
}

void check_partial_funcs (KureContext * c)
{
	struct { int size1, size2; } args[] = {
			{1,1}, {1,2}, {2,1}, {2,2},
			{2,3}, {3,2}, {3,3}, {3,4},
			{4,3}, {4,4}, {5,4}, {4,5},
			{-1,-1} }, *iter = NULL;
	Mat * mR = mat_new(1,1);
	KureRel * R = kure_rel_new (c);

	for (iter = args ; iter->size1 > 0 && iter->size2 > 0 ; ++iter) {
		Kure_success success = kure_partial_funcs_si (R, iter->size1, iter->size2);
		check_result (success, "Checking if kure_partial_funcs_si works for "
				"size1=%d, size2=%d", iter->size1, iter->size2);
		if (success) {
			mat_partial_funcs (mR, iter->size1, iter->size2);
			check_result_equal(R, mR, "Checking if kure_partial_funcs_si is correct for "
					"size1=%d, size2=%d", iter->size1, iter->size2);
		}
	}

	kure_rel_destroy(R);
	mat_destroy(mR);
}

void check_total_funcs (KureContext * c)
{
	struct { int size1, size2; } args[] = {
			{1,1}, {1,2}, {2,1}, {2,2},
			{2,3}, {3,2}, {3,3}, {3,4},
			{4,3}, {4,4}, {5,4}, {4,5},
			{-1,-1} }, *iter = NULL;
	Mat * mR = mat_new(1,1);
	KureRel * R = kure_rel_new (c);

	for (iter = args ; iter->size1 > 0 && iter->size2 > 0 ; ++iter) {
		Kure_success success = kure_total_funcs_si (R, iter->size1, iter->size2);
		check_result (success, "Checking if kure_total_funcs_si works for "
				"size1=%d, size2=%d", iter->size1, iter->size2);
		if (success) {
			mat_total_funcs (mR, iter->size1, iter->size2);
			check_result_equal(R, mR, "Checking if kure_total_funcs_si is correct for "
					"size1=%d, size2=%d", iter->size1, iter->size2);
		}
	}

	kure_rel_destroy(R);
	mat_destroy(mR);
}


void check_product_order (KureContext * c)
{
	int n_probs = 6;
	float probs [] = { 0.0, 0.1, 0.25, 0.5, 0.75, 1.0 };
	struct { int size1, size2; } args[] = {
				{1,1}, {1,2}, {2,1}, {2,2},
				{2,3}, {3,2}, {3,3}, {3,4},
				{4,3}, {4,4}, {5,4}, {4,5},
				{-1,-1} }, *iter = NULL;
	Mat * mR = mat_new(1,1), *mS = mat_new(1,1), *mZ = mat_new(1,1);
	KureRel * R = kure_rel_new (c), *S = kure_rel_new (c), *Z = kure_rel_new (c);
	int i,j;

	for (i = 0 ; i < n_probs ; i++) {
		for (j = 0 ; j < n_probs ; j ++) {
			for (iter = args ; iter->size1 > 0 && iter->size2 > 0 ; ++iter) {
				Kure_success success;

				kure_rel_set_size_si (R, iter->size1, iter->size1);
				kure_random_simple (R, probs[i]);

				kure_rel_set_size_si (S, iter->size2, iter->size2);
				kure_random_simple (S, probs[j]);

				kure_to_mat (mR, R);
				kure_to_mat (mS, S);

				success = kure_product_order (Z, R, S);
				check_result (success, "Checking if kure_product_order works for "
								"size1=%d, size2=%d, probs: %.2f, %.2f", iter->size1,
								iter->size2, probs[i], probs[j]);
				if (success) {
					mat_product_order (mZ, mR, mS);
					check_result_equal(Z, mZ, "Checking if kure_product_order is correct for "
										"size1=%d, size2=%d, probs: %.2f, %.2f",
										iter->size1, iter->size2, probs[i], probs[j]);
				}
			}
		}
	}
}

void check_sum_order (KureContext * c)
{
	int n_probs = 6;
	float probs [] = { 0.0, 0.1, 0.25, 0.5, 0.75, 1.0 };
	struct { int size1, size2; } args[] = {
				{1,1}, {1,2}, {2,1}, {2,2},
				{2,3}, {3,2}, {3,3}, {3,4},
				{4,3}, {4,4}, {5,4}, {4,5},
				{-1,-1} }, *iter = NULL;
	Mat * mR = mat_new(1,1), *mS = mat_new(1,1), *mZ = mat_new(1,1);
	KureRel * R = kure_rel_new (c), *S = kure_rel_new (c), *Z = kure_rel_new (c);
	int i,j;

	for (i = 0 ; i < n_probs ; i++) {
		for (j = 0 ; j < n_probs ; j ++) {
			for (iter = args ; iter->size1 > 0 && iter->size2 > 0 ; ++iter) {
				Kure_success success;

				kure_rel_set_size_si (R, iter->size1, iter->size1);
				kure_random_simple (R, probs[i]);

				kure_rel_set_size_si (S, iter->size2, iter->size2);
				kure_random_simple (S, probs[j]);

				kure_to_mat (mR, R);
				kure_to_mat (mS, S);

				success = kure_sum_order (Z, R, S);
				check_result (success, "Checking if kure_sum_order works for "
								"size1=%d, size2=%d, probs: %.2f, %.2f", iter->size1,
								iter->size2, probs[i], probs[j]);
				if (success) {
					mat_sum_order (mZ, mR, mS);
					check_result_equal(Z, mZ, "Checking if kure_sum_order is correct for "
										"size1=%d, size2=%d, probs: %.2f, %.2f",
										iter->size1, iter->size2, probs[i], probs[j]);
				}
			}
		}
	}
}

/* This test was added because there was a problem related to counting
 * minterms in the 64-bit version of Relview. */
void check_queens_slow (KureContext * context)
{
	int n_min = 4, n_max = 9;
	int results[] = {2,10,4,40,92,352}; // http://en.wikipedia.org/wiki/Queens_problem

	static const char * code =
			"main(b)\n"
			"  DECL R\n"
			"  BEG  R = QueenRel(b);\n"
			"       RETURN GreIndep(R)\n"
			"  END.\n"
			"\n"
			"{**********************************************************\n"
			" Enumeration of all maximum indepedent sets of an arbitra-\n"
			" ry directed graph following the lines of the HOA 93 paper \n"
			" of Berghammer et al.\n"
			"**********************************************************}\n"
			"\n"
			"GreIndep(R)\n"
			"  DECL Epsi, REpsi, H, C, s\n"
			"  BEG  Epsi = epsi(On1(R));\n"
			"       REpsi = R * Epsi;\n"
			"       H = Epsi & REpsi;\n"
			"       C = cardrel(On1(R));\n"
			"       s = -(L1n(R) * H)^\n"
			"       RETURN s & -((-C)^*s)\n"
			"  END.\n"
			"\n"
			"{**********************************************************\n"
			" QueenRel computes for a vector b of length n the queen-\n"
			" attack-relation for an n*n chessboard graph \n"
			"**********************************************************}\n"
			"\n"
			"QueenRel(b)\n"
			"  DECL Prod = PROD(succ(b),succ(b));\n"
			"       pi, rho, S, R, Ho, U, Ve, A, B, D1, D2\n"
			"  BEG  pi = p-1(Prod);\n"
			"       rho = p-2(Prod);\n"
			"       S = succ(b);\n"
			"       R = [pi*S,rho];\n"
			"       Ho = trans(R) | trans(R^);        {horizontal moves}\n"
			"       U = [pi,rho*S];\n"
			"       Ve = trans(U) | trans(U^);          {vertical moves}\n"
			"       A = [pi*S,rho*S];\n"
			"       D1 = trans(A) | trans(A^);     {main diagonal moves}\n"
			"       B = [pi*S^,rho*S];\n"
			"       D2 = trans(B) | trans(B^)     {other diagonal moves}\n"
			"       RETURN Ho | Ve | D1 | D2\n"
			"  END.\n";

	int n;
	lua_State * L = kure_lua_new(context);
	KureError * err = NULL;
	if ( ! kure_lang_load (L, code, &err)) {
		fprintf (os, "Unable to load program for QUEENS (slow) test.\n Program was:\n'%s'\n"
				"Error message: '%s'\n", code, err?err->message:"(unknown)");
		abort ();
	}

	for (n = n_min ; n <= n_max ; ++n) {
		int n_sols;
		KureRel *v, *sols;
		Kure_success success;
		static const char chunk[] = "main(n)";

		v = kure_rel_new_with_size_si (context, n, 1); // nx1 vector.
		kure_lua_set_rel_copy (L, "n", v);
		kure_rel_destroy (v);

		sols = kure_lang_exec(L, chunk, &err);
		if (!sols) {
			fprintf (os, "Unable to execute chunk '%s' in QUEENS (slow) test. \n"
					"Error message: '%s'\n", chunk, err?err->message:"(unknown)");
			abort();
		}
		n_sols = kure_get_entries_si (sols, NULL);
		success = (n_sols == results[n-n_min]);
		check_result(success, "Checking if QUEENS (slow) is correct for n=%d players", n);
		if (!success) {
			fprintf (os, "Result was %d but should have been %d for %d players.\n", n_sols, results[n-n_min], n);
			abort ();
		}

		kure_rel_destroy (sols);
	}

	kure_lua_destroy(L);
}



/**
 * Checks for number of entries. We have encountered some problems with
 * counting entries von 64-bit platforms and some versions of CUDD.
 */
void check_entries (KureContext * context)
{
	int n;

	/* Checks for epsi(n) for some _even_ n. */
	for (n = 2 ; n <= 100 ; n += 2) {
		KureRel * R = kure_rel_new (context);
		mpz_t ents, x; // x = (n * 2**n) / 2 = n/2 * 2**n

		mpz_init (ents);
		mpz_init_set_si (x, n / 2);

		kure_membership (R, n);
		kure_get_entries (R, ents);
		kure_rel_destroy (R);

		mpz_mul_2exp (x,x,n); // x *= 2**n

		Kure_success success;
		success = (0 == mpz_cmp (x, ents));
		check_result(success, "Checking if |epsi(n)|=(n*2^n)/2 for n=%d players", n);
		if (!success) {
			gmp_fprintf (os, "Result was %Zd but should have been %Zd for %d players.\n", ents, x, n);
		}

		mpz_clear (ents);
		mpz_clear (x);
	}
}


static const char * minsets_maxsets_code =
		"min(R,v) = v & -((R^ & -I(R)) * v).\n"
		"max(R,v) = min(R^,v).\n"
		"\n"
		"upset(n,v)\n"
		"  DECL	M,SS\n"
		"  BEG	M = epsi(n);\n"
		"		SS = -(M^ * -M);\n"
		"		RETURN (v^ * SS)^\n"
		"  END.\n"
		"\n"
		"downset(n,v)\n"
		"  DECL	M,SS\n"
		"  BEG	M = epsi(n);\n"
		"		SS = -(M^ * -M);\n"
		"		RETURN SS*v\n"
		"  END.\n"
		"\n"
		"minSets(n,v)\n"
		"  DECL M,SS\n"
		"  BEG	M = epsi(n);\n"
		"		SS = -(M^ * -M);\n"
		"		RETURN min(SS,v)\n"
		"  END.\n"
		"\n"
		"maxSets(n,v)\n"
		"  DECL M,SS\n"
		"  BEG	M = epsi(n);\n"
		"		SS = -(M^ * -M);\n"
		"		RETURN max(SS,v)\n"
		"  END.\n";



static const char * not_sup_sub_set_code =
		"{* x is in the result if v_x and there is no element y in w"
		" * s.t. x is-supset-of y. *}\n"
		"notSupSet(n,v,w)\n"
		"  DECL M,SS\n"
		"  BEG	M = epsi(n);\n"
		"		SS = -(M^ * -M);\n"
		"		RETURN v & -(w^*SS)^\n"
		"  END.\n"
		"\n"
		"{* x is in the result if v_x and there is no element y in w"
		" * s.t. x is-subset-of y. *}\n"
		"notSubSet(n,v,w)\n"
		"  DECL M,SS\n"
		"  BEG	M = epsi(n);\n"
		"		SS = -(M^ * -M);\n"
		"		RETURN v & -(SS*w)\n"
		"  END.\n";



// Declared in KureImpl.h
DdNode * not_sup_set (DdManager * manager, DdNode * f, DdNode * g);
DdNode * not_sub_set (DdManager * manager, DdNode * f, DdNode * g);
Kure_bool contains_empty_set (DdManager * manager, DdNode * f);
Kure_bool contains_all_set (DdManager * manager, DdNode * f);
DdNode * empty_set (DdManager * manager, size_t top, size_t nvars);
DdNode * all_set (DdManager * manager, size_t top, size_t nvars);


void check_not_sup_sub_set (KureContext * context,
		const char * test_name,
		int ns [], // 0 terminated
		double prob_min, double prob_step, double prob_max,
		int iters,
		// Available vectors are: n,v,w
		const char * eval_expr, // e.g. "notSupSet(n,v,w)"
		DdNode * (*eval_func) (DdManager*,DdNode*,DdNode*))
{
	const char * code = not_sup_sub_set_code;

	/* Load the program code ... */
	lua_State * L = kure_lua_new(context);
	KureError * err = NULL;
	if ( ! kure_lang_load (L, code, &err)) {
		fprintf (os, "Unable to load program for 'not_sup_set' test.\n Program was:\n'%s'\n"
				"Error message: '%s'\n", code, err?err->message:"(unknown)");
		abort ();
	}

	int *n_ptr;
	for (n_ptr = ns ; *n_ptr > 0 ; ++n_ptr) {
		int n = *n_ptr;
		double prob, i;

		KureRel *v = kure_rel_new_with_size_si(context, (int)1<<n, 1);
		KureRel *w = kure_rel_new_with_proto (v);
		KureRel *nv = kure_rel_new_with_size_si(context, n, 1);

		kure_lua_set_rel_copy (L, "n", nv);

		for (prob = prob_min ; prob <= prob_max ; prob += prob_step) {
			for (i = 0 ; i < iters ; ++i) {
				KureRel *res, *res2;
				DdNode * resNode;
				//Kure_success success;
				GTimer * timer = g_timer_new ();
				gdouble relalg_elapsed, bdd_elapsed; // secs

				kure_random_simple (v, prob);
				kure_random_simple (w, prob);

				kure_lua_set_rel_copy (L, "v", v);
				kure_lua_set_rel_copy (L, "w", w);

				g_timer_start(timer);
				res = kure_lang_exec(L, eval_expr, &err);
				if (!res) {
					fprintf (os, "Unable to execute chunk '%s' in '%s' test. \n"
							"Error message: '%s'\n",
							eval_expr, test_name, err?err->message:"(unknown)");
					abort();
				}

				relalg_elapsed = g_timer_elapsed (timer, NULL);

				g_timer_start (timer); // restart
				resNode = eval_func (kure_context_get_manager(context),
						kure_rel_get_bdd(v), kure_rel_get_bdd(w));
				Cudd_Ref (resNode);
				res2 = kure_rel_new_with_proto (v);
				kure_rel_set_bdd (res2, resNode);
				Cudd_Deref (resNode);

				bdd_elapsed = g_timer_elapsed(timer, NULL);
				g_timer_destroy(timer);

				if ( !kure_equals (res,res2,NULL)) {
					fprintf (stderr, "\nv=\n");
					dump_kure_rel_tranposed (stderr, v, "\t");
					fprintf (stderr, "w=\n");
					dump_kure_rel_tranposed (stderr, w, "\t");

					fprintf (stderr, "res (expected)=\n");
					dump_kure_rel_tranposed (stderr, res, "\t");
					fprintf (stderr, "res (as is)\n");
					dump_kure_rel_tranposed (stderr, res2, "\t");

					abort();
				}

				check_kure_result_equal(res, res2, "Checking if '%s' is correct for "
						"n=%d, prob=%lf (speedup=%.3lf, relalg=%.3lf, bdd=%.3lf in secs)",
						test_name, n, prob, relalg_elapsed / bdd_elapsed,
						relalg_elapsed, bdd_elapsed);

				kure_rel_destroy (res2);
				kure_rel_destroy (res);
			}
		}

		kure_rel_destroy (v);
		kure_rel_destroy (w);
		kure_rel_destroy (nv);
	}

	kure_lua_destroy(L);
}


void check_contains_empty (KureContext * context)
{
	const char * test_name = "contains_empty";

	/* Load the program code ... */
	lua_State * L = kure_lua_new(context);
	KureError * err = NULL;

	int ns[] = {1,2,3,4,5,6,7,0};
	double probs[] = {0, 0.1, 0.2, 0.5, 0.75, -1.0};
	int iters = 100;

	int *n_ptr;

	for (n_ptr = ns ; *n_ptr > 0 ; ++n_ptr) {
		int n = *n_ptr;
		double * prob_ptr;

		KureRel *v = kure_rel_new_with_size_si(context, 1<<n, 1);

		for (prob_ptr = probs ; *prob_ptr >= .0 ; prob_ptr ++) {
			double prob = *prob_ptr;
			int i;

			for (i = 0 ; i < iters ; ++i) {
				KureRel *res;

				kure_random_simple (v, prob);
				kure_lua_set_rel_copy (L, "v", v);

				const char * eval_expr = "incl(init(v),v)";
				res = kure_lang_exec(L, eval_expr, &err);
				if (!res) {
					fprintf (os, "Unable to execute chunk '%s' in '%s' test. \n"
							"Error message: '%s'\n",
							eval_expr, test_name, err?err->message:"(unknown)");
					abort();
				}

				check_result(kure_get_bit_si(res,0,0,NULL) == contains_empty_set (kure_context_get_manager(context), kure_rel_get_bdd(v))
						, "Checking if '%s' is correct for "
						"n=%d, prob=%lf", test_name, n, prob);

				kure_rel_destroy (res);
			}
		}

		kure_rel_destroy (v);
	}

	kure_lua_destroy(L);
}


void check_contains_all (KureContext * context)
{
	const char * test_name = "contains_all";

	/* Load the program code ... */
	lua_State * L = kure_lua_new(context);
	KureError * err = NULL;

	int ns[] = {1,2,3,4,5,6,7,0};
	double probs[] = {0, 0.1, 0.2, 0.5, 0.75, -1.0};
	int iters = 100;

	int *n_ptr;

	for (n_ptr = ns ; *n_ptr > 0 ; ++n_ptr) {
		int n = *n_ptr;
		double * prob_ptr;

		int rows = (int)(1 << n);
		KureRel *v = kure_rel_new_with_size_si(context, rows, 1);

		for (prob_ptr = probs ; *prob_ptr >= .0 ; prob_ptr ++) {
			double prob = *prob_ptr;
			int i;

			for (i = 0 ; i < iters ; ++i) {
				DdManager *manager = kure_context_get_manager(context);

				kure_random_simple (v, prob);

				DdNode *f = kure_rel_get_bdd(v);
				Kure_bool v_contains_all = kure_get_bit_si (v, rows-1, 0, NULL);
				Kure_bool result = contains_all_set(manager, f);
				Kure_bool success = (v_contains_all == result);

//				if (!success) {
//					fprintf (stderr, "v was:\n");
//					dump_kure_rel_tranposed (stderr, v, "   ");
//					fprintf (stderr, "Result is %d, but should have been %d.\n", v_contains_all, result);
//				}

				check_result(success
						, "Checking if '%s' is correct for "
						"n=%d, prob=%lf", test_name, n, prob);
			}
		}

		kure_rel_destroy (v);
	}

	kure_lua_destroy(L);
}


void check_empty_set (KureContext * context)
{
	const char * test_name = "empty_set";

	/* Load the program code ... */
	lua_State * L = kure_lua_new(context);
	KureError * err = NULL;

	int ns[] = {1,2,3,4,5,6,7,0};

	int *n_ptr;
	for (n_ptr = ns ; *n_ptr > 0 ; ++n_ptr) {
		int n = *n_ptr;
		KureRel *res, *res2;
		DdNode *resNode;
		DdManager * manager = kure_context_get_manager(context);
		DdNode *f,*g;

		KureRel * v = kure_rel_new_with_size_si (context, (int)1<<n, 1);
		kure_lua_set_rel_copy (L, "v", v);

		const char * eval_expr = "init(v)";
		res = kure_lang_exec(L, eval_expr, &err);
		if (!res) {
			fprintf (os, "Unable to execute chunk '%s' in '%s' test. \n"
					"Error message: '%s'\n",
					eval_expr, test_name, err?err->message:"(unknown)");
			abort();
		}

		resNode = empty_set (manager, 0, n);
		Cudd_Ref (resNode);
		res2 = kure_rel_new_with_proto (res);
		kure_rel_set_bdd (res2, resNode);
		Cudd_Deref (resNode);

		Kure_success success;

		check_result (kure_get_entries_si (res2, &success) == 1,
				"Checking if '%s' has just a single entry for n=%d",
				test_name, n);
		assert (success); // would indicate much to many entries.

//		f = kure_rel_get_bdd(res);
//		g = kure_rel_get_bdd(res2);
//
//		fprintf (stderr, "res=%p, res2=%p\n", f, g);
//		if (f != g) {
//			FILE * fp = fopen ("/tmp/res.dot", "w+");
//			Cudd_DumpDot( manager, 1, &f, NULL, NULL, fp);
//			fclose (fp);
//			fp = fopen ("/tmp/res2.dot", "w+");
//			Cudd_DumpDot( manager, 1, &g, NULL, NULL, fp);
//			fclose (fp);
//			abort ();
//		}

		check_kure_result_equal (res, res2, "Checking if '%s' is correct for "
				"n=%d", test_name, n);

		kure_rel_destroy (res);
		kure_rel_destroy (res2);
		kure_rel_destroy (v);
	}

	kure_lua_destroy(L);
}


void check_all_set (KureContext * context)
{
	const char * test_name = "all_set";

	int ns[] = {1,2,3,4,5,6,7,0};

	int *n_ptr;
	for (n_ptr = ns ; *n_ptr > 0 ; ++n_ptr) {
		int n = *n_ptr;
		KureRel *res2;
		DdNode *resNode;
		DdManager * manager = kure_context_get_manager(context);
		DdNode *f,*g;

		int rows = (int)(1<<n);
		KureRel * v = kure_rel_new_with_size_si (context, rows, 1);

		kure_set_bit_si (v, TRUE, rows-1, 0);

		resNode = all_set (manager, 0, n);
		Cudd_Ref (resNode);
		res2 = kure_rel_new_with_proto (v);
		kure_rel_set_bdd (res2, resNode);
		Cudd_Deref (resNode);

		Kure_success success;

		check_result (kure_get_entries_si (res2, &success) == 1,
				"Checking if '%s' has just a single entry for n=%d",
				test_name, n);
		assert (success); // would indicate much to many entries.

		check_kure_result_equal (v, res2, "Checking if '%s' is correct for "
				"n=%d", test_name, n);

		kure_rel_destroy (res2);
		kure_rel_destroy (v);
	}
}


void check_not_sup_set (KureContext * context)
{
	int ns[] = {1,2,3,4,5,7,10,0};
	double prob_min = 0.05, prob_step = 0.05, prob_max = 0.6;
	int iters = 100;

	check_not_sup_sub_set (context, "not_sup_set",
			ns, prob_min, prob_step, prob_max, iters,
			"notSupSet(n,v,w)", not_sup_set);
}


void check_not_sub_set (KureContext * context)
{
	int ns[] = {1,2,3,4,5,7,10,0};
	double prob_min = 0.05, prob_step = 0.05, prob_max = 0.6;
	int iters = 100;

	check_not_sup_sub_set (context, "not_sub_set",
			ns, prob_min, prob_step, prob_max, iters,
			"notSubSet(n,v,w)", not_sub_set);
}


void check_sets (KureContext * context,
		const char * test_name,
		int ns [], // 0 terminated
		double prob_min, double prob_step, double prob_max,
		int iters,
		// Available vectors are: n,v
		const char * input_expr, // e.g. "upset(n,v)"
		// result of input_expr is available as vector u
		const char * eval_expr, // e.g. "minSets(n,u)"
		Kure_success (*eval_func) (KureRel*, const KureRel*))
{
	/* Load the program code ... */
	lua_State * L = kure_lua_new(context);
	KureError * err = NULL;
	if ( ! kure_lang_load (L, minsets_maxsets_code, &err)) {
		fprintf (os, "Unable to load program for test.\n Program was:\n'%s'\n"
				"Error message: '%s'\n", minsets_maxsets_code, err?err->message:"(unknown)");
		abort ();
	}

	int *n_ptr;
	for (n_ptr = ns ; *n_ptr > 0 ; ++n_ptr) {
		int n = *n_ptr;
		double prob, i;

		KureRel *v = kure_rel_new_with_size_si(context, (int)1<<n, 1);
		KureRel *nv = kure_rel_new_with_size_si(context, n, 1);

		kure_lua_set_rel_copy (L, "n", nv);

		for (prob = prob_min ; prob <= prob_max ; prob += prob_step) {
			for (i = 0 ; i < iters ; ++i) {
				KureRel *res, *res2, *u;
				Kure_success success;
				GTimer * timer = g_timer_new ();
				gdouble old_elapsed, new_elapsed; // secs

				kure_random_simple (v, prob);
				kure_lua_set_rel_copy (L, "v", v);

				u = kure_lang_exec(L, input_expr, &err);
				if (!u) {
					fprintf (os, "Unable to execute chunk '%s' in '%s' test. \n"
							"Error message: '%s'\n",
							input_expr, test_name, err?err->message:"(unknown)");
					abort();
				}

				kure_lua_set_rel_copy (L, "u", u);

				g_timer_start(timer);
				res = kure_lang_exec(L, eval_expr, &err);
				if (!res) {
					fprintf (os, "Unable to execute chunk '%s' in '%s' test. \n"
							"Error message: '%s'\n",
							eval_expr, test_name, err?err->message:"(unknown)");
					abort();
				}

				old_elapsed = g_timer_elapsed (timer, NULL);

				g_timer_start (timer); // restart
				res2 = kure_rel_new (context);
				success = eval_func (res2, u);
				if ( !success) {
					err = kure_context_get_error(context);
					fprintf (stderr, "'%s' failed. Reason: %s\n",
							test_name, err ? err->message : "(unknown)");
					abort ();
				}
				new_elapsed = g_timer_elapsed(timer, NULL);
				g_timer_destroy(timer);

				if ( !kure_equals (res,res2,NULL)) {
					fprintf (stderr, "\nu=\n");
					dump_kure_rel_tranposed (stderr, u, "\t");

					fprintf (stderr, "res (expected)=\n");
					dump_kure_rel_tranposed (stderr, res, "\t");
					fprintf (stderr, "res (as is)\n");
					dump_kure_rel_tranposed (stderr, res2, "\t");

					abort();
				}


				check_kure_result_equal(res, res2, "Checking if '%s' is correct for "
						"n=%d, prob=%lf (speedup=%.1lf, relalg=%.3lf, bdd=%.3lf in secs)",
						test_name, n, prob, old_elapsed / new_elapsed,
						old_elapsed, new_elapsed);

				kure_rel_destroy (res2);
				kure_rel_destroy (res);
				kure_rel_destroy(u);
			}
		}

		kure_rel_destroy (v);
		kure_rel_destroy (nv);
	}

	kure_lua_destroy(L);
}


void check_sets_performance (KureContext * context,
		const char * test_name,
		int ns [], // 0 terminated
		double prob_min, double prob_step, double prob_max,
		int iters,
		// Available vectors are: n,v
		const char * input_expr, // e.g. "upset(n,v)"
		// result of input_expr is available as vector u
		const char * eval_expr, // e.g. "minSets(n,u)"
		Kure_success (*eval_func) (KureRel*, const KureRel*))
{
	/* Load the program code ... */
	lua_State * L = kure_lua_new(context);
	KureError * err = NULL;
	if ( ! kure_lang_load (L, minsets_maxsets_code, &err)) {
		fprintf (os, "Unable to load program for test.\n Program was:\n'%s'\n"
				"Error message: '%s'\n", minsets_maxsets_code, err?err->message:"(unknown)");
		abort ();
	}

	int *n_ptr;

	for (n_ptr = ns ; *n_ptr > 0 ; ++n_ptr) {
		int n = *n_ptr;
		double prob, i;

		KureRel *v = kure_rel_new_with_size_si(context, (int)1<<n, 1);
		KureRel *nv = kure_rel_new_with_size_si(context, n, 1);

		kure_lua_set_rel_copy (L, "n", nv);

		for (prob = prob_min ; prob <= prob_max ; prob += prob_step) {
			for (i = 0 ; i < iters ; ++i) {
				KureRel *res, *res2, *u;
				Kure_success success;
				GTimer * timer = g_timer_new ();
				gdouble creat_elapsed, new_elapsed;

				fprintf (stderr, "Creating random vector with %d rows ... \n", kure_rel_get_rows_si(v));
				g_timer_start (timer); // restart
				kure_random_simple (v, prob);
				kure_lua_set_rel_copy (L, "u", v);
				creat_elapsed = g_timer_elapsed(timer, NULL);

				u = v;

				fprintf (stderr, "Computing min. elements ... \n");
				g_timer_start (timer); // restart
				res2 = kure_rel_new (context);
				success = eval_func (res2, u);
				if ( !success) {
					err = kure_context_get_error(context);
					fprintf (stderr, "'%s' failed. Reason: %s\n",
							test_name, err ? err->message : "(unknown)");
					abort ();
				}
				new_elapsed = g_timer_elapsed(timer, NULL);

				kure_rel_destroy (res2);

				fprintf (stderr, "performed '%s' for "
						"n=%d, prob=%lf (%.3lf in secs; creation took %.3lf secs)\n",
						test_name, n, prob, new_elapsed, creat_elapsed);

				g_timer_destroy(timer);
			}
		}

		kure_rel_destroy (v);
		kure_rel_destroy (nv);
	}

	kure_lua_destroy(L);
}


void check_minsets_upset (KureContext * context)
{
	int ns [] = {1,2,3,4,5,6,7,0};
	int iters = 100;

	check_sets (context, "minsets_upset",
			ns,
			.05, .05, .6,
			iters,
			"upset(n,v)", "minSets(n,u)",
			kure_minsets_upset);
}


void check_minsets (KureContext * context)
{
	int ns [] = {1,2,3,4,5,6,7,0};
	//int ns [] = {17,18,19,20,0};
	int iters = 100;

	check_sets (context, "minsets (with random up-set)",
			ns,
			.05, .05, .6,
			iters,
			"upset(n,v)", "minSets(n,u)",
			kure_minsets);

	check_sets (context, "minsets (with random set)",
			ns,
			.05, .05, .6,
			iters,
			"v", "minSets(n,u)",
			kure_minsets);
}



//void check_minsets_performance (KureContext * context)
//{
//	int ns [] = {30,0};
//	int iters = 10;
//
//	check_sets_performance (context, "minsets (with random set)",
//			ns,
//			.1, .1, .6,
//			iters,
//			"v", "minSets(n,u)",
//			kure_minsets);
//}


void check_maxsets_downset (KureContext * context)
{
	int ns [] = {1,2,3,4,5,6,7,0};
	int iters = 100;

	check_sets (context, "maxsets_downset",
			ns,
			.05, .05, .6,
			iters,
			"downset(n,v)", "maxSets(n,u)",
			kure_maxsets_downset);
}


void check_maxsets (KureContext * context)
{
	int ns [] = {1,2,3,4,5,6,7,0};
	int iters = 100;

	check_sets (context, "maxsets (with random down-set)",
			ns,
			.05, .05, .6,
			iters,
			"downset(n,v)", "maxSets(n,u)",
			kure_maxsets);

	check_sets (context, "maxsets (with random set)",
			ns,
			.05, .05, .6,
			iters,
			"v", "maxSets(n,u)",
			kure_maxsets);
}


void check_maxsets_minsets_size_constraints (KureContext * context)
{
	/* Check if any of the functions does only accept vector of size 2^n. */
	typedef Kure_success (*func_type) (KureRel *, const KureRel *);
	func_type funcs [] = { kure_minsets, kure_minsets_upset, kure_maxsets, kure_maxsets_downset };
	const char * names [] = { "kure_minsets", "kure_minsets_upset",
		"kure_maxsets", "kure_maxsets_downset" };
	const int funcCount = 4;
	int ns [] = {2,3,4,50,100,-1};
	KureRel * v = kure_rel_new (context);
	mpz_t rows, cols;
	int i, *nptr;
	mpz_init (rows);
	mpz_init (cols);

	for (i = 0 ; i < funcCount ; ++i) {
		func_type f = funcs[i];
		const char * name = names[i];

		for (nptr = ns ; *nptr > 0 ; nptr++) {
			int n = *nptr;

			mpz_ui_pow_ui (rows, 2, n);
			mpz_set_ui (cols, 1);

			assert(kure_rel_set_size (v, rows, cols));
			check_result (f(v,v), "Checking if %s accepts correct input for n=%d", name, n);

			// rows = rows-1
			mpz_sub_ui (rows, rows, 1);
			assert(kure_rel_set_size (v, rows, cols));
			check_result ( !f(v,v), "Checking if %s rejects correct input for n=%d and rows=(2^n)-1", name, n);

			// rows = rows+2
			mpz_add_ui (rows, rows, 2);
			assert(kure_rel_set_size (v, rows, cols));
			check_result ( !f(v,v), "Checking if %s rejects correct input for n=%d and rows=(2^n)+1", name, n);
		}
	}

	mpz_clear (cols);
	mpz_clear (rows);
	kure_rel_destroy (v);
}


int main ()
{
	KureContext * context = kure_context_new();
	kure_context_ref (context);

	os = stdout;

#if 1
	check_basic_rels(context);

	check_is_univalent (context);

	check_set_row (context);
	check_vec_inj (context);

	check_perm_simple (context);
	check_random_simple (context);

	// Randomness is hard to check due to additional constraints. Produces lots
	// of false negatives. Therefore ignored.
	//check_random_no_cycles (context);

	check_mult_norm_transp (context);
	check_mult_transp_norm (context);
	check_mult_transp_transp (context);

	check_projections (context);
	check_injections (context);

	check_right_residue (context);
	check_left_residue (context);
	check_symm_quot (context);

	check_tupling (context);
	check_direct_sum (context);

	check_epsi (context);
	check_less_card (context);

	check_vec_begin (context);
	check_vec_next (context);
	check_vec_point (context);

	check_atom (context);

	check_refl_hull (context);
	check_symm_hull (context);
	check_trans_hull (context);

	check_dsl (context);

	check_product_order (context);
	check_sum_order (context);

	check_partial_funcs (context);
	check_total_funcs (context);

	/* Some practical examples. */
	check_queens_slow (context);

	check_entries (context);

	check_contains_empty (context);
	check_contains_all (context);
	check_empty_set (context);
	check_all_set (context);

	check_not_sup_set (context);
	check_not_sub_set (context);

	check_minsets_upset (context);
	check_minsets (context);
	check_maxsets_downset (context);
	check_maxsets (context);

	check_maxsets_minsets_size_constraints (context);
#endif

	check_left_tupling (context);

	kure_context_destroy(context);
	return 0;
}
