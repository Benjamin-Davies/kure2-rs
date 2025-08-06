/*
 * shell.c
 *
 *  Copyright (C) 2010,2011,2012 Stefan Bolus, University of Kiel, Germany
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

#define _GNU_SOURCE // sscanf with %a...
#include "Kure.h"
#include <stdio.h>
#include <stdlib.h> // random
#include <string.h>
#include <time.h>
#include <assert.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <glib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <setjmp.h>
#include <unistd.h> // getpid


typedef enum _Verbosity
{
	/* the higher, the less important */
	VERBOSE_QUIET = 0,
	VERBOSE_WARN = 1,
	VERBOSE_PROGRESS = 2,
	VERBOSE_INFO = 3,
	VERBOSE_DEBUG = 4,
	VERBOSE_TRACE = 5,
	VERBOSE_ALL
} Verbosity;

#ifdef VERBOSE
# undef VERBOSE
#endif
# define VERBOSE(level,cmd) if(g_verbosity>=level) { cmd; }

void verbose_printf(Verbosity level, const char * fmt, ...);

Verbosity g_verbosity;

void verbose_printf(Verbosity level, const char * fmt, ...)
{
	if (g_verbosity >= level) {
		va_list ap;
		va_start (ap, fmt);
		vprintf (fmt, ap);
		va_end (ap);
	}
}


/*!
 * Command line options.
 */
typedef struct _Options
{
	gchar ** lua_files; // LUA files to load
	gchar ** prog_files; // PROG files to load
	gchar ** inline_code;
	gboolean interactive; // Run interactive shell.
} Options;


// lineno is 1-indexed. Returns NULL is the given line is out of bounds.
// Does not include the trailing newline.
static char * _getline_in_string (const char * s, int lineno)
{
	int curline = 1;
	const char * p = s;
	while (*p && curline < lineno) {
		if (*p == '\n') curline ++;
		p++;
	}
	if (!*p) return NULL;
	else {
		size_t linelen = 0;
		const char * linestart = p;
		while (*p && *p != '\n') { linelen++; p++; }
		return g_strndup (linestart, linelen);
	}
}


void _my_line_hook (lua_State * L, lua_Debug * ar)
{
	int ret;

	// print a traceback
	lua_getglobal(L, "debug");
	lua_getfield(L,-1,"traceback");
	ret = lua_pcall(L, 0, 1, 0);
	if (ret != 0) {
		const char * msg = lua_tostring(L,-1);
		fprintf (stderr, "debug.traceback failed. Reason: %s\n", msg);
		lua_pop(L,1);
		return;
	}
	else {
		printf ("Got traceback: %s\n", lua_tostring(L,-1));
	}

	ret = lua_getinfo(L, "nS", ar);
	if (ret) {
		char * line = NULL;

		if (ar->source && *ar->source != '@')
			line = _getline_in_string(ar->source, ar->currentline);
		else if (ar->source) {
			line = g_strdup_printf ("(line is in file \"%s\")", ar->source+1);
		}

		printf ("Got line %d, name=\"%s\", namewhat=\"%s\",\n"
			"\tline=\"%s\", short_src=\"%s\", linedefined=%d, "
			"\tlastlinedefined=%d, what=\"%s\"\n", ar->currentline,
			ar->name,
			ar->namewhat, line?line:"(nil)",
			ar->short_src, ar->linedefined,
			ar->lastlinedefined, ar->what?ar->what:"(nil)");

		if (line) g_free (line);
	}
	else fprintf (stderr, "INFO lua_getinfo failed.\n");

	fprintf (stderr, "Press <RET> to continue.\n");
	getchar();
}


// name -> ()
static int _my_assert_func (lua_State * L)
{
	const char * name = lua_tostring (L, 1);
	int ch;

	printf ("Assertion \"%s\" failed! Press 'd' and <RET> to "
			"install a line hook.\n", name);
	ch = getchar();
	if ('d' == ch) {
		printf ("Well, setting a debugger (line) hook.\n");
		lua_sethook(L, _my_line_hook, LUA_MASKLINE, 0);
	}
	printf ("continuing ...\n");
	return 0;
}


static void console (const char * str, int len)
{
	fwrite(str, len, 1, stdout);
}

static void consolef (const char * fmt, ...)
{
	gchar * s = NULL;
	va_list ap;

	va_start (ap, fmt);
	s = g_strdup_vprintf (fmt, ap);
	va_end (ap);

	console (s, strlen(s));
	g_free (s);
}


static const char * _get_help_message ()
{
	static const char * s =
			"The console can handle LUA as well as code in Relview's programming\n"
			"language. To execute LUA code, just type it in and press enter. Example:\n"
			"   for i = 1, 5 do print(i..\"\\n\") end\n"
			"\n"
			"To load and execute a file with LUA code, use 'load <file>'. Example:\n"
			"   load /tmp/a_file_with_code.lua\n"
			"\n"
			"To execute an assignment in Relview's programming language, use'!<var>\n"
			"= <relview-expr>'. Example:\n"
			"   !R = f(R)*(-S|T^)\n"
			"\n"
			"To load a file with functions and programs in Relview's programming\n"
			"language, use '@load <file>'. Example:\n"
			"   @load /tmp/a_file_with_code.prog\n"
			"\n"
			"To load a function or program without a file, use\n"
			"'@<relview-file-or-prog>'. Example:\n"
			"   @f(X)=X*X^.  @P(X) DECL Y BEG Y=I(X); RETURN Y|X END\n"
			"\n"
			"To parse a function or program without a file and without loading it into\n"
			"the current state, use '@parse <relview-file-or-prog>'. In this case,\n"
			"the LUA code is written to the console. Example:\n"
			"   @parse f(X)=X*X^.  @P(X) DECL Y BEG Y=I(X); RETURN Y|X END\n"
			"\n"
			"For your convenience, you can use\n"
			"   p <rel-name>     or     dump(<rel-name>)\n"
			"to show the contents of a given relation and\n"
			"   eval(\"<relview-expr>\")\n"
			"short for kure.eval(...). The global context is called __c. The relation $\n"
			"is named __dollar since $ is not a valid variable name in LUA.\n";
	return s;
}

static void _show_help ()
{
	printf (_get_help_message());
}


/*!
 * Replaces the lua 'print' function to redirect stdout to the console.
 * See also _exec_line. Lua's print can take any number or arguments.
 */
static int _lua_print_console (lua_State *L)
{
	int n = lua_gettop(L);    /* number of arguments */
	int i;

	for (i = 1 ; i <= n ; ++i) {
		const char * s;
		/* for any non string, call tostring */
		if (lua_isstring (L,i)) {
			/* With respect to the LUA manual, lua_tostring only works
			 * with string and integers. */
			consolef("%s", lua_tostring(L, i));
		}
		else {
			lua_getfield(L, LUA_GLOBALSINDEX, "tostring");
			lua_pushvalue(L, i);
			lua_call (L, 1, 1); // pops the functions and the argument.
			s = lua_tostring (L, -1);
			consolef("%s", s);
			lua_pop(L,1);
		}
	}
	return 0;
}


#define CMD_FUNC(cmd_name) \
		static void _on_##cmd_name##_cmd (lua_State * L, const gchar * line, gint argc, gchar ** argv)


CMD_FUNC(exit) { kill (getpid(), SIGINT); }
CMD_FUNC(help) { _show_help(); }

CMD_FUNC(export)
{
	const gchar * fmt = argv[1];
	const gchar * rel_name = argv[2];
	const gchar * file = argv[3];

	if (g_str_equal (fmt, "dot")) {

		lua_getglobal(L, rel_name);
		if (lua_isnil(L,-1)) {
			printf ("No such relation with name \"%s\".\n", rel_name);
		}
		else {
			KureRel * rel;
			DdNode * node;
			KureContext * context;
			DdManager * manager;
			FILE * fp;

			rel = *(KureRel**)lua_touserdata(L,-1);
			node = kure_rel_get_bdd(rel);
			context = kure_rel_get_context(rel);
			manager = kure_context_get_manager(context);

			fp = fopen (file, "w+");
			if ( !fp) {
				printf ("Unable to open file \"%s\" for writing.\n", file);
			}
			else {
				Cudd_DumpDot(manager, 1, &node, NULL, NULL, fp);
				printf ("Wrote relation \"%s\" to file \"%s\" in DOT format.\n", rel_name, file);
				fclose (fp);
			}
		}
	}
	else {
		printf ("Unknown export format \"%s\".\n", fmt);
	}
}

/* Load a LUA file. */
CMD_FUNC(load)
{
	if (argc < 2) {
		printf ("Invalid argument. Usage: .%s <file>\n", argv[0]);
	}
	else {
		const gchar * filename = argv[1];
		int error;

		verbose_printf (VERBOSE_INFO, "Loading file \"%s\" ...\n", filename);

		/* Load a file as chunk and execute it. */
		error = luaL_loadfile (L, filename);

		error = error || lua_pcall(L, 0, 0, 0);
		if ( error) {
			printf ("%s\n", lua_tostring(L, -1));
			lua_pop(L, 1);  /* pop error message from the stack */
		}
	}
}


/*!
 * Execute a single line.
 */
static void _exec_line (lua_State * L, const char * line)
{
	char exec = 1, error = 0;
	int line_len = strlen (line);

	/* Replace the global 'print' function by our own function. It doesn't
	 * seem to work to call io.output() or to replace io.stdout. This is
	 * an solution which works in many cases. */
	lua_getfield(L, LUA_GLOBALSINDEX, "print");
	lua_setfield(L, LUA_GLOBALSINDEX, "_print_saved");
	lua_pushcfunction (L, _lua_print_console);
	lua_setfield(L, LUA_GLOBALSINDEX, "print");


	if (line[0] == '.') {
		gint argc;
		gchar ** argv;
		GError * err = NULL;

		if ( !g_shell_parse_argv (line+1, &argc, &argv, &err)) {
			fprintf (stderr, "Unable to parse command line \"%s\".\n", line);
			g_error_free (err);
		}
		else {
			const struct {
				gchar * name;
				void (*func) (lua_State*,const gchar*,gint,gchar**);
			} commands [] = {
					{ "exit", _on_exit_cmd },
					{ "help", _on_help_cmd },
					{ "export", _on_export_cmd },
					{ "load", _on_load_cmd },
					{ NULL, NULL }
			}, *iter = commands;

			if (argc > 0) {
				for ( ; iter->name ; ++iter) {
					if (g_str_equal(iter->name, argv[0])) {
						iter->func (L, line, argc, argv);
						break;
					}
				}

				if ( !iter->name)
					printf ("Unknown command: \"%s\".\n", argv[0]);
			}

			g_strfreev (argv);
		}

		exec = 0;
	}
	else if (line[0] == '!') {
		// $ matches \n in the following.
		GError *gerr = NULL;
		const char * pattern = "^\\s*([a-z_][a-z0-9_]*)\\s*=\\s*([^\n]+)$";
		GRegex * regex = g_regex_new (pattern, G_REGEX_CASELESS, 0, &gerr);

		exec = 0;

		if (!regex) {
			g_warning ("graph-drawing: Error in regex '%s'. Reason: %s",
					pattern, gerr->message);
			g_error_free(gerr);
		}
		else {
			GMatchInfo * m = NULL;
			if ( !g_regex_match (regex, line+1, (GRegexMatchFlags)0, &m)) {
				consolef ("Invalid format for input. Syntax is \"!<var> = <relview-code>\".\n");
			}
			else {
				KureError * err = NULL;
				Kure_success success;
				char *var, *code;
				var = g_match_info_fetch (m, 1);
				code = g_match_info_fetch (m, 2);

				success = kure_lang_assign (L, var, code, &err);
				if ( !success) {
					consolef ("Error. Reason: %s\n", err->message);
					kure_error_destroy(err);
				}
			}

			g_regex_unref (regex);
		}
	}
	else if (line[0] == '?') {
		_show_help();
		exec = 0;
	}
	else if (line[0] == 'p' && line[1] == ' ') {
		char * code = g_strdup_printf ("dump(%s)", line+2);
		exec = 0;
		(void)luaL_dostring (L, code);
		g_free (code);
	}
	else if (line[0] == '@') {
		KureError * err = NULL;

		/* The line is in RelView Syntax. Those lines are not executed. */
		exec = 0;
		if (g_str_has_prefix (line+1, "load")) {
			char file[128];
			sscanf(line+1, "load %s", file);
			if ( !kure_lang_load_file (L, file, &err)) {
				consolef ("Error. Reason: %s\n", err->message);
				kure_error_destroy(err);
			}
		}
		else if (g_str_has_prefix (line+1, "parse")) {
			char * s = kure_lang_to_lua (line+6, &err);
			if ( !s) {
				consolef ("Error. Reason: %s\n", err->message);
				kure_error_destroy(err);
			}
			else {
				consolef ("%s", s);
				free (s);
			}
		}
		else {
			if ( !kure_lang_load (L, line+1, &err)) {
				consolef ("Error. Reason: %s\n", err->message);
				kure_error_destroy(err);
			}
		}
	}
	else {
		error = luaL_loadbuffer(L, line, line_len, "line");
	}

	if (exec) {
		error = error || lua_pcall(L, 0, 0, 0);
		if (error) {
			consolef ("%s\n", lua_tostring(L, -1));
			lua_pop(L, 1);  /* pop error message from the stack */
		}
	}

	/* Reset the global 'print' function. See above. */
	lua_getfield(L, LUA_GLOBALSINDEX, "_print_saved");
	lua_setfield(L, LUA_GLOBALSINDEX, "print");
}


/*!
 * Shows version information for the application.
 */
static void _show_version_information (FILE * fp)
{
	int bits = sizeof(void*) * 8;

	fprintf (fp, "Kure2 shell %d.%d.%d (%d-bits, CUDD "CUDD_VERSION")\n"
			"Copyright (C) 2010,2011,2012 Stefan Bolus, University of Kiel, Germany\n"
			"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
			"This is free software: you are free to change and redistribute it.\n"
			"There is NO WARRANTY, to the extent permitted by law.\n"
			"\n"
			"Visit <http://www.sf.net/projects/kure> for the most recent version.\n"
			"\n", KURE_MAJOR_VERSION, KURE_MINOR_VERSION, KURE_MICRO_VERSION, bits);
}


/*!
 * Parses the command line (options) and returns a corresponding options.
 * Terminates the program on some option. Returns true on success and false
 * on error. Does not throw an exception.
 *
 * \return True on success, false otherwise.
 */
static gboolean _parse_options (Options * opts, int argc, char ** argv)
{
	GError * error = NULL;
	GOptionContext * context = g_option_context_new ("");
	gboolean show_version = FALSE;
	gint verbosity = VERBOSE_INFO; // quiet
	gboolean be_quiet = FALSE;

	opts->inline_code = NULL;
	opts->lua_files = NULL;
	opts->prog_files = NULL;
	opts->interactive = FALSE;

	GOptionEntry entries[] = {
			{ "version", 0, 0, G_OPTION_ARG_NONE, &show_version, "Show version information and exit.", NULL },
			{ "verbose", 'v', 0, G_OPTION_ARG_INT, &verbosity, "The higher the value, the more chatty is the program. [default=2]", "{0,1,..}" },
			{ "quiet", 0, 0, G_OPTION_ARG_NONE, &be_quiet, "Suppress any non-relevant output. Same as --verbose=0.", NULL },
			{ "file", 'f', 0, G_OPTION_ARG_STRING_ARRAY, (gpointer)&opts->lua_files, "LUA File to run.", "PATH" },
			{ "prog-file", 'p', 0, G_OPTION_ARG_STRING_ARRAY, (gpointer)&opts->prog_files, "PROG File to run.", "PATH" },
			{ "execute", 'e', 0, G_OPTION_ARG_STRING_ARRAY, (gpointer)&opts->inline_code, "Inline code to run.", "CODE" },
			{ "interactive", 'i', 0, G_OPTION_ARG_NONE, &opts->interactive, "Run interactive shell after executing code." },
			{ NULL }
	};

	g_option_context_add_main_entries(context, entries, NULL);
	g_option_context_set_description (context, "Bug reports to <name>@informatik.uni-kiel.de where <name>=stb\n");

	if (!g_option_context_parse(context, &argc, &argv, &error))
	{
		fprintf (stderr, "Unable to parse command line. Reason: %s\n", error->message);
	   	g_error_free (error);
	   	return FALSE;
   }

	if (show_version) {
		_show_version_information (stderr);
		exit (0);
	}

	/* Verbosity. Ignore --verbose=... and -v ... if --quiet is given. */
	if (be_quiet)
		g_verbosity = VERBOSE_QUIET;
	else
		g_verbosity = (Verbosity) verbosity;

	/* If no code or file to execute is given, the default behavior is to show
	 * a command prompt. */
	if (!opts->inline_code && !opts->lua_files) {
		opts->interactive = TRUE;
	}

	return TRUE;
}


static int _interactive_main (lua_State * L)
{
	char * line = NULL;

	verbose_printf (VERBOSE_INFO, "Enter ? on a single line to see the help message. You can exit at any time using CTRL+C.\n");

	using_history ();

	while (1) {
		if (line) free (line);
		line = readline("kure> ");
		if ( !line)
			break;

		if (!*line) continue; // empty line
		else {
			/* Don't save duplicates */
			HIST_ENTRY * he = previous_history();
			if ( !he || !g_str_equal (he->line, line))
				add_history (line);
		}

		_exec_line(L, line);
	}

	if (line) free (line);

	return 0;
}


jmp_buf env; // used to catch SIGINT

static void _on_sigint (int signum)
{
	verbose_printf (VERBOSE_INFO, "\nCaught SIGINT ... leaving interactive mode.\n");
	longjmp(env, 1);
}


int main (int argc, char ** argv)
{
	Options opts;

	if ( !_parse_options (&opts, argc, argv))
		return 1;
	else {
		int ret = 0;
		lua_State * L = NULL;
		KureContext * context = kure_context_new();

		L = kure_lua_new (context); // context is now owned by the LUA_State.

		/* Add auxiliary functions dump(<rel>) and eval(<relview-code>). */
		(void)luaL_dostring (L, "function dump(R) print(kure.dump(R)) end");
		(void)luaL_dostring (L, "eval = kure.eval");

		kure_lang_set_assert_func(L, _my_assert_func);

		/* If there have been file arguments on the command line, execute
		 * them before any other code is executed. */
		if (opts.lua_files) {
			gchar ** ptr;

			for (ptr = opts.lua_files ; *ptr ; ++ptr) {
				const gchar * filename = *ptr;
				gchar * cnt;
				gsize cnt_size;
				GError * err = NULL;
				gboolean success;

				verbose_printf (VERBOSE_INFO, "Loading LUA file \"%s\" ...", filename);
				success = g_file_get_contents (*ptr, &cnt, &cnt_size, &err);
				if ( !success) {
					fprintf (stderr, "Unable to read file \"%s\". Reason: %s", *ptr, err->message);
					g_error_free (err);
				}
				else {
					int error = luaL_loadbuffer (L, cnt, cnt_size, filename);
					if (error) {
						fprintf(stderr, "Unable to load file \"%s\". Reason: %s\n",
								filename, lua_tostring(L, -1));
						exit (1);
					}
					else {
						lua_pcall(L, 0, 0, 0);
					}
					g_free (cnt);
				}
			}

			g_strfreev (opts.lua_files);
		}

		/* If there have been file arguments for PROG file on the command line,
		 * execute them before any other code is executed. */
		if (opts.prog_files) {
			gchar ** ptr;

			for (ptr = opts.prog_files ; *ptr ; ++ptr) {
				const gchar * filename = *ptr;
				KureError *kerr = NULL;
				Kure_success success;

				verbose_printf (VERBOSE_INFO, "Loading PROG file \"%s\" ...", filename);
				success = kure_lang_load_file (L, filename, &kerr);
				if (!success) {
					fprintf(stderr, "Unable to load file \"%s\". Reason: %s\n",
							filename, kerr ? kerr->message : "(unknown)");
					exit (1);
				}
			}

			g_strfreev (opts.prog_files);
		}

		/* If there have been inline code arguments on the command line, execute
		 * all of them in order. */
		if (opts.inline_code) {
			gchar ** ptr;

			for (ptr = opts.inline_code ; *ptr ; ++ptr) {
				const gchar * code = *ptr;
				if (*code != '\0') {
					verbose_printf (VERBOSE_INFO, "Executing \"%s\" ...", code);
					_exec_line(L, code);
				}
			}

			g_strfreev (opts.inline_code);
		}

		if (opts.interactive) {
			signal (SIGINT, _on_sigint);
			if (0 == setjmp (env)) {
				ret = _interactive_main (L);
				printf ("\n");
			}
			else {
				/* after longjmp */
			}
			signal (SIGINT, SIG_DFL);
		}

		lua_close(L);

		return ret;
	}
}
