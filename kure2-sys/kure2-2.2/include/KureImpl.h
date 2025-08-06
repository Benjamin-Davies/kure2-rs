/*
 * KureImpl.h
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

#ifndef KUREIMPL_H_
#define KUREIMPL_H_

#include "Kure.h"
//#include "bddInt.h"
#include <limits.h>

#ifndef MIN
# define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
# define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define KURE_NEW0(type,n) ((type*)calloc((n),sizeof(type)))
#define KURE_NEW(type,n) ((type*)malloc((n)*sizeof(type)))


struct _KureContext
{
	DdManager * manager;
	// TODO: Caches

	KureError * error;

	KureRandomFunc random_func;
	void * random_udata;

	int refs;
};

struct _KureRel
{
	KureContext * context;
	DdNode * bdd;
	mpz_t rows, cols;
};


/*******************************************************************************
 *                              Domains and Types                              *
 *                                                                             *
 ******************************************************************************/

/*!
 * If you plan to add new elements here, you have to
 *    - You have to implement these function.
 *    - You have to add the functions to the existing classes (see KureDom.c)
 *
 * You should always add elements at the end.
 */
typedef struct _KureDomClass
{
#define THIS KureDom*
	const char * name;
	KureDom* (*new_copy) (const THIS);
	void (*destroy) (THIS);
	Kure_bool (*get_comp) (const THIS, mpz_t, int);
	int (*get_comp_count) (const THIS);
#undef THIS
} KureDomClass;

/*!
 * From a relational point of view, domains are types, or to be more precisely,
 * type constructors like direct products and direct sums.
 */
/*interface*/ struct _KureDom
{
	KureDomClass * c;
};

typedef struct _KureDirectProduct
{
	KureDomClass * c;
	mpz_t first, second;
} KureDirectProduct;

typedef struct _KureDirectSum
{
	KureDomClass * c;
	mpz_t first, second;
} KureDirectSum;

#define _KURE_DOM_TEST_CLASS(dom,c) \
	((Kure_bool) (kure_dom_get_class((KureDom*)(dom)) == kure_##c##_get_class()))

#define KURE_DOM_IS_DIRECT_PRODUCT(dom) _KURE_DOM_TEST_CLASS(dom,direct_product)
#define KURE_DOM_IS_DIRECT_SUM(dom) _KURE_DOM_TEST_CLASS(dom,direct_sum)

const char * 			kure_dom_get_name (const KureDom*);
const KureDomClass * 	kure_dom_get_class (const KureDom*);
const KureDomClass * 	kure_direct_product_get_class ();
const KureDomClass * 	kure_direct_sum_get_class ();


/*******************************************************************************
 *                                  Language                                   *
 *                                                                             *
 ******************************************************************************/

KureRel ** _kure_lua_rel_placeholder (lua_State * L);
KureRel * _kure_lua_rel_new (lua_State * L, KureContext * context);
KureRel * _kure_lua_torel (lua_State * L, int pos, KureError ** perr);

KureDom ** _kure_lua_dom_placeholder (lua_State * L);

/* KureLuaBinding.c */
void luaopen_kure (lua_State * L);

#define Kure_num_vars(n) kure_num_vars(n)
int kure_num_vars (const mpz_t n);

#define KURE_INLINE

/* a<(=)b? */
#define gmp_less(a,b) (mpz_cmp((a),(b))<0)
#define gmp_leq(a,b) (mpz_cmp((a),(b))<=0)
/* a>(=)b? */
#define gmp_greater(a,b)  (mpz_cmp((a),(b))>0)
#define gmp_geq(a,b) (mpz_cmp((a),(b))>=0)

#define gmp_equal(a,b) (mpz_cmp((a),(b)) == 0)

//mpz_divisible_2exp_p((a), mpz_sizeinbase((a),2))
/* Test whether there is just one bit set. */
#define gmp_is_pow2(a) (mpz_popcount(a) == 1)

#define Kure_is_odd(x) ((x)%2)
#define Kure_is_even(x) (!Kure_is_odd(x))

#define Kure_assert_same_context3(a,b,c) ;
#define Kure_assert_same_context2(a,b) ;


/* bdd/bdd.c */
DdNode *build_bdd(DdManager *manager, DdNode **array, int number_of_bdds);

/* bdd/bddCompose.c */
DdNode *komp_bdd(DdManager *manager, DdNode *first_bdd, DdNode *second_bdd, mpz_t ROWS, mpz_t COLS);
DdNode *komp_bdd_transp_norm(DdManager *manager, DdNode *first_bdd, DdNode *second_bdd, mpz_t ROWS, mpz_t COLUMNS);
DdNode *komp_bdd_norm_transp(DdManager *manager, DdNode *first_bdd, DdNode *second_bdd, mpz_t FIRST_ROWS, mpz_t SECOND_ROWS, mpz_t COLUMNS);
DdNode *komp_bdd_transp_transp(DdManager *manager, DdNode *first_bdd, DdNode *second_bdd, mpz_t FIRST_ROWS, mpz_t SECOND_ROWS, mpz_t COLUMNS);
/* bdd/bddDivers.c */
int number_of_vars2(unsigned long n);
Kure_bool int_is_pow2(unsigned long n);
/* bdd/bddFileops.c */
DdNode *load_bdd(DdManager *manager, FILE *fp, unsigned int num_of_var);
Kure_bool store_bdd(DdManager *manager, FILE *fp, DdNode *bdd);
/* bdd/bddFind.c */
DdNode *find_row_in_bdd(DdNode *rel, int number_vars, mpz_t row);
DdNode *find_column_in_bdd(DdNode *rel, int number_vars_rows, int number_vars_cols, mpz_t col);
/* bdd/bddMinterm.c */
DdNode *make_minterm(DdManager *manager, mpz_t width, mpz_t height, mpz_t _col, mpz_t _row);
DdNode *make_minterm_si(DdManager *manager, mpz_t width, mpz_t height, int col, int row);
int *first_fulfill_intlist(DdManager *manager, DdNode *bdd, int nvars);
DdNode *rvIntlist2Bdd(DdManager *manager, int *indices, int *assign, int vars);
void rvBddCountMinterm(DdManager *manager, DdNode *f, int nvars, mpz_t ret);
/* bdd/bddRandom.c */
DdNode *random_bdd(KureContext *context, int lines, int columns, float prob, KureRandomFunc random_func, void *user_data);
DdNode *rvBddRandomCF(KureContext *context, int lines, float prob, KureRandomFunc random_func, void *user_data);
DdNode *rvBddRandomPerm(KureContext *context, int lines, KureRandomFunc random_func, void *user_data);
/* bdd/bddRelation.c */
DdNode *L_bdd_cache(DdManager *table, DdNode *f);
DdNode *L_bdd(KureContext *context, mpz_t rows, mpz_t cols);
DdNode *L_row_bdd_cache(DdManager *table, DdNode *f);
DdNode *L_row_bdd(KureContext *context, mpz_t number_of_ones, int index_of_root);
DdNode *trans_bdd(KureContext *context, DdNode *bdd, mpz_t rows, mpz_t cols);
DdNode *neg_bdd(KureContext *context, DdNode *bdd, mpz_t rows, mpz_t cols);
DdNode *ident_bdd_cache(DdManager *table, DdNode *f);
DdNode *ident_bdd(KureContext *context, mpz_t rows);
DdNode *next_bdd_cache(DdManager *table, DdNode *f);
DdNode *next_bdd(KureContext *context, mpz_t rows);
DdNode *inj1_bdd_cache(DdManager *table, DdNode *f);
DdNode *inj1_bdd(KureContext *context, mpz_t rows, mpz_t cols);
DdNode *inj2_bdd_cache(DdManager *table, DdNode *f);
DdNode *inj2_bdd(KureContext *context, mpz_t rows, mpz_t cols);
DdNode *pro1_bdd_cache(DdManager *table, DdNode *f);
DdNode *pro1_bdd(KureContext *context, mpz_t rows, mpz_t cols);
DdNode *pro2_bdd_cache(DdManager *table, DdNode *f);
DdNode *pro2_bdd(KureContext *context, mpz_t rows, mpz_t cols);
DdNode *null_bdd(DdManager *manager);
DdNode *or_bdd(DdManager *manager, DdNode *f, DdNode *g);
DdNode *and_bdd(DdManager *manager, DdNode *f, DdNode *g);
DdNode *lesscard_bdd(KureContext *context, int n);
DdNode *subsetvector_bdd(KureContext *context, int set_card, int subset_card);
DdNode *warshall_bdd(KureContext *context, DdNode *first_bdd, int rows);
/* bdd/bddShift.c */
DdNode *shift_bdd(DdManager *manager, DdNode *f, int border, int value_1, int value_2);
DdNode *shift_bdd_recur(DdManager *manager, DdNode *f, DdNode *reference_bdd, int border, int value_1, int value_2);

/* bdd/bddMinSets.c */
DdNode * minsets_upset (DdManager * manager, size_t nvars, DdNode * f);
DdNode * minsets (DdManager * manager, size_t nvars, DdNode * f);
DdNode * maxsets_downset (DdManager * manager, size_t nvars, DdNode * f);
DdNode * maxsets (DdManager * manager, size_t nvars, DdNode * f);
DdNode * empty_set (DdManager * manager, size_t top, size_t nvars);
DdNode * all_set (DdManager * manager, size_t top, size_t nvars);
Kure_bool contains_empty_set (DdManager * manager, DdNode * f);
Kure_bool contains_all_set (DdManager * manager, DdNode * f);
DdNode * not_sup_set (DdManager * manager, DdNode * f, DdNode * g);
DdNode * not_sub_set (DdManager * manager, DdNode * f, DdNode * g);

/* relation/relationBaseops.c */
void null_rel(KureRel *rel);
void L_rel(KureRel *rel);
void eins_rel(KureRel *rel);
void oder_rel(KureRel *arg1, KureRel *arg2, KureRel *rop);
void und_rel(KureRel *arg1, KureRel *arg2, KureRel *rop);
void transp_rel(KureRel *arg, KureRel *rop);
void kompl_rel(KureRel *arg, KureRel *rop);

/* relation/relationBits.c */
void mp_set_rel_bit(KureRel *rel, mpz_t row, mpz_t col);
void mp_clear_rel_bit(KureRel *rel, mpz_t row, mpz_t col);
Kure_bool mp_get_rel_bit_full(KureRel *rel, mpz_t row, mpz_t col, int vars_rows, int vars_cols);
Kure_bool mp_get_rel_bit(KureRel *rel, mpz_t row, mpz_t col);
Kure_bool get_rel_bit_full(KureRel *rel, int row, int col, int vars_rows, int vars_cols);
Kure_bool get_rel_bit(KureRel *rel, int row, int col);
void clear_rel_bit(KureRel *rel, int row, int col);
void set_rel_bit(KureRel *rel, int row, int col);
/* relation/relationCardinality.c */
void lesscard_rel(KureRel *vec, KureRel *rop);
Kure_bool subsetvector_rel(KureRel *pot, KureRel *vec, KureRel *rop);
void rvRelationCountColumns(KureRel *vec, mpz_t rop);
void rvRelationCountEntries(KureRel *rel, mpz_t rop);
/* relation/relationClosures.c */
void transh_rel_old(KureRel *arg, KureRel *rop);
void reflh_rel(KureRel *arg, KureRel *rop);
void symmh_rel(KureRel *arg, KureRel *rop);
void transh_rel(KureRel *arg, KureRel *rop);
/* relation/relationColumns.c */
Kure_bool set_rel_line(KureRel *rel, mpz_t _row);
Kure_bool clear_rel_line(KureRel *rel, mpz_t row);
/* relation/relationCompose.c */
void mult_rel(KureRel *arg1, KureRel *arg2, KureRel *rop);
void mult_rel_transp_norm(KureRel *arg1, KureRel *arg2, KureRel *rop);
void mult_rel_transp_transp(KureRel *arg1, KureRel *arg2, KureRel *rop);
void mult_rel_norm_transp(KureRel *arg1, KureRel *arg2, KureRel *rop);
/* relation/relationDomains.c */
void pro1_rel(KureRel *rop, mpz_t rows, mpz_t cols);
void pro2_rel(KureRel *rop, mpz_t rows, mpz_t cols);
void prord_rel(KureRel *arg1, KureRel *arg2, KureRel *rop);
void tup_P_rel(KureRel *arg1, KureRel *arg2, KureRel *rop);
void tupling_left_rel (KureRel * arg1, KureRel * arg2, KureRel * rop);
void inj1_rel(KureRel *rop, mpz_t rows, mpz_t cols);
void inj2_rel(KureRel *rop, mpz_t rows, mpz_t cols);
void suord_rel(KureRel *arg1, KureRel *arg2, KureRel *rop);
void tup_S_rel(KureRel *arg1, KureRel *arg2, KureRel *rop);
/* relation/relationPowersets.c */
void epsi_rel(KureRel *rop, int n);
void paf_rel(KureRel *rop, int size1, mpz_t size2);
void totf_rel(KureRel *rop, int size1, mpz_t size2);
/* relation/relationRandom.c */
void random_rel_full(KureRel *rop, mpz_t rows, mpz_t cols, float b, KureRandomFunc random_func, void *user_data);
void random_rel(KureRel *rop, float b, KureRandomFunc random_func, void *user_data);
void rvRelationRandomCF_full(KureRel *rop, mpz_t size, float b, KureRandomFunc random_func, void *user_data);
void rvRelationRandomCF(KureRel *rop, float b, KureRandomFunc random_func, void *user_data);
void rvRelationRandomPerm_full(KureRel *rop, mpz_t size, KureRandomFunc random_func, void *user_data);
void rvRelationRandomPerm(KureRel *rop, KureRandomFunc random_func, void *user_data);
/* relation/relationResidue.c */
void res_li_rel(KureRel *relS, KureRel *relR, KureRel *rop);
void res_re_rel(KureRel *relR, KureRel *relS, KureRel *rop);
void syq_rel(KureRel *relR, KureRel *relS, KureRel *rop);
/* relation/relationTests.c */
Kure_bool test_line_complete(KureRel *r, int x, int y, KureDirection direction);
Kure_bool test_on_empty(KureRel *rel);
Kure_bool test_on_equal(KureRel *rel1, KureRel *rel2);
Kure_bool test_on_inclusion(KureRel *rel1, KureRel *rel2);
Kure_bool test_on_eind(KureRel *rel);
Kure_bool line_completeness(KureRel *r);
/* relation/relationVectors.c */
void dom_rel(KureRel *relR, KureRel *rop);
void init_vector(KureRel *rop, mpz_t rows, mpz_t cols);
Kure_bool next_vector(KureRel *rel, KureRel *result);
void succ_vector(KureRel *rop, mpz_t size);
Kure_bool point(KureRel *arg, KureRel *rop);
Kure_bool atom(KureRel *arg, KureRel *rop);
Kure_bool inj_vector(KureRel *arg, KureRel *rop);


#endif /* KUREIMPL_H_ */
