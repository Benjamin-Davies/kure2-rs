/*
 * dsl_to_lua.c
 *
 *  Copyright (C) 2011 Stefan Bolus, University of Kiel, Germany
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

/*!
 * Parses files in the domain-specific language (DSL; Relview language) and
 * outputs the corresponding LUA code generated.
 *
 * \author stb
 * \date 16/02/2011
 */

#include "Kure.h"
#include <stdlib.h>
#include <glib.h>

static Kure_bool _on_function (void*, const char*, const char*);
static Kure_bool _on_program  (void*, const char*, const char*);

int main (int argc, char ** argv)
{
	if (argc == 1) {
		printf ("usage: %s file...\n", argv[0]);
		return EXIT_FAILURE;
	}
	else {
		KureContext * c = kure_context_new (); // unreferenced
		lua_State * l = kure_lua_new(c);
		KureParserObserver o;
		int i;

		/* Called by the parser when a DSL "function" was parsed. */
		o.onFunction = _on_function;

		/* Similar to onProgram. Called by the parser when a DSL "program"
		 * is parsed. */
		o.onProgram  = _on_program;

		/* Passed to the callbacks as first argument. */
		o.object     = NULL;

		for (i = 1 ; i < argc ; ++i) {
			KureError * err = NULL;
			Kure_success success;

			printf ("Parsing file \"%s\" ...", argv[1]);
			o.object = argv[i];
			success = kure_lang_parse_file (argv[i], &o, &err);
			if ( !success) {
				printf ("Parsing file \"%s\" failed! Reason: %s\n",
						argv[i], err?err->message:"Unknown");
				kure_error_destroy (err);
			}
		}

		/* The Lua state holds the single reference to the KureContext so it
		 * the context will be destroyed by the this call. */
		kure_lua_destroy (l);

		return EXIT_SUCCESS;
	}
}


static Kure_bool _dump_parsed_object (const char * type_name,
		const char * filename, const char * code, const gchar * lua_code)
{
	printf ("Found %s in file %s\n"
			"--- DSL CODE BEGINS ---\n"
			"%s\n"
			"--- DSL CODE ENDS ---\n"
			"--- LUA CODE BEGINS ---\n"
			"%s\n"
			"--- LUA_CODE ENDS ---\n", type_name, filename, code, lua_code);
	return TRUE; /* continue parsing */
}


Kure_bool _on_function (void * user_data, const char * code, const char * lua_code)
{
	return _dump_parsed_object ("function",  (const char*)user_data, code, lua_code);
}

Kure_bool _on_program  (void * user_data, const char * code, const char * lua_code)
{
	return _dump_parsed_object ("program",  (const char*)user_data, code, lua_code);
}
