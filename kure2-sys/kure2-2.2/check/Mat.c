/*
 * Mat.c
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

#include "Mat.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "Kure.h" // SIZEOF_INT

static double random01() { return random() / (double) RAND_MAX; }

Mat * mat_new (int rows, int cols)
{
  Mat * self = calloc (1, sizeof(Mat));
  self->rows = rows;
  self->cols = cols;
  self->data = calloc (rows * cols, 1);
  return self;
}

#define BIT(mat,i,j) (mat)->data[(i)*(mat)->cols+(j)]

int mat_same_dim (const Mat * a, const Mat * b)
{
  return a->rows == b->rows && a->cols == b->cols;
}

// non-destructive if self->rows == rows && self->cols == cols.
void mat_resize (Mat * self, int rows, int cols)
{
  if (self->rows != rows || self->cols != cols) {
	  free (self->data);
	  self->data = calloc (rows*(size_t)cols, 1);
	  self->rows = rows;
	  self->cols = cols;
  }
}

void mat_bin_op (Mat * rop, const Mat * a, const Mat * b, char (*f)(char,char))
{
  int i,j;
  int rows = a->rows, cols = a->cols;

  assert (mat_same_dim (a,b));
  mat_resize (rop, rows, cols);

  for (i = 0 ; i < rows ; i ++)
    for (j = 0 ; j < cols ; j ++)
      BIT(rop,i,j) = f(BIT(a,i,j), BIT(b,i,j));
}

static char _and(char a, char b) { return a && b; }
static char _or(char a, char b) { return a || b; }

void mat_and (Mat * rop, const Mat * a, const Mat * b)
{ mat_bin_op(rop, a, b, _and); }

void mat_or (Mat * rop, const Mat * a, const Mat * b)
{ mat_bin_op(rop, a, b, _or); }

void mat_transpose (Mat * rop, const Mat * a)
{
  int i,j;
  Mat * tmp = mat_new (a->cols, a->rows);

  for (i = 0 ; i < a->rows ; i ++)
    for (j = 0 ; j < a->cols ; j ++) {
    	BIT(tmp,j,i) = BIT(a,i,j);
    }

  mat_swap (rop, tmp);
  mat_destroy (tmp);
}

void mat_negate (Mat * rop, const Mat * a)
{
  int i,size = a->rows * a->cols;

  mat_resize (rop, a->rows, a->cols);

  for (i = 0 ; i < size ; i ++)
    rop->data[i] = !a->data[i];
}

#if 0
// replace rop by a. A is no longer valid after this op.
// Does nothing if rop == a.
void mat_replace (Mat * rop, Mat * a)
{
  if (rop != a) {
    char * tmp;

    mat_dtor (rop);
    tmp = rop->data;
    rop->data = a->data;
    a->data = tmp;
    rop->rows = a->rows;
    rop-.cols = a->cols;

    mat_destroy (a);
  }
}
#endif

void mat_dtor (Mat * self)
{
  if (self->data) free (self->data);
}

void mat_destroy (Mat * self)
{
  if (self) {
    mat_dtor (self);
    free (self);
  }
}

void mat_swap (Mat * a, Mat * b)
{
  Mat tmp = *a;
  *a = *b;
  *b = tmp;
}

// rop = a*b
void mat_mult (Mat * rop, const Mat * a, const Mat * b)
{
	int i,j,k;
	Mat * tmp;

	if (a->cols != b->rows) {
		fprintf (stderr, "mat_mult: %d = a->cols != b->rows = %d. Aborting!\n",
				a->cols, b->rows);
		assert (a->cols == b->rows);
	}
	tmp = mat_new (a->rows, b->cols);

	for (i = 0 ; i < a->rows ; i ++) {
		for (j = 0 ; j < b->cols ; j ++) {
			char match = 0;
			for (k = 0 ; k < a->cols && !match ; k ++)
				match = BIT(a,i,k) && BIT(b,k,j);
			BIT(tmp,i,j) = match;
		}
	}

	mat_swap (rop, tmp);
	mat_destroy (tmp);
}

void mat_mult_norm_transp (Mat *rop, const Mat *a, const Mat *b)
{
	Mat * bt = mat_new (1,1);
	mat_transpose (bt, b);
	mat_mult (rop, a, bt);
	mat_destroy (bt);
}

void mat_mult_transp_norm (Mat *rop, const Mat *a, const Mat *b)
{
	Mat * at = mat_new (1,1);
	mat_transpose (at, a);
	mat_mult (rop, at, b);
	mat_destroy (at);
}

void mat_mult_transp_transp (Mat *rop, const Mat *a, const Mat *b)
{
	Mat * at = mat_new (1,1), *bt = mat_new (1,1);
	mat_transpose (at, a);
	mat_transpose (bt, b);
	mat_mult (rop, at, bt);
	mat_destroy (at);
	mat_destroy (bt);
}

void mat_overlay (Mat * self, char (*f) (int i, int j, void*), void * user_data)
{
  int i, j;

  for (i = 0 ; i < self->rows ; i ++)
    for (j = 0 ; j < self->cols ; j ++)
      BIT(self,i,j) = (BIT(self,i,j) || f(i,j,user_data));
}

void mat_fill (Mat * self, char (*f) (int i, int j, void*), void * user_data)
{
  int i, j;

  for (i = 0 ; i < self->rows ; i ++)
    for (j = 0 ; j < self->cols ; j ++)
      BIT(self,i,j) = f(i,j,user_data);
}


static char _L(int i, int j, void * dummy) { return 1; }
static char _O(int i, int j, void * dummy) { return 0; }
static char _I(int i, int j, void * dummy) { return i==j; }
static char _Rand(int i, int j, void * p) { return (random01()<=*(float*)p) ? 1 : 0; }

void mat_L (Mat * self) { mat_fill (self, _L, NULL); }
void mat_O (Mat * self) { mat_fill (self, _O, NULL); }
void mat_I (Mat * self) { mat_fill (self, _I, NULL); }
void mat_random (Mat * self, float p) { mat_fill(self, _Rand, (void*)&p); }
void mat_random_full (Mat * self, float p, int rows, int cols) {
	mat_resize(self, rows, cols);
	mat_random(self, p);
}

static char _mat_cmp (const Mat * a, const Mat * b, char (*cmp) (char, char))
{
  int i,j;

  assert (mat_same_dim(a,b));

  for (i = 0 ; i < a->rows ; i ++)
    for (j = 0 ; j < a->cols ; j ++)
      if (! cmp(BIT(a,i,j), BIT(b,i,j)))
	return 0;

  return 1;
}

static char _equals (char a, char b) { return a==b; }
static char _incl (char a, char b) { return !a || b; }

char mat_equals (const Mat * a, const Mat * b) { return _mat_cmp (a,b,_equals); }
char mat_incl (const Mat * a, const Mat * b) { return _mat_cmp (a,b,_incl); }

char mat_getbit (Mat * self, int i, int j) { return BIT(self, i, j); }
void mat_setbit (Mat * self, int i, int j, char yesno) { BIT(self, i, j) = yesno; }

void mat_assign (Mat * rop, const Mat * a)
{
  if (rop != a) {
    mat_resize (rop, a->rows, a->cols);
    memcpy (rop->data, a->data, a->rows * a->cols);
  }
}

Mat * mat_clone (const Mat * self)
{
  Mat * ret = mat_new (1,1);
  mat_assign (ret, self);
  return ret;
}

void mat_epsi (Mat * self, int n)
{
  int i,j,width = 1;
  mat_resize (self, n, (int)pow(2,n));

  for (i = self->rows-1 ; i >=0 ; --i) {
    char in = 0;
    int left = width;

    for (j = 0 ; j < self->cols ; j ++) {
      BIT(self,i,j) = in;
      -- left;
      if (left <= 0) {
	in = !in;
	left = width;
      }
    }

    width <<= 1;
  }
}

void mat_p_1 (Mat * self, int size1, int size2)
{
  int i,k;

  mat_resize (self, size1 * size2, size1);

  for (k = 0 ; k < size1 ; k ++) {
    for (i = 0 ; i < size2 ; i ++) {
      BIT(self,i+size2*k,k) = 1;
    }
  }
}

void mat_p_2 (Mat * self, int size1, int size2)
{
  int i,k;

  mat_resize (self, size1 * size2, size2);

  for (k = 0 ; k < size1 ; k ++)
    for (i = 0 ; i < size2 ; i ++)
      BIT(self,i+size2*k,i) = 1;
}

void mat_i_1 (Mat * self, int size1, int size2)
{
  int i;

  mat_resize (self, size1, size1 + size2);

  for (i = 0 ; i < size1 ; i ++)
      BIT(self,i,i) = 1;
}

void mat_i_2 (Mat * self, int size1, int size2)
{
  int i;

  mat_resize (self, size2, size1 + size2);

  for (i = 0 ; i < size2 ; i ++)
      BIT(self,i,size1+i) = 1;
}

// row is 0-indexed
char mat_is_row_compl (Mat * self, int row)
{
	int j;

	for (j = 0 ; j < self->cols ; j ++)
		if (! BIT(self,row,j)) return 0;
	return 1;
}

char mat_is_row_consistent (Mat * self, int row)
{
	int j;
	char should_be = BIT(self,row,0);

	for (j = 1 ; j < self->cols ; j ++)
		if (BIT(self,row,j) != should_be) return 0;
	return 1;
}

char mat_is_vec (Mat * self)
{
	int i;

	for (i = 0 ; i < self->rows ; i ++)
		if (!mat_is_row_consistent(self,i)) return 0;
	return 1;
}

void mat_fill_row (Mat * self, int row) {
	int j; for (j = 0 ; j < self->cols ; ++j) BIT(self, row, j) = 1; }


char mat_is_empty (Mat * self)
{
	int i, size = self->rows * self->cols;
	for (i = 0 ; i < size && !self->data[i] ; i ++);
	return !(i < size);
}

void mat_set_row (Mat * self, char yesno, int i)
{
	int j;
	for (j = 0 ; j < self->cols ; ++j)
		BIT(self,i,j) = yesno;
}

void mat_vec_random (Mat * self, float p) {
	int i;
	for (i = 0 ; i < self->rows ; i ++)
		if (random01() <= p) mat_fill_row (self,i);
}

void mat_vec_random_full (Mat * self, float p, int rows, int cols)
{
	mat_resize (self, rows, cols);
	mat_vec_random (self, p);
}

int mat_vec_num_entries (Mat * self)
{
	int i, n = 0;
	for (i = 0 ; i < self->rows ; i ++)
		if (BIT(self,i,0)) n++;
	return n;
}


void mat_vec_begin (Mat * self)
{
	mat_O(self);
	mat_set_row (self, 1, 0);
}

void mat_vec_next (Mat * self)
{
	int i;

	assert (mat_is_vec (self));
	if (mat_is_empty(self))
		return;

	for (i = 0 ; i < self->rows && ! BIT(self,i,0) ; ++i);

	mat_set_row (self, 0, i);
	if (i+1 < self->rows)
		mat_set_row (self, 1, i+1);
}

void mat_inj (Mat * rop, Mat * vec)
{
	int n = mat_vec_num_entries (vec);
	if (n < 1) {
		fprintf (stderr, "mat_inj: Got empty vector.\n");
		exit (1);
	}
	else {
		int i, row;
		mat_resize(rop, n, vec->rows);
		mat_O(rop);
		for (i = 0, row = 0 ; i < vec->rows ; i ++) {
			if (BIT(vec,i,0)) {
				BIT(rop,row,i) = 1;
				row ++;
			}
		}
	}
}

void mat_dump_full (Mat * self, FILE * s, char one_ch, char zero_ch, int max_ents)
{
	if (self->rows * self->cols > max_ents || self->rows * self->cols < 2) {
		fprintf(s, "Matrix is too big! (rows*cols>max_ents)");
	}
	else {
		int i, j;

		fprintf(s, "__Matrix (%d rows, %d cols):__\n", self->rows, self->cols);
		for (i = 0; i < self->rows; ++i) {
			for (j = 0; j < self->cols; ++j)
				fputc(BIT(self,i,j) ? one_ch : zero_ch, s);
			fputc ('\n', s);
		}
	}
}

void mat_dump (Mat * self)
{
	mat_dump_full (self, stdout, '1', '.', 2<<16);
}

// compare a matrix with another object of arbitrary type
// but which has to have the same size.
char mat_cmp (const Mat * self, void * obj, char (*get_bit)(int,int,void*))
{
	int i,j;

	for (i = 0 ; i < self->rows ; ++i)
		for (j = 0 ; j < self->cols; ++j)
			if (!!get_bit(i,j,obj) != !!BIT(self,i,j))
				return 0;
	return 1;
}

// compare a matrix with another object of arbitrary type
// but which has to have the same size.
void  mat_set_by_func (Mat * self, void * obj, char (*get_bit)(int,int,void*)) {
	mat_fill(self, get_bit, obj); }

void mat_get_by_func (const Mat * self, void * obj, void (*set_bit)(int,int,char,void*)) {
	int i,j;
	for (i = 0 ; i < self->rows ; ++i)
		for (j = 0 ; j < self->cols ; ++j)
			set_bit(i,j,BIT(self,i,j),obj);
}

// rop := S/R = -(-S*R^)
void mat_left_residue (Mat * rop, const Mat * S, const Mat * R)
{
	Mat * nS = mat_new (1,1), *Rt = mat_new (1,1);
	mat_negate (nS, S);
	mat_transpose (Rt, R);
	mat_mult (rop, nS, Rt);
	mat_negate (rop, rop);
	mat_destroy (nS);
	mat_destroy (Rt);
}

extern void dump_mat(FILE*,Mat*,const char*);

// rop := R\S = -(R^-S)
void mat_right_residue (Mat * rop, const Mat * R, const Mat * S)
{
	Mat * Rt = mat_new (1,1), *nS = mat_new (1,1);
	mat_negate (nS, S);
	mat_transpose (Rt, R);
	mat_mult (rop, Rt, nS);
	mat_negate (rop, rop);
	mat_destroy (Rt);
	mat_destroy (nS);
}

// rop := syq(R,S) = (R\S) & (R^/S^) = -(R^*-S) & -(-R^*S)
void mat_symm_quotient (Mat * rop, const Mat * R, const Mat * S)
{
	Mat * Rt = mat_tmp(), *St = mat_tmp();
	Mat * X = mat_tmp(), *Y = mat_tmp();

	mat_transpose (Rt, R);
	mat_transpose (St, S);

	mat_left_residue(X, Rt, St);
	mat_right_residue (Y, R, S);

	mat_destroy (Rt);
	mat_destroy (St);

	mat_and (rop, X, Y);
	mat_destroy (X);
	mat_destroy (Y);
}

// arg1 and arg2 need to have to same number of rows.
void mat_tupling (Mat * rop, const Mat * arg1, const Mat * arg2)
{
	int rows = arg1->rows;
	int i,j,k;

	assert (arg1->rows == arg2->rows);
	mat_resize (rop, rows, arg1->cols * arg2->cols);

	for (i = 0 ; i < rows ; ++i) {
		for (j = 0 ; j < arg1->cols ; ++j)
			for (k = 0 ; k < arg2->cols ; ++k)
				BIT(rop,i,j*arg2->cols+k) = BIT(arg1,i,j) && BIT(arg2,i,k);
	}
}

// arg1 and arg2 need to have to same number of rows.
void mat_right_tupling (Mat * rop, const Mat * arg1, const Mat * arg2)
{
	mat_tupling (rop,arg1,arg2);
}


// arg1 and arg2 need to have to same number of columns.
void mat_left_tupling (Mat * rop, const Mat * arg1, const Mat * arg2)
{
	int cols = arg1->cols;
	int i,j,k;

	assert (arg1->cols == arg2->cols);
	mat_resize (rop, arg1->rows * arg2->rows, cols);

	for (i = 0 ; i < arg1->rows ; ++i) {
		for (j = 0; j < arg2->rows ; ++j) {
			for (k = 0 ; k < cols ; ++k) {
				BIT(rop,i*arg2->rows+j,k) = BIT(arg1,i,k) && BIT(arg2,j,k);
			}
		}
	}
}


void mat_direct_sum (Mat * rop, const Mat * arg1, const Mat * arg2)
{
	int cols = arg1->cols;
	int i,j;

	assert (arg1->cols == arg2->cols);
	mat_resize (rop, arg1->rows + arg2->rows, cols);

	for (i = 0 ; i < arg1->rows ; ++i)
		for (j = 0 ; j < cols ; ++j)
			BIT(rop,i,j) = BIT(arg1,i,j);

	for (i = 0 ; i < arg2->rows ; ++i)
		for (j = 0 ; j < cols ; ++j)
			BIT(rop,i+arg1->rows,j) = BIT(arg2,i,j);
}

// See http://gurmeet.net/puzzles/fast-bit-counting-routines/
#define TWO(c)     (0x1u << (c))
#define MASK(c) \
  (((unsigned int)(-1)) / (TWO(TWO(c)) + 1u))
#define COUNT(x,c) \
  ((x) & MASK(c)) + (((x) >> (TWO(c))) & MASK(c))

#ifndef SIZEOF_INT
# error "SIZEOF_INT undefined"
#endif

static int bitcount (unsigned int n)  {
   n = COUNT(n, 0) ;
   n = COUNT(n, 1) ;
   n = COUNT(n, 2) ;
   n = COUNT(n, 3) ;
   n = COUNT(n, 4) ;
#if SIZEOF_INT == 8
   n = COUNT(n, 5) ;    /* for 64-bit integers */
#endif
   return n ;
}


// Result has dim. 2^n x 2^n
void mat_less_card (Mat * rop, int n)
{
	int rows = 1<<n;
	int i,j;

	assert (n <= sizeof(int)*8-1/*sign*/);

	mat_resize (rop, rows, rows);
	for (i = 0 ; i < rows ; ++i)
		for (j = 0 ; j < rows ; ++j)
			BIT(rop,i,j) = (bitcount(i) <= bitcount(j));
}


void mat_refl_hull (Mat * rop, const Mat * arg)
{
	if (rop != arg)
		mat_assign (rop, arg);

	mat_overlay (rop, _I, NULL);
}

//static char _O(int i, int j, void * dummy) { return 0; }
static char _symm (int i, int j,  void * user_data) {
	Mat * mat = (Mat*) user_data;
	return BIT(mat,i,j) || BIT(mat, j, i);
}

void mat_symm_hull (Mat * rop, const Mat * arg)
{
	assert (arg->cols == arg->rows);

	if (rop != arg)
		mat_assign (rop, arg);

	mat_overlay (rop, _symm, rop);
}

void mat_trans_hull (Mat * rop, const Mat * arg)
{
	int i;
	Mat * tmp = mat_new (1,1);

	assert (arg->cols == arg->rows);
	if (rop != arg)
		mat_assign (rop, arg);

	for (i = 0 ; i < rop->rows+1 ; i ++) {
		mat_mult (tmp,rop,rop);
		mat_or (rop,tmp,rop);
	}
	mat_destroy(tmp);
}

/* Holds R_xy and R_xz => R_yz) ? */
char mat_is_univalent (Mat * self)
{
	int i;

	for (i = 0 ; i < self->rows ; ++i) {
		int ents = 0, j;

		for (j = 0 ; j < self->cols ; ++j)
			if (BIT(self,i,j)) ents ++;

		if (ents > 1)
			return FALSE;
	}
	return TRUE;
}

void mat_partial_funcs (Mat * rop, int size1, int size2)
{
	int i,j,k,col,factor = 1;

	mat_resize (rop, size1 * size2, (int)pow(size2+1, size1));
	mat_O(rop);

	for (i = 0 ; i < size1 ; ++i) {
		for (j = 0 ; j < size2 ; ++j) {
			for (col = (j+1)*factor ; col < rop->cols ; col += factor * (size2+1))
				for (k = 0 ; k < factor ; ++k) {
					int x = k+col;
					int y = j+(i*size2);
					BIT(rop,y,x) = 1;
				}
		}
		factor *= size2+1;
	}
}

void mat_total_funcs (Mat * rop, int size1, int size2)
{
	int i,j,k,col,factor = 1;

	mat_resize (rop, size1 * size2, (int)pow(size2, size1));
	mat_O(rop);

	for (i = 0 ; i < size1 ; ++i) {
		for (j = 0 ; j < size2 ; ++j) {
			for (col = j*factor ; col < rop->cols ; col += factor * size2)
				for (k = 0 ; k < factor ; ++k) {
					int x = k+col;
					int y = j+(i*size2);
					BIT(rop,y,x) = 1;
				}
		}
		factor *= size2;
	}
}

void mat_product_order (Mat * rop, const Mat * arg1, const Mat * arg2)
{
	Mat * pi = mat_new(1,1), *rho = mat_new(1,1),
			*X = mat_new(1,1), *Y = mat_new(1,1);

	assert (arg1->cols == arg1->rows);
	assert (arg2->cols == arg2->rows);

	// X := pi * arg1 * pi^
	mat_p_1 (pi, arg1->rows, arg2->rows);
	mat_mult_norm_transp(X, arg1, pi);
	mat_mult(X, pi, X);

	// Y = rho * arg2 * rho^
	mat_p_2 (rho, arg1->rows, arg2->rows);
	mat_mult_norm_transp(Y, arg2, rho);
	mat_mult(Y,rho,Y);

	mat_destroy (pi);
	mat_destroy (rho);

	mat_and(rop, X,Y);

	mat_destroy(Y);
	mat_destroy (X);
}

void mat_sum_order (Mat * rop, const Mat * arg1, const Mat * arg2)
{
	Mat * iota = mat_new(1,1), *kappa = mat_new(1,1),
			*X = mat_new(1,1), *Y = mat_new(1,1);

	assert (arg1->cols == arg1->rows);
	assert (arg2->cols == arg2->rows);

	// X := iota^ * arg1 * iota
	mat_i_1 (iota, arg1->rows, arg2->rows);
	mat_mult_transp_norm(X, iota, arg1);
	mat_mult(X, X, iota);

	// Y = kappa^ * arg2 * kappa
	mat_i_2 (kappa, arg1->rows, arg2->rows);
	mat_mult_transp_norm(Y, kappa, arg2);
	mat_mult(Y,Y,kappa);

	mat_destroy (iota);
	mat_destroy (kappa);

	mat_or(rop, X,Y);

	mat_destroy(Y);
	mat_destroy (X);
}
