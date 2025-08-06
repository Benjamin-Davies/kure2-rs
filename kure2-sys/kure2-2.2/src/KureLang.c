/*
 * KureLang.c
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

#include "KureImpl.h"
#include <assert.h>
#include <string.h>
#include <glib.h>
#include <lualib.h> // luaL_openlibs
#include <lauxlib.h> // luaL_dostring (Macro)


Kure_success kure_lua_set_rel_copy (lua_State * L, const char * name, KureRel * rel)
{
	KureRel * copy = kure_rel_new_copy (rel);
	KureRel ** pp = _kure_lua_rel_placeholder (L);
	*pp = copy;
	if (strcmp(name, "$") == 0)
		lua_setglobal(L, KURE_DOLLAR_SUBST);
	else
		lua_setglobal(L, name);
	return TRUE;
}

#if 0
KureRel * kure_lua_get_rel_copy (lua_State * L, const char * name)
{
	if (strcmp(name, "$") == 0) lua_getglobal(L, KURE_DOLLAR_SUBST);
	else lua_getglobal(L, name);

	if (lua_isnoneornil(L,-1)) return NULL;
	return kure_rel_new_copy (_kure_lua_rel_get(L, -1));
}
#endif

Kure_success kure_lua_set_dom_copy (lua_State * L, const char * name, KureDom * dom)
{
	KureDom * copy = kure_dom_new_copy (dom);
	KureDom ** pp = _kure_lua_dom_placeholder (L);
	*pp = copy;
	lua_setglobal(L, name);
	return TRUE;
}

extern size_t g_kure_compat_size;
extern char g_kure_compat[]; /* from src/lang/kure_compat.c */

lua_State *	kure_lua_new (KureContext * context)
{
	lua_State * L = lua_open();
	int error;

	luaL_openlibs (L);
	luaopen_kure (L);

	kure_context_ref(context);
	kure_lua_set_global_context (L, context); // TODO: Where from?

	error = luaL_loadbuffer (L, g_kure_compat, g_kure_compat_size,
			"kure.compat code");
	if (error) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
		fprintf(stderr, "This is a permanent error. Aborting ...\n");
		assert (error == 0);
		return NULL; // avoid warning.
	}
	else {
		lua_pcall(L, 0, 0, 0);
		return L;
	}
}

void kure_lua_destroy (lua_State * L)
{
	lua_close (L);
}


// Note: You can use "return <expr>" to get the result of an expr.
KureRel * kure_lua_exec (lua_State * L, const char * chunk, KureError ** perr)
{
	int error;

	/* Append a stack trace to every error message. See also the last argument
	 * to lua_pcall below. */
	lua_getglobal(L, "kure");
	lua_getfield(L, -1, "traceback");
	lua_remove(L, -2);

#ifdef KURE_VERBOSE
	printf ("\t--- Lua-Code begin ---\n%s\n\t--- Lua-Code end ---\n", chunk);
#endif

	error = luaL_loadstring(L, chunk) || lua_pcall(L, 0, 1, -2);
	if (error) {
		kure_set_error (perr, 0, "Execution of Lua chunk \"%s\" failed. Reason: %s",
				chunk, lua_tostring (L,-1));
		lua_pop(L,1);
		return NULL;
	}
	else {
		const KureRel *result = kure_lua_torel(L, -1, perr);
		if (result) {
			/* The result relation belongs to Lua and will be garbage collected.
			 * Hence, we have to create a copy of it. */
			return kure_rel_new_copy(result);
		} else return NULL;
	}
}

#define YY_TYPEDEF_YY_SCANNER_T // this is ugly, but seems to work. TODO
typedef void * yyscan_t;

#include "parser.h" // YYLTYPE, parser callback"
#include "lexer.h"


KureRel * kure_lang_exec (lua_State * L, const char * expr, KureError ** perr)
{
	char * luacode;
	KureRel * ret = NULL;

#ifdef KURE_VERBOSE
	printf ("\t--- RelView-Code begin ---\n%s\n\t--- RelView-Code end ---\n", expr);
#endif

	luacode = kure_lang_expr_to_lua(expr, perr);
	if (!luacode) {
		return NULL;
	}
	else {
		int error;
		gchar * real_luacode = g_strconcat("return ", luacode, NULL);

		/* Append a stack trace to every error message. See also the last argument
		 * to lua_pcall below. */
		lua_getglobal(L, "kure");
		lua_getfield(L, -1, "traceback");
		lua_remove(L, -2);

#ifdef KURE_VERBOSE
		printf ("\t--- Lua-Code begin ---\n%s\n\t--- Lua-Code end ---\n", real_luacode);
#endif

		luaL_loadstring(L, real_luacode);
		error = lua_pcall (L, 0, 1, -2);
		if (error) {
			kure_set_error (perr, KURE_ERR_INV_ARG, "Unable to evaluate "
					"expression \"%s\".\nReason: %s\nLua-Code was:\n\t--- "
					"Lua-Code begin ---\n%s\n\t--- Lua-Code end ---\n",
					expr, lua_tostring(L,-1), real_luacode);
		}
		else {
			const KureRel *result = kure_lua_torel(L, -1, perr);
			if (result) {
				/* The relation belongs to Lua and will be garbage collected.
				 * Hence, we have to copy it. */
				ret = kure_rel_new_copy (result);
			}
		}

		free (luacode);
		g_free (real_luacode);
	}

	return ret;
}

Kure_success kure_lang_assign (lua_State * L, const char * var,
							   const char * expr, KureError ** perr)
{
	KureRel * rel = kure_lang_exec (L, expr, perr);
	if ( !rel)
		return FALSE;
	else {
		kure_lua_set_rel_copy (L, var, rel);
		kure_rel_destroy(rel);
		return TRUE;
	}
}

typedef struct
{
	GString * gencode;
	const char * transl_unit;
	KureParserObserver * observer;
} ToLuaInfo;

char * _subset_loc (const char * s, const YYLTYPE * ploc)
{
	int l = 1, c = 1;
	const char * p = s;
	enum { IN, OUT } state = OUT;
	GString * part = g_string_new ("");

	for ( ; *p != '\0' ; ++p) {
		if (state == OUT) {
			if (ploc->first_line == l && ploc->first_column == c) {
				state = IN;
				g_string_append_c (part, *p);
			}
		}
		else if (state == IN) {
			if (l > ploc->last_line
					|| (l >= ploc->last_line && c > ploc->last_column)) {
				break;
			}
			else g_string_append_c (part, *p);
		}

		switch (*p) {
			case '\n': c = 1; ++l; break;
			default: ++c;
		}
	}

	return g_string_free (part, FALSE);
}

/*!
 * Implements the onProgram operation of a \ref ParserObserver.
 */
static void _on_program (void * object, const char * gencode, const YYLTYPE * ploc)
{
	ToLuaInfo * info = (ToLuaInfo*) object;
	char * origcode = _subset_loc (info->transl_unit, ploc);

#if 0
	printf ("on_function: gencode=\"%s\", l%d.c%d-l%d.c%d (\"%s\"): ", gencode, ploc->first_line,
		ploc->first_column, ploc->last_line, ploc->last_column, origcode);
#endif

	if (info->gencode) g_string_append(info->gencode, gencode);
	if (info->observer) {
		KureParserObserver * o = info->observer;
		if (o->onProgram) o->onProgram (o->object, origcode, gencode);
	}

	free (origcode);
}

/*!
 * Implements the onFunction operation of a \ref ParserObserver.
 */
static void _on_function (void * object, const char * gencode, const YYLTYPE * ploc)
{
	ToLuaInfo * info = (ToLuaInfo*) object;
	char * origcode = _subset_loc (info->transl_unit, ploc);

#if 0
	printf ("on_function: gencode=\"%s\", l%d.c%d-l%d.c%d (\"%s\"): ", gencode, ploc->first_line,
		ploc->first_column, ploc->last_line, ploc->last_column, origcode);
#endif

	if (info->gencode) g_string_append(info->gencode, gencode);
	if (info->observer) {
		KureParserObserver * o = info->observer;
		if (o->onFunction) o->onFunction (o->object, origcode, gencode);
	}

	free (origcode);
}

/* See the Bison parser file how to enable debugging. */
//extern int kure_yydebug;

/*!
 * pgencode and o may be NULL. If both are NULL, only the syntax will be
 * checked.
 */
static Kure_success _kure_lang_to_lua_wrap (const char * code, char ** pgencode,
		KureParserObserver * o, int start_symbol, KureError ** perr)
{
	int error;
	yyscan_t scanner;
	YY_BUFFER_STATE buf;
	ToLuaInfo info = {0};
	ParserObserver po;
	GString * luacode = g_string_new ("");
	LexerInfo lexinfo;
	GString * err_str = g_string_new ("");
	GQueue stack;

	lexinfo.column = 1; /*Starting column. Must be 1!*/
	lexinfo.start_symbol = start_symbol;
	lexinfo.err = err_str;

	po.onProgram = _on_program;
	po.onFunction = _on_function;
	po.object = &info;

	if (pgencode)
		info.gencode = g_string_new ("");
	info.transl_unit = code;
	info.observer = o;

	kure_yylex_init_extra(&lexinfo, &scanner );
	buf = kure_yy_scan_string( code, scanner);
	kure_yyset_lineno(1 /*Starting line. Would be 0 otherwise.*/, scanner);
	error = kure_yyparse (scanner, luacode, &po, err_str, &stack);
	kure_yy_delete_buffer(buf,scanner);
	kure_yylex_destroy( scanner );

	if (error) {
		kure_set_error(perr, KURE_ERR_INV_ARG, "Unable to parse code. "
				"The parser returned %d and says:\n%s", error,
				err_str->str);
		g_string_free (luacode, TRUE);
		g_string_free (err_str, TRUE);
		if (info.gencode)
			g_string_free(info.gencode, TRUE);
		return FALSE;
	}
	else {
		g_string_free (luacode, TRUE);
		g_string_free (err_str, TRUE);
		if (pgencode)
			*pgencode = g_string_free(info.gencode, FALSE);
		return TRUE;
	}
}

char * kure_lang_to_lua (const char * transl_unit, KureError ** perr)
{
	char * gencode = NULL;
	Kure_success success = _kure_lang_to_lua_wrap (transl_unit, &gencode, NULL,
			START_DEFAULT, perr);
	if (success) return gencode; else return NULL;
}

char * kure_lang_expr_to_lua (const char * expr, KureError ** perr)
{
	/* We can't use kure_lang_to_lua because it depends on the observer which
	 * can't handle expressions. */

	int error;
	yyscan_t scanner;
	YY_BUFFER_STATE buf;
	GString * luacode = g_string_new ("");
	LexerInfo lexinfo;
	GString * err_str = g_string_new ("");
	GQueue stack;

	//kure_yydebug = 1;

	lexinfo.column = 1; /*Starting column. Must be 1!*/
	lexinfo.start_symbol = START_EXPR;
	lexinfo.err = err_str;

	kure_yylex_init_extra(&lexinfo, &scanner );
	buf = kure_yy_scan_string(expr, scanner);
	kure_yyset_lineno(1 /*Starting line. Would be 0 otherwise.*/, scanner);
	error = kure_yyparse (scanner, luacode, NULL, err_str, &stack);
	kure_yy_delete_buffer(buf,scanner);
	kure_yylex_destroy( scanner );

	if (error) {
		kure_set_error(perr, KURE_ERR_INV_ARG, "Unable to parse code. "
				"The parser returned %d and says:\n%s", error,
				err_str->str);
		g_string_free (err_str, TRUE);
		g_string_free (luacode, TRUE);
		return NULL;
	}
	else {
		g_string_free (err_str, TRUE);
		return g_string_free (luacode, FALSE);
	}
}

Kure_success kure_lang_parse (const char * transl_unit,
		KureParserObserver * o, KureError ** perr)
{
	return _kure_lang_to_lua_wrap(transl_unit, NULL, o, START_DEFAULT, perr);
}


/*!
 * Implements lua_Reader. Used by \ref _kure_lang_load_full in conjunction with
 * \ref lua_load.
 */
static const char * _kure_lua_string_reader (lua_State * L, void * data, size_t /*out*/ * psize)
{
	static char buf [1024+1];
	const char ** pptr = (const char**) data;
	int i = 0 ;
	const char * s = *pptr;

	for ( i = 0 ; s[i] && i < 1024 ; ++i)
		buf[i] = s[i];
	buf[i] = '\0';
	*psize = i;
	*pptr = &s[i];

	return buf;
}

static Kure_success _kure_lang_load_full (lua_State * L, const char * transl_unit,
		KureError ** perr, const char * chunkname)
{
	char * luacode = kure_lang_to_lua(transl_unit, perr);
	if (!luacode)
		return FALSE;
	else {
		int error;
		char * ptr = luacode;

#ifdef KURE_VERBOSE
		printf("\t--- Lua-Code begin ---\n%s\n\t--- Lua-Code end ---\n",
				luacode);
#endif

		error = lua_load (L, _kure_lua_string_reader, &ptr, chunkname);
		error = error || lua_pcall(L, 0, LUA_MULTRET, 0);
		if (error) {
			kure_set_error(perr, KURE_ERR_INV_ARG, "Unable to parse "
				"generated Lua code. Reason: %s Code was:\n\t--- "
				"Lua-Code begin ---\n%s\n\t--- Lua-Code end ---\n",
					lua_tostring(L,-1), luacode);
		}

		free(luacode);
		return !error;
	}
}

Kure_success kure_lang_load (lua_State * L, const char * transl_unit, KureError ** perr)
{
	return _kure_lang_load_full (L, transl_unit, perr, transl_unit);
}

Kure_success kure_lang_load_file (lua_State * L, const char * file, KureError ** perr)
{
	GError * gerr = NULL;
	gchar * buf = NULL;
	Kure_bool success = g_file_get_contents (file, &buf, NULL, &gerr);
	if ( !success) {
		kure_set_error(perr, KURE_ERR_INV_ARG, gerr->message);
		g_error_free (gerr);
	}
	else {
		success = _kure_lang_load_full (L, buf, perr, file);
		g_free (buf);
	}

	return success;
}

Kure_success kure_lang_parse_file (const char * file,
		KureParserObserver * o, KureError ** perr)
{
	GError * gerr = NULL;
	gchar * buf = NULL;
	Kure_bool success = g_file_get_contents (file, &buf, NULL, &gerr);
	if ( !success) {
		kure_set_error(perr, KURE_ERR_INV_ARG, gerr->message);
		g_error_free (gerr);
	}
	else {
		success = _kure_lang_to_lua_wrap (buf, NULL, o, START_DEFAULT, perr);
		g_free (buf);
	}

	return success;
}

void kure_lang_set_assert_func (lua_State * L, int(*func)(lua_State*))
{
	/* Store the function in kure.assert_func. */
	lua_getfield(L, LUA_GLOBALSINDEX, "kure");
	lua_pushcfunction (L, func);
	lua_setfield(L, -2, "assert_func");
}
