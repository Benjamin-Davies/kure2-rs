#include "KureImpl.h"
#include <assert.h>

#if 0
/****************************************************************************/
/* NAME: test_line_clear                                                    */
/* FUNKTION: testet ob eine Zeile komplett mit 0 besetzt ist                */
/* UEBERGABEPARAMETER: Rel * (die Relation)                                 */
/*                     Kure_MINT * (Zeile)                                       */
/* RUECKGABEWERT: TRUE falls ja, sonst FALSE                                */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 05.09.95                                                             */
/* LETZTE AENDERUNG AM: 21.02.2000 (Benutzung von BDD-Routinen)             */
/* 01.03.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
int test_line_clear (Rel * rel, Kure_MINT * LINE)
{
  DdNode * row;
  int vars;
  int result = FALSE;

  if (rel) {
    if (Kure_mp_groesser_null (LINE) >= 0 && (Kure_mp_mcmp (LINE, rel->hoehe) < 0)) {
      vars = Kure_mp_number_of_vars (rel->hoehe);
      row = find_row_in_bdd (rel->bdd, vars, LINE);
      if (row == null_bdd ())
        result = TRUE;
    } else
      printf ("ERROR (test_line_clear): Value out of range\n");
  } else
    printf ("ERROR (test_line_clear): No relation defined\n");

  return (result);
}
#endif


/****************************************************************************/
/* NAME: test_line_complete                                                 */
/* FUNKTION: testet ob eine Zeile, Spalte, Diagonale komplett ist           */
/* UEBERGABEPARAMETER: Rel * (die Relation)                                 */
/*                     int y (Zeile), int x (Spalte)                        */
/*                     int direction je nach (|, -, \, /)                   */
/* RUECKGABEWERT: TRUE falls ja, sonst FALSE                                */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 04.09.95                                                             */
/* LETZTE AENDERUNG AM: 04.09.95                                            */
/* 04.08.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
Kure_bool test_line_complete (KureRel * r, int x, int y, KureDirection direction)
{
  int vars_rows = Kure_num_vars(r->rows);
  int vars_cols = Kure_num_vars(r->cols);
  int rows  = mpz_get_si (r->rows);
  int cols = mpz_get_si (r->cols);

  //printf ("rows: %d, cols: %d, x: %d, y: %d\n", rows, cols, x,y);

    	int rx, ry;
    	int start_x, start_y;
    	int x_step = 0, y_step = 0;

    	if (KURE_DIR_DOWN_RIGHT == direction) /* \ */ {
    		int min_off = MIN(x,y);
    		start_x = x - min_off;
    		start_y = y - min_off;
    		x_step = 1;
    		y_step = 1;
    	}
    	else if (KURE_DIR_DOWN_UP == direction) /* | */ {
    		start_y = 0;
    		start_x = x;
    		y_step = 1;
    	}
    	else if (KURE_DIR_LEFT_RIGHT == direction) /* - */ {
    		start_y = y;
    		start_x = 0;
    		x_step = 1;
    	}
    	else if (KURE_DIR_DOWN_LEFT == direction) /* / */ {
    		int bot_dist = rows - y - 1;
    		int min_off = MIN(bot_dist, x);
    		start_x = x - min_off;
    		start_y = y + min_off;
    		x_step = 1;
    		y_step = -1;
    	}

    	assert (start_x >= 0 && start_y >= 0);

    	//printf ("start_x: %d, start_y: %d\n", start_x, start_y);

    	for (rx = start_x, ry = start_y
    			; rx < cols && ry < rows && rx >= 0 && ry >= 0
    			; rx += x_step, ry += y_step)
    	{
    		if ( !get_rel_bit_full (r, ry, rx, vars_rows, vars_cols))
    			return FALSE;
    	}

    	return TRUE;

#if 0
      switch (direction) {
      case KURE_DIR_DOWN_RIGHT:
        i = 0;
        while ((ret == TRUE) && (x >= i) && (y >= i)) {
          if (get_rel_bit_full (r, y-i, x - i, vars_rows, vars_cols) == 0)
            ret = FALSE;
          else
            i ++;
        }

        i = 0;
        while ((ret == TRUE) && ((x + i) < cols) &&
               ((y + i) < rows)) {
          if (get_rel_bit_full (r, y+1,x + i, vars_rows, vars_cols) == 0)
            ret = FALSE;
          else
            i ++;
        }
        break;

      case KURE_DIR_DOWN_UP:
        i = 0;
        while ((ret == TRUE) && (y >= i)) {
          if (get_rel_bit_full (r, y-i, x, vars_rows, vars_cols) == 0)
            ret = FALSE;
          else
            i ++;
        }

        i = 0;
        while ((ret == TRUE) && (y + i) < rows) {
          if (get_rel_bit_full (r, y+1, x, vars_rows, vars_cols) == 0)
            ret = FALSE;
          else
            i ++;
        }
        break;

      case KURE_DIR_LEFT_RIGHT:
        i = 0;
        while ((ret == TRUE) && (x >= i)) {
          if (get_rel_bit_full (r, y, x - i, vars_rows, vars_cols) == 0)
            ret = FALSE;
          else {
        	  printf ("bit %d,%d is set\n", x-i,y);
            i ++;
          }
        }

        i = 0;
        while ((ret == TRUE) && (x + i) < cols) {
          if (get_rel_bit_full (r, y, x + i, vars_rows, vars_cols) == 0)
            ret = FALSE;
          else
            i ++;
        }
        break;

      case KURE_DIR_DOWN_LEFT:
        i = 0;
        while ((ret == TRUE) && (x >= i) && ((y + i) < rows)) {
          if (get_rel_bit_full (r, y+1,x - i, vars_rows, vars_cols) == 0)
            ret = FALSE;
          else
            i ++;
        }

        i = 0;
        while ((ret == TRUE) && ((x + i) < cols) && (y >= i)) {
          if (get_rel_bit_full (r, y-i,x + i, vars_rows, vars_cols) == 0)
            ret = FALSE;
          else
            i ++;
        }
        break;

      default:
        printf ("ERROR (test_line_complete): No direction defined\n");
        ret = FALSE;
      }
    } else
      printf ("ERROR (test_line_complete): Value out of range\n");
  } else
    printf ("ERROR (test_line_complete): No relation defined\n");

  return (ret);
#endif
}



/****************************************************************************/
/* NAME: test_on_empty                                                      */
/* FUNKTION: testet eine Relationen auf Nullrelation                        */
/* UEBERGABEPARAMETER: Rel * (die Relation)                                 */
/* RUECKGABEWERT: TRUE falls gleich, sonst FALSE                            */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 16.11.95                                                             */
/* LETZTE AENDERUNG AM: 06.08.1999 (Umstellung auf BDD-Version)             */
/****************************************************************************/
Kure_bool test_on_empty (KureRel * rel)
{
	DdManager * manager = rel->context->manager;
	return rel->bdd == Cudd_Not(Cudd_ReadOne(manager));
}


/****************************************************************************/
/* NAME: test_on_equal                                                      */
/* FUNKTION: testet zwei Relationen auf Gleichheit                          */
/* UEBERGABEPARAMETER: 2 X Rel * (die Relationen)                           */
/* RUECKGABEWERT: TRUE falls gleich, sonst FALSE                            */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 04.04.95                                                             */
/* LETZTE AENDERUNG AM: 06.08.1999 (Umstellung auf BDD-Version)             */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
Kure_bool test_on_equal (KureRel * rel1, KureRel * rel2)
{
	if (mpz_cmp(rel1->rows, rel2->rows) == 0
			&& mpz_cmp(rel1->cols, rel2->cols) == 0) {
		/* BDDs are canonical, so we can easily test node equality. */
		return rel1->bdd == rel2->bdd;
	}
	else return FALSE;
}

/****************************************************************************/
/* NAME: test_on_inclusion                                                  */
/* FUNKTION: testet zwei Relationen auf Inclusion                           */
/* UEBERGABEPARAMETER: 2 X Rel * (die Relationen)                           */
/* RUECKGABEWERT: TRUE falls Inclusion, sonst FALSE                         */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 04.04.95                                                             */
/* LETZTE AENDERUNG AM: 06.08.1999 (Umstellung auf BDD-Version)             */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
Kure_bool test_on_inclusion (KureRel * rel1, KureRel * rel2)
{
	if (mpz_cmp(rel1->rows, rel2->rows) == 0
				&& mpz_cmp(rel1->cols, rel2->cols) == 0) {
		return Cudd_bddLeq (rel1->context->manager, rel1->bdd, rel2->bdd);
	}
	else return FALSE;
}


/****************************************************************************/
/* NAME: test_on_eind                                                       */
/* FUNKTION: testet Relation auf Eindeutigkeit                              */
/* UEBERGABEPARAMETER: Rel * (die Relation)                                 */
/* RUECKGABEWERT: TRUE falls ja, sonst FALSE                                */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 10.04.95                                                             */
/* LETZTE AENDERUNG AM: 06.08.1999 (Umstellung auf BDD-Version)             */
/****************************************************************************/
//
Kure_bool test_on_eind (KureRel * rel)
{
	KureContext * context = rel->context;
	DdManager * manager = rel->context->manager;
	DdNode * ident, *ident_neg;
	DdNode * tmp, *rel_neg;
	Kure_bool ret = FALSE;

	ident = ident_bdd (rel->context, rel->cols);
	Cudd_Ref(ident);
	ident_neg = neg_bdd (context, ident, rel->cols, rel->cols);
	Cudd_Ref(ident_neg);
	Cudd_RecursiveDeref(manager, ident);

	tmp = komp_bdd (manager, rel->bdd, ident_neg, rel->rows, rel->cols);
	Cudd_Ref(tmp);
	Cudd_RecursiveDeref(manager, ident_neg);
	rel_neg = neg_bdd (context, rel->bdd, rel->rows, rel->cols);
	Cudd_Ref(rel_neg);

	ret = Cudd_bddLeq(manager, tmp, rel_neg);
	Cudd_RecursiveDeref(manager, tmp);
	Cudd_RecursiveDeref(manager, rel_neg);

	return ret;
}


/****************************************************************************/
/* NAME: line_completeness                                                  */
/* FUNKTION: prueft eine Relation ob sie zeilenkonstant ist                 */
/* UEBERGABEPARAMETER: Rel * (die Relation)                                 */
/* RUECKGABEWERT: TRUE falls ja, sonst FALSE                                */
/* ERSTELLT VON: Peter Schneider                                            */
/* AM: 14.11.95                                                             */
/* LETZTE AENDERUNG AM: 07.10.1999 (Ausnutzung der BDD-Routinen)            */
/* 01.04.2000 (Umstellung auf grosse Zahlen)                                */
/****************************************************************************/
Kure_bool line_completeness (KureRel * r)
{
	if (mpz_cmp_si(r->cols, 1) == 0) {
		/* Relations with one columns are trivially line complete. */
		return TRUE;
	}
	else {
		KureContext * context = r->context;
		DdManager * manager = context->manager;
		int num_vars_rows = Kure_num_vars(r->rows);
		int num_vars_cols = Kure_num_vars(r->cols);
		int * array, i;
		DdNode * cube, *tmp1, *tmp2, *L_row;
		Kure_bool ret = FALSE;

		array = (int *) alloca (num_vars_cols * sizeof (int));
		for (i = 0; i < num_vars_cols; i ++)
			array [i] = num_vars_rows + i;
		cube = Cudd_IndicesToCube (manager, array, num_vars_cols);
		Cudd_Ref(cube);

	    /* Ueber die Variablen der Spalten wird Existenz-quantifiziert */
	    tmp1 = Cudd_bddExistAbstract (manager, r->bdd, cube);
	    Cudd_Ref(tmp1);
	    Cudd_RecursiveDeref(manager, cube);

	    /* L_row stellt eine Zeile mit allen Eintraegen dar */
	    L_row = L_row_bdd (context, r->cols, num_vars_rows);
	    Cudd_Ref (L_row);

	    tmp2 = and_bdd(manager, tmp1, L_row);
	    Cudd_Ref(tmp2);
	    Cudd_RecursiveDeref(manager, tmp1);
	    Cudd_RecursiveDeref(manager, L_row);

	    ret = Cudd_bddLeq(manager, tmp2, r->bdd);
	    Cudd_RecursiveDeref(manager, tmp2);

	    return ret;
	}
}
