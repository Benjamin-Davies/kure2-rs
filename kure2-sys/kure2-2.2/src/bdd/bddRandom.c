#include "KureImpl.h"
#include <string.h> // memset

#define ITE(var,t,e) Cudd_bddIte(manager, (var), (t), (e))
#define ITH(k) Cudd_bddIthVar(manager, (k))
#define REF(node) Cudd_Ref(node)
#define ITER_DEREF(node)   Cudd_IterDerefBdd(manager, (node))

/*!
 * Creates a random relation with the given number of rows (lines) and columns
 * in the given context. Each pair (edge) appears with the given probability.
 *
 * \author Ulf Milanese, Stefan Bolus
 * \date Jan. 2002
 * \param context The context in which the BDD is created.
 * \param lines The number of lines for the relation.
 * \param columns The number columns for the relation.
 * \param prob The probability for each bit to be set. Has to be in [0,1].
 * \param random_func A unary (random) function which returns values in [0,1].
 * \param user_data Data passed to the random function.
 */
DdNode * random_bdd(KureContext * context, int lines, int columns, float prob,
		KureRandomFunc random_func, void * user_data)
{
	DdManager * manager = context->manager;

	if (prob <= .0f) {
		/* Falls die Wahrscheinlichkeit einer Kante gleich Null ist, geben wir
		 * die logische Null zurueck. */
		return Cudd_ReadLogicZero(manager);
	}
	else if (prob >= 1.0f) {
		/* Falls die Wahrscheinlichkeit einer Kante gleich Eins ist, geben wir
		 * die All-Relation zurueck. */
		mpz_t _cols, _rows;
		DdNode * ret;

		mpz_init_set_si (_rows, lines);
		mpz_init_set_si (_cols, columns);
		ret = L_bdd(context, _rows, _cols);
		mpz_clear(_rows);
		mpz_clear(_cols);
		return ret;
	}
	else {
		DdNode * tmp;
		DdNode * one = Cudd_ReadOne(manager);
		DdNode * zero = Cudd_Not(one);

		char * state;
		DdNode ** result;

		int var_columns    = number_of_vars2(columns);
		int var_lines      = number_of_vars2(lines);
		int nvars          = var_columns + var_lines;
		int rest_in_row    = columns;
		int columns_is_pow = int_is_pow2(columns);
		int lines_is_pow   = int_is_pow2(lines);

		int ready = FALSE;

		state  = alloca(nvars);
		memset(state, 0, nvars);
		result = alloca(nvars * sizeof(DdNode*));

		while ( !ready) {
			DdNode * g;
			int i;
			float value = random_func(user_data);

			/* Ist die Zufallszahl kleiner der angegebenen Wahrscheinlichkeit, */
			/* wird die logische Eins erzeugt, ansonsten die Null */
			tmp = (value < prob) ? one : zero;
			Cudd_Ref(tmp);

			/* In der WHILE-Schleife wird das binaere Inkrementieren durchgefuehrt */
			/* Fuer jedes Bit, das sich aendert, wird eine BDD-Variable benoetigt. */
			/* Sind wir noch nicht am Ende und es gibt einen Uebertrag? */
			for ( i = nvars-1 ; i>=0 && state[i] ; --i, tmp = g) {
				state[i] = FALSE;

				/* In result [nvars] stehen nur Konstanten, in allen anderen
				 result daraus zusammengesetzte BDDs. */
				g = ITE(ITH(i), tmp, result[i]);
				REF(g);
				ITER_DEREF(tmp);
				ITER_DEREF(result[i]);
			}

			/* Falls wir noch nicht am Ende sind, wird das Zwischenergebnis
			 gespeichert. */
			if (i >= 0) {
				result[i] = tmp;
				state[i] = TRUE;

				/* Falls wir mit einer Zeile fertig sind, dann muessen wir an den
				 Anfang der naechsten Zeile springen */
				rest_in_row--;
				if (0 == rest_in_row) {
					/* Falls die Anzahl der Spalten keine Zweier-Potenz ist, muessen
					 wir noch mit Nullen auffuellen */
					if (!columns_is_pow) {
						int j;

						tmp = zero;
						Cudd_Ref(tmp);
						for (j = nvars-1; j >= var_lines; --j) {
							if ( !state[j]) {
								g = ITE(ITH(j), zero, tmp);
								REF(g);
								ITER_DEREF(tmp);
							} else {
								g = ITE(ITH(j), tmp, result[j]);
								state[j] = FALSE;
								REF(g);
								ITER_DEREF(tmp);
								ITER_DEREF(result[j]);
							}
							tmp = g;
						}

						/* tmp enthaelt nun den BDD, der die aktuelle Zeile repraesentiert */
						/* Nun muss der Sprung in die naechste Zeile simuliert werden */
						/* Sind wir noch nicht am Ende und es gibt einen Uebertrag? */
						for ( i = var_lines-1 ; i >= 0 && state[i] ; -- i) {
							state[i] = 0;
							g = ITE(ITH(i), tmp, result[i]);
							REF(g);
							ITER_DEREF(tmp);
							ITER_DEREF(result[i]);
							tmp = g;
						}
						if (i >= 0) {
							result[i] = tmp;
							state[i] = TRUE;
						} else
							ready = TRUE;
					}
					rest_in_row = columns;
					lines--;

					/* Falls dies die letzte Zeile war und lines keine Zweier-Potenz
					 ist, dann muessen wir auch hier mit Nullen auffuellen */
					if (lines == 0) {
						if (!lines_is_pow) {
							int j;

							tmp = zero;
							Cudd_Ref (zero);
							for (j = var_lines-1; j >= 0; --j) {
								if ( !state[j]) {
									g = ITE(ITH(j), zero, tmp);
									REF(g);
									ITER_DEREF(tmp);
								} else {
									state[j] = 0;
									g = ITE(ITH(j), tmp, result[j]);
									REF(g);
									ITER_DEREF(tmp);
									ITER_DEREF(result[j]);
								}
								tmp = g;
							}
							/* tmp enthaelt nun den BDD, der die Relation repraesentiert */
						}
						ready = TRUE;
					}
				} /* 0 == rest_on_row */
			} else
				/* Ansonsten wird in tmp das Endergebnis gespeichert. */
				ready = TRUE;
		} // while not ready

		Cudd_Deref(tmp);
		return (tmp);
	}
}


/*!
 * Computes a cycle-free graph with the given number of nodes in which each
 * edge appears with probability prob. The BDD is created in the given context.
 * The random function is as in \ref random_bdd.
 *
 * \author Ulf Milanese, Stefan Bolus
 * \date Feb. 2002
 * \param context The context in which the BDD is created.
 * \param lines The number of lines/columns for the relation. Can be regarded
 *              as the number of nodes in the graph.
 * \param prob The probability for each edge to appear. Has to be in [0,1].
 * \param random_func A unary (random) function which returns values in [0,1].
 * \param user_data Data passed to the random function.
 */
DdNode * rvBddRandomCF (KureContext * context, int lines, float prob,
		KureRandomFunc random_func, void * user_data)
{
	DdManager * manager = context->manager;
	DdNode * one = Cudd_ReadOne(manager);

	/* Return an empty graph in case of probability equals 0. The opposite is
	 * not true if prob=1, because we could have cycles. */
	if (prob <= .0)
		return Cudd_Not(one);
	else {
		int vars_rows = number_of_vars2(lines);
		int num_of_var = 2 * vars_rows;
		DdNode * tmp = NULL;
		DdNode * result[num_of_var];
		Kure_bool state[num_of_var];
		Kure_bool ready = 0;

		int actual = 0;
		int rest_in_row = 1;
		int lines_is_pow = int_is_pow2(lines);

		memset (state, 0, sizeof(Kure_bool) * num_of_var);

		while ( !ready) {
			int i;

			float value = random_func (user_data);

			if ((value < prob) && (actual != 0))
				tmp = one;
			else
				tmp = Cudd_Not(one);
			Cudd_Ref(tmp);

			/* In der WHILE-Schleife wird das binaere Inkrementieren durchgefuehrt */
			/* Fuer jedes Bit, das sich aendert, wird eine BDD-Variable benoetigt. */

			/* Sind wir noch nicht am Ende und es gibt einen Uebertrag? */
			for (i = num_of_var - 1 ; i >= 0 && state[i] ; --i)
			{
				/* In result [num_of_var] stehen nur Konstanten, in allen anderen
				 result daraus zusammengesetzte BDDs. */
				DdNode * foo = Cudd_bddIte(manager, ITH(i), tmp, result[i]);
				Cudd_Ref(foo);
				ITER_DEREF(tmp);
				ITER_DEREF(result[i]);
				tmp = foo;
				state[i] = FALSE;
			}

			/* Falls wir noch nicht am Ende sind, wird das Zwischenergebnis
			 gespeichert. */
			if (i >= 0)
			{
				result[i] = tmp;
				state[i] = TRUE;

				/* Falls wir mit einer Zeile fertig sind, dann muessen wir an den
				 Anfang der naechsten Zeile springen */
				rest_in_row--;
				if (rest_in_row == 0)
				{
					rest_in_row = ++actual;
					/* Falls die Anzahl der Spalten keine Zweier-Potenz ist, muessen
					 wir noch mit Nullen auffuellen */
					tmp = Cudd_Not(one);
					Cudd_Ref(tmp);

					for (i = num_of_var-1; i >= vars_rows; --i)
					{
						DdNode * foo;

						if ( !state[i])
						{
							foo = Cudd_bddIte(manager, ITH(i), Cudd_Not(one), tmp);
							Cudd_Ref(foo);
						}
						else
						{
							foo = Cudd_bddIte(manager, ITH(i), tmp, result[i]);
							Cudd_Ref(foo);
							ITER_DEREF(result[i]);
							state[i] = FALSE;
						}
						ITER_DEREF(tmp);
						tmp = foo;
					}

					/* tmp enthaelt nun den BDD, der die aktuelle Zeile repraesentiert */
					/* Nun muss der Sprung in die naechste Zeile simuliert werden */
					/* Sind wir noch nicht am Ende und es gibt einen Uebertrag? */
					for (i = vars_rows-1 ; i >= 0 && state[i] ; --i)
					{
						DdNode * foo = Cudd_bddIte(manager, ITH(i), tmp, result[i]);
						Cudd_Ref(foo);
						ITER_DEREF(tmp);
						ITER_DEREF(result[i]);
						tmp = foo;
						state[i] = FALSE;
					}

					if (i >= 0)
					{
						result[i] = tmp;
						state[i] = TRUE;
					}
					else
						ready = TRUE;

					lines--;
					/* Falls dies die letzte Zeile war und lines keine Zweier-Potenz
					 ist, dann muessen wir auch hier mit Nullen auffuellen */
					if (lines == 0)
					{
						if (!lines_is_pow)
						{
							tmp = Cudd_Not(one);
							Cudd_Ref(tmp);
							for (i = vars_rows-1 ; i >= 0 ; --i)
							{
								DdNode * foo;
								if ( !state[i])
								{
									foo = Cudd_bddIte(manager, ITH(i), Cudd_Not(one), tmp);
									Cudd_Ref(foo);
								}
								else
								{
									foo = Cudd_bddIte(manager, ITH(i), tmp, result[i]);
									Cudd_Ref(foo);
									ITER_DEREF(result[i]);
									state[i] = FALSE;
								}
								ITER_DEREF(tmp);
								tmp = foo;
							}
							/* tmp enthaelt nun den BDD, der die Relation repraesentiert */
						}
						ready = TRUE;
					}
				}
			}
			else
				/* Ansonsten wird in tmp das Endergebnis gespeichert. */
				ready = TRUE;
		}

		if (tmp) Cudd_Deref (tmp);
		return (tmp);
	}
}





/*!
 * Computes a random permutation relation, i.e. a random bijective function
 * f: R->R s.t. |R|=lines, in the given context. The random function is as
 * in \ref random_bdd.
 *
 * \author Ulf Milanese, Stefan Bolus
 * \date Feb. 2002
 * \param context The context in which the BDD is created.
 * \param lines The number of lines/columns for the relation. Can be regarded
 *              as the functions domain.
 * \param random_func A unary (random) function which returns values in [0,1].
 * \param user_data Data passed to the random function.
 */
DdNode * rvBddRandomPerm (KureContext * context, int lines,
		KureRandomFunc random_func, void * user_data)
{
	DdManager * manager = context->manager;
	int var_lines = number_of_vars2(lines);
	int num_of_var = 2 * var_lines;
	int i;
	DdNode * perm;
	int * perm_ass;
	int * perm_ind;
	int * array;
	int actual = 0;

	/* Array und cube werden fuer die Permutation benoetigt. */
	array = (int *) malloc(lines * sizeof(int));

	/* Berechnung der Permutation */
	perm_ass = (int *) alloca(num_of_var * sizeof(int));
	perm_ind = (int *) alloca(num_of_var * sizeof(int));

	printf ("rvBddRandomPerm: lines(param)=%d, var_lines=%d, num_of_var=%d\n",
			lines, var_lines, num_of_var);

	/* Initialisierungen */
	for (i = 0; i < lines; i++)
		array[i] = i;
	for (i = 0; i < num_of_var; i++)
		perm_ind[i] = i;

	perm = null_bdd(manager);
	Cudd_Ref(perm);

	for (actual = lines-1 ; actual >= 0 ; actual --)
	{
		DdNode * tmp, *minterm;

		float value = random_func (user_data);
		int j = (float) (actual+1) * value;
		int permuted = array[j];
		int original = actual;
		int k;

		for (k = 1; k <= var_lines; k++)
		{
			perm_ass[var_lines - k] = original & 0x1;
			original >>= 1;
			perm_ass[(var_lines << 1) - k] = permuted & 0x1;
			permuted >>= 1;
		}

		minterm = rvIntlist2Bdd (manager, perm_ind, perm_ass, num_of_var);
		Cudd_Ref (minterm);
		tmp = Cudd_bddOr(manager, minterm, perm);
		Cudd_Ref (tmp);
		Cudd_IterDerefBdd(manager, perm);
		Cudd_IterDerefBdd(manager, minterm);
		perm = tmp;

		/* Das array der noch nicht benutzten Permutationswerte wird
		 aktualisiert. */
		for (k = j; k <= actual; k++)
			array[k] = array[k + 1];
	}

	free(array);

	if (perm) Cudd_Deref (perm);
	return (perm);
}
