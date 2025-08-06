#include "KureImpl.h"
#include <string.h> // memset

/****************************************************************************/
/* NAME: make_minterm                                                       */
/* FUNKTION: Erzeugt einen BDD, der einen Eintrag einer Relation darstellt  */
/* UEBERGABEPARAMETER: Kure_MINT * (Anzahl der Zeilen),                     */
/*                     Kure_MINT * (Anzahl der Spalten),                    */
/*                     Kure_MINT * (Zeile), Kure_MINT * (Spalte)            */
/* RUECKGABEWERT: DdNode * (Resultat-BDD) (NULL, falls erfolglos)           */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 29.07.1999                                                           */
/* LETZTE AENDERUNG AM: 29.07.1999                                          */
/* 01.03.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/

DdNode * make_minterm (DdManager * manager, mpz_t width, mpz_t height,
		mpz_t _col, mpz_t _row)
{
	Kure_bool is_valid = (mpz_cmp_si(_row,0) >= 0 && mpz_cmp_si(_col,0) >= 0
			&& gmp_less(_row, height) && gmp_less(_col, width));

	if ( !is_valid) {
		gmp_printf ("make_minterm: Minterm invalid! (rows=%Zd x cols=%Zd, (%Zd, %Zd))\n",
				height, width, _row, _col);
		return NULL;
	}
	else {
		DdNode * minterm;
		int vars_col = Kure_num_vars(width);
		int vars_row = Kure_num_vars(height);
		int i;
		mpz_t col, row;

		mpz_init_set (col, _col);
		mpz_init_set (row, _row);

		minterm = Cudd_ReadOne(manager);
		Cudd_Ref(minterm);

		for (i = vars_col + vars_row - 1 ; i >= vars_row ; --i) {
			DdNode * ith = Cudd_bddIthVar(manager, i), *tmp;

			if (mpz_even_p(col))
				ith = Cudd_Not(ith);
			tmp = Cudd_bddAnd (manager, minterm, ith);
			if (!tmp) {
				Cudd_RecursiveDeref (manager, minterm);
				minterm = NULL;
				goto cleanup;
			}
			Cudd_Ref (tmp);
			Cudd_RecursiveDeref(manager, minterm);
			minterm = tmp;
			mpz_tdiv_q_ui (col, col, 2); /* col := col / 2 */
		}

		for (i = vars_row - 1 ; i >= 0 ; --i) {
			DdNode * ith = Cudd_bddIthVar(manager, i), *tmp;
			if (mpz_even_p(row))
				ith = Cudd_Not(ith);
			tmp = Cudd_bddAnd (manager, minterm, ith);
			if (!tmp) {
				Cudd_RecursiveDeref(manager, minterm);
				minterm = NULL;
				goto cleanup;
			}
			Cudd_Ref (tmp);
			Cudd_RecursiveDeref(manager, minterm);
			minterm = tmp;
			mpz_tdiv_q_ui (row, row, 2); /* row := row / 2 */
		}

cleanup:
		mpz_clear (col);
		mpz_clear (row);

		if (minterm)
			Cudd_Deref (minterm);
		return minterm;
	}
}

DdNode * make_minterm_si (DdManager * manager, mpz_t width, mpz_t height,
		int col, int row)
{
	DdNode * ret;
	mpz_t _col, _row;
	mpz_init_set_si(_col, col);
	mpz_init_set_si(_row, row);
	ret = make_minterm(manager, width, height, _col, _row);
	mpz_clear (_col);
	mpz_clear (_row);
	return ret;
}


/****************************************************************************/
/* NAME: first_fulfill_intlist                                              */
/* FUNKTION: Gibt die erste erfuellende Belegung eines BDDs aus             */
/* UEBERGABEPARAMETER: DdNode * (BDD), int (Anzahl der Variablen)           */
/* RUECKGABEWERT: int * (Liste von Nullen und Einsen)                       */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 02.12.1999                                                           */
/* LETZTE AENDERUNG AM: 02.12.1999                                          */
/****************************************************************************/
int * first_fulfill_intlist (DdManager * manager, DdNode * bdd, int nvars)
{
  int * result = (int *) malloc (sizeof (int) * nvars);
  int position;
  DdNode * one;
  DdNode * zero;
  DdNode * node;
  DdNode * N;
  DdNode * T;
  DdNode * E;

  /* Die Resultat-Liste wird mit Nullen gefuellt. */
  for (position = 0; position < nvars; position ++)
    result [position] = 0;

  one = Cudd_ReadOne (manager);
  zero = Cudd_Not(one);

  /* The constant 0 function has no on-set cubes. */
  if (bdd == zero) {
    printf ("ERROR (first_fulfill_intlist): No element in relation\n");
    free (result);
    return (NULL);
  }

  if (bdd != one) {
    node = bdd;
    for (;;) {
      /* N ist nie ein konstanter Knoten (one oder zero) */
      N = Cudd_Regular (node);

      /* Falls der BDD-Index groesser ist als die Anzahl der Integers */
      if (N->index > nvars)
        break;

      T = Cudd_T (N);
      E = Cudd_E (N);
      if (Cudd_IsComplement (node)) {
        T = Cudd_Not (T);
        E = Cudd_Not (E);
      }

      /* Trifft der Algorithmus auf one, sind wir fertig */
      if (E == one)
        break;
      else
        /* Die Auswahl des ELSE-Pfades fuehrt zur logischen Null. */
        /* Deshalb muss an dieser Stelle eine Eins gesetzt werden. */
        if (E == zero) {
          result [N->index] = 1;
          if (T == one)
            break;
          else
            node = T;
        } else
          node = E;
    }
  }

  return (result);
}


/****************************************************************************/
/* NAME: rvIntlist2Bdd                                                      */
/* FUNKTION: Erzeugt einen BDD aus einer Liste von 0en und 1en              */
/* UEBERGABEPARAMETER: int * (Liste von 0en und 1en),                       */
/*                     int (Anzahl der Variablen)                           */
/* RUECKGABEWERT: DdNode * (Resultat-BDD) (NULL, falls erfolglos)           */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 15-FEB-2002                                                          */
/****************************************************************************/
DdNode * rvIntlist2Bdd (DdManager * manager, int * indices, int * assign, int vars)
{
  DdNode * tmp;
  DdNode * minterm;
  int i;

  minterm = Cudd_ReadOne(manager);
  Cudd_Ref (minterm);

  for (i = vars; i > 0; i --) {
	  DdNode * th = Cudd_bddIthVar(manager, indices[i-1]);

    if (! assign [i - 1])
    	th = Cudd_Not(th);
    tmp = Cudd_bddAnd(manager, minterm, th);
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(manager, minterm);
    minterm = tmp;
  }

  if (minterm)
	  Cudd_Deref (minterm);
  return (minterm);
}


/*!
 * Converts a DDApa number, which are used in Cudd, into a GMP integer number.
 *
 * \author stb
 */
static void _ddapa_to_gmp (DdApaNumber apa, int digits, mpz_t gmp)
{
    int i;
    mpz_t t,s;
    mpz_init (s);
    mpz_init (t);
    mpz_set_ui (gmp, 0);
    for (i = digits-1 ; i >= 0 ; i --) {
        mpz_set_ui (s, apa[i]);
        mpz_mul_2exp (t, s, (digits-i-1)*DD_APA_BITS);
        mpz_add (gmp, gmp, t);
    }
    mpz_clear (t);
    mpz_clear (s);
}


/*!
 * Returns the number of bits set for the relation. This value equals the
 * number of minterms of the representing BDD.
 *
 * \param ret The number of minterms. Must be initialized.
 */
void rvBddCountMinterm (DdManager * manager, DdNode * f, int nvars, mpz_t ret)
{
	int digits = 0;
	DdApaNumber minterms = Cudd_ApaCountMinterm(manager,f,nvars,&digits);
	//Cudd_ApaPrintMinterm(stdout,manager,f,nvars);
	_ddapa_to_gmp(minterms, digits, ret);
	//gmp_printf ("rvBddCountMinterm: Got %Zd, should be %.1lf (vars: %d)\n", ret, Cudd_CountMinterm(manager,f,nvars), nvars);
}
