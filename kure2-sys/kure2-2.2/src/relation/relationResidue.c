#include "KureImpl.h"


/****************************************************************************/
/* NAME: res_li_rel                                                         */
/* FUNKTION: berechnet das links_residuum einer Relation                    */
/* UEBERGABEPARAMETER: 2 x Rel * (Relationen), 1. + 2. Argument             */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 03.08.1999 (Umstellung auf BDD-Version)                                  */
/* 29.03.2000 (Benutzung von komp_bdd_norm_transp)                          */
/****************************************************************************/
// Input: cols(S)=cols(R)
// Ouput: S/R=-(-S*R^t), rows:=rows(S), cols:=rows(2)
void res_li_rel (KureRel * relS, KureRel * relR, KureRel * rop)
{
	KureContext * context = rop->context;
	DdManager * manager = relS->context->manager;
	DdNode * negS, *tmp;

	negS = neg_bdd (context, relS->bdd, relS->rows, relS->cols);
	Cudd_Ref (negS);
	tmp = komp_bdd_norm_transp(manager, negS, relR->bdd,
			relS->rows, relR->rows, relS->cols);
	Cudd_Ref (tmp);
	Cudd_RecursiveDeref(manager, negS);

	Cudd_RecursiveDeref(manager, rop->bdd);
	rop->bdd = neg_bdd(context, tmp, relS->rows, relR->rows);
	Cudd_Ref(rop->bdd);
	Cudd_RecursiveDeref(manager, tmp);
	/* Order is mandatory. */
	mpz_set (rop->cols, relR->rows);
	mpz_set (rop->rows, relS->rows);
}


/****************************************************************************/
/* NAME: res_re_rel                                                         */
/* FUNKTION: berechnet das rechts_Residuum einer Relation                   */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. + 2. Argument             */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 03.08.1999 (Umstellung auf BDD-Version)                                  */
/****************************************************************************/
// Input: rows(S)=rows(R)
// Ouput: R\S=-(R^t*-S), rows:=rows(R), cols:=cols(S)
void res_re_rel (KureRel * relR, KureRel * relS, KureRel * rop)
{
	KureContext * context = rop->context;
	DdManager * manager = relS->context->manager;
	DdNode * negS, *tmp;

	negS = neg_bdd (context, relS->bdd, relS->rows, relS->cols);
	Cudd_Ref (negS);
	tmp = komp_bdd_transp_norm (manager, relR->bdd, negS, relR->rows, relR->cols);
	Cudd_Ref (tmp);
	Cudd_RecursiveDeref(manager, negS);

	Cudd_RecursiveDeref(manager, rop->bdd);
	rop->bdd = neg_bdd(context, tmp, relR->cols, relS->cols);
	Cudd_Ref(rop->bdd);
	Cudd_RecursiveDeref(manager, tmp);
	/* Order is mandatory. */
	mpz_set (rop->rows, relR->cols);
	mpz_set (rop->cols, relS->cols);
}


/****************************************************************************/
/* NAME: syq_rel                                                            */
/* FUNKTION: berechnet den symetrischen Quotienten                          */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 03.08.1999 (Umstellung auf BDD-Version)                                  */
/* 13.04.2000 (Benutzung von komp_bdd_transp_norm)                          */
/****************************************************************************/
// Input: Rels of same height.
// Output: syq(R,s)=(R\S) & (R^/S^)=-(R^*-S) & -(-R^*S),
//   rows:=cols(R), cols:=cols(S)
void syq_rel (KureRel * relR, KureRel * relS, KureRel * rop)
{
	KureContext * context = rop->context;
	DdManager * manager = relS->context->manager;
	DdNode *neg_S, *neg_R, *tmp1, *tmp2, *tmp3;

	/* First compute the right residue */
	neg_S = neg_bdd (context, relS->bdd, relS->rows, relS->cols);
	Cudd_Ref(neg_S);
	tmp1 = komp_bdd_transp_norm (manager, relR->bdd, neg_S, relR->rows, relR->cols);
	Cudd_Ref(tmp1);
	Cudd_RecursiveDeref(manager, neg_S);
	tmp2 = neg_bdd(context, tmp1, relR->cols, relS->cols);
	Cudd_Ref(tmp2);
	Cudd_RecursiveDeref(manager, tmp1);

	/* Afterwards compute the left residue. */
	neg_R = neg_bdd (context, relR->bdd, relR->rows, relR->cols);
	Cudd_Ref(neg_R);
	tmp1 = komp_bdd_transp_norm (manager, neg_R, relS->bdd, relR->rows, relR->cols);
	Cudd_Ref(tmp1);
	Cudd_RecursiveDeref(manager, neg_R);
    tmp3 = neg_bdd (context, tmp1, relR->cols, relS->cols);
    Cudd_Ref(tmp3);
    Cudd_RecursiveDeref(manager,tmp1);

    Cudd_RecursiveDeref(manager, rop->bdd);
    rop->bdd = and_bdd(manager, tmp2, tmp3);
    Cudd_Ref(rop->bdd);
    Cudd_RecursiveDeref(manager, tmp2);
    Cudd_RecursiveDeref(manager, tmp3);

    // Order is mandatory
    mpz_set(rop->rows, relR->cols);
    mpz_set(rop->cols, relS->cols);
}
