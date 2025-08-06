#include "KureImpl.h"


/****************************************************************************/
/* NAME: set_rel_line                                                       */
/* FUNKTION: besetzt bei der angegebenen Rel die Zeile mit 1'en             */
/* UEBERGABEPARAMETER: Rel * r (die Relation) Kure_MINT * (die Zeile)            */
/* RUECKGABEWERT: TRUE falls erfolgreich, sonst FALSE                       */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 14.03.96                                                             */
/* LETZTE AENDERUNG AM: 14.04.2000 (Benutzung der BDD-Routinen)             */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// row is 0-indexed
Kure_bool set_rel_line (KureRel * rel, mpz_t _row)
{
	if (!rel) return FALSE;
	else if (mpz_cmp_si(_row,0) < 0 || !gmp_less(_row,rel->rows)) return FALSE;
	else {
		DdManager * manager = rel->context->manager;
		int num_vars_rows = Kure_num_vars(rel->rows);
		DdNode * L_row = L_row_bdd(rel->context, rel->cols, num_vars_rows);

		if (L_row) {
			DdNode * tmp;
			mpz_t row;
			int i;
			Cudd_Ref (L_row);

			mpz_init_set (row, _row);
			for (i = num_vars_rows - 1 ; i >= 0 ; i --) {
				DdNode * ith = Cudd_bddIthVar(manager, i);
				if (mpz_even_p(row))
					ith = Cudd_Not(ith);
				tmp = and_bdd (manager, L_row, ith);
				if ( !tmp) {
					mpz_clear (row);
					Cudd_RecursiveDeref(manager, L_row);
					return FALSE;
				}

				Cudd_Ref (tmp);
				Cudd_RecursiveDeref(manager, L_row);
				L_row = tmp;

				mpz_tdiv_q_2exp (row,row,1); // row /= 2^2
			}

			mpz_clear (row);
			tmp = or_bdd (manager, rel->bdd, L_row);
			if ( !tmp) {
				Cudd_RecursiveDeref(manager, L_row);
				return FALSE;
			}
			Cudd_Ref(tmp);
			Cudd_RecursiveDeref(manager, L_row);
			Cudd_RecursiveDeref(manager, rel->bdd);
			rel->bdd = tmp;

			return TRUE;
		}
		else return FALSE;
	}
}


/****************************************************************************/
/* NAME: clear_rel_line                                                     */
/* FUNKTION: besetzt bei der angegebenen Rel die Zeile mit 0'en             */
/* UEBERGABEPARAMETER: Rel * r (die Relation) Kure_MINT * (die Zeile)            */
/* RUECKGABEWERT: TRUE falls erfolgreich, sonst FALSE                       */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 14.03.96                                                             */
/* LETZTE AENDERUNG AM: 14.04.2000 (Benutzung der BDD-Routinen)             */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
Kure_bool clear_rel_line (KureRel * rel, mpz_t row)
{
	if (!rel) return FALSE;
	else if (mpz_cmp_si(row,0) < 0 || !gmp_less(row,rel->rows))
		return FALSE;
	else {
		int num_vars_rows = Kure_num_vars(rel->rows);
		DdNode * L_row = L_row_bdd(rel->context, rel->cols, num_vars_rows);
		if (L_row) {
			DdManager * manager = rel->context->manager;
			int i;
			DdNode * tmp;
			mpz_t cur_row;

			Cudd_Ref (L_row);
			mpz_init_set (cur_row, row);

			for (i = num_vars_rows - 1; i >= 0; i --) {
				DdNode * ith = Cudd_bddIthVar(manager, i);

				if (mpz_even_p(cur_row))
					ith = Cudd_Not(ith);
				tmp = and_bdd(manager, L_row, ith);
				if (!tmp) {
					mpz_clear (cur_row);
					Cudd_RecursiveDeref(manager, L_row);
					return FALSE;
				}

				Cudd_Ref (tmp);
				Cudd_RecursiveDeref(manager, L_row);
				L_row = tmp;

				mpz_tdiv_q_2exp (cur_row, cur_row, 1); // cur_row /= 2^1
			}

			mpz_clear (cur_row);

		    /* Die Negation der erzeugten Zeile wird mit der Original-Relation
		     * AND-verknuepft */
			tmp = and_bdd(manager, rel->bdd, Cudd_Not(L_row));
			if (!tmp) {
				Cudd_RecursiveDeref(manager, L_row);
				return FALSE;
			}
			Cudd_Ref(tmp);
			Cudd_RecursiveDeref(manager, L_row);
			Cudd_RecursiveDeref(manager, rel->bdd);
			rel->bdd = tmp;

			return TRUE;
		}
		else return FALSE;
	}
}
