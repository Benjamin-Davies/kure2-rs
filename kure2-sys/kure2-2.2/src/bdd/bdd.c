#include "KureImpl.h"

/****************************************************************************/
/* NAME: build_bdd                                                          */
/* FUNKTION: Baut aus einem Array von BDDs einen kompletten BDD             */
/* UEBERGABEPARAMETER: DdNode * [] (Array von BDD),                         */
/*                     int (Anzahl der BDDs im Array)                       */
/* RUECKGABEWERT: DdNode *                                                  */
/* ERSTELLT VON: Ulf Milanese                                               */
/* AM: 16.02.2000                                                           */
/* LETZTE AENDERUNG AM: 16.02.2000                                          */
/* Umstellung auf grosse Zahlen 01.08.2000                                  */
/****************************************************************************/
DdNode * build_bdd (DdManager * manager, DdNode ** array, int number_of_bdds)
{
  DdNode * zero = null_bdd (manager);
  int index = number_of_vars2 (number_of_bdds) - 1;
  int left = number_of_bdds;
  int i = 0, j = 0;

  Cudd_Ref(zero);
  for ( ; index >= 0; index--)
  {
		DdNode * node = Cudd_bddIthVar(manager, index);

		i = 0;
		j = 0;

		while (i < (left - 1)) {
			DdNode * tmp = Cudd_bddIte(manager, node, array[i + 1], array[i]);
			Cudd_Ref(tmp);
			Cudd_IterDerefBdd(manager, array[i]);
			Cudd_IterDerefBdd(manager, array[i+1]);
			array[j] = tmp;
			i += 2;
			j++;
		}

		if (left & 0x1) {
			DdNode * tmp = Cudd_bddIte(manager, node, zero, array[i]);
			Cudd_Ref(tmp);
			Cudd_IterDerefBdd(manager, array[i]);
			array[j] = tmp;
			j++;
		}

		left = j;
  }

  Cudd_Deref (array [0]);
  return (array [0]);
}

