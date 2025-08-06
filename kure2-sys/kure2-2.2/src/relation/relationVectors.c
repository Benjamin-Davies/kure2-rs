#include "KureImpl.h"


/****************************************************************************/
/* NAME: dom_rel                                                            */
/* FUNKTION: berechnet die Domain einer Relation                            */
/* UEBERGABEPARAMETER: 2 x Rel * (Relationen), Argument                     */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 05.08.1999 (Umstellung auf BDD-Version)                                  */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// Input: Any relation
// Output: rows:=rows(R), cols:=1
void dom_rel (KureRel * relR, KureRel * rop)
{
	DdManager * manager = relR->context->manager;

	mpz_set(rop->rows, relR->rows);
	mpz_set_si(rop->cols, 1);

	/* Simple case? R is equal to its domain. */
	if (mpz_cmp_si(relR->cols, 1) == 0) {
		/* This part also works, if relR equals rop. */
		Cudd_Ref(relR->bdd);
		Cudd_RecursiveDeref(manager, rop->bdd);
		rop->bdd = relR->bdd;
	}
	else {
		int num_vars_rows = Kure_num_vars(relR->rows);
		int num_vars_cols = Kure_num_vars(relR->cols);
		int * array, i;
		DdNode * cube, *res;

		 /* Es wird ein Array erzeugt mit den Indizes der Variablen,
		       ueber die quantifiziert werden muss */
		array = (int *) alloca (num_vars_cols * sizeof (int));
		for (i = 0; i < num_vars_cols; i ++)
			array [i] = num_vars_rows + i;
		cube = Cudd_IndicesToCube(manager, array, num_vars_cols);
		Cudd_Ref(cube);


	    /* Ueber die Variablen der Spalten wird Existenz-quantifiziert */
		res = Cudd_bddExistAbstract(manager, relR->bdd, cube);
		Cudd_Ref(res);
		Cudd_RecursiveDeref(manager, cube);
		Cudd_RecursiveDeref(manager, rop->bdd);
		rop->bdd = res;
	}
}


/****************************************************************************/
/* NAME: init_vector                                                        */
/* FUNKTION: besetzt eine Relation mit 1. Zeile                             */
/* UEBERGABEPARAMETER: Rel * (die Relation)                                 */
/* RUECKGABEWERT: TRUE falls ja, sonst FALSE                                */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 08.11.95                                                             */
/* LETZTE AENDERUNG AM: 21.02.97 (Aenderung von init_vector auf init)       */
/* 13.04.2000 (Benutzung von BDD-Routinen)                                  */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
void init_vector (KureRel * rop, mpz_t rows, mpz_t cols)
{
	DdManager * manager = rop->context->manager;
	int num_vars_rows = Kure_num_vars(rows);
	int i;
	DdNode * row;

	row = L_row_bdd (rop->context, cols, num_vars_rows);
	Cudd_Ref(row);

	for (i = num_vars_rows - 1; i >= 0; i--) {
		DdNode * tmp = and_bdd(manager, Cudd_Not (Cudd_bddIthVar(manager, i)), row);
		Cudd_Ref(tmp);
		Cudd_RecursiveDeref(manager, row);
		row = tmp;
	}
	Cudd_RecursiveDeref(manager, rop->bdd);
	rop->bdd = row;

	mpz_set (rop->rows, rows);
	mpz_set (rop->cols, cols);
}


/****************************************************************************/
/* NAME: next_vector                                                        */
/* FUNKTION: besetzt einen Vector mit der naechsten Zeile nach der          */
/*           besetzten                                                      */
/* UEBERGABEPARAMETER: Rel * (der Vector)                                   */
/* RUECKGABEWERT: TRUE falls ja, sonst FALSE                                */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 08.11.95                                                             */
/* LETZTE AENDERUNG AM: 08.11.95                                            */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// If the last row was set, the relation is set empty.
// Sets result to 0 and sets the successor, if there is one. Sets nothing
// otherwise. FALSE is only returned on error, e.g. if rel is not a point.
Kure_bool next_vector (KureRel * rel, KureRel * result)
{
	DdManager * manager = rel->context->manager;
	int i, rows;
	int complete_lines = 0;
	int last;

	// Avoid an integer overflow.
	if (! mpz_fits_sint_p(rel->rows)) {
		rows = INT_MAX;
	}
	else rows = mpz_get_si(rel->rows);

	/* Count the number of lines completely set ... */
	for ( i = 0 ; i < rows ; ++ i) {
		if (test_line_complete (rel, 0, i, KURE_DIR_LEFT_RIGHT)) {
			complete_lines ++;
			last = i;
		}
	}

	/* ... This value must be one here. */
	if (complete_lines != 1) {
		return FALSE;
	}
	else
	{

		/* We don't need rel anymore, so it's now safe to remove the
		 * reference of the result even if rel = result. */
		Cudd_RecursiveDeref(manager, result->bdd);
		result->bdd = Cudd_Not(Cudd_ReadOne(manager));
		Cudd_Ref(result->bdd);

		/* Must be set before set_rel_line is called. */
		mpz_set (result->rows, rel->rows);
		mpz_set (result->cols, rel->cols);

		if (last < rows - 1)
		{
			mpz_t mp_row;
			mpz_init_set_si(mp_row, last + 1);
			set_rel_line(result, mp_row);
			mpz_clear(mp_row);
		}

		return TRUE;
	}
}


/****************************************************************************/
/* NAME: succ_vector                                                        */
/* FUNKTION: liefert eine Rel mit Vector-hoehe X Vector-hoehe               */
/*           mit besetzter Nebendiagonale ab 0 / 1                          */
/* UEBERGABEPARAMETER: Rel * (der Vector)                                   */
/* RUECKGABEWERT: TRUE falls erfolgreich, sonst FALSE                       */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 08.11.95                                                             */
/* LETZTE AENDERUNG AM: 08.03.2000 (Benutzung von next_bdd)                 */
/****************************************************************************/
// Output: size x size. Adds (ORs) the nebendiagonale.
void succ_vector (KureRel * rop, mpz_t size)
{
	DdManager * manager = rop->context->manager;
	DdNode *f = NULL, *g = NULL;

	f = next_bdd(rop->context, size);
	Cudd_Ref(f);
	g = or_bdd(manager, f, rop->bdd);
	Cudd_Ref(g);
	Cudd_RecursiveDeref(manager, f);
	Cudd_RecursiveDeref(manager, rop->bdd);
	rop->bdd = g;

	mpz_set(rop->rows, size);
	mpz_set(rop->cols, size);
}


/****************************************************************************/
/* NAME: point                                                              */
/* FUNKTION: liefert in (Rel *) result eine Relation                        */
/*           r muss zeilenkonstant sein -> erste Zeile                      */
/* UEBERGABEPARAMETER: Rel * r (die Relation) und Rel * (die Ergebnisrel)   */
/* RUECKGABEWERT: TRUE falls erfolgreich, sonst FALSE                       */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 16.11.95                                                             */
/* LETZTE AENDERUNG AM: 11.12.96 (Umbenennung elem_vector -> point)         */
/*                      Ergebnis zeilenkonstant                             */
/* 30.08.1999 (Benutzung der CUDD-Routinen fuer BDDs)                       */
/* 16.09.1999 (Es wird immer das erste Element ausgegeben)                  */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// Returns FALSE, if the rel is empty.
Kure_bool point (KureRel * arg, KureRel * rop)
{
	DdManager * manager = arg->context->manager;
	DdNode * one = Cudd_ReadOne(manager);

	if (arg->bdd == Cudd_Not(one)) {
		/* Empty vector. */
		return FALSE;
	}
	else {
		  int number_vars_rows = Kure_num_vars(arg->rows);
		  mpz_t pos, cols;
		  DdNode * L_row;

		  mpz_init_set_si(pos,0);
		  mpz_init_set_si(cols,1);

		  L_row = L_row_bdd (rop->context, arg->cols, number_vars_rows);
		  Cudd_Ref(L_row);

		  if (arg->bdd == one) {
			  DdNode * f = make_minterm (manager, cols, arg->rows, pos,pos);
			  Cudd_Ref(f);
		      Cudd_RecursiveDeref(manager,rop->bdd);
		      rop->bdd = and_bdd (manager, f, L_row);
		      Cudd_Ref(rop->bdd);
		      Cudd_RecursiveDeref(manager, f);
		  }
		  else {
			  int number_vars_cols = Kure_num_vars(arg->cols);
			  int * cube, i;
			  mpz_t row, tmp;
			  DdNode * f;

			  mpz_init (row);
			  mpz_init (tmp);
			  cube = first_fulfill_intlist (manager, arg->bdd,
			                                number_vars_rows + number_vars_cols);

			  for (i=0; i < number_vars_rows ; ++ i) {
				  if (1 == cube[number_vars_rows-i-1]) {
					  mpz_ui_pow_ui (tmp, 2, i);
					  mpz_add (row, row, tmp); // row += 2^i
				  }
			  }

			  free (cube);

			  mpz_set_si(pos, 0);
			  mpz_set_si(cols, 1);

			  f = make_minterm (manager, cols, arg->rows, pos, row);
			  Cudd_Ref(f);
			  Cudd_RecursiveDeref(manager, rop->bdd);
			  rop->bdd = and_bdd (manager, f, L_row);
			  Cudd_Ref(rop->bdd);
			  Cudd_RecursiveDeref(manager, f);

			  mpz_clear(row);
			  mpz_clear(tmp);
		  }

		  Cudd_RecursiveDeref(manager, L_row);

		  mpz_clear(pos);
		  mpz_clear(cols);

		  mpz_set (rop->rows, arg->rows);
		  mpz_set (rop->cols, arg->cols);

		  return TRUE;
	}
}



/****************************************************************************/
/* NAME: atom                                                               */
/* FUNKTION: liefert in (Rel *) result eine Relation                        */
/*           mit einem Eintrag aus r                                        */
/* UEBERGABEPARAMETER: Rel * r (die Relation) und Rel * (die Ergebnisrel)   */
/* RUECKGABEWERT: TRUE falls erfolgreich, sonst FALSE                       */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 16.11.95                                                             */
/* LETZTE AENDERUNG AM: 11.12.96 (Umbenennung elem -> point)                */
/* 30.08.1999 (Benutzung der CUDD-Routinen fuer BDDs)                       */
/* 16.09.1999 (Es wird immer das erste Element ausgegeben)                  */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
Kure_bool atom (KureRel * arg, KureRel * rop)
{
	DdManager * manager = arg->context->manager;
	DdNode * one = Cudd_ReadOne(manager);

	if (arg->bdd == Cudd_Not(one)) {
		/* Empty relation. */
		return FALSE;
	}
	else {
		mpz_set (rop->rows, arg->rows);
		mpz_set (rop->cols, arg->cols);

		if (arg->bdd == one) {
			Cudd_RecursiveDeref(manager, rop->bdd);
			rop->bdd = make_minterm_si (manager, arg->cols, arg->rows, 0,0);
			Cudd_Ref(rop->bdd);
		}
		else {
			int number_vars_rows = Kure_num_vars(arg->rows);
			int number_vars_cols = Kure_num_vars (arg->cols);
			int * cube = first_fulfill_intlist (manager, arg->bdd,
					number_vars_rows + number_vars_cols);
			mpz_t row, col, pot;
			int i;

			mpz_init_set_ui (pot, 1/*2^0*/);
			mpz_init (row);
			mpz_init (col);

			for (i = 0; i < number_vars_rows; i++) {
				if (1 == cube[number_vars_rows - i - 1])
					mpz_add(row, row, pot); // row += 2^i
				mpz_mul_ui(pot, pot, 2); // pot *= 2 (=>2^i)
			}
			mpz_set_ui(pot, 1/*2^0*/);
			for (i = 0; i < number_vars_cols; i++) {
				if (1 == cube[number_vars_cols - i - 1 + number_vars_rows])
					mpz_add(col, col, pot); // col += 2^i
				mpz_mul_ui(pot, pot, 2); // pot *= 2 (=>2^i)
			}

			free (cube);

			Cudd_RecursiveDeref(manager, rop->bdd);
			rop->bdd = make_minterm(manager, rop->cols, rop->rows, col, row);
			Cudd_Ref(rop->bdd);

			mpz_clear (row);
			mpz_clear (col);
			mpz_clear (pot);
		}

		return TRUE;
	}
}

/****************************************************************************/
/* NAME: inj_vector                                                         */
/* FUNKTION: liefert in (Rel *) result die Injektion der Relation r         */
/* UEBERGABEPARAMETER: Rel * r (die Relation) und Rel * (die Ergebnisrel)   */
/* RUECKGABEWERT: TRUE falls erfolgreich, sonst FALSE                       */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 27.11.95                                                             */
/* LETZTE AENDERUNG AM: 07.10.1999 (Benutzung der BDD-Routinen)             */
/* 01.08.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
Kure_bool inj_vector  (KureRel * arg, KureRel * rop)
{
	DdManager * manager = arg->context->manager;
	DdNode * one = Cudd_ReadOne(manager);
	int number_vars_rows = Kure_num_vars(arg->rows);
	int number_vars_cols = Kure_num_vars(arg->cols);
	int * array, i;
	DdNode * f, *g;
	mpz_t row, col, pot, nat_zero, nat_one;
	mpz_t rows; // of the result

	/* Initialize the result to Zero. */
	Cudd_IterDerefBdd(manager, rop->bdd);
	rop->bdd = Cudd_Not(one);
	Cudd_Ref(rop->bdd);

	/* If the given vector has more than just one column, then f becomes
	 * arg's first column. */
	if (number_vars_cols != 0) {
		DdNode * cube;

		array = (int *) malloc (number_vars_cols * sizeof(int));
		for (i = 0; i < number_vars_cols; i++)
			array[i] = number_vars_rows + i;
		cube = Cudd_IndicesToCube (manager, array, number_vars_cols);
		Cudd_Ref (cube);
		free (array); array = NULL;

	    /* Ueber die Variablen der Spalten wird Existenz-quantifiziert */
		f  = Cudd_bddExistAbstract (manager, arg->bdd, cube);
		Cudd_Ref(f);
		Cudd_RecursiveDeref(manager, cube);
	}
	else {
		f = arg->bdd;
		Cudd_Ref(f); // TODO: necessary?
	}

	/* The result has as many rows as the argument has completely
	 * filled rows. */
	mpz_init(rows);
	rvBddCountMinterm(manager, f, number_vars_rows, rows);
	if (mpz_cmp_ui(rows, 0) == 0) {
		printf ("ERROR (inj_vector): Result is not a vector or empty.\n");
		mpz_clear (rows);
		return FALSE;
	}

	//gmp_printf ("inj_vector: There are %Zd rows filled in vector.\n", rows);

	mpz_init(row);
	mpz_init(col);
	mpz_init(pot);
	mpz_init_set_ui(nat_zero, 0); /* just more explicit */
	mpz_init_set_ui(nat_one, 1); /* constant */
    for ( ; gmp_less(row,rows) ; mpz_add_ui(row,row,1))
    {
    	DdNode * minterm_arg, *minterm_rop;

		/* The constant 0 function has no on-set cubes. */
    	if (f == Cudd_Not(one)) {
    		// TODO: Error handling.
    		printf("ERROR (inj_vector): Not enough elements found.\n");
    		mpz_clear(row);
    		mpz_clear(col);
    		mpz_clear(pot);
    		mpz_clear(nat_zero);
    		mpz_clear(nat_one);
    		mpz_clear(rows);
    		Cudd_RecursiveDeref(manager, f);
    		return FALSE;
    	}

    	/* Die erste erfuellende Belegung wird gesucht und der entsprechende
    	 * Wert in COLUMN abgespeichert */
    	array = first_fulfill_intlist(manager, f, number_vars_rows);
    	mpz_set_si(col,0);
    	//mpz_ui_pow_ui(pot, 2, number_vars_rows-1); // pot := 2^num_vars_rows-1
		for (i = 0 ; i < number_vars_rows ; i++) {
			if (array[i]) {
				mpz_setbit (col, number_vars_rows - i - 1);
				//mpz_add (col, col, pot); // col += 2^(number_vars_rows-i-1)
				//mpz_tdiv_q_2exp (pot,pot,1); /* pot /= 2 */
			}
		}
		free(array);

		/* In tmp_1 wird diese Belegung geloescht */
		minterm_arg = make_minterm (manager, nat_one, arg->rows, nat_zero, col);
		Cudd_Ref(minterm_arg);
		g = and_bdd(manager, f, Cudd_Not(minterm_arg));
		Cudd_Ref(g);
		Cudd_RecursiveDeref(manager, f);
		Cudd_RecursiveDeref(manager, minterm_arg);
		f = g;

		/* In result->bdd wird diese Belegung hinzugefuegt */
		minterm_rop = make_minterm(manager, arg->rows, rows, col, row);
		Cudd_Ref (minterm_rop);
		g = or_bdd(manager, rop->bdd, minterm_rop);
		Cudd_Ref(g);
		Cudd_RecursiveDeref(manager, rop->bdd);
		Cudd_RecursiveDeref(manager, minterm_rop);
		rop->bdd = g;
	}

    Cudd_RecursiveDeref(manager, f);

	mpz_set (rop->cols, arg->rows);
	mpz_set (rop->rows, rows);

	mpz_clear(row);
	mpz_clear(col);
	mpz_clear(pot);
	mpz_clear(nat_zero);
	mpz_clear(nat_one);
	mpz_clear(rows);

	return TRUE;
}
