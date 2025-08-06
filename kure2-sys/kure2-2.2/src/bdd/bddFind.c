#include "KureImpl.h"


/****************************************************************************/
/* NAME: find_row_in_bdd                                                    */
/* FUNKTION: Berechnet zu einem BDD einer Relation den BDD einer bestimmten */
/* Zeile                                                                    */
/* UEBERGABEPARAMETER: DdNode * (BDD), int (Anzahl der Variablen),          */
/*                     Kure_MINT * ((i - 1)-te Zeile der Relation [0 .. n-1])    */
/* RUECKGABEWERT: DdNode * (BDD der Zeile)                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 15.02.2000                                                           */
/* LETZTE AENDERUNG AM: 15.02.2000                                          */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
DdNode * find_row_in_bdd (DdNode * rel,	int number_vars, mpz_t row)
{
	while (! Cudd_IsConstant(rel)
			&& (Cudd_Regular(rel)->index < number_vars)) {
		DdNode * tmp;
		int counter = number_vars - Cudd_Regular(rel)->index - 1;

		if (mpz_tstbit(row, counter))
			tmp = Cudd_T(rel);
		else
			tmp = Cudd_E(rel);
		if (Cudd_IsComplement(rel))
			tmp = Cudd_Not(tmp);

		rel = tmp;
	}

	return rel;
}

/****************************************************************************/
/* NAME: find_column_in_bdd                                                 */
/* FUNKTION: Berechnet zu einem Zeilen-BDD den BDD einer bestimmten Spalte  */
/* UEBERGABEPARAMETER: DdNode * (BDD),                                      */
/*                     int (Anzahl der Variablen der Zeilen),               */
/*                     int (Anzahl der Variablen der Spalten),              */
/*                     int (i-te Spalte der Relation)                       */
/* RUECKGABEWERT: DdNode * (BDD der Spalte)                                 */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 16.02.2000                                                           */
/* LETZTE AENDERUNG AM: 16.02.2000                                          */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
DdNode * find_column_in_bdd (DdNode * rel, int number_vars_rows,
                             int number_vars_cols, mpz_t col)
{
	DdNode * iter;

	for ( iter = rel ; ! Cudd_IsConstant(iter) ; ) {
		DdNode * child;
		int counter;

		counter = number_vars_rows
				+ number_vars_cols - Cudd_Regular(iter)->index - 1;

		if (mpz_tstbit(col, counter))
			child = Cudd_T(iter);
		else
			child = Cudd_E(iter);
		if (Cudd_IsComplement(iter))
			child = Cudd_Not(child);

		iter = child;
	}

	return iter;
}
