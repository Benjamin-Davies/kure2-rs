/*
 * Mat.h
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

#ifndef MAT_H_
#define MAT_H_

#include <stdio.h> // FILE

typedef struct _Mat
{
  int rows, cols;
  char * data;
} Mat;

#define mat_tmp() mat_new(1,1)

Mat *mat_new(int rows, int cols);
int mat_same_dim(const Mat *a, const Mat *b);
void mat_resize(Mat *self, int rows, int cols);
void mat_bin_op(Mat *rop, const Mat *a, const Mat *b, char (*f)(char, char));
void mat_and(Mat *rop, const Mat *a, const Mat *b);
void mat_or(Mat *rop, const Mat *a, const Mat *b);
void mat_transpose(Mat *rop, const Mat *a);
void mat_negate(Mat *rop, const Mat *a);
void mat_dtor(Mat *self);
void mat_destroy(Mat *self);
void mat_swap(Mat *a, Mat *b);
void mat_mult(Mat *rop, const Mat *a, const Mat *b);
void mat_mult_norm_transp(Mat *rop, const Mat *a, const Mat *b);
void mat_mult_transp_norm(Mat *rop, const Mat *a, const Mat *b);
void mat_mult_transp_transp(Mat *rop, const Mat *a, const Mat *b);
void mat_overlay (Mat * self, char (*f) (int i, int j, void*), void * user_data);
void mat_fill(Mat *self, char (*f)(int i, int j, void *), void *user_data);
void mat_L(Mat *self);
void mat_O(Mat *self);
void mat_I(Mat *self);
void mat_random(Mat *self, float p);
void mat_random_full(Mat *self, float p, int rows, int cols);
char mat_equals(const Mat *a, const Mat *b);
char mat_incl(const Mat *a, const Mat *b);
char mat_is_empty (Mat * self);
char mat_getbit(Mat *self, int i, int j);
void mat_setbit(Mat *self, int i, int j, char yesno);
void mat_assign(Mat *rop, const Mat *a);
Mat *mat_clone(const Mat *self);
void mat_epsi(Mat *self, int n);
void mat_p_1(Mat *self, int size1, int size2);
void mat_p_2(Mat *self, int size1, int size2);
void mat_i_1(Mat *self, int size1, int size2);
void mat_i_2(Mat *self, int size1, int size2);
char mat_is_row_compl(Mat *self, int row);
char mat_is_row_consistent(Mat *self, int row);
char mat_is_vec(Mat *self);
void mat_fill_row(Mat *self, int row);
void mat_set_row (Mat *self, char yesno, int i);
void mat_vec_random(Mat *self, float p);
void mat_vec_random_full(Mat *self, float p, int rows, int cols);
void mat_vec_begin (Mat * self);
void mat_vec_next (Mat * self);
int mat_vec_num_entries(Mat *self);
void mat_inj(Mat *rop, Mat *vec);
void mat_dump_full(Mat *self, FILE *s, char one_ch, char zero_ch, int max_ents);
void mat_dump(Mat *self);
char mat_cmp(const Mat *self, void *obj, char (*get_bit)(int, int, void *));
void mat_set_by_func(Mat *self, void *obj, char (*get_bit)(int, int, void *));
void mat_get_by_func(const Mat *self, void *obj, void (*set_bit)(int, int, char, void *));

void mat_left_residue (Mat * rop, const Mat * S, const Mat * R);
void mat_right_residue (Mat * rop, const Mat * R, const Mat * S);
void mat_symm_quotient (Mat * rop, const Mat * R, const Mat * S);
void mat_tupling (Mat * rop, const Mat * arg1, const Mat * arg2);
void mat_left_tupling (Mat * rop, const Mat * arg1, const Mat * arg2);
void mat_right_tupling (Mat * rop, const Mat * arg1, const Mat * arg2);
void mat_direct_sum (Mat * rop, const Mat * arg1, const Mat * arg2);

/* Note: This is actually a less-or-equal comparison. */
void mat_less_card (Mat * rop, int n);

void mat_refl_hull (Mat * rop, const Mat * arg);
void mat_symm_hull (Mat * rop, const Mat * arg);
void mat_trans_hull (Mat * rop, const Mat * arg);

char mat_is_univalent (Mat * self);

void mat_partial_funcs (Mat * rop, int size1, int size2);
void mat_total_funcs (Mat * rop, int size1, int size2);

void mat_product_order (Mat * rop, const Mat * arg1, const Mat * arg2);
void mat_sum_order (Mat * rop, const Mat * arg1, const Mat * arg2);

#endif /* MAT_H_ */
