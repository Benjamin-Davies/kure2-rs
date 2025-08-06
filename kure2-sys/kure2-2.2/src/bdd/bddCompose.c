#include "KureImpl.h"

/****************************************************************************/
/* NAME: komp_bdd                                                           */
/* FUNKTION: Komposition zweier Relationen, die als BDDs dargestellt sind   */
/* UEBERGABEPARAMETER: DdNode * (1. Relation), DdNode * (2. Relation),      */
/*                     Kure_MINT * (Zeilen der 1. Relation), Kure_MINT * (Spalten)    */
/* RUECKGABEWERT: DdNode * (Ergebnis-BDD)                                   */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 03.08.1999                                                           */
/* LETZTE AENDERUNG AM: 03.08.1999                                          */
/* 08.03.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
DdNode * komp_bdd (DdManager * manager,
				   DdNode * first_bdd, DdNode * second_bdd,
                   mpz_t ROWS, mpz_t COLS)
{
	int number_vars_rows = Kure_num_vars(ROWS);
	int number_vars_columns = Kure_num_vars(COLS);
	DdNode * f,*h;

	/* Der Variablen des zweiten BDDs werden geshiftet */
	f = shift_bdd(manager, second_bdd, 0, 0, number_vars_rows);
	Cudd_Ref (f);
	h = NULL;

	if (number_vars_columns > 0) {
		DdNode *g,*cube;
		int * array;
		int i;

		/* Zur schnelleren Abarbeitung wird die Moeglichkeit zur Ausfuehrung
		 * der Garbage Collection ausgeschaltet */
		Cudd_DisableGarbageCollection(manager);

		/* first_bdd und tmp_bdd1 werden miteinander multipliziert */
		g = Cudd_bddAnd(manager, first_bdd, f);
		Cudd_Ref (g);
		Cudd_RecursiveDeref(manager, f);

		/* Es wird ein Array erzeugt mit den Indizes der Variablen,
		 * ueber die quantifiziert werden muss */
		array = (int *) malloc(number_vars_columns * sizeof(int));
		for (i = 0 ; i < number_vars_columns; i++)
			array[i] = number_vars_rows + i;
		cube = Cudd_IndicesToCube (manager, array, number_vars_columns);
		Cudd_Ref (cube);
		free (array);

		h = Cudd_bddExistAbstract (manager, g, cube);
		Cudd_Ref (h);

		Cudd_RecursiveDeref (manager, g);
		Cudd_RecursiveDeref (manager, cube);

		/* Nach Ausfuehrung der Komposition wird die Garbage Collection */
		/* wieder ermoeglicht */
		Cudd_EnableGarbageCollection(manager);
	}
	else {
		/* Falls keine Variablen ausquantifiziert werden muessen */
		h = Cudd_bddAnd (manager, first_bdd, f);
		Cudd_Ref (h);
		Cudd_RecursiveDeref(manager, f);
	}

	{
		DdNode * ret = shift_bdd(manager, h, number_vars_rows, 0, -number_vars_columns);
		Cudd_Ref (ret);
		Cudd_RecursiveDeref (manager, h);
		Cudd_Deref (ret);
		return ret;
	}
}


/****************************************************************************/
/* NAME: komp_bdd_transp_norm                                               */
/* FUNKTION: Komposition zweier Relationen, die als BDDs dargestellt sind   */
/*           (1. transponiert, 2. nicht transponiert)                       */
/* UEBERGABEPARAMETER: DdNode * (1. Relation), DdNode * (2. Relation),      */
/*                     Kure_MINT * (Zeilen), Kure_MINT int (Spalten der 1. Relation)  */
/* RUECKGABEWERT: DdNode * (Ergebnis-BDD)                                   */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 24.01.2000                                                           */
/* LETZTE AENDERUNG AM: 24.01.2000                                          */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
DdNode * komp_bdd_transp_norm (DdManager * manager, DdNode * first_bdd,
							   DdNode * second_bdd,
                               mpz_t ROWS, mpz_t COLUMNS)
{
	int number_vars_rows = Kure_num_vars (ROWS);
	int number_vars_columns = Kure_num_vars (COLUMNS);
	DdNode * f,*h;

	f = shift_bdd(manager, second_bdd, number_vars_rows, 0, number_vars_columns);
	Cudd_Ref (f);
	h = NULL;

	if (number_vars_rows > 0) {
		DdNode *g, *cube;
		int i;
		int * array;

		/* Zur schnelleren Abarbeitung wird die Moeglichkeit zur Ausfuehrung *
		 * der Garbage Collection ausgeschaltet */
		Cudd_DisableGarbageCollection(manager);

		g = Cudd_bddAnd(manager, first_bdd, f);
		Cudd_Ref(g);
		Cudd_RecursiveDeref(manager, f);

		/* Es wird ein Array erzeugt mit den Indizes der Variablen,
		 * ueber die quantifiziert werden muss */
		array = (int *) malloc(number_vars_rows * sizeof(int));
		for (i = 0; i < number_vars_rows; i++)
			array[i] = i;
		cube = Cudd_IndicesToCube(manager, array, number_vars_rows);
		Cudd_Ref(cube);
		free(array);

		h = Cudd_bddExistAbstract(manager, g, cube);
		Cudd_Ref(h);

		Cudd_RecursiveDeref(manager, g);
		Cudd_RecursiveDeref(manager, cube);

		Cudd_EnableGarbageCollection(manager);
	}
	else {
		/* Falls keine Variablen ausquantifiziert werden muessen */
		h = Cudd_bddAnd(manager, first_bdd, f);
		Cudd_Ref(h);
		Cudd_RecursiveDeref(manager, f);
	}

	{
		/* Die Variablen der Schnittmenge werden aus dem Resultat entfernt */
		DdNode * ret = shift_bdd(manager, h, 0, 0, -number_vars_rows);
		Cudd_Ref (ret);
		Cudd_RecursiveDeref(manager, h);
		Cudd_Deref(ret);
		return ret;
	}
}


/****************************************************************************/
/* NAME: komp_bdd_norm_transp                                               */
/* FUNKTION: Komposition zweier Relationen, die als BDDs dargestellt sind   */
/*           (1. nicht transponiert, 2. transponiert)                       */
/* UEBERGABEPARAMETER: DdNode * (1. Relation), DdNode * (2. Relation),      */
/*                     Kure_MINT * (Zeilen der 1. Relation), Kure_MINT * (Zeilen der  */
/*                     2. Relation), Kure_MINT * (Spalten der Relationen)        */
/* RUECKGABEWERT: DdNode * (Ergebnis-BDD)                                   */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 25.01.2000                                                           */
/* LETZTE AENDERUNG AM: 25.01.2000                                          */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
DdNode * komp_bdd_norm_transp (DdManager * manager,
		DdNode * first_bdd, DdNode * second_bdd,
        mpz_t FIRST_ROWS, mpz_t SECOND_ROWS, mpz_t COLUMNS)
{
	int number_vars_first_rows = Kure_num_vars(FIRST_ROWS);
	int number_vars_second_rows = Kure_num_vars(SECOND_ROWS);
	int number_vars_columns = Kure_num_vars(COLUMNS);
	DdNode *f,*k, *ret = NULL;

	f = shift_bdd (manager, first_bdd, number_vars_first_rows, 0,
            number_vars_second_rows);
	Cudd_Ref (f);
	k = shift_bdd (manager, second_bdd, 0, 0, number_vars_first_rows);
	Cudd_Ref(k);

	if (number_vars_columns > 0) {
		DdNode *g, *cube;
		int i;
		int * array;

	    /* Zur schnelleren Abarbeitung wird die Moeglichkeit zur Ausfuehrung */
	    /* der Garbage Collection ausgeschaltet */
	    Cudd_DisableGarbageCollection (manager);

	    g = Cudd_bddAnd (manager, f, k);
	    Cudd_Ref (g);
	    Cudd_RecursiveDeref(manager, f);
	    Cudd_RecursiveDeref(manager, k);

	    /* Es wird ein Array erzeugt mit den Indizes der Variablen,
	     * ueber die quantifiziert werden muss */
	    array = (int *) malloc (number_vars_columns * sizeof (int));
	    for (i = 0; i < number_vars_columns; i ++)
	    	array [i] = number_vars_first_rows + number_vars_second_rows + i;
	    cube = Cudd_IndicesToCube(manager, array, number_vars_columns);
	    Cudd_Ref (cube);
	    free (array);

	    ret = Cudd_bddExistAbstract (manager, g, cube);
	    Cudd_Ref (ret);
	    Cudd_RecursiveDeref(manager, g);
	    Cudd_RecursiveDeref(manager, cube);

	    Cudd_EnableGarbageCollection (manager);
	}
	else {
		ret = Cudd_bddAnd(manager, f,k);
		Cudd_Ref (ret);
	    Cudd_RecursiveDeref(manager, f);
	    Cudd_RecursiveDeref(manager, k);
	}

	Cudd_Deref (ret);
	return ret;
}


/****************************************************************************/
/* NAME: komp_bdd_transp_transp                                             */
/* FUNKTION: Komposition zweier Relationen, die als BDDs dargestellt sind   */
/*           (beide transponiert)                                           */
/* UEBERGABEPARAMETER: DdNode * (1. Relation), DdNode * (2. Relation),      */
/*                     Kure_MINT * (Zeilen der 1. Relation), Kure_MINT * (Zeilen der  */
/*                     der 2. Relation), Kure_MINT * (Spalten der 1. Relation)   */
/* RUECKGABEWERT: DdNode * (Ergebnis-BDD)                                   */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 24.01.2000                                                           */
/* LETZTE AENDERUNG AM: 24.01.2000                                          */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
DdNode * komp_bdd_transp_transp (DdManager * manager,
		DdNode * first_bdd, DdNode * second_bdd,
        mpz_t FIRST_ROWS, mpz_t SECOND_ROWS, mpz_t COLUMNS)
{
	int number_vars_first_rows = Kure_num_vars(FIRST_ROWS);
	int number_vars_second_rows = Kure_num_vars(SECOND_ROWS);
	int number_vars_columns = Kure_num_vars(COLUMNS);
	DdNode *f,*h;

	f = shift_bdd (manager, first_bdd, 0, 0, number_vars_second_rows);
	Cudd_Ref (f);
	h = NULL;

	if (number_vars_first_rows > 0) {
		DdNode *g,*cube;
		int i;
		int * array;

		/* Zur schnelleren Abarbeitung wird die Moeglichkeit zur Ausfuehrung */
	    /* der Garbage Collection ausgeschaltet */
	    Cudd_DisableGarbageCollection (manager);

	    g = Cudd_bddAnd (manager, f, second_bdd);
	    Cudd_Ref (g);
	    Cudd_RecursiveDeref(manager, f);

	    /* Es wird ein Array erzeugt mit den Indizes der Variablen,
	       ueber die quantifiziert werden muss */
	    array = (int *) malloc (number_vars_first_rows * sizeof (int));
	    for (i = 0; i < number_vars_first_rows; i ++)
	    	array [i] = number_vars_second_rows + i;
	    cube = Cudd_IndicesToCube(manager, array, number_vars_first_rows);
	    Cudd_Ref (cube);
	    free (array);

	    h = Cudd_bddExistAbstract(manager, g, cube);
	    Cudd_Ref (h);
	    Cudd_RecursiveDeref(manager, g);
	    Cudd_RecursiveDeref(manager, cube);

	    Cudd_EnableGarbageCollection (manager);
	}
	else {
		h = Cudd_bddAnd (manager, f, second_bdd);
		Cudd_Ref (h);
		Cudd_RecursiveDeref(manager, f);
	}


	{
		DdNode * ret = shift_bdd (manager, h, number_vars_second_rows, number_vars_columns,
                -number_vars_first_rows - number_vars_second_rows);
		Cudd_Ref (ret);
		Cudd_RecursiveDeref(manager, h);
		Cudd_Deref (ret);
		return ret;
	}
}
