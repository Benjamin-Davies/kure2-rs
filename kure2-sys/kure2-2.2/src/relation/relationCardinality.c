#include "KureImpl.h"


/****************************************************************************/
/* NAME: lesscard_rel                                                       */
/* FUNKTION: Berechnet die Cardinalitaets-Inklusions-Relation               */
/* UEBERGABEPARAMETER: 2 x Rel * (Relationen), Argument                     */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 04.10.2000                                                           */
/****************************************************************************/
// The vector's number of rows must fit into signed int.
void lesscard_rel (KureRel * vec, KureRel * rop)
{
	int rows = mpz_get_si(vec->rows);
	DdManager * manager = vec->context->manager;
	DdNode * tmp = lesscard_bdd (vec->context, mpz_get_si (vec->rows));
	if (tmp) {
		Cudd_Ref(tmp);
		Cudd_RecursiveDeref(manager, rop->bdd);
		rop->bdd = tmp;
		mpz_ui_pow_ui(rop->rows, 2, rows); // rows,cols = 2^vec_rows
		mpz_set(rop->cols, rop->rows);
	}
}

/****************************************************************************/
/* NAME: subsetvector_rel                                                   */
/* FUNKTION: Filtert aus einem Potenzmengenvektor alle Elemente heraus,     */
/*           deren Kardinalitaet groesser gleich der Groesse eines anderen  */
/*           Vektors ist                                                    */
/* UEBERGABEPARAMETER: 3 x Rel * (Relationen), Potenzmengenvektor, Argument */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 24.10.2000                                                           */
/* 27.10.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// The result has the same size as the input vector.
// pot must have 2^k rows, for any k>0
// both arguments have to be vectors (line complete). Can have more than
// one column.
/* Preconditions: (i) pot->rows = 2^k?
 * 		(ii) log2(pot->rows)>=vec->rows
 * 		(iii) vec->rows <= MAXINT?
 */
Kure_bool subsetvector_rel (KureRel * pot, KureRel * vec, KureRel * rop)
{
	if (!pot || !vec || !rop)
		return FALSE;
#if 0
	else if ( !gmp_is_pow2(pot->rows)) // pot->rows = 2^k?
		return FALSE;
	else if ( !gmp_leq(vec->rows, Kure_num_vars(pot->rows))) // log2(pot->rows)>=vec->rows
		return FALSE;
	else if ( !mpz_fits_sint_p(vec->rows)) // vec->rows <= MAXINT?
		return FALSE;
#endif
	else {
		int rows = mpz_get_si(vec->rows);
		DdManager * manager = vec->context->manager;
		DdNode * tmp = subsetvector_bdd(vec->context, Kure_num_vars(pot->rows), rows);
		if (tmp) {
			DdNode * tmp2;
			Cudd_Ref (tmp);
			tmp2 = and_bdd (manager, tmp, pot->bdd);
			if (tmp2) {
				Cudd_Ref (tmp2);
				Cudd_RecursiveDeref(manager, rop->bdd);
				rop->bdd = tmp2;
				mpz_set (rop->cols, pot->cols);
				mpz_set (rop->rows, pot->rows);
			}
			Cudd_RecursiveDeref(manager, tmp);
			return TRUE;
		}
		else return FALSE;
	}
}



/****************************************************************************/
/* NAME: rvRelationCountColumns                                             */
/* FUNKTION: Berechnet in einem Vektor die Anzahl der Eins-Stellen          */
/* UEBERGABEPARAMETER: Vektor                                               */
/* RUECKGABEWERT: Grosse Zahl                                               */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 13-MAR-2002                                                          */
/****************************************************************************/
void rvRelationCountColumns (KureRel * vec, mpz_t rop)
{
	if (! vec) return;

	mpz_set_si (rop, 0);
	if (line_completeness(vec)) {
		DdManager * manager = vec->context->manager;
		int num_vars_rows = Kure_num_vars(vec->rows);

		if (mpz_cmp_si(vec->cols, 1) > 0) /* more than one column? */ {
			int num_vars_cols = Kure_num_vars(vec->cols);
			int * array;
			int i;
			DdNode * cube, *tmp;

		    /* Es wird ein Array erzeugt mit den Indizes der Variablen,
		     * ueber die quantifiziert werden muss */
		    array = (int *) alloca (num_vars_cols * sizeof (int));
		    for (i = 0; i < num_vars_cols; i ++)
		    	array[i] = num_vars_rows + i;
		    cube = Cudd_IndicesToCube(manager, array, num_vars_cols);
		    Cudd_Ref (cube);

		    /* Ueber die Variablen der Spalten wird Existenz-quantifiziert */
		    tmp = Cudd_bddExistAbstract(manager, vec->bdd, cube);
		    Cudd_Ref (tmp);

		    rvBddCountMinterm(manager, tmp, num_vars_rows, rop);

		    Cudd_RecursiveDeref(manager, cube);
		    Cudd_RecursiveDeref(manager, tmp);
		}
		else {
			rvBddCountMinterm(manager, vec->bdd, num_vars_rows, rop);
		}
	}
	else /* not a vector */ { }
}

/****************************************************************************/
/* NAME: rvRelationCountEntries                                             */
/* FUNKTION: Berechnet in einer Relation die Anzahl der Eins-Stellen        */
/* UEBERGABEPARAMETER: Relation                                             */
/* RUECKGABEWERT: Grosse Zahl                                               */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 13-MAR-2002                                                          */
/****************************************************************************/
void rvRelationCountEntries (KureRel * rel, mpz_t rop)
{
	if (rel) {
		int nvars = Kure_num_vars(rel->rows) + Kure_num_vars(rel->cols);
		rvBddCountMinterm(rel->context->manager, rel->bdd, nvars, rop);
	}
}



