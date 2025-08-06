#include "KureImpl.h"

/****************************************************************************/
/* NAME: null_rel                                                           */
/* FUNKTION: fuellt die Relation mit lauter 0'en                            */
/* UEBERGABEPARAMETER: Rel * (die Relation)                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: (?)                                                        */
/* AM: (?)                                                                  */
/* LETZTE AENDERUNG AM: 29.07.1999 (Umstellung auf BDD-Version)             */
/* 22.09.1999 (Das Bitmuster wird ebenfalls mit Nullen gefuellt)            */
/****************************************************************************/
void null_rel (KureRel * rel)
{
	if (rel) {
		DdManager * manager = rel->context->manager;

		Cudd_RecursiveDeref (manager, rel->bdd);
		rel->bdd = Cudd_Not(Cudd_ReadOne(manager));
		Cudd_Ref (rel->bdd);
	}
}


/****************************************************************************/
/* NAME: L_rel                                                              */
/* FUNKTION: fuellt die Relation mit lauter 1'en                            */
/* UEBERGABEPARAMETER: Rel * (die Relation)                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 18.03.96 (copy & paste von null_rel)                                 */
/* LETZTE AENDERUNG AM: 18.03.96                                            */
/****************************************************************************/
void L_rel (KureRel * rel)
{
	if (rel) {
		DdManager * manager = rel->context->manager;

		Cudd_RecursiveDeref (manager, rel->bdd);
		rel->bdd = L_bdd (rel->context, rel->rows, rel->cols);
		Cudd_Ref (rel->bdd);
	}
}


/****************************************************************************/
/* NAME: eins_rel                                                           */
/* FUNKTION: Identitaet einer Rel                                           */
/* UEBERGABEPARAMETER: Relation, die zu I gemacht wird                      */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: (?)                                                        */
/* AM: (?)                                                                  */
/* LETZTE AENDERUNG AM: 30.07.1999 (Umstellung auf BDD-Version)             */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
void eins_rel (KureRel * rel)
{
	if (gmp_equal(rel->cols, rel->rows)) {
		DdNode * I = ident_bdd (rel->context, rel->rows);
		if (!I) printf ("ERROR (eins_rel): ident_bdd has failed!\n");
		else {
			Cudd_Ref (I);
			Cudd_RecursiveDeref (rel->context->manager, rel->bdd);
			rel->bdd = I;
		}
	}
	else printf ("ERROR (eins_rel): Dimensions do not match\n");
}


/****************************************************************************/
/* NAME: oder_rel                                                           */
/* FUNKTION: verknuepft zwei relation mit "|" und speichert Ergebnis        */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 02.08.1999 (Umstellung auf BDD-Version)                                  */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// rop = arg1 OR arg2
// Context of rop doesn't change. Must be in the same context.
void oder_rel (KureRel * arg1, KureRel * arg2, KureRel * rop)
{
	DdManager * manager = arg1->context->manager;
	DdNode * res = or_bdd(manager, arg1->bdd, arg2->bdd);
	if (res) {
		Cudd_Ref (res);
		Cudd_RecursiveDeref (manager, rop->bdd);
		rop->bdd = res;
		mpz_set (rop->cols, arg1->cols);
		mpz_set (rop->rows, arg1->rows);
	}
}


/****************************************************************************/
/* NAME: und_rel                                                            */
/* FUNKTION: verknuepft zwei relation mit "&" und speichert Ergebnis        */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), 1. und 2. Argument           */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 02.08.1999 (Umstellung auf BDD-Version)                                  */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
void und_rel (KureRel * arg1, KureRel * arg2, KureRel * rop)
{
	DdManager * manager = arg1->context->manager;
	DdNode * res = and_bdd(manager, arg1->bdd, arg2->bdd);
	if (res) {
		Cudd_Ref (res);
		Cudd_RecursiveDeref (manager, rop->bdd);
		rop->bdd = res;
		mpz_set (rop->cols, arg1->cols);
		mpz_set (rop->rows, arg1->rows);
	}
}


/****************************************************************************/
/* NAME: transp_rel                                                         */
/* FUNKTION: transponiert eine Relation                                     */
/* UEBERGABEPARAMETER: 2 x Rel * (Relationen), Argument                     */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 02.08.1999 (Umstellung auf BDD-Version)                                  */
/****************************************************************************/
// rop := arg1^
void transp_rel (KureRel * arg, KureRel * rop)
{
	DdManager * manager = arg->context->manager;
	DdNode * res = trans_bdd(arg->context, arg->bdd, arg->rows, arg->cols);

	if (res) {
		Cudd_Ref (res);
		Cudd_RecursiveDeref(manager, rop->bdd);
		rop->bdd = res;
		if (arg != rop) {
			mpz_set(rop->rows, arg->cols); // transposed!
			mpz_set(rop->cols, arg->rows);
		}
		else {
			/* The other approach would work. */
			mpz_swap (rop->rows, rop->cols);
		}
	}
}


/****************************************************************************/
/* NAME: kompl_rel                                                          */
/* FUNKTION: berechnet das Komplement einer Relation                        */
/* UEBERGABEPARAMETER: 2 x Rel * (Relationen), Argument                     */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 02.08.1999 (Umstellung auf BDD-Version)                                  */
/****************************************************************************/
void kompl_rel (KureRel * arg, KureRel * rop)
{
	DdManager * manager = arg->context->manager;
	DdNode * L = L_bdd(arg->context, arg->rows, arg->cols);
	if (L) {
		DdNode * res;
		Cudd_Ref (L);

		res = and_bdd(manager, Cudd_Not(arg->bdd), L);
		if (res) {
			Cudd_Ref (res);
			Cudd_RecursiveDeref(manager, L);
			Cudd_RecursiveDeref(manager, rop->bdd);
			rop->bdd = res;
			if (arg != rop) {
				mpz_set (rop->rows, arg->rows);
				mpz_set (rop->cols, arg->cols);
			}
		}
	}
}
