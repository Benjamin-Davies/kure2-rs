#include "KureImpl.h"


/****************************************************************************/
/* NAME: pro1_rel                                                           */
/* FUNKTION: (?)                                                            */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 03.08.1999 (Umstellung auf BDD-Version)                                  */
/* 28.03.2000 (Benutzung einer eigenen BDD-Routine)                         */
/****************************************************************************/
void pro1_rel (KureRel * rop, mpz_t first, mpz_t second)
{
	mpz_t p;
	mpz_init (p);
	mpz_mul(p, first, second);

	Cudd_RecursiveDeref(rop->context->manager, rop->bdd);
	rop->bdd = pro1_bdd (rop->context, p, first);
	Cudd_Ref(rop->bdd);
	mpz_set (rop->rows, p);
	mpz_set (rop->cols, first);

	mpz_clear (p);
}


/****************************************************************************/
/* NAME: pro2_rel                                                           */
/* FUNKTION: (?)                                                            */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 03.08.1999 (Umstellung auf BDD-Version)            }                     */
/* 28.03.2000 (Benutzung einer eigenen BDD-Routine)                         */
/****************************************************************************/
void pro2_rel (KureRel * rop, mpz_t first, mpz_t second)
{
	mpz_t p;
	mpz_init (p);
	mpz_mul(p, first, second);

	Cudd_RecursiveDeref(rop->context->manager, rop->bdd);
	rop->bdd = pro2_bdd (rop->context, p, second);
	Cudd_Ref(rop->bdd);
	mpz_set (rop->rows, p);
	mpz_set (rop->cols, second);

	mpz_clear (p);
}


#if 0
/* Implementation is now on a higher level. See KureRel.c. */

/****************************************************************************/
/* NAME: prord_rel                                                          */
/* FUNKTION:                                                                */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 03.08.1999 (Umstellung auf BDD-Version)                                  */
/* 21.02.2000 (Ausnutzung der BDD-Routinen)                                 */
/* 13.04.2000 (Benutzung von pro1_bdd und pro2_bdd)                         */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// let prod=arg1->cols * arg2->cols. Then the result has $prod rows and
// $prod columns.
void prord_rel (KureRel * arg1, KureRel * arg2, KureRel * rop)
     /* rel3 muss ungleich rel1 und rel2 sein! ... Why? -stb*/
{
	KureContext * context = rop->context;
	DdManager * manager = context->manager;
	DdNode * pi1, *pi2;
	DdNode * tmp1, *tmp2, *tmp3;
	mpz_t prod;

	Kure_assert_same_context3(arg1,arg2,rop);

	mpz_init (prod);
	mpz_mul (prod, arg1->cols, arg2->cols);

	pi1 = pro1_bdd (context, arg1->cols, prod); Cudd_Ref (pi1);
	pi2 = pro2_bdd (context, arg2->cols, prod); Cudd_Ref (pi2);

	/* Compute pi1*R*pi1^ */
	tmp1 = komp_bdd (manager, pi1, arg1->bdd, prod, arg1->cols);
	Cudd_Ref(tmp1);
	tmp2 = komp_bdd_norm_transp (manager, tmp1, pi1, prod, prod, arg1->cols);
	Cudd_Ref(tmp2);
	Cudd_RecursiveDeref(manager, tmp1);
	Cudd_RecursiveDeref(manager, pi1);

	/* Compute pi2*S*pi2^ */
	tmp1 = komp_bdd (manager, pi2, arg2->bdd, prod, arg2->cols);
	Cudd_Ref (tmp1);
	tmp3 = komp_bdd_norm_transp (manager, tmp1, pi2, prod, prod, arg2->cols);
	Cudd_Ref (tmp3);
	Cudd_RecursiveDeref(manager, tmp1);
	Cudd_RecursiveDeref(manager, pi2);

	tmp1 = and_bdd (manager, tmp2, tmp3);
	Cudd_Ref (tmp1);
	Cudd_RecursiveDeref(manager, tmp2);
	Cudd_RecursiveDeref(manager, tmp3);

	Cudd_RecursiveDeref (manager, rop->bdd);
	rop->bdd = tmp1;
	mpz_set (rop->rows, prod);
	mpz_set (rop->cols, prod);

	mpz_clear (prod);
}
#endif


/****************************************************************************/
/* NAME: tup_P_rel                                                          */
/* FUNKTION: (?)                                                            */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 03.08.1999 (Umstellung auf BDD-Version)                                  */
/* 28.02.2000 (Ausnutzung der BDD-Routinen)                                 */
/* 13.04.2000 (Benutzung von pro1_bdd und pro2_bdd)                         */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// pre: arg1->rows == arg2->rows
// Result has arg1->rows and arg1->cols * arg2->cols columns.
void tup_P_rel (KureRel * arg1, KureRel * arg2, KureRel * rop)
     /* relT muss ungleich relR und relS sein! ... Why? -stb */
{
		KureContext * context = rop->context;
		DdManager * manager = context->manager;
		DdNode * pi1, *pi2;
		DdNode * tmp1, *tmp2, *tmp3;
		mpz_t prod;

		Kure_assert_same_context3(arg1,arg2,rop);

		mpz_init (prod);
		mpz_mul (prod, arg1->cols, arg2->cols);

		pi1 = pro1_bdd (context, prod, arg1->cols); Cudd_Ref (pi1);
		pi2 = pro2_bdd (context, prod, arg2->cols); Cudd_Ref (pi2);

		/* Compute R * (\pi_1)^ */
		tmp2 = komp_bdd_norm_transp (manager, arg1->bdd, pi1, arg1->rows,
									 prod, arg1->cols);
		Cudd_Ref (tmp2);
		Cudd_RecursiveDeref(manager, pi1);

		/* Compute S * (\pi_2)^ */
		tmp3 = komp_bdd_norm_transp (manager, arg2->bdd, pi2, arg2->rows,
										 prod, arg2->cols);
		Cudd_Ref (tmp3);
		Cudd_RecursiveDeref(manager, pi2);

		tmp1 = and_bdd (manager, tmp2, tmp3);
		Cudd_Ref (tmp1);
		Cudd_RecursiveDeref(manager, tmp2);
		Cudd_RecursiveDeref(manager, tmp3);

		Cudd_RecursiveDeref (manager, rop->bdd);
		rop->bdd = tmp1;
		mpz_set (rop->rows, arg1->rows);
		mpz_set (rop->cols, prod);

		mpz_clear (prod);
}


/*!
 * Implements the left tupling operation |[R,S]. If R has type
 * \f$R:X\leftrightarrow Z\f$ and S has type \f$S:Y\leftrightarrow Z\f$, then
 * \f$\llbracket R,S]\f$ has type \f$X\times Y\leftrightarrow Z\f$ and it
 * holds:
 * \f[
 * \llbracket R,S] = \pi R\cap \rho S\,.
 * \f]
 *
 * \pre Expects arg1->cols == arg2->cols.
 *
 * \author stefan
 * \date Aug 16, 2012
 *
 * \param arg1 The relation R.
 * \param arg2 the relation S.
 * \param rop Output relation. Must have been initialized.
 */
void tupling_left_rel (KureRel * arg1, KureRel * arg2, KureRel * rop)
{
		KureContext * context = rop->context;
		DdManager * manager = context->manager;
		DdNode * pi = NULL, *rho = NULL;
		DdNode * result, *pi_R = NULL, *rho_S = NULL;
		mpz_t prod;

		Kure_assert_same_context3 (arg1,arg2,rop);

		mpz_init (prod);
		mpz_mul (prod, arg1->rows, arg2->rows);

		pi  = pro1_bdd (context, prod, arg1->rows); Cudd_Ref (pi);
		rho = pro2_bdd (context, prod, arg2->rows); Cudd_Ref (rho);

		/* Compute \pi * R */
		pi_R = komp_bdd (manager, pi, arg1->bdd, prod, arg1->rows);
		Cudd_Ref (pi_R);
		Cudd_RecursiveDeref(manager, pi); pi = NULL;

		/* Compute \rho * S */
		rho_S = komp_bdd (manager, rho, arg2->bdd, prod, arg2->rows);
		Cudd_Ref (rho_S);
		Cudd_RecursiveDeref(manager, rho); rho = NULL;

		result = and_bdd (manager, pi_R, rho_S);
		Cudd_Ref (result);
		Cudd_RecursiveDeref(manager, pi_R); pi_R = NULL;
		Cudd_RecursiveDeref(manager, rho_S); rho_S = NULL;

		Cudd_RecursiveDeref (manager, rop->bdd);
		rop->bdd = result;
		mpz_set (rop->rows, prod);
		mpz_set (rop->cols, arg1->cols);

		mpz_clear (prod);
}


/****************************************************************************/
/* NAME: inj1_rel                                                           */
/* FUNKTION: (?)                                                            */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 03.08.1999 (Umstellung auf BDD-Version)                                  */
/* 28.03.2000 (Benutzung einer eigenen BDD-Routine)                         */
/****************************************************************************/
void inj1_rel (KureRel * rop, mpz_t first, mpz_t second)
{
	DdNode * inj;
	mpz_t s;
	mpz_init (s);
	mpz_add (s, first, second);

	//gmp_printf ("inj1_rel: first=%Zd, second=%Zd, s=%Zd\n", first, second, s);

	inj = inj1_bdd (rop->context, first, s);
	if (inj) {
		Cudd_Ref(inj);
		Cudd_RecursiveDeref(rop->context->manager, rop->bdd);
		rop->bdd = inj;
		mpz_set(rop->rows, first);
		mpz_set(rop->cols, s);
	}

	mpz_clear (s);
}


/****************************************************************************/
/* NAME: inj2_rel                                                           */
/* FUNKTION: (?)                                                            */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 03.08.1999 (Umstellung auf BDD-Version)                                  */
/* 28.03.2000 (Benutzung einer eigenen BDD-Routine)                         */
/****************************************************************************/
void inj2_rel (KureRel * rop, mpz_t first, mpz_t second)
{
	DdNode * inj;
	mpz_t s;
	mpz_init (s);
	mpz_add (s, first, second);

	//gmp_printf ("inj2_rel: first=%Zd, second=%Zd, s=%Zd\n", first, second, s);

	inj = inj2_bdd (rop->context, second, s);
	if (inj) {
		Cudd_Ref(inj);
		Cudd_RecursiveDeref(rop->context->manager, rop->bdd);
		rop->bdd = inj;
		mpz_set(rop->rows, second);
		mpz_set(rop->cols, s);
	}

	mpz_clear (s);
}


#if 0
/* Implementation is now on a higher level. See KureRel.c. */
/****************************************************************************/
/* NAME: suord_rel                                                          */
/* FUNKTION: (?)                                                            */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 05.08.1999 (Umstellung auf BDD-Version)                                  */
/* 13.04.2000 (Benutzung von inj1_bdd und inj2_bdd)                         */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
void suord_rel (KureRel * arg1, KureRel * arg2, KureRel * rop)
     /* relS muss ungleich relA und relB sein! ... Why? -stb*/
{
	if (arg1 && arg2 && rop) {
		KureContext * context = rop->context;
		DdManager * manager = context->manager;
		DdNode * iota1, *iota2;
		DdNode * tmp1, *tmp2, *tmp3;
		mpz_t prod;

		Kure_assert_same_context3(arg1,arg2,rop);

		mpz_init (prod);
		mpz_mul (prod, arg1->cols, arg2->cols);

		iota1 = inj1_bdd (context, arg1->cols, prod);
		Cudd_Ref (iota1);
		iota2 = inj2_bdd (context, arg2->cols, prod);
		Cudd_Ref (iota2);

		/* Compute iota1^*R*iota1 */
		tmp1 = komp_bdd_transp_norm (manager, iota1, arg1->bdd, arg1->cols, prod);
		Cudd_Ref(tmp1);
		tmp2 = komp_bdd(manager, tmp1, iota1, prod, arg1->cols);
		Cudd_Ref(tmp2);
		Cudd_RecursiveDeref(manager, tmp1);
		Cudd_RecursiveDeref(manager, iota1);

	    /* Compute iota2^*S*iota2 */
	    tmp1 = komp_bdd_transp_norm (manager, iota2, arg2->bdd, arg2->cols, prod);
	    Cudd_Ref (tmp1);
	    tmp3 = komp_bdd (manager, tmp1, iota2, prod, arg2->cols);
	    Cudd_Ref (tmp3);
		Cudd_RecursiveDeref(manager, tmp1);
		Cudd_RecursiveDeref(manager, iota2);

		tmp1 = or_bdd(manager, tmp2, tmp3);
		Cudd_Ref (tmp1);
		Cudd_RecursiveDeref(manager, tmp2);
		Cudd_RecursiveDeref(manager, tmp3);

		Cudd_RecursiveDeref(manager, rop->bdd);
		rop->bdd = tmp1;
		mpz_set (rop->rows, prod);
		mpz_set (rop->cols, prod);

		mpz_clear (prod);
	} else
		printf ("ERROR (prord_rel): One argument is missing!\n");
}
#endif


/****************************************************************************/
/* NAME: tup_S_rel                                                          */
/* FUNKTION: (?)                                                            */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 05.08.1999 (Umstellung auf BDD-Version)                                  */
/* 13.04.2000 (Benutzung von inj1_bdd und inj2_bdd)                         */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/* 22-MAR-2002 (UMI): Optimizations                                         */
/****************************************************************************/
// pre: ??
// The result has arg1->rows + arg2->rows rows and arg2->cols columns.
void tup_S_rel (KureRel * arg1, KureRel * arg2, KureRel * rop)
{
	KureContext * context = rop->context;
	DdManager * manager = context->manager;
	DdNode * inj1, *inj2;
	DdNode * tmp1, *tmp2, *res;
	mpz_t sum;

	mpz_init (sum);
	mpz_add (sum, arg1->rows, arg2->rows);

	inj1 = inj1_bdd (context, arg1->rows, sum);
	Cudd_Ref (inj1);

	tmp1 = komp_bdd_transp_norm (manager, inj1, arg1->bdd, arg1->rows, sum);
	Cudd_Ref (tmp1);
	Cudd_RecursiveDeref(manager, inj1);

	inj2 = inj2_bdd (context, arg2->rows, sum);
	Cudd_Ref (inj2);

	tmp2 = komp_bdd_transp_norm (manager, inj2, arg2->bdd, arg2->rows, sum);
	Cudd_Ref (tmp2);
	Cudd_RecursiveDeref(manager, inj2);

	res = or_bdd (manager, tmp1, tmp2);
	Cudd_Ref(res);
	Cudd_RecursiveDeref(manager,tmp1);
	Cudd_RecursiveDeref(manager,tmp2);

	Cudd_RecursiveDeref(manager, rop->bdd);
	rop->bdd = res;
	mpz_set (rop->rows, sum);
	mpz_set (rop->cols, arg2->cols);

	mpz_clear (sum);
}

