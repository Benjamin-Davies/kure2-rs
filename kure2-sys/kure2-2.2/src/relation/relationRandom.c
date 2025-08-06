#include "KureImpl.h"


/****************************************************************************/
/* NAME: random_rel                                                         */
/* FUNKTION: fuellt eine Relation zu x%                                     */
/* UEBERGABEPARAMETER: Rel * (die zu fuellende Relation), int (%-Wert)      */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 28.01.2001                                                           */
/* LETZTE AENDERUNG: Benutzung des verbesserten random_bdd                  */
/****************************************************************************/
void random_rel_full(KureRel * rop, mpz_t rows, mpz_t cols, float b,
		KureRandomFunc random_func, void * user_data)
{
	if (rop && mpz_cmp_si(rows,0)>0 && mpz_cmp_si(cols,0)>0
			&& b>=.0f && b<=100.0f)
	{
		if ( !mpz_fits_sint_p(rows) || !mpz_fits_sint_p(cols)) {
			printf ("random_rel_full: Relation is too big. Max rows/cols "
					"is %d.\n", INT_MAX);
		}
		else {
			DdManager * manager = rop->context->manager;
			DdNode * tmp = random_bdd (rop->context, mpz_get_si(rows),
					mpz_get_si(cols), b, random_func, user_data);
			if (tmp) {
				Cudd_Ref(tmp);
				Cudd_RecursiveDeref(manager, rop->bdd);
				rop->bdd = tmp;
				mpz_set (rop->rows, rows);
				mpz_set (rop->cols, cols);
			}
		}
	}
}

void random_rel (KureRel * rop, float b, KureRandomFunc random_func, void * user_data)
{
	random_rel_full (rop, rop->rows, rop->cols, b, random_func, user_data);
}


/****************************************************************************/
/* NAME: rvRelationRandomCF                                                 */
/* FUNKTION: fuellt eine Relation zu x% mit einem kreiskreien Graphen       */
/* UEBERGABEPARAMETER: Rel * (die zu fuellende Relation), int (%-Wert)      */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 08-JAN-2002                                                          */
/****************************************************************************/
// Returns a quadratic relation of the given size.
void rvRelationRandomCF_full(KureRel * rop, mpz_t size, float b,
		KureRandomFunc random_func, void * user_data)
{
	if (rop && mpz_cmp_si(size,0)>0	&& b>=.0f && b<=100.0f)
	{
		if ( !mpz_fits_sint_p(size)) {
			printf ("rvRelationRandomCF_full: Relation is too big. Max size "
					"is %d.\n", INT_MAX);
		}
		else {
			DdManager * manager = rop->context->manager;
			DdNode * tmp = rvBddRandomCF (rop->context, mpz_get_si(size),
					b, random_func, user_data);
			if (tmp) {
				Cudd_Ref(tmp);
				Cudd_RecursiveDeref(manager, rop->bdd);
				rop->bdd = tmp;
				mpz_set (rop->rows, size);
				mpz_set (rop->cols, size);
			}
		}
	}
}

void rvRelationRandomCF(KureRel * rop, float b, KureRandomFunc random_func,
		void * user_data)
{
	if (rop && mpz_cmp(rop->cols, rop->rows) == 0)
		rvRelationRandomCF_full (rop, rop->rows, b, random_func, user_data);
}


/*!
 * Fisher–Yates shuffle (see Wikipedia)
 */
static void _shuffle (int * array, size_t n, KureRandomFunc random_func, void * user_data)
{
	if (n > 1) {
		size_t i;
		for (i = 0; i < n; i++) {
			float r = random_func(user_data); /* in [0,1] */
			size_t j = r*i;

			if (j != i) {
				int t = array[j];
				array[j] = array[i];
				array[i] = t;
			}
		}
	}
}

/****************************************************************************/
/* NAME: rvRelationRandomPerm                                               */
/* FUNKTION: fuellte eine quadratische Relation mit einer Permutation       */
/* UEBERGABEPARAMETER: Rel * (die zu fuellende Relation),                   */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 20-FEB-2002                                                          */
/****************************************************************************/
void rvRelationRandomPerm_full(KureRel * rop, mpz_t size,
		KureRandomFunc random_func, void * user_data)
{
	if (rop && mpz_cmp_si(size,0) > 0) {
		if ( !mpz_fits_sint_p(size)) {
			printf ("rvRelationRandomPerm_full: Relation is too big. Max size "
					"is %d.\n", INT_MAX);
		}
		else {
#if 1
			/* Use simpler shuffle algorithm. */
			int n = mpz_get_si(size), i;
			int * perm = KURE_NEW(int, n);

			kure_O (rop);
			kure_rel_set_size (rop, size, size);

			for (i = 0 ; i < n ; i ++)
				perm[i] = i;

			_shuffle (perm, n, random_func, user_data);

			for (i = 0 ; i < n ; i ++)
				kure_set_bit_si (rop, TRUE, i, perm[i]);
#else
			DdManager * manager = rop->context->manager;
			DdNode * tmp = rvBddRandomPerm (rop->context, mpz_get_si(size),
					random_func, user_data);
			if (tmp) {
				Cudd_Ref(tmp);
				Cudd_RecursiveDeref(manager, rop->bdd);
				rop->bdd = tmp;
				mpz_set (rop->rows, size);
				mpz_set (rop->cols, size);
			}
#endif
		}
	}
}

void rvRelationRandomPerm(KureRel * rop, KureRandomFunc random_func,
		void * user_data)
{
	if (rop && mpz_cmp(rop->cols, rop->rows) == 0)
		rvRelationRandomPerm_full (rop, rop->rows, random_func, user_data);
}

