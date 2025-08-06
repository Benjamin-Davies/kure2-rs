#include "KureImpl.h"
#include "cuddInt.h"

/****************************************************************************/
/* NAME: shift_bdd                                                          */
/* FUNKTION: Prozedur zum Umschichten von Variablen in einem BDD,           */
/*           die Menge der Variablen wird in zwei Mengen aufgeteilt, die    */
/*           mit verschiedenen Werten geshiftet werden koennen.             */
/* UEBERGABEPARAMETER: DdNode * (BDD), int (Index der Grenzvariablen),      */
/*                     int (1. Shift-Wert), int (2. Shift-Wert)             */
/* RUECKGABEWERT: DdNode * (neuer BDD)                                      */
/* ERSTELLT VON: Barbara Leoniuk                                            */
/* AM: 02.08.1999                                                           */
/* LETZTE AENDERUNG AM: 23.09.1999 (Aufteilung in shift_bdd und             */
/* shift_bdd_recur)                                                         */
/* 22.10.1999 (Verbesserter Referenz-BDD fuer CashLookup)                   */
/****************************************************************************/
DdNode * shift_bdd(DdManager * manager, DdNode * f, int border, int value_1, int value_2)
{
	DdNode * tmp_1;
	DdNode * tmp_2;
	DdNode * tmp_3;
	DdNode * reference_bdd;
	DdNode * result;
	int inter_sum;

	/* Es wird ein BDD aufgebaut, der border, value_1 und value_2 kodiert */
	/* tmp_1 kodiert border */
	tmp_1 = Cudd_bddIthVar(manager, border);

	/* tmp_2 kodiert value_1, falls value_1 negativ ist, wird tmp_2 negiert */
	if (value_1 >= 0) {
		inter_sum = border + value_1 + 1;
		tmp_2 = Cudd_bddIthVar(manager, inter_sum);
	} else {
		inter_sum = border - value_1 + 1;
		tmp_2 = Cudd_Not (Cudd_bddIthVar (manager, inter_sum));
	}

	/* tmp_3 kodiert border und value_1 */
	tmp_3 = Cudd_bddAnd(manager, tmp_1, tmp_2);
	Cudd_Ref (tmp_3);

	/* tmp_1 kodiert value_2, falls value_2 negativ ist, wird tmp_1 negiert */
	if (value_2 >= 0)
		tmp_1 = Cudd_bddIthVar(manager, inter_sum + value_2 + 1);
	else
		tmp_1 = Cudd_Not (Cudd_bddIthVar(manager, inter_sum - value_2 + 1));

	/* reference_bdd liefert das gewuenschte Ergebnis */
	reference_bdd = Cudd_bddAnd(manager, tmp_3, tmp_1);
	Cudd_Ref (reference_bdd);
	Cudd_RecursiveDeref(manager, tmp_3);

	/* Cudd_DisableGarbageCollection (manager); */
	result = shift_bdd_recur(manager, f, reference_bdd, border, value_1, value_2);
	Cudd_Ref(result);

	/* Cudd_EnableGarbageCollection (manager); */
	Cudd_RecursiveDeref(manager, reference_bdd);

	Cudd_Deref (result);
	return (result);
}

/****************************************************************************/
/* NAME: shift_bdd_cache                                                    */
/* FUNKTION: Pseudo-Prozedur ohne Funktion, die fuer das CacheLookup2 und   */
/*           CacheInsert2 des CUDD-Packages als Parameter uebergeben wird.  */
/* UEBERGABEPARAMETER: DdManager *, DdNode *, DdNode *                      */
/* RUECKGABEWERT: DdNode *                                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 22.10.1999                                                           */
/* LETZTE AENDERUNG AM: 22.10.1999                                          */
/****************************************************************************/
static DdNode * shift_bdd_cache(DdManager * table, DdNode * f, DdNode * g)
{ return NULL; }

/****************************************************************************/
/* NAME: shift_bdd_recur                                                    */
/* FUNKTION: Rekursive Prozedur zum Umschichten von Variablen in einem BDD, */
/*           die Menge der Variablen wird in zwei Mengen aufgeteilt, die    */
/*           verschiedenen Werten geshiftet werden koennen.                 */
/* UEBERGABEPARAMETER: DdNode * (BDD), int (Index der Grenzvariablen),      */
/*                     int (1. Shift-Wert), int (2. Shift-Wert)             */
/* RUECKGABEWERT: DdNode * (neuer BDD)                                      */
/* ERSTELLT VON: Barbara Leoniuk                                            */
/* AM: 02.08.1999                                                           */
/* LETZTE AENDERUNG AM: 22.10.1999 (Verwendung von cuddCacheLookup2)        */
/****************************************************************************/
DdNode * shift_bdd_recur(DdManager * manager, DdNode * f,
		DdNode * reference_bdd, int border, int value_1, int value_2)
{
	/* Falls es nichts zu shiften gibt (z. B. bei Vektoren) */
	Kure_bool dont_need_shift = ((value_1 == 0 || border == 0) && (value_2 == 0));

	/* Falls f eine Konstante oder leer ist, muss nicht geshiftet werden */
	if (dont_need_shift || !f || Cudd_IsConstant(f))
		return (f);
	else {
		DdNode * res;

		/* Check cache */
		res = cuddCacheLookup2(manager, shift_bdd_cache, f, reference_bdd);
		if (res)
			return (res);
		else {
			DdNode * node;
			DdNode * shift_e;
			DdNode * shift_t;
			int index = Cudd_Regular(f)->index;
			DdNode * e = Cudd_E (f);
			DdNode * t = Cudd_T (f);

			shift_e = shift_bdd_recur(manager, e, reference_bdd, border, value_1, value_2);
			Cudd_Ref(shift_e);
			shift_t = shift_bdd_recur(manager, t, reference_bdd, border, value_1, value_2);
			Cudd_Ref(shift_t);

			if (index < border)
				/* Alle Variablen mit (index < border) werden um value_1 geshiftet */
				node = Cudd_bddIthVar(manager, index + value_1);
			else
				node = Cudd_bddIthVar(manager, index + value_2);

			res = Cudd_bddIte(manager, node, shift_t, shift_e);
			Cudd_Ref (res);

			/* What does this suppose to me? */
			if (Cudd_IsComplement (f) && (f != Cudd_Not(Cudd_ReadOne(manager))))
				res = Cudd_Not (res);

			Cudd_RecursiveDeref(manager, shift_e);
			Cudd_RecursiveDeref(manager, shift_t);

			cuddCacheInsert2(manager, shift_bdd_cache, f, reference_bdd, res);
			Cudd_Deref(res);
			return (res);
		}
	}
}
