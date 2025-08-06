#include "KureImpl.h"

/****************************************************************************/
/* NAME: epsi_rel                                                           */
/* FUNKTION: berechnet die epsi-Relation                                    */
/* UEBERGABEPARAMETER: 2 x Rel * (Relationen), Argument                     */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 05.08.1999 (Umstellung auf BDD-Version)                                  */
/* 13.04.2000 (Benutzung vonb build_bdd)                                    */
/* 01.08.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
/* Result will have n rows and 2^n columns. */
void epsi_rel (KureRel * rop, int n)
{
	if (rop && n > 0) {
		DdManager * manager = rop->context->manager;
		/* The variables for the exponential part have to be offset by the
		 * number of variables necessary to encode the domain part with
		 * n rows. */
		int index = number_of_vars2 (n);
		int i;
		DdNode ** row_bdds, *res;

		//printf ("index for n=%d: %d\n", n,index);

		row_bdds = (DdNode**) malloc (sizeof(DdNode*) * (n+10));
		for (i = 0 ; i < n ; i ++) {
			row_bdds[i] = Cudd_bddIthVar(manager, index+i);
			Cudd_Ref (row_bdds[i]);
		}

		res = build_bdd(manager,row_bdds,n);
		Cudd_Ref (res);
		Cudd_RecursiveDeref(manager, rop->bdd);
		rop->bdd = res;
		mpz_set_si(rop->rows, n);
		mpz_ui_pow_ui (rop->cols, 2, n);

		// The nodes in row_bdds are deref'd by build_bdd.
	}
}

/****************************************************************************/
/* NAME: paf_rel                                                            */
/* FUNKTION: (?)                                                            */
/* UEBERGABEPARAMETER: 3  x Rel * (Relationen), 1. + 2. Argument            */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 06.08.1999 (Umstellung auf BDD-Version)                                  */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// Input: Two sizes.
// Output: A relation with size1*size2 rows and (size2+1)^size1 columns.
void paf_rel (KureRel * rop, int size1, mpz_t size2)
{
	mpz_t size2_1; // size2 + 1

	mpz_init (size2_1);
	mpz_add_ui (size2_1, size2, 1);

	null_rel(rop);
	mpz_mul_si (rop->rows, size2, size1); // rows := size1 * size2
	mpz_pow_ui (rop->cols, size2_1, size1); // cols = (size2+1)^size1

	null_rel(rop);

	/* The relation is built bit by bit! */
	{
		int i;
		mpz_t j,k;
		mpz_t factor, y, x;
		mpz_t col;

		mpz_init(j);		mpz_init(k);
		mpz_init_set_si(factor, 1);
		mpz_init(x);	mpz_init(y);
		mpz_init (col);

		for (i=0 ; i<size1 ; ++i)
		{
			mpz_set_si (j, 0);
			for ( ; gmp_less(j, size2) ; mpz_add_ui(j,j,1))
			{
				mpz_add_ui (x, j, 1);
				mpz_mul (col, factor, x); // col = (j+1)*factor
				while (gmp_less(col, rop->cols))
				{
					mpz_set_si (k,0);
					for ( ; gmp_less(k,factor) ; mpz_add_ui(k,k,1))
					{
						mpz_add (x, k, col); // Column to set (k+col)
						mpz_mul_si (y, size2, i);
						mpz_add (y, y, j); // Row to set (j + i*size2)
						mp_set_rel_bit (rop, y, x);
					}
					// col := col + factor * (size2+1)
					mpz_addmul(col, factor, size2_1);
				}
			}
		    // factor := factor * (size2+1)
		    mpz_mul(factor, factor, size2_1);
		}

		mpz_clear(j);		mpz_clear(k);
		mpz_clear(factor);
		mpz_clear(x);		mpz_clear(y);
		mpz_clear(col);
	}

	mpz_clear(size2_1);
}


/****************************************************************************/
/* NAME: totf_rel                                                           */
/* FUNKTION:                                                                */
/* UEBERGABEPARAMETER: 3  x Rel * (Relationen), 1. + 2. Argument            */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 06.08.1999 (Umstellung auf BDD-Version)                                  */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// Input: Two sizes
// Output: cols=size2^size1, rows=size1*size2
void totf_rel (KureRel * rop, int size1, mpz_t size2)
{
	null_rel(rop);
	mpz_mul_si (rop->rows, size2, size1); // rows := size1 * size2
	mpz_pow_ui (rop->cols, size2, size1); /* cols = (size2)^size1 */

	/* The relation is built bit by bit! */
	{
		int i;
		mpz_t j,k;
		mpz_t factor, x, y;
		mpz_t col;

		mpz_init(j);		mpz_init(k);
		mpz_init_set_si(factor, 1);
		mpz_init(x);		mpz_init(y);
		mpz_init (col);

		for (i = 0 ; i < size1 ; ++i)
		{
			mpz_set_si (j, 0);
			for ( ; gmp_less(j, size2) ; mpz_add_ui(j,j,1))
			{
				mpz_mul (col, factor, j); // col = j*factor
				while (gmp_less(col, rop->cols))
				{
					mpz_set_si (k,0);
					for ( ; gmp_less(k,factor) ; mpz_add_ui(k,k,1))
					{
						mpz_add (x, k, col); // Column to set (k+col)
						mpz_mul_ui (y, size2, i);
						mpz_add (y, y, j); // Row to set (j + i*size2)
						mp_set_rel_bit (rop, y, x);
					}
					// col := col + factor * size2
					mpz_addmul(col, factor, size2);
				}
			}
			// factor := factor * size2
			mpz_mul(factor, factor, size2);
		}

		mpz_clear(j);		mpz_clear(k);
		mpz_clear(factor);
		mpz_clear(x);		mpz_clear(y);
		mpz_clear(col);
	}
}
