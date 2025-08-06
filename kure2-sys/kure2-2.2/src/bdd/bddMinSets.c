/*
 * bddMinSets.c
 *
 *  Copyright (C) 2012 Stefan Bolus, University of Kiel, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#define _KURE_USE_CUDD_INTERNALS 1

#include "KureImpl.h"
#include <assert.h>

// REMARK: Cudd_E/T, use Cudd_Regular for its argument; cuddT/E do not.

/* NOTE: When a local cache is used (e.g. GHashTable), all nodes stored in
 * the cache have to be references beforehand and have to be deref'd when
 * the cache is cleared. Otherwise, nodes stored in the cache may die. */

/* Some macros, that make life with Cudd a little more comfortable. */
#define CUDD_ONE(manager)  Cudd_ReadOne(manager)
#define CUDD_ZERO(manager) Cudd_Not(CUDD_ONE(manager))
#define CUDD_VAR(node) (Cudd_Regular(node)->index)
#define CUDD_GETREF(node) (Cudd_Regular(node)->ref)

/* Used as keys for the global CUDD cache below. */
static DdNode * _minsets_upset_key (DdManager * manager, DdNode * arg1, DdNode * arg2) { return 0; }
static DdNode * _maxsets_downset_key (DdManager * manager, DdNode * arg1, DdNode * arg2) { return 0; }
static DdNode * _minsets_key (DdManager * manager, DdNode * arg1, DdNode * arg2) { return 0; }
static DdNode * _maxsets_key (DdManager * manager, DdNode * arg1, DdNode * arg2) { return 0; }


static DdNode * _minsets_upset_aux (DdManager * manager, size_t n, DdNode * f, size_t level)
{
	if (f == CUDD_ZERO(manager)) return f;
	else if (f == CUDD_ONE(manager) && level >= n) return f;
	else if (level < CUDD_VAR(f) /* i.e. we have skipped a redundant node */) {
		DdNode *r, *tmp = _minsets_upset_aux (manager, n, f, level+1);
		Cudd_Ref(tmp);

		/* Because ZERO is complemented, apply de Morgan's law. */
		r = Cudd_Not(cuddUniqueInter(manager, level, CUDD_ONE(manager), Cudd_Not(tmp)));

		Cudd_Deref(tmp);

		return r;
	}
	else {
		DdNode *t,*e,*r,*tmp,*fv,*fnv;
		DdNode * nth = Cudd_bddIthVar(manager, n);

		assert(CUDD_VAR(f) == level);
		if (CUDD_GETREF(f) > 1) {
			/* Storing the n in the cache is necessary because new variables
			 * are added at the bottom and results can change if these new
			 * redundant nodes have been ignored in previous runs. */
			r = cuddCacheLookup2 (manager, _minsets_upset_key, nth, f);
			if (r) return r;
		}

		fv = Cudd_T(f);
		fnv = Cudd_E(f);
		if (Cudd_IsComplement(f)) {
			fv = Cudd_Not(fv); fnv = Cudd_Not(fnv);
		}

		tmp = _minsets_upset_aux (manager, n, fv, level+1);
		Cudd_Ref(tmp);
		e = _minsets_upset_aux (manager, n, fnv, level+1);
		Cudd_Ref(e);
		// perform: t := min(then(f)) setminus min(else(f))
		t = Cudd_bddAnd(manager, tmp, Cudd_Not(e));
		Cudd_Ref(t);

		if (t == e) // both could be ZERO.
			r = t;
		else if (Cudd_IsComplement(t))
			r = Cudd_Not(cuddUniqueInter(manager,level,Cudd_Not(t),Cudd_Not(e)));
		else
			r = cuddUniqueInter(manager,level,t,e);

		Cudd_Ref (r);

		Cudd_RecursiveDeref(manager, tmp); // not referenced by the returned node.
		Cudd_Deref(t);
		Cudd_Deref(e);

		if (CUDD_GETREF(f) > 1) {
			cuddCacheInsert2 (manager, _minsets_upset_key, nth, f, r);
		}

		Cudd_Deref(r);

		return r;
	}
}



static DdNode * _not_sup_set_aux (DdManager * manager, DdNode * f, DdNode * g)
{
	DdNode * const zero = CUDD_ZERO(manager);

	if (f == g) return zero;
	else if (g == zero) return f;
	//else if (contains_empty_set(manager, g)) return zero;
	else if (f == zero) return zero;
	else {
		DdNode * res = NULL;

		if (CUDD_GETREF(f) > 1 || CUDD_GETREF(g) > 1) {
			res = cuddCacheLookup2 (manager, _minsets_key, f, g);
		}

		if ( !res) /* cache-miss */ {
			DdNode *e, *t;
			int top_f = CUDD_VAR(f), top_g = CUDD_VAR(g);

			assert(Cudd_ReadPerm (manager, top_f) == top_f);
			assert(Cudd_ReadPerm (manager, top_g) == top_g);

			if (top_f < top_g) {
				DdNode *fv, *fnv;

				fv  = Cudd_T (f);
				fnv = Cudd_E (f);
				if (Cudd_IsComplement (f)) {
					fv = Cudd_Not (fv);
					fnv = Cudd_Not (fnv);
				}

				e = _not_sup_set_aux (manager, fnv, g);
				Cudd_Ref (e);
				t = _not_sup_set_aux (manager, fv, g);
				Cudd_Ref (t);
			}
			else if (top_f == top_g) {
				DdNode *tmp, *fv, *fnv, *gv, *gnv;

				fv  = Cudd_T (f);
				fnv = Cudd_E (f);
				if (Cudd_IsComplement (f)) {
					fv = Cudd_Not (fv);
					fnv = Cudd_Not (fnv);
				}

				gv  = Cudd_T (g);
				gnv = Cudd_E (g);
				if (Cudd_IsComplement (g)) {
					gv = Cudd_Not (gv);
					gnv = Cudd_Not (gnv);
				}

				e = _not_sup_set_aux (manager, fnv, gnv);
				Cudd_Ref (e);

				tmp = Cudd_bddOr (manager, gv, gnv);
				Cudd_Ref (tmp);

				t = _not_sup_set_aux (manager, fv, tmp);
				Cudd_Ref (t);

				Cudd_RecursiveDeref (manager, tmp);
			}
			else if (top_f > top_g) {
				DdNode *tmp, *gv, *gnv;

				gv  = Cudd_T (g);
				gnv = Cudd_E (g);
				if (Cudd_IsComplement (g)) {
					gv = Cudd_Not (gv);
					gnv = Cudd_Not (gnv);
				}

				e = _not_sup_set_aux (manager, f, gnv);
				Cudd_Ref (e);

				tmp = Cudd_bddOr (manager, gv, gnv);
				Cudd_Ref (tmp);

				t = _not_sup_set_aux (manager, f, tmp);
				Cudd_Ref (t);

				Cudd_RecursiveDeref (manager, tmp);
			}

			if (t == e)
				res = t;
			else {
				int index = MIN(top_g,top_f);
				if (Cudd_IsComplement(t))
					res = Cudd_Not(cuddUniqueInter(manager, index, Cudd_Not(t), Cudd_Not(e)));
				else
					res = cuddUniqueInter(manager, index, t, e);
			}

			Cudd_Deref (t);
			Cudd_Deref (e);


			if (CUDD_GETREF(f) > 1 || CUDD_GETREF(g) > 1) {
				cuddCacheInsert2 (manager, _not_sup_set_aux, f, g, res);
			}
		}

		return res;
	}
}



static DdNode * _not_sub_set_aux (DdManager * manager, DdNode * f, DdNode * g)
{
	DdNode * const zero = CUDD_ZERO(manager);

	if (f == g) return zero;
	else if (g == zero) return f;
	//else if (contains_empty_set(manager, g)) return zero;
	else if (f == zero) return zero;
	else {
		DdNode * res = NULL;

		if (CUDD_GETREF(f) > 1 || CUDD_GETREF(g) > 1) {
			res = cuddCacheLookup2 (manager, _minsets_key, f, g);
		}

		if ( !res) /* cache-miss */ {
			DdNode *e, *t;
			int top_f = CUDD_VAR(f), top_g = CUDD_VAR(g);

			assert(Cudd_ReadPerm (manager, top_f) == top_f);
			assert(Cudd_ReadPerm (manager, top_g) == top_g);

			if (top_f < top_g) {
				DdNode *fv, *fnv;

				fv  = Cudd_T (f);
				fnv = Cudd_E (f);
				if (Cudd_IsComplement (f)) {
					fv = Cudd_Not (fv);
					fnv = Cudd_Not (fnv);
				}

				e = _not_sub_set_aux (manager, fnv, g);
				Cudd_Ref (e);
				t = _not_sub_set_aux (manager, fv, g);
				Cudd_Ref (t);
			}
			else if (top_f == top_g) {
				DdNode *tmp, *fv, *fnv, *gv, *gnv;

				fv  = Cudd_T (f);
				fnv = Cudd_E (f);
				if (Cudd_IsComplement (f)) {
					fv = Cudd_Not (fv);
					fnv = Cudd_Not (fnv);
				}

				gv  = Cudd_T (g);
				gnv = Cudd_E (g);
				if (Cudd_IsComplement (g)) {
					gv = Cudd_Not (gv);
					gnv = Cudd_Not (gnv);
				}

				tmp = Cudd_bddOr (manager, gv, gnv);
				Cudd_Ref (tmp);

				e = _not_sub_set_aux (manager, fnv, tmp);
				Cudd_Ref (e);
				Cudd_RecursiveDeref (manager, tmp);

				t = _not_sub_set_aux (manager, fv, gv);
				Cudd_Ref (t);
			}
			else if (top_f > top_g) {
				DdNode *tmp, *gv, *gnv;

				gv  = Cudd_T (g);
				gnv = Cudd_E (g);
				if (Cudd_IsComplement (g)) {
					gv = Cudd_Not (gv);
					gnv = Cudd_Not (gnv);
				}

				t = _not_sub_set_aux (manager, f, gv);
				Cudd_Ref (t);

				tmp = Cudd_bddOr (manager, gv, gnv);
				Cudd_Ref (tmp);

				e = _not_sub_set_aux (manager, f, tmp);
				Cudd_Ref (e);

				Cudd_RecursiveDeref (manager, tmp);
			}

			if (t == e)
				res = t;
			else {
				int index = MIN(top_g,top_f);
				if (Cudd_IsComplement(t))
					res = Cudd_Not(cuddUniqueInter(manager, index, Cudd_Not(t), Cudd_Not(e)));
				else
					res = cuddUniqueInter(manager, index, t, e);
			}

			Cudd_Deref (t);
			Cudd_Deref (e);


			if (CUDD_GETREF(f) > 1 || CUDD_GETREF(g) > 1) {
				cuddCacheInsert2 (manager, _not_sup_set_aux, f, g, res);
			}
		}

		return res;
	}
}



/*!
 * Adaption of extraZddMinimal in the Extra 2.0 library by Alan Mishchenko.
 *
 * \author stefan
 * \date Aug 7, 2012
 */
static DdNode * _minsets_aux (DdManager * manager, size_t n, DdNode * f, size_t level)
{
	if (f == CUDD_ZERO(manager)) return f;
	else if (f == CUDD_ONE(manager) && level >= n) return f;
	else if (level < CUDD_VAR(f) /* i.e. have we skipped a redundant node */) {
		DdNode *r, *tmp = _minsets_aux (manager, n, f, level+1);
		Cudd_Ref(tmp);

		/* Because ZERO is complemented, apply de Morgan's law. */
		r = Cudd_Not(cuddUniqueInter(manager, level, CUDD_ONE(manager), Cudd_Not(tmp)));

		Cudd_Deref(tmp);

		return r;
	}
	else {
		DdNode *t,*e,*r,*tmp,*fv,*fnv;
		DdNode * nth = Cudd_bddIthVar(manager, n);

		assert(CUDD_VAR(f) == level);
		if (CUDD_GETREF(f) > 1) {
			/* Storing the n in the cache is necessary because new variables
			 * are added at the bottom and results can change if these new
			 * redundant nodes have been ignored in previous runs. */
			r = cuddCacheLookup2 (manager, _minsets_key, nth, f);
			if (r) return r;
		}

		fv = Cudd_T(f);
		fnv = Cudd_E(f);
		if (Cudd_IsComplement(f)) {
			fv = Cudd_Not(fv);
			fnv = Cudd_Not(fnv);
		}

		tmp = _minsets_aux (manager, n, fv, level+1);
		Cudd_Ref(tmp);
		e = _minsets_aux (manager, n, fnv, level+1);
		Cudd_Ref(e);
		t = not_sup_set (manager, tmp, e);
		Cudd_Ref(t);


		if (t == e) // both could be ZERO.
			r = t;
		else if (Cudd_IsComplement(t))
			r = Cudd_Not(cuddUniqueInter(manager,level,Cudd_Not(t),Cudd_Not(e)));
		else
			r = cuddUniqueInter(manager,level,t,e);

		Cudd_Ref (r);

		// tmp may have no further reference.
		Cudd_RecursiveDeref(manager, tmp);

		Cudd_Deref(t);
		Cudd_Deref(e);

		if (CUDD_GETREF(f) > 1) {
			cuddCacheInsert2 (manager, _minsets_key, nth, f, r);
		}

		Cudd_Deref(r);

		return r;
	}
}



static DdNode * _maxsets_downset_aux (DdManager * manager, size_t n, DdNode * f, size_t level)
{
	if (f == CUDD_ZERO(manager)) return f;
	else if (f == CUDD_ONE(manager) && level >= n) return f;
	else if (level < CUDD_VAR(f) /* i.e. we have skipped a redundant node */) {
		DdNode *res;
		DdNode *e = CUDD_ZERO(manager);
		DdNode *t = _maxsets_downset_aux (manager, n, f, level+1);
		Cudd_Ref(t);

		if (Cudd_IsComplement(t))
			res = Cudd_Not(cuddUniqueInter(manager, level, Cudd_Not(t), Cudd_Not(e)));
		else
			res = cuddUniqueInter(manager, level, t,e);

		Cudd_Deref(t);

		return res;
	}
	else {
		DdNode *t,*e,*res,*tmp,*fv,*fnv;
		DdNode * nth = Cudd_bddIthVar(manager, n);

		assert(CUDD_VAR(f) == level);
		if (CUDD_GETREF(f) > 1) {
			/* Storing the n in the cache is necessary because new variables
			 * are added at the bottom and results can change if these new
			 * redundant nodes have been ignored in previous runs. */
			res = cuddCacheLookup2 (manager, _maxsets_downset_key, nth, f);
			if (res) return res;
		}

		fv = Cudd_T(f);
		fnv = Cudd_E(f);
		if (Cudd_IsComplement(f)) {
			fv = Cudd_Not(fv); fnv = Cudd_Not(fnv);
		}

		t = _maxsets_downset_aux (manager, n, fv, level+1);
		Cudd_Ref(t);
		tmp = _maxsets_downset_aux (manager, n, fnv, level+1);
		Cudd_Ref(tmp);
		// perform: e := max(else(f)) setminus max(then(f))
		e = Cudd_bddAnd(manager, tmp, Cudd_Not(t));
		Cudd_Ref(e);

		if (t == e) // both could be ZERO.
			res = t;
		else if (Cudd_IsComplement(t))
			res = Cudd_Not(cuddUniqueInter(manager,level,Cudd_Not(t),Cudd_Not(e)));
		else
			res = cuddUniqueInter(manager,level,t,e);

		Cudd_Ref (res);

		Cudd_RecursiveDeref(manager, tmp); // not referenced by the returned node.
		Cudd_Deref(t);
		Cudd_Deref(e);

		if (CUDD_GETREF(f) > 1) {
			cuddCacheInsert2 (manager, _maxsets_downset_key, nth, f, res);
		}

		Cudd_Deref(res);

		return res;
	}
}



/*!
 * Adaption of extraZddMaximal in the Extra 2.0 library by Alan Mishchenko.
 *
 * \author stefan
 * \date Aug 8, 2012
 */
static DdNode * _maxsets_aux (DdManager * manager, size_t n, DdNode * f, size_t level)
{
	if (f == CUDD_ZERO(manager)) return f;
	else if (f == CUDD_ONE(manager) && level >= n) return f;
	else if (level < CUDD_VAR(f) /* i.e. have we skipped a redundant node */) {
		DdNode *res;
		DdNode *e = CUDD_ZERO(manager);
		DdNode *t = _maxsets_aux (manager, n, f, level+1);
		Cudd_Ref(t);

		if (Cudd_IsComplement(t))
			res = Cudd_Not(cuddUniqueInter(manager, level, Cudd_Not(t), Cudd_Not(e)));
		else
			res = cuddUniqueInter(manager, level, t,e);

		Cudd_Deref(t);

		return res;
	}
	else {
		DdNode *t,*e,*r,*tmp,*fv,*fnv;
		DdNode * nth = Cudd_bddIthVar(manager, n);

		assert(CUDD_VAR(f) == level);
		if (CUDD_GETREF(f) > 1) {
			/* Storing the n in the cache is necessary because new variables
			 * are added at the bottom and results can change if these new
			 * redundant nodes have been ignored in previous runs. */
			r = cuddCacheLookup2 (manager, _maxsets_key, nth, f);
			if (r) return r;
		}

		fv = Cudd_T(f);
		fnv = Cudd_E(f);
		if (Cudd_IsComplement(f)) {
			fv = Cudd_Not(fv);
			fnv = Cudd_Not(fnv);
		}

		t = _maxsets_aux (manager, n, fv, level+1);
		Cudd_Ref(t);
		tmp = _maxsets_aux (manager, n, fnv, level+1);
		Cudd_Ref(tmp);
		e = not_sub_set (manager, tmp, t);
		Cudd_Ref(e);

		if (t == e) // both could be ZERO.
			r = t;
		else if (Cudd_IsComplement(t))
			r = Cudd_Not(cuddUniqueInter(manager,level,Cudd_Not(t),Cudd_Not(e)));
		else
			r = cuddUniqueInter(manager,level,t,e);

		Cudd_Ref (r);

		// tmp may have no further reference.
		Cudd_RecursiveDeref(manager, tmp);

		Cudd_Deref(t);
		Cudd_Deref(e);

		if (CUDD_GETREF(f) > 1) {
			cuddCacheInsert2 (manager, _maxsets_key, nth, f, r);
		}

		Cudd_Deref(r);

		return r;
	}
}


/*!
 * Returns whether set(f) contains the empty set, that is, if the path
 * containing only 0-edges ends in the 1-sink.
 *
 * \author stefan
 * \date Aug 7, 2012
 *
 * \param manager The CUDD manager.
 * \param f The Boolean function.
 */
Kure_bool contains_empty_set (DdManager * manager, DdNode * f)
{
	if (Cudd_IsConstant(f))
		return Cudd_ReadOne(manager) == f;
	else {
		DdNode * fnv = Cudd_E(f); // take the 0-edge.

		if (Cudd_IsComplement(f)) {
			fnv = Cudd_Not(fnv);
		}

		return contains_empty_set (manager, fnv);
	}
}


/*!
 * Returns whether set(f) contains the 'all' set, that is, if the path
 * containing only 1-edges ends in the 1-sink.
 *
 * \author stefan
 * \date Aug 8, 2012
 *
 * \param manager The CUDD manager.
 * \param f The Boolean function.
 */
Kure_bool contains_all_set (DdManager * manager, DdNode * f)
{
	if (Cudd_IsConstant(f))
		return Cudd_ReadOne(manager) == f;
	else {
		DdNode * fv = Cudd_T(f); // take the 1-edge.

		if (Cudd_IsComplement(f)) {
			fv = Cudd_Not(fv);
		}

		return contains_all_set (manager, fv);
	}
}


/*!
 * Returns a \ref DdNode that represents the empty set for the variables with
 * indices top,..,top+nvars-1. The only path ending in the 1-sink in that BDD
 * is the path with only 0-edges. All other paths end in the 0-sink.
 *
 * \note In the Kure library, the variables of the nodes correspond to the
 * indices of the variables in the variable ordering, because no reordering
 * happens. In other systems, where reordering may occur, this method should
 * not be used.
 *
 * \author stefan
 * \date Aug 7, 2012
 */
DdNode * empty_set (DdManager * manager, size_t top, size_t nvars)
{
	if (0 == nvars) {
		return CUDD_ONE(manager);
	}
	else {
		DdNode * res;
		DdNode * tmp = empty_set (manager, top+1, nvars-1);
		Cudd_Ref (tmp);

		/* Because ZERO is complemented, apply de Morgan's law. */
		res = Cudd_Not(cuddUniqueInter(manager, top,
					CUDD_ONE(manager), // 1-edge points to ZERO (i.e. -ONE)
					Cudd_Not(tmp) // paths of 0-edges points to 1-sink
				));
		Cudd_Deref (tmp);
		return res;
	}
}


/*!
 * Returns a \ref DdNode that represents the all set for the variables with
 * indices top,..,top+nvars-1. The only path ending in the 1-sink in that BDD
 * is the path with only 1-edges. All other paths end in the 0-sink.
 *
 * \note In the Kure library, the variables of the nodes correspond to the
 * indices of the variables in the variable ordering, because no reordering
 * happens. In other systems, where reordering may occur, this method should
 * not be used.
 *
 * \author stefan
 * \date Aug 8, 2012
 */
DdNode * all_set (DdManager * manager, size_t top, size_t nvars)
{
	if (0 == nvars) {
		return CUDD_ONE(manager);
	}
	else {
		DdNode * res;
		DdNode * tmp = all_set (manager, top+1, nvars-1);
		Cudd_Ref (tmp);

		if (Cudd_IsComplement(tmp))
			res = Cudd_Not (cuddUniqueInter (manager, top,
					Cudd_Not(tmp), // paths of 1-edges points to 1-sink
					CUDD_ONE(manager))); // 1-edge points to ZERO (i.e. -ONE)
		else
			res = cuddUniqueInter (manager, top,
					tmp, // paths of 1-edges points to 1-sink
					CUDD_ZERO(manager)); // 1-edge points to ZERO (i.e. -ONE)

		Cudd_Deref (tmp);
		return res;
	}
}


/*!
 * Removes all elements S in set(f) for which there is an element T in set(g)
 * such that T is a subset of S, formally:
 * \f[
 * \textsf{set}(\textsf{notsupset}(f,g)) =
 *   \{ x\in \textsf{set}(f) \mid \forall y\in \textsf{set}(g) \colon y\not \subset x \}\,.
 * \f]
 * The implementation has been adopted from that
 * of extraZddNotSupSet in the Extra 2.0 library by Alan Mishchenko. The
 * function is used to implement the minsets operation. See also appendix B
 * in "An Introduction to Zero-Suppressed Binary Decision Diagrams" by
 * A. Mishchenko (2001).
 *
 * \note The function is non-static for testing purposes.
 *
 * \author stefan
 * \date Aug 7, 2012
 */
DdNode * not_sup_set (DdManager * manager, DdNode * f, DdNode * g)
{
	return _not_sup_set_aux (manager,f,g);
}


/*!
 * Removes all elements S in set(f) for which there is an element T in set(g)
 * such that S is a subset of T, formally:
 * \f[
 * \textsf{set}(\textsf{notsupset}(f,g)) =
 *   \{ x\in \textsf{set}(f) \mid \forall y\in \textsf{set}(g) \colon x\not \subset y \}\,.
 * \f]
 * The implementation has been adopted from that
 * of extraZddNotSupSet in the Extra 2.0 library by Alan Mishchenko. The
 * function is used to implement the minsets operation. See also appendix B
 * in "An Introduction to Zero-Suppressed Binary Decision Diagrams" by
 * A. Mishchenko (2001).
 *
 * \note The function is exported for testing purposes.
 *
 * \author stefan
 * \date Aug 8, 2012
 */
DdNode * not_sub_set (DdManager * manager, DdNode * f, DdNode * g)
{
	return _not_sub_set_aux (manager,f,g);
}


/*!
 * \todo Add description.
 *
 * \author stefan
 * \date Aug 6, 2012
 *
 * \param manager The CUDD manager.
 * \param nvars The number of variables.
 * \param f The Boolean function.
 * \returns Returns a \ref DdNode representing the minimal elements of f.
 */
DdNode * minsets_upset (DdManager * manager, size_t nvars, DdNode * f)
{
	if ( !Cudd_IsConstant(f) && contains_empty_set (manager, f))
		return empty_set (manager, 0, nvars);
	else
		return _minsets_upset_aux (manager, nvars, f, 0);
}


/*!
 * \todo Add description.
 *
 * \author stefan
 * \date Aug 7, 2012
 *
 * \param manager The CUDD manager.
 * \param nvars The number of variables.
 * \param f The Boolean function.
 * \returns Returns a \ref DdNode representing the minimal elements of f.
 */
DdNode * minsets (DdManager * manager, size_t nvars, DdNode * f)
{
	if ( !Cudd_IsConstant(f) && contains_empty_set (manager, f))
		/* Even CUDD_VAR(f) could be unequal to 0, we always assume that start
		 * with the first variable, as it is true in Kure2. */
		return empty_set (manager, 0 , nvars);
	else
		return _minsets_aux (manager, nvars, f, 0);
}


/*!
 * \todo Add description.
 *
 * \author stefan
 * \date Aug 7, 2012
 *
 * \param manager The CUDD manager.
 * \param nvars The number of variables.
 * \param f The Boolean function.
 * \returns Returns a \ref DdNode representing the maximal elements of f.
 */
DdNode * maxsets_downset (DdManager * manager, size_t nvars, DdNode * f)
{
	if ( !Cudd_IsConstant(f) && contains_all_set (manager, f))
		/* Even CUDD_VAR(f) could be unequal to 0, we always assume that start
		 * with the first variable, as it is true in Kure2. */
		return all_set (manager, 0 , nvars);
	else
		return _maxsets_downset_aux (manager, nvars, f, 0);
}


/*!
 * \todo Add description.
 *
 * \author stefan
 * \date Aug 7, 2012
 *
 * \param manager The CUDD manager.
 * \param nvars The number of variables.
 * \param f The Boolean function.
 * \returns Returns a \ref DdNode representing the maximal elements of f.
 */
DdNode * maxsets (DdManager * manager, size_t nvars, DdNode * f)
{
	if ( !Cudd_IsConstant(f) && contains_all_set (manager, f))
		/* Even CUDD_VAR(f) could be unequal to 0, we always assume that start
		 * with the first variable, as it is true in Kure2. */
		return all_set (manager, 0 , nvars);
	else
		return _maxsets_aux (manager, nvars, f, 0);
}

