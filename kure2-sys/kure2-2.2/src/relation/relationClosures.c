#include "KureImpl.h"


/****************************************************************************/
/* NAME: transh_rel                                                         */
/* FUNKTION: berechnet die transitive Huelle einer Relation                 */
/* UEBERGABEPARAMETER: 2 x Rel * (Relationen), Argument                     */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/* 03.08.1999 (Umstellung auf BDD-Version)                                  */
/****************************************************************************/
// arg must be quadratic.
void transh_rel_old (KureRel * arg, KureRel * rop)
{
	if ( !arg || !rop) printf ("ERROR (transh_rel): One argument is missing\n");
	else {
		DdManager * manager = arg->context->manager;
		DdNode * tmp1;
		DdNode * tmp2;
		DdNode * tmp3;

		tmp1 = arg->bdd;
		Cudd_Ref (tmp1);

		/* Loop until the relation is transitive. */
		for ( ; tmp1 != tmp2 ; tmp1 = tmp3) {
			tmp2 = tmp1;
			tmp1 = komp_bdd(manager, tmp2, tmp2, arg->rows, arg->cols);
			Cudd_Ref (tmp1);
			tmp3 = or_bdd(manager, tmp1, tmp2);
			Cudd_Ref(tmp3);
			Cudd_RecursiveDeref(manager, tmp1);
			Cudd_RecursiveDeref(manager, tmp2);
		}

		if (tmp1) {
			Cudd_RecursiveDeref(manager, rop->bdd);
			rop->bdd = tmp1;
			if (arg != rop) {
				mpz_set (rop->rows, arg->rows);
				mpz_set (rop->cols, arg->cols);
			}
		}
	}
}


/****************************************************************************/
/* NAME: reflh_rel                                                          */
/* FUNKTION: berechnet die reflexive Huelle einer Relation                  */
/* UEBERGABEPARAMETER: 2 x Rel * (Relationen), Argument                     */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/****************************************************************************/
// rop := I(arg) | arg.
// arg must be quadratic
void reflh_rel (KureRel * arg, KureRel * rop)
{
	if (arg && rop) {
		if (arg == rop) {
			KureRel * aux = kure_rel_new_with_size(arg->context,
					arg->rows, arg->cols);
			eins_rel(aux);
			oder_rel(arg, aux, rop);
			kure_rel_destroy(aux);
		}
		else {
			mpz_set(rop->cols, arg->cols);
			mpz_set(rop->rows, arg->rows);
			eins_rel(rop);
			oder_rel(arg,rop,rop);
		}
	}
}


/****************************************************************************/
/* NAME: symh_rel                                                           */
/* FUNKTION: berechnet die symetrische Huelle einer Relation                */
/* UEBERGABEPARAMETER: 2 x Rel * (Relationen), Argument                     */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: ???????? (Uebernahme aus Muenchen)                         */
/* AM: ????????                                                             */
/* LETZTE AENDERUNG AM: 15.07.1999 (Kein Rueckgabewert)                     */
/****************************************************************************/
// rop := arg | arg^, so arg must be homogeneous
void symmh_rel (KureRel * arg, KureRel * rop)
{
	if (arg && rop) {
		if (arg == rop) {
			KureRel * aux = kure_rel_new(arg->context);
			transp_rel(arg, aux);
			oder_rel(arg, aux, rop);
			kure_rel_destroy(aux);
		}
		else {
			transp_rel(arg, rop);
			oder_rel(arg, rop, rop);
		}
	}
}


/****************************************************************************/
/* NAME: transh_rel_warshall                                                */
/* FUNKTION: berechnet die transitive Huelle einer Relation                 */
/* UEBERGABEPARAMETER: 2 x Rel * (Relationen), Argument                     */
/*                     und Ergebnisrelation                                 */
/* RUECKGABEWERT: KEINER                                                    */
/* ERSTELLT VON: Barbara Leoniuk                                            */
/* AM: 01.06.2001                                                           */
/****************************************************************************/
//Input must be quadratic.
void transh_rel (KureRel * arg, KureRel * rop)
{
	if (arg && rop) {
		/* If there are more than 10000 rows, use the old algorithm?
		 * Why? -stb */
		if (mpz_cmp_si(arg->cols, 10000) > 0) {
			transh_rel_old(arg,rop);
		}
		else {
			DdManager * manager = arg->context->manager;
			DdNode * tmp = warshall_bdd (arg->context, arg->bdd, mpz_get_si(arg->cols));
			if (tmp) {
				Cudd_Ref (tmp);
				Cudd_RecursiveDeref(manager, rop->bdd);
				rop->bdd = tmp;
				if (arg != rop) {
					mpz_set (rop->rows, arg->rows);
					mpz_set (rop->cols, arg->cols);
				}
			}
		}
	}
	else printf ("ERROR (transh_rel): One argument is missing\n");
}
