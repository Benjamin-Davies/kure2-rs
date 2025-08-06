#include "KureImpl.h"


/****************************************************************************/
/* NAME: L_bdd_cache                                                        */
/* FUNKTION: Pseudo-Prozedur ohne Funktion, die fuer das CacheLookup und    */
/*           CacheInsert des CUDD-Packages als Parameter uebergeben wird.   */
/* UEBERGABEPARAMETER: DdManager *, DdNode *                                */
/* RUECKGABEWERT: DdNode *                                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 14.04.2000                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000                                          */
/****************************************************************************/
DdNode * L_bdd_cache (DdManager * table, DdNode * f)
{
  return (NULL);
}


/****************************************************************************/
/* NAME: L_bdd                                                              */
/* FUNKTION: Erzeugt einen BDD, der die All-Relation darstellt              */
/* UEBERGABEPARAMETER: Kure_MINT * (Anzahl der Zeilen),                   	    */
/*                                Kure_MINT * (Anzahl der Spalten)               */
/* RUECKGABEWERT: DdNode * (BDD)                                            */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 03.08.1999                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000 (Benutzung des Cache)                    */
/* 04.06.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
DdNode * L_bdd (KureContext * context, mpz_t rows, mpz_t cols)
{
	// TODO: Cache is missing!

	DdManager * manager = context->manager;
	int number_vars_rows = Kure_num_vars(rows);
	DdNode * L_row, *L_column, *ret;

	/* L_row ist der BDD einer Zeile der All-Relation */
	L_row = L_row_bdd (context, cols, number_vars_rows);
	Cudd_Ref (L_row);

	/* L_column ist der BDD einer Spalte der All-Relation */
	L_column = L_row_bdd (context, rows, 0);
	Cudd_Ref(L_column);

	ret = and_bdd (context->manager, L_row, L_column);
	Cudd_Ref(ret);
	Cudd_RecursiveDeref(manager, L_row);
	Cudd_RecursiveDeref(manager, L_column);

	Cudd_Deref(ret);
	return ret;
}



/****************************************************************************/
/* NAME: L_row_bdd_cache                                                    */
/* FUNKTION: Pseudo-Prozedur ohne Funktion, die fuer das CacheLookup und    */
/*           CacheInsert des CUDD-Packages als Parameter uebergeben wird.   */
/* UEBERGABEPARAMETER: DdManager *, DdNode *                                */
/* RUECKGABEWERT: DdNode *                                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 14.04.2000                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000                                          */
/****************************************************************************/
DdNode * L_row_bdd_cache (DdManager * table, DdNode * f)
{
  return (NULL);
}

/****************************************************************************/
/* NAME: L_row_bdd                                                          */
/* FUNKTION: Erzeugt einen BDD, der eine Zeile einer L-Relation darstellt   */
/* UEBERGABEPARAMETER: Kure_MINT * (Anzahl der Spalten),                         */
/*                     int (Index der ersten Variable)                      */
/* RUECKGABEWERT: DdNode * (Resultat-BDD)                                   */
/* ERSTELLT VON: Barbara Leoniuk                                            */
/* AM: 29.07.1999                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000 (Benutzung des Cache)                    */
/* 04.06.2000 (Umstellung auf grosse Zahlen)                                */
/* 26.11.2001 (Fehler mit Erzeugung des Referenz-BDDs behoben)              */
/****************************************************************************/
DdNode * L_row_bdd(KureContext * context, mpz_t number_of_ones, int index_of_root)
{
	// TODO: Cache is missing!

	DdManager * manager = context->manager;
	DdNode * one = Cudd_ReadOne(manager);
	DdNode * zero = Cudd_Not(one);
	DdNode * ret;
	//mpz_t m;
	int num_vars = Kure_num_vars(number_of_ones);
	int index = index_of_root + num_vars - 1;
	int bit = 0;
	mpz_t x;

	mpz_init(x);
	mpz_sub_ui(x,number_of_ones,1); /* x := number_of_ones - 1 */

	ret = one;
	Cudd_Ref(ret);

	for ( ; bit < num_vars ; ++bit, --index)
	{
		DdNode * tmp;
		DdNode * ith = Cudd_bddIthVar(manager, index);

		/* Die Eingabe wird in die einzelnen Bits zerlegt und daraus wird */
		/* der Resultat-BDD aufgebaut */
		if (mpz_tstbit(x, bit))
			tmp = Cudd_bddIte(manager, ith, ret, one);
		else
			tmp = Cudd_bddIte(manager, ith, zero, ret);
		Cudd_Ref(tmp);
		Cudd_Deref(ret); // `ret` is ref'd by `tmp`.
		ret = tmp;
	}

	mpz_clear (x);

	Cudd_Deref(ret);
	return ret;
}


/*****************************************************************************/
/* NAME: trans_bdd                                                           */
/* FUNKTION: Transponiert einen BDD, der eine Relation darstellt             */
/* UEBERGABEPARAMETER: DdNode * (Relation), Kure_MINT * (Zeilen), Kure_MINT * (Spalten)*/
/* RUECKGABEWERT: DdNode * (Ergebnis-BDD)                                    */
/* ERSTELLT VON: Ulf Milanese                                                */
/* AM: 03.08.1999                                                            */
/* LETZTE AENDERUNG AM: 03.08.1999                                           */
/* 08.03.2000 (Umstellung auf grosse Zahlen)                                 */
/*****************************************************************************/
DdNode * trans_bdd(KureContext * context, DdNode * bdd, mpz_t rows, mpz_t cols)
{
	int number_vars_rows = Kure_num_vars(rows);
	int number_vars_columns = Kure_num_vars(cols);

	if (number_vars_rows > 0 && number_vars_columns > 0) {
		return shift_bdd(context->manager, bdd, number_vars_rows,
				number_vars_columns, -number_vars_rows);
	} else
		return bdd;
}


/****************************************************************************/
/* NAME: neg_bdd                                                            */
/* FUNKTION: Negiert einen BDD, der eine Relation darstellt                 */
/* UEBERGABEPARAMETER: DdNode * (BDD), Kure_MINT * (Zeilen), Kure_MINT * (Spalten)    */
/* RUECKGABEWERT: DdNode * (Resultat)                                       */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 03.08.1999                                                           */
/* LETZTE AENDERUNG AM: 03.08.1999                                          */
/* 08.03.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
DdNode * neg_bdd(KureContext * context, DdNode * bdd, mpz_t rows, mpz_t cols)
{
	DdManager * manager = context->manager;
	DdNode * all_bdd, *ret;

	/* Der negierte BDD muss auf die Zeilen und Spalten der Original-Relation */
	/* eingeschraenkt werden */
	all_bdd = L_bdd(context, rows, cols);
	Cudd_Ref(all_bdd);
	ret = Cudd_bddAnd(manager, Cudd_Not(bdd), all_bdd);
	Cudd_Ref(ret);
	Cudd_RecursiveDeref(manager, all_bdd);

	Cudd_Deref(ret);
	return ret;
}



/****************************************************************************/
/* NAME: ident_bdd_cache                                                    */
/* FUNKTION: Pseudo-Prozedur ohne Funktion, die fuer das CacheLookup und    */
/*           CacheInsert des CUDD-Packages als Parameter uebergeben wird.   */
/* UEBERGABEPARAMETER: DdManager *, DdNode *                                */
/* RUECKGABEWERT: DdNode *                                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 14.04.2000                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000                                          */
/****************************************************************************/
DdNode * ident_bdd_cache (DdManager * table, DdNode * f)
{
  return (NULL);
}


/****************************************************************************/
/* NAME: ident_bdd                                                          */
/* FUNKTION: Erzeugt einen BDD, der die Identische Relation darstellt       */
/* UEBERGABEPARAMETER: Kure_MINT * (Zeilen)                                      */
/* RUECKGABEWERT: DdNode * (Resultat)                                       */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 06.08.1999                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000 (Benutzung des Cache)                    */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
DdNode * ident_bdd (KureContext * context, mpz_t rows)
{
	// Cache is missing.
	DdNode * ret = NULL;//kure_cache_gmp_lookup (context->ident_cache, rows);
	if (ret) {
		return ret;
	}
	else {
		DdManager * manager = context->manager;
		int vars = Kure_num_vars(rows);
		DdNode * one = Cudd_ReadOne(manager);
		DdNode * zero = Cudd_Not(one);
		DdNode * L, *g;
		int i;

		ret = Cudd_ReadOne(manager);
		Cudd_Ref(ret);
		for (i = 2*vars-1 ; i >= vars ; --i) {
			DdNode *left, *right;
			DdNode * ith = Cudd_bddIthVar(manager, i);
			DdNode * f = Cudd_bddIte(manager, ith, zero, one);
			Cudd_Ref (f);

			left = Cudd_bddAnd(manager, ret, f);
			Cudd_Ref (left);
			Cudd_RecursiveDeref(manager, f);

			f = Cudd_bddIte(manager, ith, one, zero);
			Cudd_Ref (f);
			right = Cudd_bddAnd(manager, ret, f);
			Cudd_Ref (right);
			Cudd_RecursiveDeref(manager, f);

			Cudd_RecursiveDeref(manager, ret);
			ret = Cudd_bddIte(manager, Cudd_bddIthVar(manager, i-vars), right, left);
			Cudd_Ref (ret);
			Cudd_RecursiveDeref(manager, left);
			Cudd_RecursiveDeref(manager, right);
		}

		L = L_bdd (context, rows, rows);
		Cudd_Ref (L);
		g = Cudd_bddAnd (manager, ret, L);
		Cudd_Ref (g);
		Cudd_RecursiveDeref(manager, L);
		Cudd_RecursiveDeref(manager, ret);

		ret = g;

		//kure_cache_gmp_insert (context->ident_cache, rows, ret);
		Cudd_Deref (ret);
		return ret;
	}
}



/****************************************************************************/
/* NAME: next_bdd_cache                                                     */
/* FUNKTION: Pseudo-Prozedur ohne Funktion, die fuer das CacheLookup und    */
/*           CacheInsert des CUDD-Packages als Parameter uebergeben wird.   */
/* UEBERGABEPARAMETER: DdManager *, DdNode *                                */
/* RUECKGABEWERT: DdNode *                                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 22.10.1999                                                           */
/* LETZTE AENDERUNG AM: 22.10.1999                                          */
/****************************************************************************/
DdNode * next_bdd_cache (DdManager * table, DdNode * f)
{
  return (NULL);
}


/****************************************************************************/
/* NAME: next_bdd                                                           */
/* FUNKTION: Erzeugt einen BDD, der die obere Nebendiagonale darstellt      */
/* UEBERGABEPARAMETER: Kure_MINT * (Zeilen)                                      */
/* RUECKGABEWERT: DdNode * (Resultat)                                       */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 08.03.2000                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000 (Benutzung des Cache)                    */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
DdNode * next_bdd (KureContext * context, mpz_t rows)
{
	// TODO: Cache is missing.
	DdNode * ret = NULL; //Kure_cache_gmp_lookup (context->next_cache, rows);
	if (ret) return ret;
	else {
		DdManager * manager = context->manager;
		int vars = Kure_num_vars(rows);
		DdNode * one = Cudd_ReadOne(manager);
		DdNode * zero = Cudd_Not(one);
		DdNode * left, *right;
		DdNode * f, *g;
		int i;

		ret = zero;
		Cudd_Ref (ret);

		for (i = (2 * vars - 1); i >= vars; i --) {
		    /* Rekursionsschritt: Uebernahme der bisher berechneten Teilrelation */
		    /* in das obere linke und untere rechte Viertel */
			DdNode * ith = Cudd_bddIthVar(manager, i);
			DdNode *h;
			int j;

			DdNode * f = Cudd_bddIte (manager, ith, zero, one);
			Cudd_Ref (f);
			left = Cudd_bddAnd(manager, ret, f);
			Cudd_Ref (left);
			Cudd_RecursiveDeref(manager, f);

			f = Cudd_bddIte (manager, ith, one, zero);
			Cudd_Ref (f);
			right = Cudd_bddAnd (manager, ret, f);
			Cudd_Ref (right);
			Cudd_RecursiveDeref(manager, f);

			f = Cudd_bddIte(manager, Cudd_bddIthVar(manager, i-vars), right, left);
			Cudd_Ref(f);

			Cudd_RecursiveDeref(manager, left);
			Cudd_RecursiveDeref(manager, right);

			/* Only f is valid at this point. */

		    /* Erzeugen des einzelnen Elementes im rechten oberen Viertel der */
		    /* Relation, als Belegung zum Beispiel: ---01111---10000 */
			g = one;
			Cudd_Ref (g);
			for (j = (2 * vars - 1); j > i; j --) {
				h = Cudd_bddAnd(manager, g, Cudd_Not(Cudd_bddIthVar(manager, j)));
				Cudd_Ref (h);
				Cudd_RecursiveDeref(manager, g);
				g = Cudd_bddAnd (manager, h, Cudd_bddIthVar(manager, j-vars));
				Cudd_Ref (g);
				Cudd_RecursiveDeref(manager, h);
			}

			/* f and g are valid. */
			h = Cudd_bddAnd (manager, ith, g);
			Cudd_Ref (h);
			Cudd_RecursiveDeref(manager, g);
			g = Cudd_bddAnd (manager, h, Cudd_Not(Cudd_bddIthVar(manager, i-vars)));
			Cudd_Ref (g);
			Cudd_RecursiveDeref(manager, h);
			Cudd_RecursiveDeref(manager, ret);
			ret = Cudd_bddOr(manager, f,g);
			Cudd_Ref (ret);
			Cudd_RecursiveDeref(manager, f);
			Cudd_RecursiveDeref(manager, g);

			/* Only ret is valid here. */
		}

		f = L_bdd(context, rows, rows);
		Cudd_Ref (f);
		g = Cudd_bddAnd (manager, f, ret);
		Cudd_Ref (g);
		Cudd_RecursiveDeref(manager, f);
		Cudd_RecursiveDeref(manager, ret);

		ret = g;
		//Kure_cache_gmp_insert (context->next_cache, rows, ret);
		Cudd_Deref (g);
		return g;
	}
}




/****************************************************************************/
/* NAME: inj1_bdd_cache                                                     */
/* FUNKTION: Pseudo-Prozedur ohne Funktion, die fuer das CacheLookup und    */
/*           CacheInsert des CUDD-Packages als Parameter uebergeben wird.   */
/* UEBERGABEPARAMETER: DdManager *, DdNode *                                */
/* RUECKGABEWERT: DdNode *                                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 14.04.2000                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000                                          */
/****************************************************************************/
DdNode * inj1_bdd_cache (DdManager * table, DdNode * f)
{
  return (NULL);
}


/****************************************************************************/
/* NAME: inj1_bdd                                                           */
/* FUNKTION: Erzeugt einen BDD, der die 1. Injektionsrelation darstellt     */
/* UEBERGABEPARAMETER: Kure_MINT * (Breite der Relation), Kure_MINT * (Hoehe)         */
/* RUECKGABEWERT: DdNode * (BDD)                                            */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 21.03.2000                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000 (Benutzung der Cache)                    */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// Achtung: Geaenderte Reihenfolge von rows, cols!
DdNode * inj1_bdd (KureContext * context, mpz_t rows, mpz_t cols)
{
	DdNode * ret = NULL; //Kure_cache_gmp_2lookup (context->inj1_cache, rows,  cols);
	if (ret) return ret;
	else {
		mpz_t mem_needed;

		/* Actually, we cannot handle big numbers because we won't have
		 * enough memory. */

		mpz_init (mem_needed);
		mpz_mul_si (mem_needed, rows, sizeof(DdNode*));
		if ( !mpz_fits_sint_p (mem_needed)) /* unsigned wouldn't make a difference */ {
			mpz_clear (mem_needed);
			return NULL;
		}
		else {
			DdManager * manager = context->manager;
			int si_rows = mpz_get_si (rows);
			DdNode * one = Cudd_ReadOne(manager);
			DdNode ** row_bdds = (DdNode**)alloca (sizeof(DdNode*) * si_rows);
			int number_vars_rows = Kure_num_vars(rows);
			int number_vars_columns = Kure_num_vars(cols);
			DdNode * ret = NULL;
			int i;

			mpz_clear (mem_needed);

			for (i = 0 ; i < si_rows ; i ++) {
				int var;
				int j = i;

				row_bdds[i] = one;
				Cudd_Ref(row_bdds[i]);

				for (var = number_vars_rows + number_vars_columns - 1
						; var >= number_vars_rows; --var) {
					DdNode * f, *varth = Cudd_bddIthVar(manager, var);

					if (Kure_is_even (j))
						varth = Cudd_Not (varth);
					f = Cudd_bddAnd(manager, row_bdds[i], varth);
					Cudd_Ref (f);
					Cudd_RecursiveDeref(manager, row_bdds[i]);

					row_bdds[i] = f;
					j >>= 1;
				}
			}

			ret = build_bdd (manager, row_bdds, si_rows);
			Cudd_Ref (ret);

			//Kure_cache2_gmp_insert (context->inj1_bdd, rows, cols, ret);
			Cudd_Deref (ret);
			return ret;
		}
	}
}

/****************************************************************************/
/* NAME: inj2_bdd_cache                                                     */
/* FUNKTION: Pseudo-Prozedur ohne Funktion, die fuer das CacheLookup und    */
/*           CacheInsert des CUDD-Packages als Parameter uebergeben wird.   */
/* UEBERGABEPARAMETER: DdManager *, DdNode *                                */
/* RUECKGABEWERT: DdNode *                                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 14.04.2000                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000                                          */
/****************************************************************************/
DdNode * inj2_bdd_cache (DdManager * table, DdNode * f)
{
  return (NULL);
}


/****************************************************************************/
/* NAME: inj2_bdd                                                           */
/* FUNKTION: Erzeugt einen BDD, der die 2. Injektionsrelation darstellt     */
/* UEBERGABEPARAMETER: Kure_MINT * (Breite der Relation), Kure_MINT * (Hoehe)         */
/* RUECKGABEWERT: DdNode * (BDD)                                            */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 21.03.2000                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000 (Benutzung der Cache)                    */
/* 15.08.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// Achtung: Geaenderte Reihenfolge der Argumente.
DdNode * inj2_bdd (KureContext * context, mpz_t rows, mpz_t cols)
{
	// TODO: Cache is missing.
	DdNode * ret = NULL;//Kure_cache_gmp_2lookup (context->inj2_cache, rows,  cols);
	if (ret) return ret;
	else {
		mpz_t mem_needed;

		/* Actually, we cannot handle big numbers because we won't have
		 * enough memory. */

		mpz_init (mem_needed);
		mpz_mul_si (mem_needed, rows, sizeof(DdNode*));
		if ( !mpz_fits_sint_p (mem_needed)) /* unsigned wouldn't make a difference */ {
			mpz_clear (mem_needed);
			return NULL;
		}
		else {
			DdManager * manager = context->manager;
			int si_rows = mpz_get_si (rows);
			DdNode * one = Cudd_ReadOne(manager);
			DdNode ** row_bdds = (DdNode**)alloca (sizeof(DdNode*) * si_rows);
			int number_vars_rows = Kure_num_vars(rows);
			int number_vars_columns = Kure_num_vars(cols);
			DdNode * ret = NULL;
			int i;
			mpz_t j;

			mpz_clear (mem_needed);
			mpz_init (j);

			for (i = 0; i < si_rows; i++) {
				int var;

				/* j = i + cols - rows */
				mpz_set_ui (j, i);
				mpz_sub_ui (j, j, si_rows);
				mpz_add (j, j, cols);

				row_bdds[i] = one;
				Cudd_Ref(row_bdds[i]);

				for (var = number_vars_rows + number_vars_columns - 1
						; var >= number_vars_rows; var--) {
					DdNode * f, *varth = Cudd_bddIthVar(manager, var);

					if (mpz_even_p(j))
						varth = Cudd_Not (varth);
					f = Cudd_bddAnd(manager, row_bdds[i], varth);
					Cudd_Ref (f);
					Cudd_RecursiveDeref(manager, row_bdds[i]);

					row_bdds[i] = f;
					mpz_tdiv_q_2exp (j, j, 1); /* j >>= 1 */
				}
			}

			mpz_clear (j);

			ret = build_bdd (manager, row_bdds, si_rows);
			Cudd_Ref (ret);

			//Kure_cache2_gmp_insert (context->inj2_bdd, rows, cols, ret);
			Cudd_Deref (ret);
			return ret;
		}
	}
}

/****************************************************************************/
/* NAME: pro1_bdd_cache                                                     */
/* FUNKTION: Pseudo-Prozedur ohne Funktion, die fuer das CacheLookup und    */
/*           CacheInsert des CUDD-Packages als Parameter uebergeben wird.   */
/* UEBERGABEPARAMETER: DdManager *, DdNode *                                */
/* RUECKGABEWERT: DdNode *                                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 14.04.2000                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000                                          */
/****************************************************************************/
DdNode * pro1_bdd_cache (DdManager * table, DdNode * f)
{
  return (NULL);
}


/****************************************************************************/
/* NAME: pro1_bdd                                                           */
/* FUNKTION: Erzeugt einen BDD, der die 1. Projektionsrelation darstellt    */
/* UEBERGABEPARAMETER: Kure_MINT * (Breite der Relation), Kure_MINT * (Hoehe)         */
/* RUECKGABEWERT: DdNode * (BDD)                                            */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 28.03.2000                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000 (Benutzung der Cache)                    */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// Achtung: geaenderte Reihenfolge
// rows and cols means the size of the result, not the the arguments.
// Hence, rows = cols*x for some x.
DdNode * pro1_bdd (KureContext * context, mpz_t _rows, mpz_t _cols)
{
	if (! mpz_fits_sint_p(_rows) || ! mpz_fits_sint_p(_cols))
		return NULL;
	else {
		DdManager * manager = context->manager;
		int rows = mpz_get_si (_rows), cols = mpz_get_si (_cols);
		int num_vars_rows = Kure_num_vars(_rows), num_vars_cols = Kure_num_vars(_cols);
		int div = rows / cols;
		int i;
		DdNode * one = Cudd_ReadOne (manager), *res = NULL;
		DdNode ** row_bdds = NULL;

		row_bdds = KURE_NEW(DdNode*, rows);

#if 0
		gmp_printf ("pro1_bdd: rows: %d (gmp %Zd), cols: %d (gmp %Zd), div: %d, "
				"num_vars_rows: %d, num_vars_cols: %d\n",
				rows, _rows, cols, _cols, div, num_vars_rows, num_vars_cols);
#endif

		for (i = 0 ; i < cols ; i++) {
			/* Es wird die Menge aller Zeilen mit nur einem Eintrag erzeugt */
			int j = i, var, k = i * div;
			row_bdds[k] = one;
			Cudd_Ref(one);
			for (var = num_vars_rows + num_vars_cols - 1
					; var >= num_vars_rows ; --var)
			{
				DdNode * tmp;
				DdNode * varth = Cudd_bddIthVar(manager, var);

				if ( !(j&0x1))
					varth = Cudd_Not(varth);
				tmp = Cudd_bddAnd (manager, row_bdds[k], varth);
				Cudd_Ref (tmp);
				Cudd_IterDerefBdd(manager, row_bdds[k]);
				row_bdds[k] = tmp;
				j >>= 1;
			}

			/* Die Zeile mit nur einem Eintrag werden dupliziert */
			for (j = 1; j < div; j++) {
				row_bdds[k + j] = row_bdds[k];
				Cudd_Ref(row_bdds[k + j]);
			}
		}

		res = build_bdd (manager, row_bdds, rows);
		free (row_bdds);
		return res;
	}
}

/****************************************************************************/
/* NAME: pro2_bdd_cache                                                     */
/* FUNKTION: Pseudo-Prozedur ohne Funktion, die fuer das CacheLookup und    */
/*           CacheInsert des CUDD-Packages als Parameter uebergeben wird.   */
/* UEBERGABEPARAMETER: DdManager *, DdNode *                                */
/* RUECKGABEWERT: DdNode *                                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 14.04.2000                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000                                          */
/****************************************************************************/
DdNode * pro2_bdd_cache (DdManager * table, DdNode * f)
{
  return (NULL);
}


/****************************************************************************/
/* NAME: pro2_bdd                                                           */
/* FUNKTION: Erzeugt einen BDD, der die 2. Projektionsrelation darstellt    */
/* UEBERGABEPARAMETER: Kure_MINT * (Breite der Relation), Kure_MINT * (Hoehe)         */
/* RUECKGABEWERT: DdNode * (BDD)                                            */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 28.03.2000                                                           */
/* LETZTE AENDERUNG AM: 14.04.2000 (Benutzung der Cache)                    */
/* 28.05.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
// Achtung: Reihenfolge geaendert!
DdNode * pro2_bdd (KureContext * context, mpz_t _rows, mpz_t _cols)
{
	if (! mpz_fits_sint_p(_rows) || ! mpz_fits_sint_p(_cols))
		return NULL;
	else {
		DdManager * manager = context->manager;
		int rows = mpz_get_si (_rows), cols = mpz_get_si (_cols);
		int num_vars_rows = Kure_num_vars(_rows), num_vars_cols = Kure_num_vars(_cols);
		DdNode ** row_bdds = KURE_NEW(DdNode*,rows);
		int i;
		int div = rows / cols;
		DdNode * res, *one = Cudd_ReadOne(manager);

		for (i = 0 ; i < cols ; ++i) {
			int j = i, var;

			row_bdds[i] = one;
			Cudd_Ref (one);
			for (var = num_vars_rows + num_vars_cols - 1
					; var >= num_vars_rows ; --var) {
				DdNode * ith = Cudd_bddIthVar(manager, var), *tmp;

				if ((j % 2) == 0) /* j is even */
					ith = Cudd_Not(ith);
				tmp = Cudd_bddAnd (manager, row_bdds[i], ith);
				Cudd_Ref (tmp);
				Cudd_IterDerefBdd(manager, row_bdds[i]);
				row_bdds[i] = tmp;
				j >>= 1;
			}

			for (j = 1 ; j < div ; ++j) {
				int k = i + j*cols;
				row_bdds[k] = row_bdds[i];
				Cudd_Ref (row_bdds[k]);
			}
		}

		res = build_bdd (manager, row_bdds, rows);
		// TODO: insert into cache.

		free (row_bdds);

		return res;
	}
}



/****************************************************************************/
/* NAME: null_bdd                                                           */
/* FUNKTION: Returns a bdd representing the empty relation                  */
/* UEBERGABEPARAMETER: KEINER                                               */
/* RUECKGABEWERT: DdNode *                                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 29.09.2000                                                           */
/* LETZTE AENDERUNG AM: 29.09.2000                                          */
/****************************************************************************/
KURE_INLINE DdNode * null_bdd (DdManager * manager)
{
	return Cudd_ReadLogicZero (manager);
}



/****************************************************************************/
/* NAME: or_bdd                                                             */
/* FUNKTION: Computes the OR of two BDDS                                    */
/* UEBERGABEPARAMETER: DdNode *, DdNode *                                   */
/* RUECKGABEWERT: DdNode *                                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 29.09.2000                                                           */
/* LETZTE AENDERUNG AM: 29.09.2000                                          */
/****************************************************************************/
KURE_INLINE DdNode * or_bdd (DdManager * manager, DdNode * f, DdNode * g)
{
	return Cudd_bddOr (manager, f, g);
}



/****************************************************************************/
/* NAME: and_bdd                                                            */
/* FUNKTION: Computes the AND of two BDDS                                   */
/* UEBERGABEPARAMETER: DdNode *, DdNode *                                   */
/* RUECKGABEWERT: DdNode *                                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 29.09.2000                                                           */
/* LETZTE AENDERUNG AM: 29.09.2000                                          */
/****************************************************************************/
KURE_INLINE DdNode * and_bdd (DdManager * manager, DdNode * f, DdNode * g)
{
	return Cudd_bddAnd(manager, f, g);
}


/****************************************************************************/
/* NAME: lesscard_bdd                                                       */
/* FUNKTION: Builds a BDD for a special function                            */
/* UEBERGABEPARAMETER: int n number of variables                            */
/* RUECKGABEWERT: BDD representing the following function f:                */
/*                f = 1 <=> x_0 + ... + x_(n-1) <= x_n + ... x_(2n-1)       */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 04.10.2000                                                           */
/* LETZTE AENDERUNG AM: 04.10.2000                                          */
/****************************************************************************/
DdNode * lesscard_bdd (KureContext * context, int n)
{
	DdManager * manager = context->manager;
	DdNode * one = Cudd_ReadOne (manager);
	DdNode * zero = Cudd_Not(one);
	DdNode * low [n], * high [n];
	int k = 2*n-1;
	int index = k;

	high[0] = Cudd_bddIthVar(manager, index);
	Cudd_Ref (high[0]);

	for (index = k-1 ; index >= n ; --index) {
		DdNode * var = Cudd_bddIthVar(manager, index);
		int i;

	    /* The first node in the next level is Zero if the value is zero */
	    low[0] = Cudd_bddIte (manager, var, high [0], zero);
	    Cudd_Ref (low [0]);

	    for (i = 1; i <= k-index-1; i ++) {
	    	low[i] = Cudd_bddIte (manager, var, high[i], high[i-1]);
	    	Cudd_Ref (low[i]);
	    }

	    i = k-index;
	    low[i] = Cudd_bddIte (manager, var, one, high[i-1]);
	    Cudd_Ref (low[i]);

	    for (i = 0; i <= k-index-1; i ++) {
	      Cudd_IterDerefBdd (manager, high[i]);
	      high[i] = low[i];
	    }
	    high[k-index] = low[k-index];
	}

	for (index = n-1 ; index >= 0 ; --index) {
		DdNode * var = Cudd_bddIthVar(manager, index);
		int i;

		for (i = 0 ; i <= index ; ++i) {
			if (i != n-1)
		        low[i] = Cudd_bddIte (manager, var, high[i], high[i+1]);
			else
				low[i] = Cudd_bddIte (manager, var, high[i], one);
			Cudd_Ref (low[i]);
		}

	    for (i = 0 ; (i <= index + 1) && (index != n - 1) ; ++i)
	    	Cudd_IterDerefBdd (manager, high[i]);

	    for (i = 0; i <= index; i ++) {
	    	high[i] = low[i];
	    	Cudd_Ref (high[i]);
	    }
	}

	return low[0];
}



/****************************************************************************/
/* NAME:  subsetvector_bdd (int set_card,  int subset_card)                 */
/* FUNKTION: liefert den Vektor aller Teilmengen der Maechtigket            */
/*           subset_card einer Menge mit set_card Elementen                 */
/* UEBERGABEPARAMETER: int set_card,  int subset_card                       */
/* RUECKGABEWERT: DdNode * (Resultat)                                       */
/* ERSTELLT VON: Barbara Leoniuk                                            */
/* AM: 24.10.2000                                                           */
/****************************************************************************/
DdNode * subsetvector_bdd (KureContext * context, int set_card, int subset_card)
{
	DdManager * manager = context->manager;
	DdNode * one = Cudd_ReadOne (manager);
	DdNode * zero = Cudd_Not(one);
	int k = set_card - subset_card + 1;
	DdNode * tmp [k+1];
	int i,j;

	/* The resulting BDD uses (set_card - subset_card) * (subset_card + 1) */
	/* BDD-nodes. tmp stores (set_card - subset_card) nodes and the result */
	/* is built up recursively. */

	tmp[0] = one;
	Cudd_Ref(tmp[0]);
	for (i = 1; i <= k; i ++) {
		tmp [i] = zero;
		Cudd_Ref (tmp [i]);
	}

	/* The BDD is build up 'diagonally' like the Pascal triangle, but not */
	/* from top to bottom but otherwise */
	for (j = 1 ; j <= subset_card ; ++j) {
		/* A new diagonal line is built out of the underlying diagonal */
		for (i = 1 ; i <= k ; ++i) {
			DdNode * var = Cudd_bddIthVar (manager, set_card - j - i + 1);
			DdNode * f = Cudd_bddIte (manager, var, tmp[i], tmp[i-1]);
			Cudd_Ref (f);
			Cudd_IterDerefBdd(manager, tmp[i]);
			tmp[i] = f;
		}
	}

	Cudd_Deref (tmp[k]);
	return tmp[k];
}

/****************************************************************************/
/* NAME: warshall_bdd                                                       */
/* FUNKTION: berechnet die transitive Huelle einer Relation, die als BDDs   */
/*           dargestellt ist                                                */
/* UEBERGABEPARAMETER: DdNode * (1. Relation), int (Zeilen der Relation)    */
/* RUECKGABEWERT: DdNode * (Ergebnis-BDD)                                   */
/* ERSTELLT VON: Barbara Leoniuk                                            */
/* AM: 01.06.2001                                                           */
/****************************************************************************/
DdNode * warshall_bdd (KureContext * context, DdNode * first_bdd, int rows)
{
	DdManager * manager = context->manager;
	int num_vars_rows = number_of_vars2(rows);
	int i;
	mpz_t mp_rows, mp_1, mp_i, mp_0;

	DdNode * res = first_bdd;
	Cudd_Ref (res);

	mpz_init_set_si (mp_rows, rows);
	mpz_init_set_si (mp_1, 1);
	mpz_init_set_si (mp_i, 0);
	mpz_init_set_si (mp_0, 0);

	for (i = 0 ; i < rows ; ++i, mpz_add_ui(mp_i, mp_i, 1)) {
		DdNode * next_point, *R_column, *R_row, *tmpA, *tmpB;

		/* Es wird der i-te Punkt bestimmt  */
		next_point = make_minterm (manager, mp_rows, mp_1, mp_i, mp_0);
		Cudd_Ref (next_point);

		/* ... und damit die i-te Spalte von resultat bestimmt  */
		R_column = komp_bdd (manager, res, next_point, mp_rows, mp_rows);
		Cudd_Ref (R_column);
		Cudd_IterDerefBdd(manager, next_point);

		/* Es wird die i-te Zeile von resultat bestimmt  */
		R_row = find_row_in_bdd (res, num_vars_rows, mp_i);
		Cudd_Ref (R_row);

		/* neuer Beitrag fuer das Resultat wird bestimmt */
		tmpA = Cudd_bddAnd (manager, R_column, R_row);
		Cudd_Ref (tmpA);
		Cudd_IterDerefBdd (manager, R_column);
		Cudd_IterDerefBdd (manager, R_row);

		/* ... und hinzugefuegt */
		tmpB = Cudd_bddOr(manager, res, tmpA);
		Cudd_Ref (tmpB);
		Cudd_IterDerefBdd(manager, tmpA);
		Cudd_IterDerefBdd(manager, res);
		res = tmpB;
	}

	mpz_clear (mp_rows);
	mpz_clear (mp_1);
	mpz_clear (mp_i);
	mpz_clear (mp_0);

	Cudd_Deref (res);
	return res;
}
