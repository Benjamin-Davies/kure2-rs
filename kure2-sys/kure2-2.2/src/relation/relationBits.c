#include "KureImpl.h"
#include <assert.h>

/****************************************************************************/
/* NAME: mp_set_rel_bit                                                     */
/* FUNKTION: setzt ein BIT in Zeile / Spalte der Relation                   */
/* UEBERGABEPARAMETER: Rel * (Relation), Kure_MINT * (Zeile), Kure_MINT * (Spalte)    */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 26.01.95                                                             */
/* LETZTE AENDERUNG AM: 29.07.1999 (Umstellung auf BDD-Version)             */
/* 01.03.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// Achtung: Reihenfolge geaendert!
void mp_set_rel_bit (KureRel * rel, mpz_t row, mpz_t col)
{
	DdManager * manager = rel->context->manager;
	DdNode * res;
	DdNode * minterm = make_minterm (manager, rel->cols, rel->rows, col, row);
	if (minterm) {
		Cudd_Ref(minterm);
		res = or_bdd(manager, rel->bdd, minterm);
		Cudd_Ref(res);
		Cudd_RecursiveDeref(manager, rel->bdd);
		rel->bdd = res;

		Cudd_RecursiveDeref(manager, minterm);
	}
	else fprintf (stderr, "mp_set_rel_bit: Unable to create minterm.\n");
}


/****************************************************************************/
/* NAME: mp_clear_rel_bit                                                   */
/* FUNKTION: loescht ein BIT in Zeile / Spalte der Relation                 */
/* UEBERGABEPARAMETER: Rel * (Relation), Kure_MINT * (Zeile), Kure_MINT * (Spalte)    */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 26.01.95                                                             */
/* LETZTE AENDERUNG AM: 29.07.1999 (Umstellung auf BDD-Version)             */
/* 01.03.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
void mp_clear_rel_bit (KureRel * rel, mpz_t row, mpz_t col)
{

	DdManager * manager = rel->context->manager;
	DdNode * res;
	DdNode * minterm = make_minterm(manager, rel->cols, rel->rows, col, row);
	Cudd_Ref (minterm);
	res = and_bdd (manager, rel->bdd, Cudd_Not(minterm));
	Cudd_Ref (res);
	Cudd_RecursiveDeref(manager, rel->bdd);
	rel->bdd = res;
	Cudd_RecursiveDeref(manager, minterm);
}


/****************************************************************************/
/* NAME: mp_get_rel_bit                                                     */
/* FUNKTION: ermittelt ein BIT in Zeile / Spalte der Relation               */
/* UEBERGABEPARAMETER: Rel * (Relation), Kure_MINT * (Zeile), Kure_MINT * (Spalte)    */
/* RUECKGABEWERT: 0 wenn BIT nicht gesetzt, sonst != 0                      */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 26.01.95                                                             */
/* LETZTE AENDERUNG AM: 29.07.1999 (Umstellung auf BDD-Version)             */
/* 12.04.2000 (Ausnutzung der Routinen find_row und find_column)            */
/* 01.03.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/


Kure_bool mp_get_rel_bit_full (KureRel * rel, mpz_t row, mpz_t col,
		int vars_rows, int vars_cols)
{
	DdNode * one = Cudd_ReadOne(rel->context->manager);

	if (Cudd_IsConstant(rel->bdd)) {
		if (one == rel->bdd) return TRUE;
		else return FALSE;
	}
	else {
		/* Falls die Zeile leer ist, return 0; falls die Zeile L ist, return 1; */
		DdNode * node = find_row_in_bdd (rel->bdd, vars_rows, row);
		if (one == node) return TRUE;
		else if (Cudd_Not(one) == node) return FALSE;

		/* Suche des einzelnen Eintrages */
		node = find_column_in_bdd (node, vars_rows, vars_cols, col);
		if (one == node) return TRUE;
		else {
			assert (Cudd_Not(one) == node);
			return FALSE;
		}
	}
}

Kure_bool mp_get_rel_bit (KureRel * rel, mpz_t row, mpz_t col)
{
	return mp_get_rel_bit_full(rel, row, col, Kure_num_vars(rel->rows),
			Kure_num_vars(rel->cols));

}


/****************************************************************************/
/* NAME: get_rel_bit                                                        */
/* FUNKTION: ermittelt ein BIT in Zeile / Spalte der Relation               */
/* UEBERGABEPARAMETER: Rel * (Relation), int (Zeile), int (Spalte)          */
/*                     Anzahl der Variablen (int, int)                      */
/* RUECKGABEWERT: 0 wenn BIT nicht gesetzt, sonst != 0                      */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 26.01.95                                                             */
/* LETZTE AENDERUNG AM: 29.07.1999 (Umstellung auf BDD-Version)             */
/* 12.04.2000 (Ausnutzung der Routinen find_row und find_column)            */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// Achtung: Reihenfolge geaendert.
Kure_bool get_rel_bit_full (KureRel * rel, int row, int col, int vars_rows, int vars_cols)
{
	Kure_bool ret;
	mpz_t mp_row, mp_col;
	mpz_init_set_si (mp_row, row);
	mpz_init_set_si (mp_col, col);
	ret = mp_get_rel_bit_full(rel, mp_row, mp_col, vars_rows, vars_cols);
	mpz_clear (mp_row);
	mpz_clear (mp_col);
	return ret;
}

// Achtung: Reihenfolge geaendert.
Kure_bool get_rel_bit (KureRel * rel, int row, int col)
{
	Kure_bool ret;
	mpz_t mp_row, mp_col;
	mpz_init_set_si (mp_row, row);
	mpz_init_set_si (mp_col, col);
	ret = mp_get_rel_bit(rel, mp_row, mp_col);
	mpz_clear (mp_row);
	mpz_clear (mp_col);
	return ret;
}

/****************************************************************************/
/* NAME: clear_rel_bit                                                     */
/* FUNKTION: loescht ein BIT in Zeile / Spalte der Relation                 */
/* UEBERGABEPARAMETER: Rel * (Relation), int (Zeile), int (Spalte)          */
/*                     int, int (Breite und Hoehe der Relation)             */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 26.01.95                                                             */
/* LETZTE AENDERUNG AM: 29.07.1999 (Umstellung auf BDD-Version)             */
/****************************************************************************/
// Achtung: Reihenfolge geaendert.
void clear_rel_bit (KureRel * rel, int row, int col)
{
	mpz_t mp_row, mp_col;
	mpz_init_set_si (mp_row, row);
	mpz_init_set_si (mp_col, col);
	mp_clear_rel_bit(rel, mp_row, mp_col);
	mpz_clear (mp_row);
	mpz_clear (mp_col);
}



/****************************************************************************/
/* NAME: set_rel_bit                                                       */
/* FUNKTION: setzt ein BIT in Zeile / Spalte der Relation                   */
/* UEBERGABEPARAMETER: Rel * (Relation), int (Zeile), int (Spalte)          */
/*                     int, int (Breite und Hoehe der Relation)             */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 26.01.95                                                             */
/* LETZTE AENDERUNG AM: 29.07.1999 (Umstellung auf BDD-Version)             */
/* 01.06.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// Achtung: Reihenfolge geaendert.
void set_rel_bit (KureRel * rel, int row, int col)
{
	mpz_t mp_row, mp_col;
	mpz_init_set_si (mp_row, row);
	mpz_init_set_si (mp_col, col);
	mp_set_rel_bit (rel, mp_row, mp_col);
	mpz_clear (mp_row);
	mpz_clear (mp_col);
}


