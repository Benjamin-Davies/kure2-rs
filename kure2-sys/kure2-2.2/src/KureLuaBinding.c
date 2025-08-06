/*
 * KureLuaBinding.c
 *
 *  Copyright (C) 2010 Stefan Bolus, University of Kiel, Germany
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

#include "KureImpl.h" // Doms
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lauxlib.h>
#include <lualib.h>
#include "cuddInt.h" // garbage collection

/*!
 * TODO: All lua function which output to stdout or stderr should be changed
 *       to return strings instead. Using printf can be bad in GUI applications
 *       where there is no such output.
 */

//static char * _msg_rel_expected = "Relation expected.";
static char * _msg_unknown_error = "Unknown error.";

static char * _kure_rel_key= "Kure.Rel";
static char * _kure_dom_key= "Kure.Dom";
static char * _kure_context_key= "Kure.Context";


static void 	_lkure_where (lua_State * L, int level);
static int 		_lkure_error (lua_State * L, const char *fmt, ...);


// The pointer is on the top of the stack afterwards.
KureRel ** _kure_lua_rel_placeholder (lua_State * L)
{
	KureRel ** pself = (KureRel**) lua_newuserdata (L, sizeof(KureRel*));

	/* Set the metatable. This is necessary to tell lua what GC function should
	 * be called. */
	luaL_getmetatable(L, _kure_rel_key);
	lua_setmetatable(L, -2);

	return pself;
}

// The pointer is on the top of the stack afterwards.
KureRel * _kure_lua_rel_new (lua_State * L, KureContext * context)
{
	KureRel ** pself = _kure_lua_rel_placeholder (L);
	*pself = kure_rel_new (context);
	return *pself;
}

// Never returns NULL.
static KureRel *_rel_get_unprotected (lua_State * L, int pos)
{
	return *(KureRel**) luaL_checkudata (L, pos, _kure_rel_key);
}

Kure_bool kure_lua_isrel (lua_State * L, int pos)
{
	Kure_bool is_udata = lua_isuserdata(L,pos);

	if ( !is_udata || !lua_getmetatable(L,pos)) {
		return FALSE;
	}
	else {
		Kure_bool is_rel;

		/* At this point, the object is a user-data and has an associated
		 * metatable which is on the top of the stack. */
		luaL_getmetatable(L, _kure_rel_key);
		is_rel = lua_equal(L,-1,-2);
		lua_pop(L,2);
		return is_rel;
	}
}

// Returns a relation or NULL.
KureRel * kure_lua_torel (lua_State * L, int pos, KureError ** perr)
{
	Kure_bool is_udata = lua_isuserdata(L,pos);
	void * udata = (is_udata ? lua_touserdata(L,pos) : NULL);

	if ( !is_udata || !lua_getmetatable(L,pos)) {
		kure_set_error (perr, 0, "Unable to return %s at stack position %d, "
				"got %s.", _kure_rel_key, pos, lua_typename(L,pos));
		return NULL;
	}
	else {
		Kure_bool is_rel;

		/* At this point, the object is a user-data and has an associated
		 * metatable which is on the top of the stack. */
		luaL_getmetatable(L, _kure_rel_key);
		is_rel = lua_equal(L,-1,-2);
		lua_pop(L,2);
		if (is_rel) {
			return *(KureRel**) udata;
		}
		else {
			kure_set_error (perr, 0, "Unable to return %s at stack position %d, "
					"got %s.", _kure_rel_key, pos, lua_typename(L,pos));
			return NULL;
		}
	}
}

Kure_bool kure_lua_isdom (lua_State * L, int pos)
{
	Kure_bool is_udata = lua_isuserdata(L,pos);

	if ( !is_udata || !lua_getmetatable(L,pos)) {
		return FALSE;
	}
	else {
		Kure_bool is_dom;

		/* At this point, the object is a user-data and has an associated
		 * metatable which is on the top of the stack. */
		luaL_getmetatable(L, _kure_dom_key);
		is_dom = lua_equal(L,-1,-2);
		lua_pop(L,2);
		return is_dom;
	}
}

// Returns a domain or NULL.
KureDom * kure_lua_todom (lua_State * L, int pos, KureError ** perr)
{
	Kure_bool is_udata = lua_isuserdata(L,pos);
	void * udata = (is_udata ? lua_touserdata(L,pos) : NULL);

	if ( !is_udata || !lua_getmetatable(L,pos)) {
		kure_set_error (perr, 0, "Unable to return %s at stack position %d, "
				"got %s.", _kure_dom_key, pos, lua_typename(L,pos));
		return NULL;
	}
	else {
		Kure_bool is_dom;

		/* At this point, the object is a user-data and has an associated
		 * metatable which is on the top of the stack. */
		luaL_getmetatable(L, _kure_dom_key);
		is_dom = lua_equal(L,-1,-2);
		lua_pop(L,2);
		if (is_dom) {
			return *(KureDom**) udata;
		}
		else {
			kure_set_error (perr, 0, "Unable to return %s at stack position %d, "
					"got %s.", _kure_dom_key, pos, lua_typename(L,pos));
			return NULL;
		}
	}
}

// Never returns NULL.
static KureContext *_context_get (lua_State * L, int pos)
{
	return *(KureContext**) luaL_checkudata (L, pos, _kure_context_key);
}

static Kure_bool _is_context (lua_State * L, int pos)
{
	if (0 == lua_getmetatable (L, pos)) {
		return FALSE;
	}
	else {
		luaL_getmetatable (L, _kure_context_key);
		return lua_topointer(L,-1) == lua_topointer(L,-2);
	}
}

static Kure_bool _is_rel (lua_State * L, int pos)
{
	if (0 == lua_getmetatable (L, pos)) {
		return FALSE;
	}
	else {
		// TODO: Use lua_equal instead!
		luaL_getmetatable (L, _kure_rel_key);
		return lua_topointer(L,-1) == lua_topointer(L,-2);
	}
}

/////////////////////////////////////////////////////////////////// Domains ///

// The pointer is on the top of the stack afterwards.
KureDom ** _kure_lua_dom_placeholder (lua_State * L)
{
	KureDom ** pself = (KureDom**) lua_newuserdata (L, sizeof(KureDom*));

	/* Set the metatable. This is necessary to tell lua what GC function should
	 * be called. */
	luaL_getmetatable(L, _kure_dom_key);
	lua_setmetatable(L, -2);

	return pself;
}

// Never returns NULL.
KureDom *_dom_get (lua_State * L, int pos)
{
	return *(KureDom**) luaL_checkudata (L, pos, _kure_dom_key);
}

static int _dom_gc (lua_State * L)
{
	KureDom * self = _dom_get (L,1);
	kure_dom_destroy (self);
	return 0;
}


// Never passed on failure
#define HANDLE_FAILURE(success, context) if ( !(success)) { \
	KureError * err = kure_context_get_error (context); \
	if (err && err->message) return _lkure_error (L, err->message); \
	else return _lkure_error (L, _msg_unknown_error); } \

#define BINARY_OP(name) static int lkure_##name (lua_State * L) { \
	KureRel * a = _rel_get_unprotected (L, 1); \
	KureRel * b = _rel_get_unprotected (L, 2); \
\
	KureContext * context = kure_rel_get_context (a); \
	KureRel * rop; \
	Kure_success success; \
\
	rop = _kure_lua_rel_new (L, context); \
\
	success = kure_##name (rop, a,b); \
	HANDLE_FAILURE(success, context); \
	return 1; }

BINARY_OP(and)
BINARY_OP(or)
BINARY_OP(mult)
BINARY_OP(mult_transp_norm)
BINARY_OP(mult_norm_transp)
BINARY_OP(mult_transp_transp)
BINARY_OP(subsetvec_rel)
BINARY_OP(left_residue)
BINARY_OP(right_residue)
BINARY_OP(symm_quotient)

BINARY_OP(tupling)
BINARY_OP(left_tupling)
BINARY_OP(right_tupling)
BINARY_OP(direct_sum)

BINARY_OP(product_order)
BINARY_OP(sum_order)

// Rel R -> Rel S
#define UNARY_OP(name) static int lkure_##name (lua_State * L) { \
	KureRel * a = _rel_get_unprotected (L, 1); \
	KureContext * context = kure_rel_get_context (a); \
	KureRel * rop = _kure_lua_rel_new (L, context); \
	Kure_success success = kure_##name (rop, a); \
	HANDLE_FAILURE(success, context); \
	return 1; }

UNARY_OP(transpose)
UNARY_OP(complement)

UNARY_OP(less_card)

UNARY_OP(trans_hull)
UNARY_OP(symm_hull)
UNARY_OP(refl_hull)

UNARY_OP(domain)

UNARY_OP(vec_point)
UNARY_OP(atom)
UNARY_OP(vec_inj)

UNARY_OP(minsets_upset)
UNARY_OP(minsets)
UNARY_OP(maxsets_downset)
UNARY_OP(maxsets)

// Rel R, Dom -> Rel R
#define UNARY_DOM_OP2(name,impl_name) static int lkure_##name (lua_State * L) { \
	KureRel * rop = _rel_get_unprotected(L, 1); \
    KureDom * dom = _dom_get(L, 2); \
	Kure_success success = kure_##impl_name (rop, dom); \
	HANDLE_FAILURE(success, kure_rel_get_context(rop)); \
	lua_pushvalue(L,1); \
	return 1; }

UNARY_DOM_OP2(proj_1,proj_1_dom)
UNARY_DOM_OP2(proj_2,proj_2_dom)
UNARY_DOM_OP2(inj_1,inj_1_dom)
UNARY_DOM_OP2(inj_2,inj_2_dom)

#define LUA_PROTO(name, L) static int name (lua_State * L)

// First Component
// Context, Dom -> Rel
LUA_PROTO(lkure_dom_comp1, L)
{
	KureContext * context = _context_get (L, 1);
	KureDom * dom = _dom_get(L, 2);
	if (kure_dom_get_comp_count(dom) < 2)
		return _lkure_error (L, "Domain %s has less than two components.", kure_dom_get_name(dom));
	else {
		KureRel * rop = _kure_lua_rel_new (L, context);
		Kure_success success;
		mpz_t n;
		mpz_init (n);
		kure_dom_get_comp (dom, n, 0/*first*/);
		success = kure_rel_set_size(rop, n,n);
		mpz_clear (n);
		HANDLE_FAILURE(success, context);
		return 1;
	}
}

// Second Component
// Context, Dom -> Rel
LUA_PROTO(lkure_dom_comp2, L)
{
	KureContext * context = _context_get (L, 1);
	KureDom * dom = _dom_get(L, 2);
	if (kure_dom_get_comp_count(dom) < 2)
		return _lkure_error (L, "Domain %s has less than two components.", kure_dom_get_name(dom));
	else {
		KureRel * rop = _kure_lua_rel_new (L, context);
		Kure_success success;
		mpz_t n;
		mpz_init (n);
		kure_dom_get_comp (dom, n, 1/*second*/);
		success = kure_rel_set_size(rop, n,n);
		mpz_clear (n);
		HANDLE_FAILURE(success, context);
		return 1;
	}
}

// Vec v -> Rel R
LUA_PROTO(lkure_membership, L)
{
	KureRel * vec = _rel_get_unprotected (L, 1);
	if ( !kure_rel_rows_fits_si (vec)) {
		return _lkure_error (L, "Vector is too big, i.e. has more than %d rows.", INT_MAX);
	}
	else {
		KureContext * context = kure_rel_get_context (vec);
		KureRel * rop = _kure_lua_rel_new (L, context);
		int rows = kure_rel_get_rows_si (vec);
		Kure_success success = kure_membership (rop, rows);
		HANDLE_FAILURE(success, context);
		return 1;
	}
}

// Rel R, int, int -> same Rel R
#define BINARY_INT2(name, impl_name) static int lkure_##name (lua_State * L) { \
	KureRel * rop = _rel_get_unprotected(L, 1); \
	int fst = luaL_checkinteger(L, 2); \
	int sec = luaL_checkinteger(L, 3); \
	Kure_success success = kure_##impl_name (rop, fst, sec); \
	HANDLE_FAILURE(success, kure_rel_get_context (rop)); \
	lua_pushvalue(L, 1); \
	return 1; }

// Rel R, int, int -> same Rel R
#define BINARY_INT(name) BINARY_INT2(name,name)

BINARY_INT(total_funcs_si)
BINARY_INT(partial_funcs_si)
BINARY_INT(vec_begin_full_si)
BINARY_INT(proj_1_si)
BINARY_INT(proj_2_si)
BINARY_INT(inj_1_si)
BINARY_INT(inj_2_si)

// Rel R -> same Rel R
#define INPLACE_OP(name) static int lkure_##name (lua_State * L) { \
	KureRel * self = _rel_get_unprotected (L, 1); \
	Kure_success success = kure_##name (self); \
	HANDLE_FAILURE(success, kure_rel_get_context (self)); \
	lua_pushvalue(L, 1); \
	return 1; }

INPLACE_OP(null)
INPLACE_OP(all)
INPLACE_OP(identity)

// Rel -> bool
#define UNARY_TEST(name) static int lkure_##name (lua_State * L) { \
	KureRel * self = _rel_get_unprotected (L, 1); \
	Kure_success success; \
	Kure_bool res = kure_##name (self, &success); \
	HANDLE_FAILURE(success, kure_rel_get_context (self)); \
	lua_pushboolean(L, res); \
	return 1; }

UNARY_TEST(is_empty)
UNARY_TEST(is_univalent)
UNARY_TEST(is_vector)

// Rel, Rel -> bool
#define BINARY_TEST(name) static int lkure_##name (lua_State * L) { \
	KureRel * a = _rel_get_unprotected (L, 1); \
	KureRel * b = _rel_get_unprotected (L, 2); \
	Kure_success success; \
	Kure_bool res = kure_##name (a,b, &success); \
	HANDLE_FAILURE(success, kure_rel_get_context (a)); \
	lua_pushboolean(L, res); \
	return 1; }

BINARY_TEST(equals)
BINARY_TEST(includes)
BINARY_TEST(is_cardlt)
BINARY_TEST(is_cardleq)
BINARY_TEST(is_cardeq)

// Rel -> int
LUA_PROTO(lkure_rel_get_rows_si, L)
{
	KureRel * self = _rel_get_unprotected (L, 1);
	lua_pushinteger(L, kure_rel_get_rows_si(self));
	return 1;
}

// Rel -> int
LUA_PROTO(lkure_rel_get_cols_si, L)
{
	KureRel * self = _rel_get_unprotected (L, 1);
	lua_pushinteger(L, kure_rel_get_cols_si(self));
	return 1;
}

// Rel -> bool
LUA_PROTO(lkure_rel_fits_si, L)
{
	KureRel * self = _rel_get_unprotected (L, 1);
	lua_pushboolean(L, kure_rel_fits_si(self));
	return 1;
}

#if 0
// Cannot be implemented until we know how we can lookup the userdata
// for the existing entry.

// Rel -> Context
LUA_PROTO(lkure_rel_get_context, L)
{
	KureRel * self = _rel_get_unprotected (L, 1);

}
#endif

// Rel, Rel -> bool
LUA_PROTO(lkure_rel_same_dim, L)
{
	KureRel * a = _rel_get_unprotected (L, 1);
	KureRel * b = _rel_get_unprotected (L, 2);
	lua_pushboolean(L, kure_rel_same_dim(a,b));
	return 1;
}


/* We shouldn't handle callbacks for random functions on the Lua level.
 * This is because we would have to call Lua each time we need a
 * random value. Hence, the random function must be chosen on the C
 * level.
 *
 * If the second argument is a relation R, then the probability is
 * |R| / (cols(R) * rows(R)).
 *
 * Rel x, (Prob or Rel) -> Rel x
 */
static int lkure_random (lua_State * L)
{
	KureRel * self = _rel_get_unprotected (L,1);
	KureContext * context = kure_rel_get_context (self);

	double prob;

	if (kure_lua_isrel (L,2)) {
		KureRel * P = _rel_get_unprotected (L,2);
		mpz_t entries, cols, size;
		mpf_t fprob, fsize, fentries;

		mpz_init (entries);
		mpz_init (cols);
		mpz_init (size);

		kure_get_entries (P, entries);
		kure_rel_get_rows (P, size); // size = rows
		kure_rel_get_cols (P, cols);

		mpz_mul (size, size, cols); // size = size * cols

		mpf_init (fprob);
		mpf_init (fsize);
		mpf_init (fentries);
		mpf_set_z (fsize, size);
		mpf_set_z (fentries, entries);

		mpf_div (fprob, fentries, fsize); // fprob = fentries / fsize

		prob = mpf_get_d (fprob);

		mpf_clear (fprob);
		mpf_clear (fsize);
		mpf_clear (fentries);

		mpz_clear (size);
		mpz_clear (cols);
		mpz_clear (entries);
	}
	else /* we assume a probability*/ {
		prob = luaL_checknumber (L,2);
	}

	Kure_success success = kure_random_simple (self, prob);
	HANDLE_FAILURE(success, context);
	// Return the relation.
	lua_pushvalue(L,1);
	return 1;
}


//static int lkure_random (lua_State * L)
//{
//	KureRel * self = _rel_get_unprotected (L,1);
//	KureContext * context = kure_rel_get_context (self);
//	double prob = luaL_checknumber (L,2);
//
//	Kure_success success = kure_random_simple (self, prob);
//	HANDLE_FAILURE(success, context);
//	// Return the relation.
//	lua_pushvalue(L,1);
//	return 1;
//}


// Rel x, Prob -> Rel x
static int lkure_random_no_cycles (lua_State * L)
{
	KureRel * self = _rel_get_unprotected (L,1);
	KureContext * context = kure_rel_get_context (self);
	double prob = luaL_checknumber (L,2);
	Kure_success success = kure_random_no_cycles_simple (self, prob);
	HANDLE_FAILURE(success, context);
	// Return the relation.
	lua_pushvalue(L,1);
	return 1;
}

// Rel x -> Rel x
static int lkure_random_perm (lua_State * L)
{
	KureRel * self = _rel_get_unprotected (L,1);
	KureContext * context = kure_rel_get_context (self);
	Kure_success success = kure_random_perm_simple (self);
	HANDLE_FAILURE(success, context);
	// Return the relation.
	lua_pushvalue(L,1);
	return 1;
}

// Context -> Rel (size 1x1)
//    OR
// Rel R -> Rel S (S has the same size as R)
static int lkure_new (lua_State * L)
{
	if (_is_context(L, 1)) {
		KureContext * context = _context_get(L, 1);
		KureRel ** pself = _kure_lua_rel_placeholder(L);

		*pself = kure_rel_new(context);

		return 1;
	}
	else if (_is_rel(L, 1)) {
		KureRel * proto = _rel_get_unprotected(L, 1);
		KureRel ** pself = _kure_lua_rel_placeholder(L);

		*pself = kure_rel_new_with_proto (proto);

		return 1;
	}
	else {
		return luaL_typerror(L, 1, "Rel or Context");
	}
}

static int lkure_new_n1 (lua_State * L)
{
	KureRel * proto = _rel_get_unprotected(L,1);
	KureRel ** pself = _kure_lua_rel_placeholder(L);
	mpz_t n;
	mpz_init (n);
	kure_rel_get_rows(proto, n);
	*pself = kure_rel_new_with_rows(kure_rel_get_context(proto), n);
	mpz_clear (n);
	return 1;
}

static int lkure_new_1n (lua_State * L)
{
	KureRel * proto = _rel_get_unprotected(L,1);
	KureRel ** pself = _kure_lua_rel_placeholder(L);
	mpz_t n;
	mpz_init (n);
	kure_rel_get_cols(proto, n);
	*pself = kure_rel_new_with_cols(kure_rel_get_context(proto), n);
	mpz_clear (n);
	return 1;
}

// Context, rows, cols -> Rel
static int lkure_new_si (lua_State * L)
{
	KureContext * context = _context_get (L, 1);
	int rows = luaL_checknumber (L, 2);
	int cols = luaL_checknumber (L, 3);

	KureRel ** pself = _kure_lua_rel_placeholder (L);
	*pself = kure_rel_new_with_size_si(context, rows, cols);

	return 1;
}

// Rel, row, col -> bool
static int lkure_get_bit_si(lua_State * L)
{
	KureRel * rel = _rel_get_unprotected (L, 1);
	int row = luaL_checknumber (L, 2);
	int col = luaL_checknumber (L, 3);
	Kure_success success;

	Kure_bool state = kure_get_bit_si (rel, row, col, &success);
	HANDLE_FAILURE(success, kure_rel_get_context (rel));
	// Does this work??
	lua_pushboolean (L, state);
	return 1;
}

// Rel, bool, row, col -> ()
static int lkure_set_bit_si(lua_State * L)
{
	KureRel * rel = _rel_get_unprotected (L, 1);
	int yesno = lua_toboolean(L, 2);
	int row = luaL_checknumber (L, 3);
	int col = luaL_checknumber (L, 4);
	Kure_success success = kure_set_bit_si (rel, yesno, row, col);
	HANDLE_FAILURE(success, kure_rel_get_context (rel));
	return 0;
}

static int _rel_gc (lua_State * L)
{
	KureRel * self = _rel_get_unprotected (L,1);

	kure_rel_destroy (self);
	return 0;
}

static int _context_gc (lua_State * L)
{
	KureContext * self = _context_get(L,1);
	kure_context_deref(self);
	return 0;
}

// The pointer is on the top of the stack afterwards.
static KureContext ** _context_placeholder (lua_State * L)
{
	KureContext ** pself = (KureContext**) lua_newuserdata (L, sizeof(KureContext*));

	/* Set the metatable. This is necessary to tell lua what GC function should
	 * be called. */
	luaL_getmetatable(L, _kure_context_key);
	lua_setmetatable(L, -2);

	return pself;
}

static int lkure_context_new (lua_State * L)
{
	KureContext ** pself = _context_placeholder(L);
	*pself = kure_context_new ();
	return 1;
}

/* Returns the dump instead of writing it to stdout. */
static int lkure_dump (lua_State * L)
{
	KureRel * rel = _rel_get_unprotected (L,1);

	if ( !kure_rel_fits_si(rel)) {
		return _lkure_error (L, "Relation is too big.");
	}
	else {
		int n = kure_rel_get_rows_si(rel), m = kure_rel_get_cols_si(rel);
		int i,j;
		char * s = NULL, *header = NULL, *p;
		int header_len;

		gmp_asprintf (&header, "Relation of size %d x %d (ROWS x COLS) "
				"with %d entries:\n", n,m,kure_get_entries_si(rel,NULL));
		header_len = strlen (header);

		s = (char*) malloc (n/*\n*/ + n*m + header_len +1/*\0*/);
		strcpy (s, header);
		p = s + header_len;

		for (i = 0 ; i < n ; i ++) {
			for (j = 0 ; j < m ; j ++) {
				Kure_bool bit = kure_get_bit_si(rel,i,j,NULL);
				*p++ = (bit?'X':'.');
			}
			*p++ = '\n';
		}
		*p = '\0';

		lua_pushstring (L, s);
		free (s);

		return 1;
	}
}

/* Returns a string instead of writing it to stdout. */
static int lkure_info (lua_State * L)
{
	KureRel * rel = _rel_get_unprotected (L,1);
	KureContext * context = kure_rel_get_context(rel);
	DdManager * manager = kure_context_get_manager(context);
	DdNode * bdd = kure_rel_get_bdd(rel);
	int num_vars_rows = kure_rel_get_vars_rows(rel),
		num_vars_cols = kure_rel_get_vars_cols(rel);
	int dagSize = Cudd_DagSize(bdd);
	mpz_t ents;
	double paths = Cudd_CountPath(bdd);
	double density = Cudd_Density(manager, bdd, num_vars_rows + num_vars_cols);
	mpz_t rows, cols;
	int numLeaves = Cudd_CountLeaves(bdd);
	char * s = NULL;

	mpz_init (ents);
	kure_get_entries(rel, ents);

	mpz_init (rows); mpz_init (cols);
	kure_rel_get_rows (rel, rows);
	kure_rel_get_cols (rel, cols);

	gmp_asprintf (&s, "Relation:\n"
				"   rows x cols : %Zd x %Zd\n"
				"       entries : %Zd\n"
				"          vars : %d, %d\n"
				"      DAG size : %d\n"
				"    path count : ~%.0lf\n"
			    "       density : %.3lf\n"
				"   leave count : %d\n",
				rows, cols, ents, num_vars_rows, num_vars_cols, dagSize,
				paths, density, numLeaves);

	mpz_clear (ents);
	mpz_clear (rows); mpz_clear (cols);

	lua_pushstring (L, s);
	free (s);

	return 1;
}

// Rel -> int
LUA_PROTO(lkure_get_entries_si, L)
{
	KureRel * rel = _rel_get_unprotected(L,1);
	Kure_success success;
	int ents;

	ents = kure_get_entries_si(rel, &success);
	if (! success) {
		KureContext * context = kure_rel_get_context (rel);
		KureError * err = kure_context_get_error(context);
		if (err && err->message)
			_lkure_error(L, err->message);
		else
			_lkure_error(L, _msg_unknown_error);
		return 0;
	} else {
		lua_pushinteger(L, ents);
		return 1;
	}
}

// Vec -> int
LUA_PROTO(lkure_vec_get_entries_si, L)
{
	KureRel * rel = _rel_get_unprotected(L,1);
	Kure_success success;
	int ents;

	ents = kure_vec_get_entries_si(rel, &success);
	if (! success) {
		KureContext * context = kure_rel_get_context (rel);
		KureError * err = kure_context_get_error(context);
		if (err && err->message)
			_lkure_error(L, err->message);
		else
			_lkure_error(L, _msg_unknown_error);
		return 0;
	} else {
		lua_pushinteger(L, ents);
		return 1;
	}
}

// Rel R -> same Rel R
LUA_PROTO(lkure_vec_begin, L)
{
	KureRel * self = _rel_get_unprotected(L, 1);
	Kure_success success = kure_vec_begin(self);
	if (! success) {
		KureContext * context = kure_rel_get_context (self);
		KureError * err = kure_context_get_error(context);
		if (err && err->message)
			_lkure_error(L, err->message);
		else
			_lkure_error(L, _msg_unknown_error);
		return 0;
	} else {
		lua_pushvalue(L, 1); /* push Argument on top */
		return 1;
	}
}

// Rel R -> Rel S
LUA_PROTO(lkure_vec_next, L)
{
	KureRel * arg = _rel_get_unprotected(L, 1);
	KureContext * context = kure_rel_get_context(arg);
	KureRel * rop = _kure_lua_rel_new(L, context);
	Kure_success success = kure_vec_next (rop, arg);

	if (! success) {
		KureError * err = kure_context_get_error(context);
		if (err && err->message)
			return _lkure_error(L, err->message);
		else return _lkure_error(L, _msg_unknown_error);
	} else return 1;
}

// Rel, bool, Int -> ()
LUA_PROTO(lkure_set_row_si, L)
{
	KureRel * self = _rel_get_unprotected(L, 1);
	Kure_bool yesno = lua_toboolean(L, 2);
	int row = luaL_checkinteger(L, 3);
	Kure_success success = kure_set_row_si (self, yesno, row);

	if (! success) {
		KureContext * context = kure_rel_get_context (self);
		KureError * err = kure_context_get_error(context);
		if (err && err->message)
			return _lkure_error(L, err->message);
		else return _lkure_error(L, _msg_unknown_error);
	} else return 0;
}

// Vec v -> Rel R
LUA_PROTO(lkure_successors, L)
{
	KureRel * vec = _rel_get_unprotected (L, 1);
	KureContext * context = kure_rel_get_context(vec);
	mpz_t rows;
	Kure_success success;
	KureRel * rop;
	mpz_init (rows);
	kure_rel_get_rows(vec, rows);
	rop = _kure_lua_rel_new(L, context);
	success = kure_successors(rop, rows);
	mpz_clear (rows);
	HANDLE_FAILURE(success, context);
	return 1;
}

// Rel, int -> ()
LUA_PROTO(lkure_successors_si, L)
{
	KureRel * self = _rel_get_unprotected (L, 1);
	int size = luaL_checkinteger(L, 2);
	Kure_success success = kure_successors_si(self, size);
	if (! success) {
		KureContext * context = kure_rel_get_context (self);
		KureError * err = kure_context_get_error(context);
		if (err && err->message)
			return _lkure_error(L, err->message);
		else return _lkure_error(L, _msg_unknown_error);
	} else return 0;
}

// Rel R, int, int, String [char('1'), [char('0')]] -> Same Rel R
LUA_PROTO(lkure_assign_from_string, L)
{
	KureRel * self = _rel_get_unprotected (L, 1);
	int rows = luaL_checkinteger(L, 2);
	int cols = luaL_checkinteger(L, 3);
	const char * def = luaL_checkstring (L, 4);
	const char * one_arg = lua_tostring (L, 5);
	const char * zero_arg = lua_tostring (L, 6);
	char one_ch = one_arg ? *one_arg : '1';
	char zero_ch = zero_arg ? *zero_arg : '0';

	Kure_success success
		= kure_rel_assign_from_string(self, def, rows, cols, one_ch, zero_ch);
	HANDLE_FAILURE(success, kure_rel_get_context (self));
	lua_pushvalue(L, 1);
	return 1;
}

// Rel, [char('1'), [char('0')]] -> string
LUA_PROTO(lkure_to_string, L)
{
	KureRel * self = _rel_get_unprotected (L, 1);
	const char * one_arg = lua_tostring (L, 2);
	const char * zero_arg = lua_tostring (L, 3);
	char one_ch = one_arg ? *one_arg : '1';
	char zero_ch = zero_arg ? *zero_arg : '0';

	char * s = kure_rel_to_string(self, one_ch, zero_ch);
	HANDLE_FAILURE(s != NULL, kure_rel_get_context(self));
	lua_pushstring(L, s);
	free (s);
	return 1;
}



// The pointer is on the top of the stack afterwards.
static KureDom * _direct_product_new (lua_State * L, mpz_t first, mpz_t second)
{
	KureDom ** pself = _kure_lua_dom_placeholder (L);
	*pself = kure_direct_product_new (first, second);
	return *pself;
}

// The pointer is on the top of the stack afterwards.
static KureDom * _direct_sum_new (lua_State * L, mpz_t first, mpz_t second)
{
	KureDom ** pself = _kure_lua_dom_placeholder (L);
	*pself = kure_direct_sum_new (first, second);
	return *pself;
}

// Rel R, Rel S -> Dom. R,S have to be homogeneous; can belong to different
// contexts.
LUA_PROTO(lkure_direct_product_new, L)
{
	KureRel * arg1 = _rel_get_unprotected (L, 1);
	KureRel * arg2 = _rel_get_unprotected (L, 2);

	if ( !kure_is_hom (arg1, NULL) || !kure_is_hom(arg2, NULL)) {
		return _lkure_error (L, "Arguments have to be homogeneous.");
	}
	else {
		mpz_t first, second;
		mpz_init (first);		mpz_init (second);
		kure_rel_get_cols (arg1, first);
		kure_rel_get_cols (arg2, second);

		_direct_product_new (L, first, second);

		mpz_clear (first);		mpz_clear (second);
		return 1;
	}
}

// Rel R, Rel S -> Dom. R,S have to be homogeneous; don't have to be in the
// same context.
LUA_PROTO(lkure_direct_sum_new, L)
{
	KureRel * arg1 = _rel_get_unprotected (L, 1);
	KureRel * arg2 = _rel_get_unprotected (L, 2);

	if ( !kure_is_hom (arg1, NULL) || !kure_is_hom(arg2, NULL)) {
			return _lkure_error (L, "Arguments have to be homogeneous.");
		}
		else {
			mpz_t first, second;
			mpz_init (first);		mpz_init (second);
			kure_rel_get_cols (arg1, first);
			kure_rel_get_cols (arg2, second);

			_direct_sum_new (L, first, second);

			mpz_clear (first);		mpz_clear (second);
			return 1;
		}
}

//kure.product_domain_new (Rel,Rel) -> Dom
//kure.sum_domain_new (Rel,Rel) -> Dom

// NOTE: We would need a hash table to implement this.
// Rel R -> Context
//LUA_PROTO(lkure_rel_get_context, L)
//{
//	KureRel * self = _rel_get_unprotected(L,1);
//	lua_pushlightuserdata (L, kure_rel_get_context(self)); // Doesn't work!
//	return 1;
//}

// TRUE/FALSE (1x1) -> Lua bool
LUA_PROTO(lkure_is_true, L)
{
	KureRel * rel = _rel_get_unprotected(L,1);
	if ( !kure_rel_fits_si(rel) || kure_rel_get_rows_si(rel) != 1
			|| kure_rel_get_cols_si(rel) != 1) {
		return _lkure_error(L, "Expected relation with dimension 1x1.");
	}
	else {
		lua_pushboolean(L, !kure_is_empty(rel,NULL));
		return 1;
	}
}

// Rel R, file -> ()
LUA_PROTO(lkure_write, L)
{
	KureRel * rel = _rel_get_unprotected(L, 1);
	const char * filename = luaL_checkstring(L, 2);

	Kure_success success = kure_rel_write_to_dddmp_file(rel, filename);

	HANDLE_FAILURE(success, kure_rel_get_context(rel));
	return 0;
}

// Rel R, filename -> same Rel R
LUA_PROTO(lkure_read, L)
{
	KureRel * rel = _rel_get_unprotected(L, 1);
	const char * filename = luaL_checkstring(L, 2);

	Kure_success success = kure_rel_read_from_dddmp_file(rel, filename,
			rel->rows, rel->cols);

	HANDLE_FAILURE(success, kure_rel_get_context(rel));
	lua_pushvalue(L, 1);
	return 1;
}

// Context -> ()
LUA_PROTO(lkure_garbage_collect, L)
{
	KureContext * context = _context_get(L,1);
	cuddGarbageCollect(kure_context_get_manager(context), 1);
	return 0;
}

// Context -> ()
LUA_PROTO(lkure_debug_info, L)
{
	KureContext * context = _context_get(L,1);
	Cudd_PrintInfo(kure_context_get_manager(context), stdout);
	return 0;
}

// string -> Rel R
LUA_PROTO(lkure_eval, L)
{
	const char * expr = lua_tostring (L, 1);
	KureError * err = NULL;
	KureRel * res = kure_lang_exec (L, expr, &err);
	if ( !res) {
		if (err && err->message) return _lkure_error (L, err->message);
		else return _lkure_error (L, _msg_unknown_error);
	}
	else {
		KureRel ** pself = _kure_lua_rel_placeholder (L);
		*pself = res;
		return 1;
	}
}


/* From src/ldblib.c in the Lua 5.1.4 source code. Used in \ref lkure_traceback. */
static lua_State *getthread (lua_State *L, int *arg) {
  if (lua_isthread(L, 1)) {
    *arg = 1;
    return lua_tothread(L, 1);
  }
  else {
    *arg = 0;
    return L;
  }
}

/* From src/ldblib.c in the Lua 5.1.4 source code. Used in \ref lkure_traceback. */
#define LEVELS1	12	/* size of the first part of the stack */
#define LEVELS2	10	/* size of the second part of the stack */

/* Used bu lkure_traceback to find the line number of the original domain-
 * specific code if there is one. Returns <0 if not. */
static int _get_dsl_line_number (lua_State * L, lua_Debug * ar)
{
	const char * var_name = NULL;
	int i = 1; /* first */
	while ((var_name = lua_getlocal(L,ar,i))) {
		if (0 == strcmp (var_name, "__line")) {
			int line = lua_tointeger(L, -1);
			lua_pop (L, 1);
			return line;
		}
		lua_pop (L, 1);
		i++;
	}
	return -1;
}

/*!
 * From src/ldblib.c in the Lua 5.1.4 source code. Implements debug.traceback
 * there.
 */
LUA_PROTO(lkure_traceback, L)
{
	int level;
	int firstpart = 1;  /* still before eventual `...' */
	int arg;
	lua_State *L1 = getthread(L, &arg);
	lua_Debug ar;
	if (lua_isnumber(L, arg+2)) {
		level = (int)lua_tointeger(L, arg+2);
		lua_pop(L, 1);
	}
	else
		level = (L == L1) ? 1 : 0;  /* level 0 may be this own function */
	if (lua_gettop(L) == arg)
		lua_pushliteral(L, "");
	else if (!lua_isstring(L, arg+1)) return 1;  /* message is not a string */
	else lua_pushliteral(L, "\n");
	lua_pushliteral(L, "stack traceback:");
	while (lua_getstack(L1, level++, &ar)) {
		if (level > LEVELS1 && firstpart) {
			/* no more than `LEVELS2' more levels? */
			if (!lua_getstack(L1, level+LEVELS2, &ar))
				level--;  /* keep going */
			else {
				lua_pushliteral(L, "\n\t...");  /* too many levels */
				while (lua_getstack(L1, level+LEVELS2, &ar))  /* find last levels */
					level++;
			}
			firstpart = 0;
			continue;
		}
		lua_pushliteral(L, "\n\t");
		lua_getinfo(L1, "Snl", &ar);
		lua_pushfstring(L, "%s:", ar.short_src);

		{
			/* Check if local variable __line is the first local variable. If so,
			 * use that value instead of ar.currentline. __line is used to map
			 * line numbers back to domain-specific code. */
			int line = _get_dsl_line_number(L,&ar);
			if (line >= 0) {
				lua_pushfstring(L, "%d (DSL):", line);
			}
			else if (ar.currentline > 0)
				lua_pushfstring(L, "%d:", ar.currentline);
		}

		if (*ar.namewhat != '\0')  /* is there a name? */
			lua_pushfstring(L, " in function " LUA_QS, ar.name);
		else {
			if (*ar.what == 'm')  /* main? */
				lua_pushfstring(L, " in main chunk");
			else if (*ar.what == 'C' || *ar.what == 't')
				lua_pushliteral(L, " ?");  /* C function or tail call */
			else
				lua_pushfstring(L, " in function <%s:%d>",
						ar.short_src, ar.linedefined);
		}
		lua_concat(L, lua_gettop(L) - arg);
	}
	lua_concat(L, lua_gettop(L) - arg);
	return 1;
}

static const struct luaL_reg kurelib[] = {
		{ "context_new", lkure_context_new },

		{ "new", lkure_new },
		{ "new_n1", lkure_new_n1 },
		{ "new_1n", lkure_new_1n },
		{ "new_si", lkure_new_si },

		{ "assign_from_string", lkure_assign_from_string },

		{ "to_string", lkure_to_string },

		{ "get_bit_si", lkure_get_bit_si },
		{ "set_bit_si", lkure_set_bit_si },

		{ "O", lkure_null },
		{ "null", lkure_null },
		{ "L", lkure_all },
		{ "all", lkure_all },
		{ "identity", lkure_identity },
		{ "I", lkure_identity },

		/* "and" and "or" are reserved keywords in Lua and cannot even be used
		 * with package names. */
		{ "land", lkure_and },
		{ "lor", lkure_or },
		{ "mult", lkure_mult },
		{ "mult_transp_norm", lkure_mult_transp_norm },
		{ "mult_norm_transp", lkure_mult_norm_transp },
		{ "mult_transp_transp", lkure_mult_transp_transp },

		{ "transpose", lkure_transpose },
		{ "t", lkure_transpose },
		{ "complement", lkure_complement },
		{ "c", lkure_complement },

		{ "set_row_si", lkure_set_row_si },

		{ "less_card", lkure_less_card }, // Aka cardrel
		{ "subsetvec_rel", lkure_subsetvec_rel }, // Aka cardfilter

		{ "get_entries_si", lkure_get_entries_si },
		{ "vec_get_entries_si", lkure_vec_get_entries_si },

		{ "trans_hull", lkure_trans_hull },
		{ "symm_hull", lkure_symm_hull },
		{ "refl_hull", lkure_refl_hull },

		{ "membership", lkure_membership },
		{ "epsi", lkure_membership },
		{ "partial_funcs_si", lkure_partial_funcs_si },
		{ "total_funcs_si", lkure_total_funcs_si },

		{ "random", lkure_random },
		{ "random_no_cycles", lkure_random_no_cycles },
		{ "random_perm", lkure_random_perm },

		{ "left_residue", lkure_left_residue },
		{ "right_residue", lkure_right_residue },
		{ "symm_quotient", lkure_symm_quotient },
		{ "syq", lkure_symm_quotient },

		{ "is_empty", lkure_is_empty },
		{ "is_univalent", lkure_is_univalent },
		{ "is_vector", lkure_is_vector },

		{ "equals", lkure_equals },
		{ "includes", lkure_includes },

		{ "vec_begin", lkure_vec_begin },
		{ "vec_begin_full_si", lkure_vec_begin_full_si },
		{ "vec_next", lkure_vec_next },

		{ "successors", lkure_successors },
		{ "successors_si", lkure_successors_si },
		{ "domain", lkure_domain },

		{ "vec_point", lkure_vec_point },
		{ "atom", lkure_atom },
		{ "vec_inj", lkure_vec_inj },

		{ "tupling", lkure_tupling },
		{ "left_tupling", lkure_left_tupling },
		{ "right_tupling", lkure_right_tupling },
		{ "direct_sum", lkure_direct_sum },

		{ "proj_1", lkure_proj_1 },
		{ "proj_2", lkure_proj_2 },
		{ "inj_1", lkure_inj_1 },
		{ "inj_2", lkure_inj_2 },

		{ "proj_1_si", lkure_proj_1_si },
		{ "proj_2_si", lkure_proj_2_si },
		{ "inj_1_si", lkure_inj_1_si },
		{ "inj_2_si", lkure_inj_2_si },

		{ "product_order", lkure_product_order },
		{ "sum_order", lkure_sum_order },

		{ "rel_fits_si", lkure_rel_fits_si },
		{ "rel_get_rows_si", lkure_rel_get_rows_si },
		{ "rel_get_cols_si", lkure_rel_get_cols_si },
		{ "rel_same_dim", lkure_rel_same_dim },
		{ "is_true", lkure_is_true },

		{ "read", lkure_read },
		{ "write", lkure_write },

		{ "dump", lkure_dump },
		{ "info", lkure_info },

		/* Domains */
		{ "direct_product_new", lkure_direct_product_new },
		{ "direct_sum_new", lkure_direct_sum_new },

		{ "is_cardlt", lkure_is_cardlt },
		{ "is_cardleq", lkure_is_cardleq },
		{ "is_cardeq", lkure_is_cardeq },

		{ "dom_comp1", lkure_dom_comp1 },
		{ "dom_comp2", lkure_dom_comp2 },

		{ "garbage_collect", lkure_garbage_collect },
		{ "debug_info", lkure_debug_info },
		{ "traceback", lkure_traceback },

		{ "minsets_upset", lkure_minsets_upset },
		{ "minsets", lkure_minsets },
		{ "maxsets_downset", lkure_maxsets_downset },
		{ "maxsets", lkure_maxsets },

		{ "eval", lkure_eval },

		{ NULL, NULL } };



void luaopen_kure (lua_State * L)
{
	luaL_newmetatable(L, _kure_rel_key);

	/* set its __gc field */
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, _rel_gc);
	lua_settable(L, -3);

	luaL_newmetatable(L, _kure_context_key);
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, _context_gc);
	lua_settable(L, -3);

	luaL_newmetatable(L, _kure_dom_key);
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, _dom_gc);
	lua_settable(L, -3);

	luaL_register(L, "kure", kurelib);
}


void kure_lua_set_global_context (lua_State * L, KureContext * c)
{
	KureContext ** pc = _context_placeholder(L);
	*pc = c;
	lua_setglobal(L, "__c");
}



/* Originally from src/lauxlib.c from the Lua 5.1.4 distribution. Used to
 * create a string which contains the current location. See \ref luaL_where.
 * Tailored to use DSL line numbers. See \ref _get_dsl_line_number and
 * \ref lkure_traceback. */
void _lkure_where (lua_State * L, int level)
{
	lua_Debug ar;
	if (lua_getstack(L, level, &ar)) {  /* check function at level */
		int line = _get_dsl_line_number (L, &ar);

		lua_getinfo(L, "Sl", &ar);  /* get info about it */
		if (line > 0) {
			lua_pushfstring(L, "%s:%d (DSL): ", ar.short_src, line);
			return;
		}
		else if (ar.currentline > 0) {
			lua_pushfstring(L, "%s:%d: ", ar.short_src, ar.currentline);
			return;
		}
	}
	lua_pushliteral(L, "");  /* else, no information available... */
}


/* Originally from src/lauxlib.c from the Lua 5.1.4 distribution. Used to
 * create an error without much effor. See \ref luaL_error for details.
 * Uses DSL line numbers. See \ref _lkure_where.
 */
int _lkure_error (lua_State * L, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	_lkure_where(L, 1);
	lua_pushvfstring(L, fmt, argp);
	va_end(argp);
	lua_concat(L, 2);
	return lua_error(L);
}
