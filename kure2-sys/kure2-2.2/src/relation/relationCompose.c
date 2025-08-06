#include "KureImpl.h"


/****************************************************************************/
/* NAME: mult_rel                                                           */
/* FUNKTION: Komposition zweier Relationen                                  */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 02.08.1999 (Umstellung auf BDD-Version)                                  */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// rop := arg1 * arg2
// cols(arg1) must be equal to rows(arg2)
void mult_rel (KureRel * arg1, KureRel * arg2, KureRel * rop)
{
	if ( !gmp_equal(arg1->cols, arg2->rows)) {
		printf ("ERROR (mult_rel): Dimensions do not match\n");
	}
	else {
		DdManager * manager = arg1->context->manager;
		DdNode * res = komp_bdd (manager, arg1->bdd, arg2->bdd, arg1->rows, arg1->cols);
		if (res) {
			Cudd_Ref (res);
			Cudd_RecursiveDeref(manager, rop->bdd);
			rop->bdd = res;
			mpz_set (rop->rows, arg1->rows);
			mpz_set (rop->cols, arg2->cols);
		}
	}
}


/****************************************************************************/
/* NAME: mult_rel_transp_norm                                               */
/* FUNKTION: Komposition zweier Relationen (1. transponiert, 2. nicht)      */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 24.01.2000                                                           */
/* LETZTE AENDERUNG AM: 24.01.2000                                          */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// rop := arg1^*arg2
void mult_rel_transp_norm (KureRel * arg1, KureRel * arg2, KureRel * rop)
{
	if ( !gmp_equal(arg1->rows, arg2->rows)) {
	      printf ("ERROR (mult_rel_transp_norm): Dimensions do not match.\n");
	}
	else {
		DdManager * manager = arg1->context->manager;
		DdNode * res = komp_bdd_transp_norm (manager, arg1->bdd, arg2->bdd, arg1->rows, arg1->cols);
		if (res) {
			Cudd_Ref (res);
			Cudd_RecursiveDeref(manager, rop->bdd);
			rop->bdd = res;
			mpz_set (rop->rows, arg1->cols);
			mpz_set (rop->cols, arg2->cols);
		}
	}
}


/****************************************************************************/
/* NAME: mult_rel_transp_transp                                             */
/* FUNKTION: Komposition zweier Relationen (beide transponiert)             */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 24.01.2000                                                           */
/* LETZTE AENDERUNG AM: 24.01.2000                                          */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// rop := arg1^ * arg2^
void mult_rel_transp_transp (KureRel * arg1, KureRel * arg2, KureRel * rop)
{
	if ( !gmp_equal(arg1->rows, arg2->cols)) {
		printf ("ERROR (mult_rel_transp_transp): Dimensions do not match.\n");
	} else {
		DdManager * manager = arg1->context->manager;
		DdNode * res = komp_bdd_transp_transp (manager, arg1->bdd, arg2->bdd,
				arg1->rows, arg2->rows, arg1->cols);
		if (res) {
			Cudd_Ref (res);
			Cudd_RecursiveDeref(manager, rop->bdd);
			rop->bdd = res;
			if (rop == arg1 && rop == arg2)
				mpz_swap (rop->rows, rop->cols);
			else {
				mpz_set (rop->rows, arg1->cols);
				mpz_set (rop->cols, arg2->rows);
			}
		}
	}
}


/****************************************************************************/
/* NAME: mult_rel_norm_transp                                               */
/* FUNKTION: Komposition zweier Relationen (nur die zweite transponiert)    */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 24.01.2000                                                           */
/* LETZTE AENDERUNG AM: 24.01.2000                                          */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// rop := arg1 * arg2^
void mult_rel_norm_transp (KureRel * arg1, KureRel * arg2, KureRel * rop)
{
	if ( !gmp_equal(arg1->cols, arg2->cols)) {
		printf ("ERROR (mult_rel_transp_transp): Dimensions do not match\n");
	} else {
		DdManager * manager = arg1->context->manager;
		DdNode * res = komp_bdd_norm_transp (manager, arg1->bdd, arg2->bdd,
				arg1->rows, arg2->rows, arg2->cols);
		if (res) {
			Cudd_Ref (res);
			Cudd_RecursiveDeref(manager, rop->bdd);
			rop->bdd = res;
			// Order of assignments is mandatory.
			mpz_set (rop->cols, arg2->rows);
			mpz_set (rop->rows, arg1->rows);
		}
	}
}
