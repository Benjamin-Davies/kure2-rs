
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison GLR parsers in C

      Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C GLR parser skeleton written by Paul Hilfinger.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "glr.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 1


/* "%code top" blocks.  */

/* Line 159 of glr.c  */
// #line 118 "./parser.y"

#define _GNU_SOURCE // We need vasprintf
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <KureImpl.h>
#include <glib.h>
#include "lexer.h"
#include "parser.h"




/* Line 159 of glr.c  */
// #line 70 "parser.c"
/* Substitute the variable and function names.  */
#define yyparse kure_yyparse
#define yylex   kure_yylex
#define yyerror kure_yyerror
#define yylval  kure_yylval
#define yychar  kure_yychar
#define yydebug kure_yydebug
#define yynerrs kure_yynerrs
#define yylloc  kure_yylloc

/* Copy the first part of user declarations.  */


/* Line 172 of glr.c  */
// #line 85 "parser.c"



#include "parser.h"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* Default (constant) value used for initialization for null
   right-hand sides.  Unlike the standard yacc.c template,
   here we set the default value of $$ to a zeroed-out value.
   Since the default value is undefined, this behavior is
   technically correct.  */
static YYSTYPE yyval_default;

/* Copy the second part of user declarations.  */


/* Line 243 of glr.c  */
// #line 120 "parser.c"
/* Unqualified %code blocks.  */

/* Line 244 of glr.c  */
// #line 153 "./parser.y"

  #include <glib.h>

 typedef enum {
   VAR, DOM, FUN
  } LocalDeclType;

  typedef struct {
    LocalDeclType type;
    gchar * name;
  } LocalDecl;

  struct _Context {
    GQueue/*<LocalDecl*>*/ decls;
  };

#define POP_CONTEXT { g_assert(!g_queue_is_empty(contexts)); \
    _destroy_context(g_queue_pop_tail(contexts)); }
#define PUSH_CONTEXT { g_queue_push_tail(contexts, _new_context());  }
#define CONTEXT ((Context*)contexts->tail->data)

#define IS_DECLARED(name,type) _is_declared(CONTEXT, (name), (type))
#define DECLARE(name,type) {if (_declare(CONTEXT, (name), (type)))	\
      { printf ("Warning: Object with name \"%s\" already defined!\n", name); }}

  typedef struct fun_map_t fun_map_t;

  static void _destroy_decls (LocalDecl * self, void * dummy);
  static Context * _new_context ();
  static void _destroy_context (Context * self);
  static int _is_declared (Context * c, const char * name, LocalDeclType type);
  static LocalDecl * _declare(Context * c, const char * name, LocalDeclType type);
  static void _g_free_all (void * p, ...);
  static char * map_fun (char * fun);
  static gboolean _is_reserved_fun (const gchar * name);
  static const fun_map_t * _lookup_reserved_fun (const gchar * name);
  static gchar * _check_args_code (const gchar * name, const gchar * params);

#define CONV(fmt,...) g_strdup_printf(fmt, __VA_ARGS__); _g_free_all(__VA_ARGS__, NULL);

/* If you change the name here, you also have to change it in
 * src/KureLuaBinding.c. */
#define LINE_TRACKING_VAR "__line"

  /* To be compatible with the RelView language but not being dependent on it,
   * we use a simple compatibilty tier here. That is, if an operation in Kure
   * and RelView isn't compatible we just map RelView's call to the Kure call.
   * If not, then we forward the RelView call to a compatible func., which
   * adapts the call, so Kure can serve it. The compatible tier is implemented
   * in Lua and is precomplied at build time. See src/kure_compat.lua. The
   * random functions are treated in a special way. See \ref map_fun below. */
   #define fun_map kure_fun_map
   struct fun_map_t {
    char * name;
    int arg_count;
    char * transl_name;
  } fun_map [] = { { "O",        1, "kure.compat.O" },
		   { "On1",      1, "kure.compat.On1" },
		   { "O1n",      1, "kure.compat.O1n" },
		   { "L",        1, "kure.compat.L" },
		   { "Ln1",      1, "kure.compat.Ln1"},
		   { "L1n",      1, "kure.compat.L1n" },
		   { "I",        1, "kure.compat.I" },
		   { "dom",      1, "kure.domain" },
		   { "syq",      2, "kure.syq" },
		   { "trans",    1, "kure.trans_hull" },
		   { "refl",     1, "kure.refl_hull" },
		   { "symm",     1, "kure.symm_hull" },
		   { "inj",      1, "kure.vec_inj" },
		   { "init",     1, "kure.compat.init" },
		   { "succ",     1, "kure.successors" },
		   { "next",     1, "kure.vec_next" },
		   { "point",    1, "kure.vec_point" },
		   { "atom",     1, "kure.atom" },
		   { "randomperm", 1, "kure.compat.randomperm" },
		   /* The following version of random(R,S) takes two relations.
		    * the first is for the size and the second for the prob..
		    * which is chosen as |S|/(rows(S)*cols(S)). */
		   { "random",   2, "kure.random" },
		   { "empty",    1, "kure.compat.empty" },
		   { "unival",   1, "kure.compat.unival" },
		   { "eq",       2 , "kure.compat.eq" },
		   { "incl",     2, "kure.compat.incl" },
		   { "epsi",     1, "kure.epsi" },
		   { "cardfilter", 2, "kure.compat.cardfilter" },
		   { "cardrel",  1, "kure.compat.cardrel" },
		   { "cardeq",   2, "kure.compat.cardeq" },
		   { "cardlt",   2, "kure.compat.cardlt" },
		   { "cardleq",  2, "kure.compat.cardleq" },
		   { "cardgt",   2, "kure.compat.cardgt" },
		   { "cardgeq",  2, "kure.compat.cardgeq" },
		   { "FALSE",    0, "kure.compat.FALSE" },
		   { "TRUE",     0, "kure.compat.TRUE" },
		   { "false",    0, "kure.compat.FALSE" },
		   { "true",     0, "kure.compat.TRUE" },
		   { "minsets_upset"  , 1, "kure.minsets_upset" },
		   { "minsets",         1, "kure.minsets" },
		   { "maxsets_downset", 1, "kure.maxsets_downset" },
		   { "maxsets",         1, "kure.maxsets" },
		   /* The following are also considered in the .l file because
		    * they begin with a digit. */
		   { "1-st",     1, "kure.compat.comp1" },
		   { "2-nd",     1, "kure.compat.comp2" },
		   { "p-1",      1, "kure.compat.p_1" },
		   { "p-2",      1, "kure.compat.p_2" },
		   { "i-1",      1, "kure.compat.i_1" },
		   { "i-2",      1, "kure.compat.i_2" },
		   { "p-ord",    2, "kure.product_order" },
		   { "s-ord",    2, "kure.sum_order" },
		   { "part-f",   2, "kure.compat.partial_funcs" },
		   { "tot-f",    2, "kure.compat.total_funcs" },
		   { 0 } };



/* Line 244 of glr.c  */
// #line 240 "parser.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#ifndef YYFREE
# define YYFREE free
#endif
#ifndef YYMALLOC
# define YYMALLOC malloc
#endif
#ifndef YYREALLOC
# define YYREALLOC realloc
#endif

#define YYSIZEMAX ((size_t) -1)

#ifdef __cplusplus
   typedef bool yybool;
#else
   typedef unsigned char yybool;
#endif
#define yytrue 1
#define yyfalse 0

#ifndef YYSETJMP
# include <setjmp.h>
# define YYJMP_BUF jmp_buf
# define YYSETJMP(env) setjmp (env)
# define YYLONGJMP(env, val) longjmp (env, val)
#endif

/*-----------------.
| GCC extensions.  |
`-----------------*/

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5) || __STRICT_ANSI__)
#  define __attribute__(Spec) /* empty */
# endif
#endif

#define YYOPTIONAL_LOC(Name) Name

#ifndef YYASSERT
# define YYASSERT(condition) ((void) ((condition) || (abort (), 0)))
#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  14
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   332

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  39
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  32
/* YYNRULES -- Number of rules.  */
#define YYNRULES  80
/* YYNRULES -- Number of states.  */
#define YYNSTATES  153
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 8
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   277

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     6,     2,
      31,    33,     7,     8,    32,    10,    36,     3,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    35,
       2,    34,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    37,     4,    38,     9,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     5,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     4,     8,    11,    14,    16,    19,    21,
      23,    29,    37,    43,    50,    51,    56,    58,    61,    62,
      65,    70,    73,    76,    80,    84,    90,    96,   102,   105,
     106,   112,   113,   119,   122,   123,   129,   131,   134,   135,
     137,   139,   143,   146,   150,   153,   155,   159,   163,   167,
     171,   175,   181,   187,   193,   197,   201,   205,   209,   213,
     216,   218,   220,   225,   226,   228,   230,   234,   236,   239,
     242,   246,   248,   252,   254,   256,   258,   260,   262,   266,
     275
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const signed char yyrhs[] =
{
      40,     0,    -1,    -1,    29,    41,    42,    -1,    30,    60,
      -1,    30,     1,    -1,    43,    -1,    42,    43,    -1,    49,
      -1,    56,    -1,    23,    60,    22,    47,    20,    -1,    23,
      60,    22,    47,    21,    47,    20,    -1,    26,    60,    25,
      47,    24,    -1,    27,    31,    28,    32,    60,    33,    -1,
      -1,    28,    45,    34,    60,    -1,    44,    -1,    44,    35,
      -1,    -1,    47,    46,    -1,    28,    31,    57,    33,    -1,
      50,    36,    -1,    50,    15,    -1,    48,    34,    60,    -1,
      48,    34,     1,    -1,    17,    47,    16,    60,    15,    -1,
      17,     1,    16,    60,    15,    -1,    17,    47,    16,     1,
      15,    -1,    48,    51,    -1,    -1,    48,    18,    53,    64,
      51,    -1,    -1,    48,    18,    54,     1,    51,    -1,    48,
       1,    -1,    -1,    48,    18,    55,    64,     1,    -1,    52,
      -1,    52,    36,    -1,    -1,    58,    -1,    28,    -1,    58,
      32,    28,    -1,    60,     9,    -1,    31,    59,    33,    -1,
      59,     9,    -1,    59,    -1,    60,     7,    59,    -1,    59,
       7,    60,    -1,    59,     7,    59,    -1,    60,     7,    60,
      -1,    31,    60,    33,    -1,    37,    60,    32,    60,    11,
      -1,    12,    60,    32,    60,    38,    -1,    37,    60,    32,
      60,    38,    -1,    60,     6,    60,    -1,    60,     5,    60,
      -1,    60,     4,    60,    -1,    60,     3,    60,    -1,    60,
       8,    60,    -1,    10,    60,    -1,    61,    -1,    28,    -1,
      28,    31,    62,    33,    -1,    -1,    63,    -1,    60,    -1,
      63,    32,    60,    -1,    65,    -1,    65,    35,    -1,    64,
      65,    -1,    64,    65,    35,    -1,    66,    -1,    65,    32,
      66,    -1,    67,    -1,    68,    -1,    69,    -1,    70,    -1,
      28,    -1,    48,    34,    60,    -1,    28,    34,    14,    31,
      60,    32,    60,    33,    -1,    28,    34,    13,    31,    60,
      32,    60,    33,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   371,   371,   371,   372,   373,   375,   376,   380,   381,
     386,   387,   388,   389,   393,   393,   397,   398,   401,   402,
     404,   414,   415,   417,   423,   427,   428,   429,   431,   437,
     437,   443,   443,   444,   445,   445,   448,   448,   450,   451,
     453,   454,   457,   458,   460,   461,   462,   463,   464,   465,
     468,   469,   470,   471,   472,   473,   474,   475,   476,   477,
     478,   479,   482,   503,   504,   506,   507,   512,   513,   514,
     515,   517,   518,   520,   522,   523,   524,   526,   530,   537,
     538
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "'/'", "'\\\\'", "'|'", "'&'", "'*'",
  "'+'", "'^'", "'-'", "RRBRACKET", "LLBRACKET", "SUM", "PROD", "END",
  "RET", "BEG", "DECL", "PROG", "FI", "ELSE", "THEN", "IF", "OD", "DO",
  "WHILE", "ASSERT", "IDENTIFIER", "START_DEFAULT", "START_EXPR", "'('",
  "','", "')'", "'='", "';'", "'.'", "'['", "']'", "$accept", "start",
  "$@1", "translation_unit", "translation_element", "bare_statement",
  "$@2", "statement", "statement_list", "declarator", "fundef",
  "fundef_bare", "progbody", "progdef_bare", "$@3", "$@4", "$@5",
  "progdef", "parameter_list", "real_parameter_list", "transp_expr",
  "expr", "postfix_expr", "arg_expr_list", "real_arg_expr_list",
  "local_decls", "local_decl_list", "local_decl", "local_decl_bare",
  "local_vardecl", "local_fundef", "local_domdef", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    39,    41,    40,    40,    40,    42,    42,    43,    43,
      44,    44,    44,    44,    45,    44,    46,    46,    47,    47,
      48,    49,    49,    50,    50,    51,    51,    51,    52,    53,
      52,    54,    52,    52,    55,    52,    56,    56,    57,    57,
      58,    58,    59,    59,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    61,    62,    62,    63,    63,    64,    64,    64,
      64,    65,    65,    66,    67,    67,    67,    68,    69,    70,
      70
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     3,     2,     2,     1,     2,     1,     1,
       5,     7,     5,     6,     0,     4,     1,     2,     0,     2,
       4,     2,     2,     3,     3,     5,     5,     5,     2,     0,
       5,     0,     5,     2,     0,     5,     1,     2,     0,     1,
       1,     3,     2,     3,     2,     1,     3,     3,     3,     3,
       3,     5,     5,     5,     3,     3,     3,     3,     3,     2,
       1,     1,     4,     0,     1,     1,     3,     1,     2,     2,
       3,     1,     3,     1,     1,     1,     1,     1,     3,     8,
       8
};

/* YYDPREC[RULE-NUM] -- Dynamic precedence of rule #RULE-NUM (0 if none).  */
static const unsigned char yydprec[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     3,     3,     2,     1,     4,     6,     8,     1,
       2,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0
};

/* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM.  */
static const unsigned char yymerger[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned char yydefact[] =
{
       0,     2,     0,     0,     0,     5,     0,     0,    61,     0,
       0,    45,     4,    60,     1,     0,     3,     6,     0,     8,
       0,    36,     9,    59,     0,    63,    45,     0,     0,     0,
      44,     0,     0,     0,     0,     0,     0,    42,    38,     7,
      33,     0,    29,     0,    28,    22,    21,    37,     0,    65,
       0,    64,    43,    50,     0,    45,    47,    57,    56,    55,
      54,    45,    49,    58,    40,     0,    39,     0,     0,     0,
       0,     0,    24,    23,     0,    62,     0,     0,    20,     0,
       0,     0,     0,     0,     0,    14,    16,    19,    77,     0,
       0,    67,    71,    73,    74,    75,    76,     0,     0,    52,
      66,    51,    53,    41,     0,     0,     0,     0,     0,     0,
       0,    17,     0,     0,    30,    69,     0,    68,    32,    35,
      26,    27,    25,    18,    18,     0,     0,     0,     0,    78,
      70,    72,     0,     0,     0,    15,     0,     0,    10,    18,
      12,     0,     0,     0,     0,    13,     0,     0,    11,     0,
       0,    80,    79
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const signed char yydefgoto[] =
{
      -1,     3,     4,    16,    17,    86,   110,    87,    68,    89,
      19,    20,    44,    21,    69,    70,    71,    22,    65,    66,
      11,    12,    13,    50,    51,    90,    91,    92,    93,    94,
      95,    96
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -84
static const short int yypact[] =
{
      17,   -84,     4,     2,   -16,   -84,   247,   247,   -10,   247,
     247,    -1,   323,   -84,   -84,    42,   -16,   -84,    16,   -84,
      -5,    54,   -84,   -84,   129,   247,   221,   178,   230,   247,
     -84,   247,   247,   247,   247,   247,   247,   -84,    63,   -84,
     -84,   254,    10,     8,   -84,   -84,   -84,   -84,   247,   323,
      84,    64,   -84,   -84,   247,    78,   205,   315,   315,   265,
     301,   142,   205,   110,   -84,    88,    91,   113,    99,   111,
     143,   111,   -84,   323,    59,   -84,   247,    50,   -84,   126,
     247,    12,   247,   247,   125,   -84,   136,   -84,   121,   187,
      52,   141,   -84,   -84,   -84,   -84,   -84,   210,    43,   -84,
     323,   -84,   -84,   -84,   297,   145,   310,   289,   282,   203,
     222,   -84,    38,   247,   -84,   194,   111,   -84,   -84,   -84,
     -84,   -84,   -84,   -84,   -84,   228,   247,   232,   236,   323,
     -84,   -84,   189,   196,   247,   323,   247,   247,   -84,   -84,
     -84,   185,   237,   244,   238,   -84,   247,   247,   -84,   192,
     199,   -84,   -84
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
     -84,   -84,   -84,   -84,   252,   -84,   -84,   -84,   -15,    56,
     -84,   -84,    82,   -84,   -84,   -84,   -84,   -84,   -84,   -84,
      83,    -6,   -84,   -84,   -84,   208,   -83,   167,   -84,   -84,
     -84,   -84
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -46
static const short int yytable[] =
{
      23,    24,    14,    27,    28,     5,    29,   115,    30,    72,
      45,   -31,    15,   105,     6,   115,     7,    40,     6,    49,
       7,    25,     6,    56,     7,    57,    58,    59,    60,    62,
      63,    46,     8,    41,    42,     9,     8,    73,   -29,     9,
       8,    10,    74,     9,   119,    10,     1,     2,    77,    10,
      43,   127,   128,    31,    32,    33,    34,    35,    36,    37,
      18,   101,    31,    32,    33,    34,    35,    36,    37,    41,
     100,    88,    18,    38,   104,   106,   107,   108,   -45,   -45,
      88,   -45,   -45,   -45,   -45,   -45,   -45,    30,   102,   -45,
      47,    64,    26,   -45,   -45,   -45,    76,    99,   -45,   -45,
     -45,   -45,   -45,   -45,   -45,   -45,   -45,   129,   132,   133,
     -45,   -45,    55,   -45,   -45,    81,   -45,    75,    61,    37,
     135,    78,    82,    79,   144,    83,    84,    85,   141,    80,
     142,   143,    31,    32,    33,    34,    35,    36,    37,    88,
     149,   150,   -45,   -45,    97,   -45,   -45,   -45,   -45,   -45,
     -45,    30,    38,   -45,   103,   112,   109,   -45,   -45,   -45,
     121,    48,   -45,   -45,   -45,   -45,   -45,   -45,   -45,   -45,
     -45,   111,   114,   116,   -45,   -45,   117,   -45,   -45,   118,
     -45,    31,    32,    33,    34,    35,    36,    37,    31,    32,
      33,    34,    35,    36,    37,    31,    32,    33,    34,    35,
      36,    37,    31,    32,    33,    34,    35,    36,    37,   138,
     139,    53,    82,    36,    37,    83,    84,    85,   145,    82,
     140,   113,    83,    84,    85,   151,   116,    41,    29,   130,
      30,   125,   152,    31,    32,    33,    34,    35,    36,    37,
      31,    32,    33,    34,    35,    36,    37,    31,    32,    33,
      34,    35,    36,    37,    52,    67,   126,     6,   148,     7,
     134,    82,    54,   136,    83,    84,    85,   137,    39,   146,
     -18,    34,    35,    36,    37,     8,   147,   -18,     9,    98,
     -18,   -18,   -18,   131,    10,    31,    32,    33,    34,    35,
      36,    37,    31,    32,    33,    34,    35,    36,    37,     0,
      31,    32,    33,    34,    35,    36,    37,   124,    35,    36,
      37,   123,   120,    31,    32,    33,    34,    35,    36,    37,
      33,    34,    35,    36,    37,   122,    31,    32,    33,    34,
      35,    36,    37
};

/* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
   list of conflicting reductions corresponding to action entry for
   state STATE-NUM in yytable.  0 means no conflicts.  The list in
   yyconfl is terminated by a rule number of 0.  */
static const unsigned char yyconflp[] =
{
       0,     0,     0,     0,     0,     0,     1,     0,     3,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    11,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    13,    15,
       0,    17,    19,    21,    23,    25,    27,    29,     0,    31,
       0,     0,     0,    33,    35,    37,     0,     0,    39,    41,
      43,    45,    47,    49,    51,    53,    55,     0,     0,     0,
      57,    59,     0,    61,    63,     0,    65,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    67,    69,     0,    71,    73,    75,    77,    79,
      81,    83,     0,    85,     0,     0,     0,    87,    89,    91,
       0,     0,    93,    95,    97,    99,   101,   103,   105,   107,
     109,     0,     0,     0,   111,   113,     0,   115,   117,     0,
     119,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     5,     0,
       7,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     9,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,    45,     0,    45,     0,    45,     0,    45,     0,    45,
       0,    34,     0,    48,     0,    48,     0,    48,     0,    48,
       0,    48,     0,    48,     0,    48,     0,    48,     0,    45,
       0,    48,     0,    48,     0,    48,     0,    48,     0,    48,
       0,    48,     0,    48,     0,    48,     0,    48,     0,    48,
       0,    48,     0,    48,     0,    48,     0,    48,     0,    48,
       0,    48,     0,    48,     0,    48,     0,    46,     0,    46,
       0,    46,     0,    46,     0,    46,     0,    46,     0,    46,
       0,    46,     0,    45,     0,    46,     0,    46,     0,    46,
       0,    46,     0,    46,     0,    46,     0,    46,     0,    46,
       0,    46,     0,    46,     0,    46,     0,    46,     0,    46,
       0,    46,     0,    46,     0,    46,     0,    46,     0,    46,
       0
};

static const short int yycheck[] =
{
       6,     7,     0,     9,    10,     1,     7,    90,     9,     1,
      15,     1,    28,     1,    10,    98,    12,     1,    10,    25,
      12,    31,    10,    29,    12,    31,    32,    33,    34,    35,
      36,    36,    28,    17,    18,    31,    28,    43,    28,    31,
      28,    37,    48,    31,     1,    37,    29,    30,    54,    37,
      34,    13,    14,     3,     4,     5,     6,     7,     8,     9,
       4,    11,     3,     4,     5,     6,     7,     8,     9,    17,
      76,    28,    16,    31,    80,    81,    82,    83,     0,     1,
      28,     3,     4,     5,     6,     7,     8,     9,    38,    11,
      36,    28,     9,    15,    16,    17,    32,    38,    20,    21,
      22,    23,    24,    25,    26,    27,    28,   113,   123,   124,
      32,    33,    29,    35,    36,    16,    38,    33,    35,     9,
     126,    33,    23,    32,   139,    26,    27,    28,   134,    16,
     136,   137,     3,     4,     5,     6,     7,     8,     9,    28,
     146,   147,     0,     1,     1,     3,     4,     5,     6,     7,
       8,     9,    31,    11,    28,    34,    31,    15,    16,    17,
      15,    32,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    35,    90,    32,    32,    33,    35,    35,    36,    97,
      38,     3,     4,     5,     6,     7,     8,     9,     3,     4,
       5,     6,     7,     8,     9,     3,     4,     5,     6,     7,
       8,     9,     3,     4,     5,     6,     7,     8,     9,    20,
      21,    33,    23,     8,     9,    26,    27,    28,    33,    23,
      24,    34,    26,    27,    28,    33,    32,    17,     7,    35,
       9,    28,    33,     3,     4,     5,     6,     7,     8,     9,
       3,     4,     5,     6,     7,     8,     9,     3,     4,     5,
       6,     7,     8,     9,    33,     1,    34,    10,    20,    12,
      32,    23,    32,    31,    26,    27,    28,    31,    16,    32,
      16,     6,     7,     8,     9,    28,    32,    23,    31,    71,
      26,    27,    28,   116,    37,     3,     4,     5,     6,     7,
       8,     9,     3,     4,     5,     6,     7,     8,     9,    -1,
       3,     4,     5,     6,     7,     8,     9,    25,     7,     8,
       9,    22,    15,     3,     4,     5,     6,     7,     8,     9,
       5,     6,     7,     8,     9,    15,     3,     4,     5,     6,
       7,     8,     9
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    29,    30,    40,    41,     1,    10,    12,    28,    31,
      37,    59,    60,    61,     0,    28,    42,    43,    48,    49,
      50,    52,    56,    60,    60,    31,    59,    60,    60,     7,
       9,     3,     4,     5,     6,     7,     8,     9,    31,    43,
       1,    17,    18,    34,    51,    15,    36,    36,    32,    60,
      62,    63,    33,    33,    32,    59,    60,    60,    60,    60,
      60,    59,    60,    60,    28,    57,    58,     1,    47,    53,
      54,    55,     1,    60,    60,    33,    32,    60,    33,    32,
      16,    16,    23,    26,    27,    28,    44,    46,    28,    48,
      64,    65,    66,    67,    68,    69,    70,     1,    64,    38,
      60,    11,    38,    28,    60,     1,    60,    60,    60,    31,
      45,    35,    34,    34,    51,    65,    32,    35,    51,     1,
      15,    15,    15,    22,    25,    28,    34,    13,    14,    60,
      35,    66,    47,    47,    32,    60,    31,    31,    20,    21,
      24,    60,    60,    60,    47,    33,    32,    32,    20,    60,
      60,    33,    33
};


/* Prevent warning if -Wmissing-prototypes.  */
int yyparse (yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts);

/* Error token number */
#define YYTERROR 1

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */


#define YYRHSLOC(Rhs, K) ((Rhs)[K].yystate.yyloc)
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))							\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))

/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# define YY_LOCATION_PRINT(File, Loc)			\
    fprintf (File, "%d.%d-%d.%d",			\
	     (Loc).first_line, (Loc).first_column,	\
	     (Loc).last_line,  (Loc).last_column)
#endif


#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#define YYLEX yylex (&yylval, &yylloc, yyscanner)


#undef yynerrs
#define yynerrs (yystackp->yyerrcnt)
#undef yychar
#define yychar (yystackp->yyrawchar)
#undef yylval
#define yylval (yystackp->yyval)
#undef yylloc
#define yylloc (yystackp->yyloc)
#define kure_yynerrs yynerrs
#define kure_yychar yychar
#define kure_yylval yylval
#define kure_yylloc yylloc

static const int YYEOF = 0;
static const int YYEMPTY = -2;

typedef enum { yyok, yyaccept, yyabort, yyerr } YYRESULTTAG;

#define YYCHK(YYE)							     \
   do { YYRESULTTAG yyflag = YYE; if (yyflag != yyok) return yyflag; }	     \
   while (YYID (0))

#if YYDEBUG

# ifndef YYFPRINTF
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (yyscanner);
  YYUSE (pcode);
  YYUSE (o);
  YYUSE (err);
  YYUSE (contexts);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, yyscanner, pcode, o, err, contexts);
  YYFPRINTF (yyoutput, ")");
}

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			    \
do {									    \
  if (yydebug)								    \
    {									    \
      YYFPRINTF (stderr, "%s ", Title);					    \
      yy_symbol_print (stderr, Type,					    \
		       Value, Location, yyscanner, pcode, o, err, contexts);  \
      YYFPRINTF (stderr, "\n");						    \
    }									    \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;

#else /* !YYDEBUG */

# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)

#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYMAXDEPTH * sizeof (GLRStackItem)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

/* Minimum number of free items on the stack allowed after an
   allocation.  This is to allow allocation and initialization
   to be completed by functions that call yyexpandGLRStack before the
   stack is expanded, thus insuring that all necessary pointers get
   properly redirected to new data.  */
#define YYHEADROOM 2

#ifndef YYSTACKEXPANDABLE
# if (! defined __cplusplus \
      || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	  && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL))
#  define YYSTACKEXPANDABLE 1
# else
#  define YYSTACKEXPANDABLE 0
# endif
#endif

#if YYSTACKEXPANDABLE
# define YY_RESERVE_GLRSTACK(Yystack)			\
  do {							\
    if (Yystack->yyspaceLeft < YYHEADROOM)		\
      yyexpandGLRStack (Yystack);			\
  } while (YYID (0))
#else
# define YY_RESERVE_GLRSTACK(Yystack)			\
  do {							\
    if (Yystack->yyspaceLeft < YYHEADROOM)		\
      yyMemoryExhausted (Yystack);			\
  } while (YYID (0))
#endif


#if YYERROR_VERBOSE

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static size_t
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return strlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* !YYERROR_VERBOSE */

/** State numbers, as in LALR(1) machine */
typedef int yyStateNum;

/** Rule numbers, as in LALR(1) machine */
typedef int yyRuleNum;

/** Grammar symbol */
typedef short int yySymbol;

/** Item references, as in LALR(1) machine */
typedef short int yyItemNum;

typedef struct yyGLRState yyGLRState;
typedef struct yyGLRStateSet yyGLRStateSet;
typedef struct yySemanticOption yySemanticOption;
typedef union yyGLRStackItem yyGLRStackItem;
typedef struct yyGLRStack yyGLRStack;

struct yyGLRState {
  /** Type tag: always true.  */
  yybool yyisState;
  /** Type tag for yysemantics.  If true, yysval applies, otherwise
   *  yyfirstVal applies.  */
  yybool yyresolved;
  /** Number of corresponding LALR(1) machine state.  */
  yyStateNum yylrState;
  /** Preceding state in this stack */
  yyGLRState* yypred;
  /** Source position of the first token produced by my symbol */
  size_t yyposn;
  union {
    /** First in a chain of alternative reductions producing the
     *  non-terminal corresponding to this state, threaded through
     *  yynext.  */
    yySemanticOption* yyfirstVal;
    /** Semantic value for this state.  */
    YYSTYPE yysval;
  } yysemantics;
  /** Source location for this state.  */
  YYLTYPE yyloc;
};

struct yyGLRStateSet {
  yyGLRState** yystates;
  /** During nondeterministic operation, yylookaheadNeeds tracks which
   *  stacks have actually needed the current lookahead.  During deterministic
   *  operation, yylookaheadNeeds[0] is not maintained since it would merely
   *  duplicate yychar != YYEMPTY.  */
  yybool* yylookaheadNeeds;
  size_t yysize, yycapacity;
};

struct yySemanticOption {
  /** Type tag: always false.  */
  yybool yyisState;
  /** Rule number for this reduction */
  yyRuleNum yyrule;
  /** The last RHS state in the list of states to be reduced.  */
  yyGLRState* yystate;
  /** The lookahead for this reduction.  */
  int yyrawchar;
  YYSTYPE yyval;
  YYLTYPE yyloc;
  /** Next sibling in chain of options.  To facilitate merging,
   *  options are chained in decreasing order by address.  */
  yySemanticOption* yynext;
};

/** Type of the items in the GLR stack.  The yyisState field
 *  indicates which item of the union is valid.  */
union yyGLRStackItem {
  yyGLRState yystate;
  yySemanticOption yyoption;
};

struct yyGLRStack {
  int yyerrState;
  /* To compute the location of the error token.  */
  yyGLRStackItem yyerror_range[3];

  int yyerrcnt;
  int yyrawchar;
  YYSTYPE yyval;
  YYLTYPE yyloc;

  YYJMP_BUF yyexception_buffer;
  yyGLRStackItem* yyitems;
  yyGLRStackItem* yynextFree;
  size_t yyspaceLeft;
  yyGLRState* yysplitPoint;
  yyGLRState* yylastDeleted;
  yyGLRStateSet yytops;
};

#if YYSTACKEXPANDABLE
static void yyexpandGLRStack (yyGLRStack* yystackp);
#endif

static void yyFail (yyGLRStack* yystackp, YYLTYPE *yylocp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts, const char* yymsg)
  __attribute__ ((__noreturn__));
static void
yyFail (yyGLRStack* yystackp, YYLTYPE *yylocp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts, const char* yymsg)
{
  if (yymsg != NULL)
    yyerror (yylocp, yyscanner, pcode, o, err, contexts, yymsg);
  YYLONGJMP (yystackp->yyexception_buffer, 1);
}

static void yyMemoryExhausted (yyGLRStack* yystackp)
  __attribute__ ((__noreturn__));
static void
yyMemoryExhausted (yyGLRStack* yystackp)
{
  YYLONGJMP (yystackp->yyexception_buffer, 2);
}

#if YYDEBUG || YYERROR_VERBOSE
/** A printable representation of TOKEN.  */
static inline const char*
yytokenName (yySymbol yytoken)
{
  if (yytoken == YYEMPTY)
    return "";

  return yytname[yytoken];
}
#endif

/** Fill in YYVSP[YYLOW1 .. YYLOW0-1] from the chain of states starting
 *  at YYVSP[YYLOW0].yystate.yypred.  Leaves YYVSP[YYLOW1].yystate.yypred
 *  containing the pointer to the next state in the chain.  */
static void yyfillin (yyGLRStackItem *, int, int) __attribute__ ((__unused__));
static void
yyfillin (yyGLRStackItem *yyvsp, int yylow0, int yylow1)
{
  yyGLRState* s;
  int i;
  s = yyvsp[yylow0].yystate.yypred;
  for (i = yylow0-1; i >= yylow1; i -= 1)
    {
      YYASSERT (s->yyresolved);
      yyvsp[i].yystate.yyresolved = yytrue;
      yyvsp[i].yystate.yysemantics.yysval = s->yysemantics.yysval;
      yyvsp[i].yystate.yyloc = s->yyloc;
      s = yyvsp[i].yystate.yypred = s->yypred;
    }
}

/* Do nothing if YYNORMAL or if *YYLOW <= YYLOW1.  Otherwise, fill in
 * YYVSP[YYLOW1 .. *YYLOW-1] as in yyfillin and set *YYLOW = YYLOW1.
 * For convenience, always return YYLOW1.  */
static inline int yyfill (yyGLRStackItem *, int *, int, yybool)
     __attribute__ ((__unused__));
static inline int
yyfill (yyGLRStackItem *yyvsp, int *yylow, int yylow1, yybool yynormal)
{
  if (!yynormal && yylow1 < *yylow)
    {
      yyfillin (yyvsp, *yylow, yylow1);
      *yylow = yylow1;
    }
  return yylow1;
}

/** Perform user action for rule number YYN, with RHS length YYRHSLEN,
 *  and top stack item YYVSP.  YYLVALP points to place to put semantic
 *  value ($$), and yylocp points to place for location information
 *  (@$).  Returns yyok for normal return, yyaccept for YYACCEPT,
 *  yyerr for YYERROR, yyabort for YYABORT.  */
/*ARGSUSED*/ static YYRESULTTAG
yyuserAction (yyRuleNum yyn, int yyrhslen, yyGLRStackItem* yyvsp,
	      YYSTYPE* yyvalp,
	      YYLTYPE* YYOPTIONAL_LOC (yylocp),
	      yyGLRStack* yystackp
	      , yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  yybool yynormal __attribute__ ((__unused__)) =
    (yystackp->yysplitPoint == NULL);
  int yylow;
  YYUSE (yyscanner);
  YYUSE (pcode);
  YYUSE (o);
  YYUSE (err);
  YYUSE (contexts);
# undef yyerrok
# define yyerrok (yystackp->yyerrState = 0)
# undef YYACCEPT
# define YYACCEPT return yyaccept
# undef YYABORT
# define YYABORT return yyabort
# undef YYERROR
# define YYERROR return yyerrok, yyerr
# undef YYRECOVERING
# define YYRECOVERING() (yystackp->yyerrState != 0)
# undef yyclearin
# define yyclearin (yychar = YYEMPTY)
# undef YYFILL
# define YYFILL(N) yyfill (yyvsp, &yylow, N, yynormal)
# undef YYBACKUP
# define YYBACKUP(Token, Value)						     \
  return yyerror (yylocp, yyscanner, pcode, o, err, contexts, YY_("syntax error: cannot back up")),     \
	 yyerrok, yyerr

  yylow = 1;
  if (yyrhslen == 0)
    *yyvalp = yyval_default;
  else
    *yyvalp = yyvsp[YYFILL (1-yyrhslen)].yystate.yysemantics.yysval;
  YYLLOC_DEFAULT ((*yylocp), (yyvsp - yyrhslen), yyrhslen);
  yystackp->yyerror_range[1].yystate.yyloc = *yylocp;

  switch (yyn)
    {
        case 2:

/* Line 936 of glr.c  */
// #line 371 "./parser.y"
    { g_queue_init(contexts); PUSH_CONTEXT; ;}
    break;

  case 4:

/* Line 936 of glr.c  */
// #line 372 "./parser.y"
    { g_string_assign(pcode, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 5:

/* Line 936 of glr.c  */
// #line 373 "./parser.y"
    { KURE_YYERROR((*yylocp), "... in this expression."); YYERROR; ;}
    break;

  case 6:

/* Line 936 of glr.c  */
// #line 375 "./parser.y"
    { g_string_append (pcode, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.code)); g_free((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.code));  ;}
    break;

  case 7:

/* Line 936 of glr.c  */
// #line 376 "./parser.y"
    {  g_string_append (pcode, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.code)); g_free((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 8:

/* Line 936 of glr.c  */
// #line 380 "./parser.y"
    { ((*yyvalp).code)=(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.code); if(o && o->onFunction) o->onFunction(o->object, ((*yyvalp).code), &(*yylocp)); ;}
    break;

  case 9:

/* Line 936 of glr.c  */
// #line 381 "./parser.y"
    { ((*yyvalp).code)=(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.code); if(o && o->onProgram) o->onProgram(o->object, ((*yyvalp).code), &(*yylocp)); ;}
    break;

  case 10:

/* Line 936 of glr.c  */
// #line 386 "./parser.y"
    { ((*yyvalp).code) = CONV ("\tif (kure.is_true(%s))\tthen\t%s\tend", (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 11:

/* Line 936 of glr.c  */
// #line 387 "./parser.y"
    { ((*yyvalp).code) = CONV ("\tif (kure.is_true(%s))\tthen\t%s\telse %s\tend", (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (7))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 12:

/* Line 936 of glr.c  */
// #line 388 "./parser.y"
    { ((*yyvalp).code) = CONV("\twhile (kure.is_true(%s)) do\t%s\tend", (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 13:

/* Line 936 of glr.c  */
// #line 389 "./parser.y"
    { ((*yyvalp).code) = CONV("\tkure.compat.assert(\"%s\",%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 14:

/* Line 936 of glr.c  */
// #line 393 "./parser.y"
    { if( !_is_declared(CONTEXT, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.code), VAR))
      { KURE_YYERROR((*yylocp), "Left hand side \"%s\" in assignment is not a local variable.", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.code)); YYERROR; } ;}
    break;

  case 15:

/* Line 936 of glr.c  */
// #line 395 "./parser.y"
    { ((*yyvalp).code) = CONV("\t%s = %s", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 16:

/* Line 936 of glr.c  */
// #line 397 "./parser.y"
    { ((*yyvalp).code) = g_strdup_printf (" "LINE_TRACKING_VAR"=%d; %s", (*yylocp).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.code)); g_free ((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 17:

/* Line 936 of glr.c  */
// #line 398 "./parser.y"
    { ((*yyvalp).code) = g_strdup_printf (" "LINE_TRACKING_VAR"=%d; %s", (*yylocp).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.code)); g_free ((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 18:

/* Line 936 of glr.c  */
// #line 401 "./parser.y"
    { ((*yyvalp).code)=g_strdup(""); ;}
    break;

  case 19:

/* Line 936 of glr.c  */
// #line 402 "./parser.y"
    {  ((*yyvalp).code) = CONV("\t%s\n%s\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 20:

/* Line 936 of glr.c  */
// #line 404 "./parser.y"
    {
  /* Check if the identifier is a RelView reserved function name. */
  if (_is_reserved_fun ((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.code))) {
    KURE_YYERROR((*yylocp), "Identifier \"%s\" in declarator is a reserved function name.", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.code));
    _g_free_all ((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.code), NULL);
    YYERROR;
  }
  else  { ((*yyvalp).sig).name = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.code); ((*yyvalp).sig).params = (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.code); }
  ;}
    break;

  case 23:

/* Line 936 of glr.c  */
// #line 417 "./parser.y"
    {
	/* Check arguments and throw an error if at least one argument is missing. */
	gchar * s = _check_args_code ((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.sig).name, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.sig).params);
	gchar * line = g_strdup_printf ("%d", (*yylocp).first_line);
	((*yyvalp).code) = CONV("function %s (%s) local "LINE_TRACKING_VAR"=%s\n%s return %s end\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.sig).name, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.sig).params, line, s, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code));
;}
    break;

  case 24:

/* Line 936 of glr.c  */
// #line 423 "./parser.y"
    { KURE_YYERROR((*yylocp), "... in the expression of function \"%s\".", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.sig).name); g_free((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.sig).params); YYERROR; ;}
    break;

  case 25:

/* Line 936 of glr.c  */
// #line 427 "./parser.y"
    { ((*yyvalp).code) = CONV("%s\n\treturn %s\nend\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 26:

/* Line 936 of glr.c  */
// #line 428 "./parser.y"
    { KURE_YYERROR((*yylocp), "... in the statement(s) ..."); YYERROR;;}
    break;

  case 27:

/* Line 936 of glr.c  */
// #line 429 "./parser.y"
    { KURE_YYERROR((*yylocp), "... in the RETURN expression ..."); YYERROR; ;}
    break;

  case 28:

/* Line 936 of glr.c  */
// #line 431 "./parser.y"
    {
	/* Check arguments and throw an error if at least one argument is missing. */
	gchar * s = _check_args_code ((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.sig).name, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.sig).params);
	gchar * line = g_strdup_printf ("%d", (*yylocp).first_line);
	((*yyvalp).code) = CONV("function %s (%s) local "LINE_TRACKING_VAR"=%d %s %s\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.sig).name, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.sig).params, line, s, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.code));
;}
    break;

  case 29:

/* Line 936 of glr.c  */
// #line 437 "./parser.y"
    { PUSH_CONTEXT ;}
    break;

  case 30:

/* Line 936 of glr.c  */
// #line 437 "./parser.y"
    {
	/* Check arguments and throw an error if at least one argument is missing. */
	gchar * s = _check_args_code ((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.sig).name, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.sig).params);
	gchar * line = g_strdup_printf ("%d", (*yylocp).first_line);
	((*yyvalp).code) = CONV("function %s (%s)\nlocal "LINE_TRACKING_VAR"=%s\n%s\n%s\n%s\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.sig).name,(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.sig).params,line,s,(((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.code)); POP_CONTEXT
;}
    break;

  case 31:

/* Line 936 of glr.c  */
// #line 443 "./parser.y"
    { PUSH_CONTEXT ;}
    break;

  case 32:

/* Line 936 of glr.c  */
// #line 443 "./parser.y"
    { KURE_YYERROR((*yylocp), "... in the DECL part of program \"%s\".", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.sig).name); g_free((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.sig).params); POP_CONTEXT; YYERROR; ;}
    break;

  case 33:

/* Line 936 of glr.c  */
// #line 444 "./parser.y"
    { KURE_YYERROR((*yylocp), "... of program \"%s\".", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.sig).name); g_free((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.sig).params); YYERROR; ;}
    break;

  case 34:

/* Line 936 of glr.c  */
// #line 445 "./parser.y"
    { PUSH_CONTEXT ;}
    break;

  case 35:

/* Line 936 of glr.c  */
// #line 445 "./parser.y"
    { KURE_YYERROR((*yylocp), "... of program \"%s\".", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.sig).name); g_free((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.sig).params); POP_CONTEXT; YYERROR; ;}
    break;

  case 38:

/* Line 936 of glr.c  */
// #line 450 "./parser.y"
    { ((*yyvalp).code) = strdup(""); ;}
    break;

  case 41:

/* Line 936 of glr.c  */
// #line 454 "./parser.y"
    { ((*yyvalp).code) = CONV("%s, %s", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 43:

/* Line 936 of glr.c  */
// #line 458 "./parser.y"
    { ((*yyvalp).code) = CONV("(%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 45:

/* Line 936 of glr.c  */
// #line 461 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.transpose(%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 46:

/* Line 936 of glr.c  */
// #line 462 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.mult_norm_transp(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 47:

/* Line 936 of glr.c  */
// #line 463 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.mult_transp_norm(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 48:

/* Line 936 of glr.c  */
// #line 464 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.mult_transp_transp(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 49:

/* Line 936 of glr.c  */
// #line 465 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.mult(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 50:

/* Line 936 of glr.c  */
// #line 468 "./parser.y"
    { ((*yyvalp).code)=CONV("(%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 51:

/* Line 936 of glr.c  */
// #line 469 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.right_tupling(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 52:

/* Line 936 of glr.c  */
// #line 470 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.left_tupling(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 53:

/* Line 936 of glr.c  */
// #line 471 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.tupling(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 54:

/* Line 936 of glr.c  */
// #line 472 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.land(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 55:

/* Line 936 of glr.c  */
// #line 473 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.lor(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 56:

/* Line 936 of glr.c  */
// #line 474 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.right_residue(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 57:

/* Line 936 of glr.c  */
// #line 475 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.left_residue(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 58:

/* Line 936 of glr.c  */
// #line 476 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.direct_sum(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.code), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 59:

/* Line 936 of glr.c  */
// #line 477 "./parser.y"
    { ((*yyvalp).code)=CONV("kure.complement(%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.code)) ;}
    break;

  case 62:

/* Line 936 of glr.c  */
// #line 482 "./parser.y"
    {
  const fun_map_t * ent = _lookup_reserved_fun ((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.code));
  if (ent) {
    if ((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.args).n != ent->arg_count) {
      KURE_YYERROR((*yylocp), "Reserved function \"%s\" expects %d argument(s). Got %d.",
		   (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.code), ent->arg_count, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.args).n);
      _g_free_all ((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.args).s,NULL);
      YYERROR;
    }
    else {
      ((*yyvalp).code)=g_strdup_printf("%s(%s)", ent->transl_name,(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.args).s);
      _g_free_all ((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.args).s,NULL);
    }
  }
  else /* Non reserved function, number of args unknown, or
        * something like "random20". */ {
    gchar * mapped = map_fun ((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.code));
    ((*yyvalp).code) = CONV("%s(%s)", mapped, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.args).s);
  }
  ;}
    break;

  case 63:

/* Line 936 of glr.c  */
// #line 503 "./parser.y"
    { ((*yyvalp).args).n = 0; ((*yyvalp).args).s = g_strdup (""); ;}
    break;

  case 65:

/* Line 936 of glr.c  */
// #line 506 "./parser.y"
    { ((*yyvalp).args).n = 1, ((*yyvalp).args).s = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.code); ;}
    break;

  case 66:

/* Line 936 of glr.c  */
// #line 507 "./parser.y"
    { ((*yyvalp).args).n = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.args).n + 1;
    ((*yyvalp).args).s = g_strconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.args).s, ", ", (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code), NULL);
    _g_free_all((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.args).s, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code), NULL);
  ;}
    break;

  case 69:

/* Line 936 of glr.c  */
// #line 514 "./parser.y"
    { ((*yyvalp).code) = CONV("%s\n%s", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 70:

/* Line 936 of glr.c  */
// #line 515 "./parser.y"
    { ((*yyvalp).code) = CONV("%s\n%s", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 72:

/* Line 936 of glr.c  */
// #line 518 "./parser.y"
    { ((*yyvalp).code) = CONV("%s\n%s", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 73:

/* Line 936 of glr.c  */
// #line 520 "./parser.y"
    { ((*yyvalp).code) = CONV("\tlocal %s", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 77:

/* Line 936 of glr.c  */
// #line 526 "./parser.y"
    { DECLARE((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.code), VAR); ((*yyvalp).code)=(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.code); ;}
    break;

  case 78:

/* Line 936 of glr.c  */
// #line 530 "./parser.y"
    {
	 /* Check arguments and throw an error if at least one argument is missing. */
	gchar * s = _check_args_code ((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.sig).name, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.sig).params);

	DECLARE((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.sig).name, FUN);
	((*yyvalp).code) = CONV("%s = function (%s) %s return %s end", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.sig).name,(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.sig).params,s,(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.code));
;}
    break;

  case 79:

/* Line 936 of glr.c  */
// #line 537 "./parser.y"
    { DECLARE((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yysemantics.yysval.code), DOM); ((*yyvalp).code) = CONV("%s = kure.direct_product_new(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.code)); ;}
    break;

  case 80:

/* Line 936 of glr.c  */
// #line 538 "./parser.y"
    { DECLARE((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yysemantics.yysval.code), DOM); ((*yyvalp).code) = CONV("%s = kure.direct_sum_new(%s,%s)", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.code),(((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.code)); ;}
    break;



/* Line 936 of glr.c  */
// #line 1728 "parser.c"
      default: break;
    }

  return yyok;
# undef yyerrok
# undef YYABORT
# undef YYACCEPT
# undef YYERROR
# undef YYBACKUP
# undef yyclearin
# undef YYRECOVERING
}


/*ARGSUSED*/ static void
yyuserMerge (int yyn, YYSTYPE* yy0, YYSTYPE* yy1)
{
  YYUSE (yy0);
  YYUSE (yy1);

  switch (yyn)
    {

      default: break;
    }
}

			      /* Bison grammar-table manipulation.  */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (yyscanner);
  YYUSE (pcode);
  YYUSE (o);
  YYUSE (err);
  YYUSE (contexts);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 29: /* "START_DEFAULT" */

/* Line 966 of glr.c  */
// #line 361 "./parser.y"
	{ /* Code */ };

/* Line 966 of glr.c  */
// #line 1787 "parser.c"
	break;

      default:
	break;
    }
}

/** Number of symbols composing the right hand side of rule #RULE.  */
static inline int
yyrhsLength (yyRuleNum yyrule)
{
  return yyr2[yyrule];
}

static void
yydestroyGLRState (char const *yymsg, yyGLRState *yys, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  if (yys->yyresolved)
    yydestruct (yymsg, yystos[yys->yylrState],
		&yys->yysemantics.yysval, &yys->yyloc, yyscanner, pcode, o, err, contexts);
  else
    {
#if YYDEBUG
      if (yydebug)
	{
	  if (yys->yysemantics.yyfirstVal)
	    YYFPRINTF (stderr, "%s unresolved ", yymsg);
	  else
	    YYFPRINTF (stderr, "%s incomplete ", yymsg);
	  yy_symbol_print (stderr, yystos[yys->yylrState],
			   NULL, &yys->yyloc, yyscanner, pcode, o, err, contexts);
	  YYFPRINTF (stderr, "\n");
	}
#endif

      if (yys->yysemantics.yyfirstVal)
	{
	  yySemanticOption *yyoption = yys->yysemantics.yyfirstVal;
	  yyGLRState *yyrh;
	  int yyn;
	  for (yyrh = yyoption->yystate, yyn = yyrhsLength (yyoption->yyrule);
	       yyn > 0;
	       yyrh = yyrh->yypred, yyn -= 1)
	    yydestroyGLRState (yymsg, yyrh, yyscanner, pcode, o, err, contexts);
	}
    }
}

/** Left-hand-side symbol for rule #RULE.  */
static inline yySymbol
yylhsNonterm (yyRuleNum yyrule)
{
  return yyr1[yyrule];
}

#define yyis_pact_ninf(yystate) \
  ((yystate) == YYPACT_NINF)

/** True iff LR state STATE has only a default reduction (regardless
 *  of token).  */
static inline yybool
yyisDefaultedState (yyStateNum yystate)
{
  return yyis_pact_ninf (yypact[yystate]);
}

/** The default reduction for STATE, assuming it has one.  */
static inline yyRuleNum
yydefaultAction (yyStateNum yystate)
{
  return yydefact[yystate];
}

#define yyis_table_ninf(yytable_value) \
  YYID (0)

/** Set *YYACTION to the action to take in YYSTATE on seeing YYTOKEN.
 *  Result R means
 *    R < 0:  Reduce on rule -R.
 *    R = 0:  Error.
 *    R > 0:  Shift to state R.
 *  Set *CONFLICTS to a pointer into yyconfl to 0-terminated list of
 *  conflicting reductions.
 */
static inline void
yygetLRActions (yyStateNum yystate, int yytoken,
		int* yyaction, const short int** yyconflicts)
{
  int yyindex = yypact[yystate] + yytoken;
  if (yyindex < 0 || YYLAST < yyindex || yycheck[yyindex] != yytoken)
    {
      *yyaction = -yydefact[yystate];
      *yyconflicts = yyconfl;
    }
  else if (! yyis_table_ninf (yytable[yyindex]))
    {
      *yyaction = yytable[yyindex];
      *yyconflicts = yyconfl + yyconflp[yyindex];
    }
  else
    {
      *yyaction = 0;
      *yyconflicts = yyconfl + yyconflp[yyindex];
    }
}

static inline yyStateNum
yyLRgotoState (yyStateNum yystate, yySymbol yylhs)
{
  int yyr;
  yyr = yypgoto[yylhs - YYNTOKENS] + yystate;
  if (0 <= yyr && yyr <= YYLAST && yycheck[yyr] == yystate)
    return yytable[yyr];
  else
    return yydefgoto[yylhs - YYNTOKENS];
}

static inline yybool
yyisShiftAction (int yyaction)
{
  return 0 < yyaction;
}

static inline yybool
yyisErrorAction (int yyaction)
{
  return yyaction == 0;
}

				/* GLRStates */

/** Return a fresh GLRStackItem.  Callers should call
 * YY_RESERVE_GLRSTACK afterwards to make sure there is sufficient
 * headroom.  */

static inline yyGLRStackItem*
yynewGLRStackItem (yyGLRStack* yystackp, yybool yyisState)
{
  yyGLRStackItem* yynewItem = yystackp->yynextFree;
  yystackp->yyspaceLeft -= 1;
  yystackp->yynextFree += 1;
  yynewItem->yystate.yyisState = yyisState;
  return yynewItem;
}

/** Add a new semantic action that will execute the action for rule
 *  RULENUM on the semantic values in RHS to the list of
 *  alternative actions for STATE.  Assumes that RHS comes from
 *  stack #K of *STACKP. */
static void
yyaddDeferredAction (yyGLRStack* yystackp, size_t yyk, yyGLRState* yystate,
		     yyGLRState* rhs, yyRuleNum yyrule)
{
  yySemanticOption* yynewOption =
    &yynewGLRStackItem (yystackp, yyfalse)->yyoption;
  yynewOption->yystate = rhs;
  yynewOption->yyrule = yyrule;
  if (yystackp->yytops.yylookaheadNeeds[yyk])
    {
      yynewOption->yyrawchar = yychar;
      yynewOption->yyval = yylval;
      yynewOption->yyloc = yylloc;
    }
  else
    yynewOption->yyrawchar = YYEMPTY;
  yynewOption->yynext = yystate->yysemantics.yyfirstVal;
  yystate->yysemantics.yyfirstVal = yynewOption;

  YY_RESERVE_GLRSTACK (yystackp);
}

				/* GLRStacks */

/** Initialize SET to a singleton set containing an empty stack.  */
static yybool
yyinitStateSet (yyGLRStateSet* yyset)
{
  yyset->yysize = 1;
  yyset->yycapacity = 16;
  yyset->yystates = (yyGLRState**) YYMALLOC (16 * sizeof yyset->yystates[0]);
  if (! yyset->yystates)
    return yyfalse;
  yyset->yystates[0] = NULL;
  yyset->yylookaheadNeeds =
    (yybool*) YYMALLOC (16 * sizeof yyset->yylookaheadNeeds[0]);
  if (! yyset->yylookaheadNeeds)
    {
      YYFREE (yyset->yystates);
      return yyfalse;
    }
  return yytrue;
}

static void yyfreeStateSet (yyGLRStateSet* yyset)
{
  YYFREE (yyset->yystates);
  YYFREE (yyset->yylookaheadNeeds);
}

/** Initialize STACK to a single empty stack, with total maximum
 *  capacity for all stacks of SIZE.  */
static yybool
yyinitGLRStack (yyGLRStack* yystackp, size_t yysize)
{
  yystackp->yyerrState = 0;
  yynerrs = 0;
  yystackp->yyspaceLeft = yysize;
  yystackp->yyitems =
    (yyGLRStackItem*) YYMALLOC (yysize * sizeof yystackp->yynextFree[0]);
  if (!yystackp->yyitems)
    return yyfalse;
  yystackp->yynextFree = yystackp->yyitems;
  yystackp->yysplitPoint = NULL;
  yystackp->yylastDeleted = NULL;
  return yyinitStateSet (&yystackp->yytops);
}


#if YYSTACKEXPANDABLE
# define YYRELOC(YYFROMITEMS,YYTOITEMS,YYX,YYTYPE) \
  &((YYTOITEMS) - ((YYFROMITEMS) - (yyGLRStackItem*) (YYX)))->YYTYPE

/** If STACK is expandable, extend it.  WARNING: Pointers into the
    stack from outside should be considered invalid after this call.
    We always expand when there are 1 or fewer items left AFTER an
    allocation, so that we can avoid having external pointers exist
    across an allocation.  */
static void
yyexpandGLRStack (yyGLRStack* yystackp)
{
  yyGLRStackItem* yynewItems;
  yyGLRStackItem* yyp0, *yyp1;
  size_t yysize, yynewSize;
  size_t yyn;
  yysize = yystackp->yynextFree - yystackp->yyitems;
  if (YYMAXDEPTH - YYHEADROOM < yysize)
    yyMemoryExhausted (yystackp);
  yynewSize = 2*yysize;
  if (YYMAXDEPTH < yynewSize)
    yynewSize = YYMAXDEPTH;
  yynewItems = (yyGLRStackItem*) YYMALLOC (yynewSize * sizeof yynewItems[0]);
  if (! yynewItems)
    yyMemoryExhausted (yystackp);
  for (yyp0 = yystackp->yyitems, yyp1 = yynewItems, yyn = yysize;
       0 < yyn;
       yyn -= 1, yyp0 += 1, yyp1 += 1)
    {
      *yyp1 = *yyp0;
      if (*(yybool *) yyp0)
	{
	  yyGLRState* yys0 = &yyp0->yystate;
	  yyGLRState* yys1 = &yyp1->yystate;
	  if (yys0->yypred != NULL)
	    yys1->yypred =
	      YYRELOC (yyp0, yyp1, yys0->yypred, yystate);
	  if (! yys0->yyresolved && yys0->yysemantics.yyfirstVal != NULL)
	    yys1->yysemantics.yyfirstVal =
	      YYRELOC(yyp0, yyp1, yys0->yysemantics.yyfirstVal, yyoption);
	}
      else
	{
	  yySemanticOption* yyv0 = &yyp0->yyoption;
	  yySemanticOption* yyv1 = &yyp1->yyoption;
	  if (yyv0->yystate != NULL)
	    yyv1->yystate = YYRELOC (yyp0, yyp1, yyv0->yystate, yystate);
	  if (yyv0->yynext != NULL)
	    yyv1->yynext = YYRELOC (yyp0, yyp1, yyv0->yynext, yyoption);
	}
    }
  if (yystackp->yysplitPoint != NULL)
    yystackp->yysplitPoint = YYRELOC (yystackp->yyitems, yynewItems,
				 yystackp->yysplitPoint, yystate);

  for (yyn = 0; yyn < yystackp->yytops.yysize; yyn += 1)
    if (yystackp->yytops.yystates[yyn] != NULL)
      yystackp->yytops.yystates[yyn] =
	YYRELOC (yystackp->yyitems, yynewItems,
		 yystackp->yytops.yystates[yyn], yystate);
  YYFREE (yystackp->yyitems);
  yystackp->yyitems = yynewItems;
  yystackp->yynextFree = yynewItems + yysize;
  yystackp->yyspaceLeft = yynewSize - yysize;
}
#endif

static void
yyfreeGLRStack (yyGLRStack* yystackp)
{
  YYFREE (yystackp->yyitems);
  yyfreeStateSet (&yystackp->yytops);
}

/** Assuming that S is a GLRState somewhere on STACK, update the
 *  splitpoint of STACK, if needed, so that it is at least as deep as
 *  S.  */
static inline void
yyupdateSplit (yyGLRStack* yystackp, yyGLRState* yys)
{
  if (yystackp->yysplitPoint != NULL && yystackp->yysplitPoint > yys)
    yystackp->yysplitPoint = yys;
}

/** Invalidate stack #K in STACK.  */
static inline void
yymarkStackDeleted (yyGLRStack* yystackp, size_t yyk)
{
  if (yystackp->yytops.yystates[yyk] != NULL)
    yystackp->yylastDeleted = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yystates[yyk] = NULL;
}

/** Undelete the last stack that was marked as deleted.  Can only be
    done once after a deletion, and only when all other stacks have
    been deleted.  */
static void
yyundeleteLastStack (yyGLRStack* yystackp)
{
  if (yystackp->yylastDeleted == NULL || yystackp->yytops.yysize != 0)
    return;
  yystackp->yytops.yystates[0] = yystackp->yylastDeleted;
  yystackp->yytops.yysize = 1;
  YYDPRINTF ((stderr, "Restoring last deleted stack as stack #0.\n"));
  yystackp->yylastDeleted = NULL;
}

static inline void
yyremoveDeletes (yyGLRStack* yystackp)
{
  size_t yyi, yyj;
  yyi = yyj = 0;
  while (yyj < yystackp->yytops.yysize)
    {
      if (yystackp->yytops.yystates[yyi] == NULL)
	{
	  if (yyi == yyj)
	    {
	      YYDPRINTF ((stderr, "Removing dead stacks.\n"));
	    }
	  yystackp->yytops.yysize -= 1;
	}
      else
	{
	  yystackp->yytops.yystates[yyj] = yystackp->yytops.yystates[yyi];
	  /* In the current implementation, it's unnecessary to copy
	     yystackp->yytops.yylookaheadNeeds[yyi] since, after
	     yyremoveDeletes returns, the parser immediately either enters
	     deterministic operation or shifts a token.  However, it doesn't
	     hurt, and the code might evolve to need it.  */
	  yystackp->yytops.yylookaheadNeeds[yyj] =
	    yystackp->yytops.yylookaheadNeeds[yyi];
	  if (yyj != yyi)
	    {
	      YYDPRINTF ((stderr, "Rename stack %lu -> %lu.\n",
			  (unsigned long int) yyi, (unsigned long int) yyj));
	    }
	  yyj += 1;
	}
      yyi += 1;
    }
}

/** Shift to a new state on stack #K of STACK, corresponding to LR state
 * LRSTATE, at input position POSN, with (resolved) semantic value SVAL.  */
static inline void
yyglrShift (yyGLRStack* yystackp, size_t yyk, yyStateNum yylrState,
	    size_t yyposn,
	    YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yytrue;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yysval = *yyvalp;
  yynewState->yyloc = *yylocp;
  yystackp->yytops.yystates[yyk] = yynewState;

  YY_RESERVE_GLRSTACK (yystackp);
}

/** Shift stack #K of YYSTACK, to a new state corresponding to LR
 *  state YYLRSTATE, at input position YYPOSN, with the (unresolved)
 *  semantic value of YYRHS under the action for YYRULE.  */
static inline void
yyglrShiftDefer (yyGLRStack* yystackp, size_t yyk, yyStateNum yylrState,
		 size_t yyposn, yyGLRState* rhs, yyRuleNum yyrule)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yyfalse;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yyfirstVal = NULL;
  yystackp->yytops.yystates[yyk] = yynewState;

  /* Invokes YY_RESERVE_GLRSTACK.  */
  yyaddDeferredAction (yystackp, yyk, yynewState, rhs, yyrule);
}

/** Pop the symbols consumed by reduction #RULE from the top of stack
 *  #K of STACK, and perform the appropriate semantic action on their
 *  semantic values.  Assumes that all ambiguities in semantic values
 *  have been previously resolved.  Set *VALP to the resulting value,
 *  and *LOCP to the computed location (if any).  Return value is as
 *  for userAction.  */
static inline YYRESULTTAG
yydoAction (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
	    YYSTYPE* yyvalp, YYLTYPE* yylocp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  int yynrhs = yyrhsLength (yyrule);

  if (yystackp->yysplitPoint == NULL)
    {
      /* Standard special case: single stack.  */
      yyGLRStackItem* rhs = (yyGLRStackItem*) yystackp->yytops.yystates[yyk];
      YYASSERT (yyk == 0);
      yystackp->yynextFree -= yynrhs;
      yystackp->yyspaceLeft += yynrhs;
      yystackp->yytops.yystates[0] = & yystackp->yynextFree[-1].yystate;
      return yyuserAction (yyrule, yynrhs, rhs,
			   yyvalp, yylocp, yystackp, yyscanner, pcode, o, err, contexts);
    }
  else
    {
      /* At present, doAction is never called in nondeterministic
       * mode, so this branch is never taken.  It is here in
       * anticipation of a future feature that will allow immediate
       * evaluation of selected actions in nondeterministic mode.  */
      int yyi;
      yyGLRState* yys;
      yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
      yys = yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred
	= yystackp->yytops.yystates[yyk];
      if (yynrhs == 0)
	/* Set default location.  */
	yyrhsVals[YYMAXRHS + YYMAXLEFT - 1].yystate.yyloc = yys->yyloc;
      for (yyi = 0; yyi < yynrhs; yyi += 1)
	{
	  yys = yys->yypred;
	  YYASSERT (yys);
	}
      yyupdateSplit (yystackp, yys);
      yystackp->yytops.yystates[yyk] = yys;
      return yyuserAction (yyrule, yynrhs, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
			   yyvalp, yylocp, yystackp, yyscanner, pcode, o, err, contexts);
    }
}

#if !YYDEBUG
# define YY_REDUCE_PRINT(Args)
#else
# define YY_REDUCE_PRINT(Args)		\
do {					\
  if (yydebug)				\
    yy_reduce_print Args;		\
} while (YYID (0))

/*----------------------------------------------------------.
| Report that the RULE is going to be reduced on stack #K.  |
`----------------------------------------------------------*/

/*ARGSUSED*/ static inline void
yy_reduce_print (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
		 YYSTYPE* yyvalp, YYLTYPE* yylocp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  int yynrhs = yyrhsLength (yyrule);
  yybool yynormal __attribute__ ((__unused__)) =
    (yystackp->yysplitPoint == NULL);
  yyGLRStackItem* yyvsp = (yyGLRStackItem*) yystackp->yytops.yystates[yyk];
  int yylow = 1;
  int yyi;
  YYUSE (yyvalp);
  YYUSE (yylocp);
  YYUSE (yyscanner);
  YYUSE (pcode);
  YYUSE (o);
  YYUSE (err);
  YYUSE (contexts);
  YYFPRINTF (stderr, "Reducing stack %lu by rule %d (line %lu):\n",
	     (unsigned long int) yyk, yyrule - 1,
	     (unsigned long int) yyrline[yyrule]);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(((yyGLRStackItem const *)yyvsp)[YYFILL ((yyi + 1) - (yynrhs))].yystate.yysemantics.yysval)
		       , &(((yyGLRStackItem const *)yyvsp)[YYFILL ((yyi + 1) - (yynrhs))].yystate.yyloc)		       , yyscanner, pcode, o, err, contexts);
      YYFPRINTF (stderr, "\n");
    }
}
#endif

/** Pop items off stack #K of STACK according to grammar rule RULE,
 *  and push back on the resulting nonterminal symbol.  Perform the
 *  semantic action associated with RULE and store its value with the
 *  newly pushed state, if FORCEEVAL or if STACK is currently
 *  unambiguous.  Otherwise, store the deferred semantic action with
 *  the new state.  If the new state would have an identical input
 *  position, LR state, and predecessor to an existing state on the stack,
 *  it is identified with that existing state, eliminating stack #K from
 *  the STACK.  In this case, the (necessarily deferred) semantic value is
 *  added to the options for the existing state's semantic value.
 */
static inline YYRESULTTAG
yyglrReduce (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
	     yybool yyforceEval, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  size_t yyposn = yystackp->yytops.yystates[yyk]->yyposn;

  if (yyforceEval || yystackp->yysplitPoint == NULL)
    {
      YYSTYPE yysval;
      YYLTYPE yyloc;

      YY_REDUCE_PRINT ((yystackp, yyk, yyrule, &yysval, &yyloc, yyscanner, pcode, o, err, contexts));
      YYCHK (yydoAction (yystackp, yyk, yyrule, &yysval,
			 &yyloc, yyscanner, pcode, o, err, contexts));
      YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyrule], &yysval, &yyloc);
      yyglrShift (yystackp, yyk,
		  yyLRgotoState (yystackp->yytops.yystates[yyk]->yylrState,
				 yylhsNonterm (yyrule)),
		  yyposn, &yysval, &yyloc);
    }
  else
    {
      size_t yyi;
      int yyn;
      yyGLRState* yys, *yys0 = yystackp->yytops.yystates[yyk];
      yyStateNum yynewLRState;

      for (yys = yystackp->yytops.yystates[yyk], yyn = yyrhsLength (yyrule);
	   0 < yyn; yyn -= 1)
	{
	  yys = yys->yypred;
	  YYASSERT (yys);
	}
      yyupdateSplit (yystackp, yys);
      yynewLRState = yyLRgotoState (yys->yylrState, yylhsNonterm (yyrule));
      YYDPRINTF ((stderr,
		  "Reduced stack %lu by rule #%d; action deferred.  Now in state %d.\n",
		  (unsigned long int) yyk, yyrule - 1, yynewLRState));
      for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
	if (yyi != yyk && yystackp->yytops.yystates[yyi] != NULL)
	  {
	    yyGLRState* yyp, *yysplit = yystackp->yysplitPoint;
	    yyp = yystackp->yytops.yystates[yyi];
	    while (yyp != yys && yyp != yysplit && yyp->yyposn >= yyposn)
	      {
		if (yyp->yylrState == yynewLRState && yyp->yypred == yys)
		  {
		    yyaddDeferredAction (yystackp, yyk, yyp, yys0, yyrule);
		    yymarkStackDeleted (yystackp, yyk);
		    YYDPRINTF ((stderr, "Merging stack %lu into stack %lu.\n",
				(unsigned long int) yyk,
				(unsigned long int) yyi));
		    return yyok;
		  }
		yyp = yyp->yypred;
	      }
	  }
      yystackp->yytops.yystates[yyk] = yys;
      yyglrShiftDefer (yystackp, yyk, yynewLRState, yyposn, yys0, yyrule);
    }
  return yyok;
}

static size_t
yysplitStack (yyGLRStack* yystackp, size_t yyk)
{
  if (yystackp->yysplitPoint == NULL)
    {
      YYASSERT (yyk == 0);
      yystackp->yysplitPoint = yystackp->yytops.yystates[yyk];
    }
  if (yystackp->yytops.yysize >= yystackp->yytops.yycapacity)
    {
      yyGLRState** yynewStates;
      yybool* yynewLookaheadNeeds;

      yynewStates = NULL;

      if (yystackp->yytops.yycapacity
	  > (YYSIZEMAX / (2 * sizeof yynewStates[0])))
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yycapacity *= 2;

      yynewStates =
	(yyGLRState**) YYREALLOC (yystackp->yytops.yystates,
				  (yystackp->yytops.yycapacity
				   * sizeof yynewStates[0]));
      if (yynewStates == NULL)
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yystates = yynewStates;

      yynewLookaheadNeeds =
	(yybool*) YYREALLOC (yystackp->yytops.yylookaheadNeeds,
			     (yystackp->yytops.yycapacity
			      * sizeof yynewLookaheadNeeds[0]));
      if (yynewLookaheadNeeds == NULL)
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yylookaheadNeeds = yynewLookaheadNeeds;
    }
  yystackp->yytops.yystates[yystackp->yytops.yysize]
    = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yylookaheadNeeds[yystackp->yytops.yysize]
    = yystackp->yytops.yylookaheadNeeds[yyk];
  yystackp->yytops.yysize += 1;
  return yystackp->yytops.yysize-1;
}

/** True iff Y0 and Y1 represent identical options at the top level.
 *  That is, they represent the same rule applied to RHS symbols
 *  that produce the same terminal symbols.  */
static yybool
yyidenticalOptions (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  if (yyy0->yyrule == yyy1->yyrule)
    {
      yyGLRState *yys0, *yys1;
      int yyn;
      for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
	   yyn = yyrhsLength (yyy0->yyrule);
	   yyn > 0;
	   yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
	if (yys0->yyposn != yys1->yyposn)
	  return yyfalse;
      return yytrue;
    }
  else
    return yyfalse;
}

/** Assuming identicalOptions (Y0,Y1), destructively merge the
 *  alternative semantic values for the RHS-symbols of Y1 and Y0.  */
static void
yymergeOptionSets (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  yyGLRState *yys0, *yys1;
  int yyn;
  for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
       yyn = yyrhsLength (yyy0->yyrule);
       yyn > 0;
       yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
    {
      if (yys0 == yys1)
	break;
      else if (yys0->yyresolved)
	{
	  yys1->yyresolved = yytrue;
	  yys1->yysemantics.yysval = yys0->yysemantics.yysval;
	}
      else if (yys1->yyresolved)
	{
	  yys0->yyresolved = yytrue;
	  yys0->yysemantics.yysval = yys1->yysemantics.yysval;
	}
      else
	{
	  yySemanticOption** yyz0p;
	  yySemanticOption* yyz1;
	  yyz0p = &yys0->yysemantics.yyfirstVal;
	  yyz1 = yys1->yysemantics.yyfirstVal;
	  while (YYID (yytrue))
	    {
	      if (yyz1 == *yyz0p || yyz1 == NULL)
		break;
	      else if (*yyz0p == NULL)
		{
		  *yyz0p = yyz1;
		  break;
		}
	      else if (*yyz0p < yyz1)
		{
		  yySemanticOption* yyz = *yyz0p;
		  *yyz0p = yyz1;
		  yyz1 = yyz1->yynext;
		  (*yyz0p)->yynext = yyz;
		}
	      yyz0p = &(*yyz0p)->yynext;
	    }
	  yys1->yysemantics.yyfirstVal = yys0->yysemantics.yyfirstVal;
	}
    }
}

/** Y0 and Y1 represent two possible actions to take in a given
 *  parsing state; return 0 if no combination is possible,
 *  1 if user-mergeable, 2 if Y0 is preferred, 3 if Y1 is preferred.  */
static int
yypreference (yySemanticOption* y0, yySemanticOption* y1)
{
  yyRuleNum r0 = y0->yyrule, r1 = y1->yyrule;
  int p0 = yydprec[r0], p1 = yydprec[r1];

  if (p0 == p1)
    {
      if (yymerger[r0] == 0 || yymerger[r0] != yymerger[r1])
	return 0;
      else
	return 1;
    }
  if (p0 == 0 || p1 == 0)
    return 0;
  if (p0 < p1)
    return 3;
  if (p1 < p0)
    return 2;
  return 0;
}

static YYRESULTTAG yyresolveValue (yyGLRState* yys,
				   yyGLRStack* yystackp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts);


/** Resolve the previous N states starting at and including state S.  If result
 *  != yyok, some states may have been left unresolved possibly with empty
 *  semantic option chains.  Regardless of whether result = yyok, each state
 *  has been left with consistent data so that yydestroyGLRState can be invoked
 *  if necessary.  */
static YYRESULTTAG
yyresolveStates (yyGLRState* yys, int yyn,
		 yyGLRStack* yystackp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  if (0 < yyn)
    {
      YYASSERT (yys->yypred);
      YYCHK (yyresolveStates (yys->yypred, yyn-1, yystackp, yyscanner, pcode, o, err, contexts));
      if (! yys->yyresolved)
	YYCHK (yyresolveValue (yys, yystackp, yyscanner, pcode, o, err, contexts));
    }
  return yyok;
}

/** Resolve the states for the RHS of OPT, perform its user action, and return
 *  the semantic value and location.  Regardless of whether result = yyok, all
 *  RHS states have been destroyed (assuming the user action destroys all RHS
 *  semantic values if invoked).  */
static YYRESULTTAG
yyresolveAction (yySemanticOption* yyopt, yyGLRStack* yystackp,
		 YYSTYPE* yyvalp, YYLTYPE* yylocp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
  int yynrhs;
  int yychar_current;
  YYSTYPE yylval_current;
  YYLTYPE yylloc_current;
  YYRESULTTAG yyflag;

  yynrhs = yyrhsLength (yyopt->yyrule);
  yyflag = yyresolveStates (yyopt->yystate, yynrhs, yystackp, yyscanner, pcode, o, err, contexts);
  if (yyflag != yyok)
    {
      yyGLRState *yys;
      for (yys = yyopt->yystate; yynrhs > 0; yys = yys->yypred, yynrhs -= 1)
	yydestroyGLRState ("Cleanup: popping", yys, yyscanner, pcode, o, err, contexts);
      return yyflag;
    }

  yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred = yyopt->yystate;
  if (yynrhs == 0)
    /* Set default location.  */
    yyrhsVals[YYMAXRHS + YYMAXLEFT - 1].yystate.yyloc = yyopt->yystate->yyloc;
  yychar_current = yychar;
  yylval_current = yylval;
  yylloc_current = yylloc;
  yychar = yyopt->yyrawchar;
  yylval = yyopt->yyval;
  yylloc = yyopt->yyloc;
  yyflag = yyuserAction (yyopt->yyrule, yynrhs,
			   yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
			   yyvalp, yylocp, yystackp, yyscanner, pcode, o, err, contexts);
  yychar = yychar_current;
  yylval = yylval_current;
  yylloc = yylloc_current;
  return yyflag;
}

#if YYDEBUG
static void
yyreportTree (yySemanticOption* yyx, int yyindent)
{
  int yynrhs = yyrhsLength (yyx->yyrule);
  int yyi;
  yyGLRState* yys;
  yyGLRState* yystates[1 + YYMAXRHS];
  yyGLRState yyleftmost_state;

  for (yyi = yynrhs, yys = yyx->yystate; 0 < yyi; yyi -= 1, yys = yys->yypred)
    yystates[yyi] = yys;
  if (yys == NULL)
    {
      yyleftmost_state.yyposn = 0;
      yystates[0] = &yyleftmost_state;
    }
  else
    yystates[0] = yys;

  if (yyx->yystate->yyposn < yys->yyposn + 1)
    YYFPRINTF (stderr, "%*s%s -> <Rule %d, empty>\n",
	       yyindent, "", yytokenName (yylhsNonterm (yyx->yyrule)),
	       yyx->yyrule - 1);
  else
    YYFPRINTF (stderr, "%*s%s -> <Rule %d, tokens %lu .. %lu>\n",
	       yyindent, "", yytokenName (yylhsNonterm (yyx->yyrule)),
	       yyx->yyrule - 1, (unsigned long int) (yys->yyposn + 1),
	       (unsigned long int) yyx->yystate->yyposn);
  for (yyi = 1; yyi <= yynrhs; yyi += 1)
    {
      if (yystates[yyi]->yyresolved)
	{
	  if (yystates[yyi-1]->yyposn+1 > yystates[yyi]->yyposn)
	    YYFPRINTF (stderr, "%*s%s <empty>\n", yyindent+2, "",
		       yytokenName (yyrhs[yyprhs[yyx->yyrule]+yyi-1]));
	  else
	    YYFPRINTF (stderr, "%*s%s <tokens %lu .. %lu>\n", yyindent+2, "",
		       yytokenName (yyrhs[yyprhs[yyx->yyrule]+yyi-1]),
		       (unsigned long int) (yystates[yyi - 1]->yyposn + 1),
		       (unsigned long int) yystates[yyi]->yyposn);
	}
      else
	yyreportTree (yystates[yyi]->yysemantics.yyfirstVal, yyindent+2);
    }
}
#endif

/*ARGSUSED*/ static YYRESULTTAG
yyreportAmbiguity (yySemanticOption* yyx0,
		   yySemanticOption* yyx1, YYLTYPE *yylocp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  YYUSE (yyx0);
  YYUSE (yyx1);

#if YYDEBUG
  YYFPRINTF (stderr, "Ambiguity detected.\n");
  YYFPRINTF (stderr, "Option 1,\n");
  yyreportTree (yyx0, 2);
  YYFPRINTF (stderr, "\nOption 2,\n");
  yyreportTree (yyx1, 2);
  YYFPRINTF (stderr, "\n");
#endif

  yyerror (yylocp, yyscanner, pcode, o, err, contexts, YY_("syntax is ambiguous"));
  return yyabort;
}

/** Starting at and including state S1, resolve the location for each of the
 *  previous N1 states that is unresolved.  The first semantic option of a state
 *  is always chosen.  */
static void
yyresolveLocations (yyGLRState* yys1, int yyn1,
		    yyGLRStack *yystackp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  if (0 < yyn1)
    {
      yyresolveLocations (yys1->yypred, yyn1 - 1, yystackp, yyscanner, pcode, o, err, contexts);
      if (!yys1->yyresolved)
	{
	  yySemanticOption *yyoption;
	  yyGLRStackItem yyrhsloc[1 + YYMAXRHS];
	  int yynrhs;
	  int yychar_current;
	  YYSTYPE yylval_current;
	  YYLTYPE yylloc_current;
	  yyoption = yys1->yysemantics.yyfirstVal;
	  YYASSERT (yyoption != NULL);
	  yynrhs = yyrhsLength (yyoption->yyrule);
	  if (yynrhs > 0)
	    {
	      yyGLRState *yys;
	      int yyn;
	      yyresolveLocations (yyoption->yystate, yynrhs,
				  yystackp, yyscanner, pcode, o, err, contexts);
	      for (yys = yyoption->yystate, yyn = yynrhs;
		   yyn > 0;
		   yys = yys->yypred, yyn -= 1)
		yyrhsloc[yyn].yystate.yyloc = yys->yyloc;
	    }
	  else
	    {
	      /* Both yyresolveAction and yyresolveLocations traverse the GSS
		 in reverse rightmost order.  It is only necessary to invoke
		 yyresolveLocations on a subforest for which yyresolveAction
		 would have been invoked next had an ambiguity not been
		 detected.  Thus the location of the previous state (but not
		 necessarily the previous state itself) is guaranteed to be
		 resolved already.  */
	      yyGLRState *yyprevious = yyoption->yystate;
	      yyrhsloc[0].yystate.yyloc = yyprevious->yyloc;
	    }
	  yychar_current = yychar;
	  yylval_current = yylval;
	  yylloc_current = yylloc;
	  yychar = yyoption->yyrawchar;
	  yylval = yyoption->yyval;
	  yylloc = yyoption->yyloc;
	  YYLLOC_DEFAULT ((yys1->yyloc), yyrhsloc, yynrhs);
	  yychar = yychar_current;
	  yylval = yylval_current;
	  yylloc = yylloc_current;
	}
    }
}

/** Resolve the ambiguity represented in state S, perform the indicated
 *  actions, and set the semantic value of S.  If result != yyok, the chain of
 *  semantic options in S has been cleared instead or it has been left
 *  unmodified except that redundant options may have been removed.  Regardless
 *  of whether result = yyok, S has been left with consistent data so that
 *  yydestroyGLRState can be invoked if necessary.  */
static YYRESULTTAG
yyresolveValue (yyGLRState* yys, yyGLRStack* yystackp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  yySemanticOption* yyoptionList = yys->yysemantics.yyfirstVal;
  yySemanticOption* yybest;
  yySemanticOption** yypp;
  yybool yymerge;
  YYSTYPE yysval;
  YYRESULTTAG yyflag;
  YYLTYPE *yylocp = &yys->yyloc;

  yybest = yyoptionList;
  yymerge = yyfalse;
  for (yypp = &yyoptionList->yynext; *yypp != NULL; )
    {
      yySemanticOption* yyp = *yypp;

      if (yyidenticalOptions (yybest, yyp))
	{
	  yymergeOptionSets (yybest, yyp);
	  *yypp = yyp->yynext;
	}
      else
	{
	  switch (yypreference (yybest, yyp))
	    {
	    case 0:
	      yyresolveLocations (yys, 1, yystackp, yyscanner, pcode, o, err, contexts);
	      return yyreportAmbiguity (yybest, yyp, yylocp, yyscanner, pcode, o, err, contexts);
	      break;
	    case 1:
	      yymerge = yytrue;
	      break;
	    case 2:
	      break;
	    case 3:
	      yybest = yyp;
	      yymerge = yyfalse;
	      break;
	    default:
	      /* This cannot happen so it is not worth a YYASSERT (yyfalse),
		 but some compilers complain if the default case is
		 omitted.  */
	      break;
	    }
	  yypp = &yyp->yynext;
	}
    }

  if (yymerge)
    {
      yySemanticOption* yyp;
      int yyprec = yydprec[yybest->yyrule];
      yyflag = yyresolveAction (yybest, yystackp, &yysval,
				yylocp, yyscanner, pcode, o, err, contexts);
      if (yyflag == yyok)
	for (yyp = yybest->yynext; yyp != NULL; yyp = yyp->yynext)
	  {
	    if (yyprec == yydprec[yyp->yyrule])
	      {
		YYSTYPE yysval_other;
		YYLTYPE yydummy;
		yyflag = yyresolveAction (yyp, yystackp, &yysval_other,
					  &yydummy, yyscanner, pcode, o, err, contexts);
		if (yyflag != yyok)
		  {
		    yydestruct ("Cleanup: discarding incompletely merged value for",
				yystos[yys->yylrState],
				&yysval, yylocp, yyscanner, pcode, o, err, contexts);
		    break;
		  }
		yyuserMerge (yymerger[yyp->yyrule], &yysval, &yysval_other);
	      }
	  }
    }
  else
    yyflag = yyresolveAction (yybest, yystackp, &yysval, yylocp, yyscanner, pcode, o, err, contexts);

  if (yyflag == yyok)
    {
      yys->yyresolved = yytrue;
      yys->yysemantics.yysval = yysval;
    }
  else
    yys->yysemantics.yyfirstVal = NULL;
  return yyflag;
}

static YYRESULTTAG
yyresolveStack (yyGLRStack* yystackp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  if (yystackp->yysplitPoint != NULL)
    {
      yyGLRState* yys;
      int yyn;

      for (yyn = 0, yys = yystackp->yytops.yystates[0];
	   yys != yystackp->yysplitPoint;
	   yys = yys->yypred, yyn += 1)
	continue;
      YYCHK (yyresolveStates (yystackp->yytops.yystates[0], yyn, yystackp
			     , yyscanner, pcode, o, err, contexts));
    }
  return yyok;
}

static void
yycompressStack (yyGLRStack* yystackp)
{
  yyGLRState* yyp, *yyq, *yyr;

  if (yystackp->yytops.yysize != 1 || yystackp->yysplitPoint == NULL)
    return;

  for (yyp = yystackp->yytops.yystates[0], yyq = yyp->yypred, yyr = NULL;
       yyp != yystackp->yysplitPoint;
       yyr = yyp, yyp = yyq, yyq = yyp->yypred)
    yyp->yypred = yyr;

  yystackp->yyspaceLeft += yystackp->yynextFree - yystackp->yyitems;
  yystackp->yynextFree = ((yyGLRStackItem*) yystackp->yysplitPoint) + 1;
  yystackp->yyspaceLeft -= yystackp->yynextFree - yystackp->yyitems;
  yystackp->yysplitPoint = NULL;
  yystackp->yylastDeleted = NULL;

  while (yyr != NULL)
    {
      yystackp->yynextFree->yystate = *yyr;
      yyr = yyr->yypred;
      yystackp->yynextFree->yystate.yypred = &yystackp->yynextFree[-1].yystate;
      yystackp->yytops.yystates[0] = &yystackp->yynextFree->yystate;
      yystackp->yynextFree += 1;
      yystackp->yyspaceLeft -= 1;
    }
}

static YYRESULTTAG
yyprocessOneStack (yyGLRStack* yystackp, size_t yyk,
		   size_t yyposn, YYLTYPE *yylocp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  int yyaction;
  const short int* yyconflicts;
  yyRuleNum yyrule;

  while (yystackp->yytops.yystates[yyk] != NULL)
    {
      yyStateNum yystate = yystackp->yytops.yystates[yyk]->yylrState;
      YYDPRINTF ((stderr, "Stack %lu Entering state %d\n",
		  (unsigned long int) yyk, yystate));

      YYASSERT (yystate != YYFINAL);

      if (yyisDefaultedState (yystate))
	{
	  yyrule = yydefaultAction (yystate);
	  if (yyrule == 0)
	    {
	      YYDPRINTF ((stderr, "Stack %lu dies.\n",
			  (unsigned long int) yyk));
	      yymarkStackDeleted (yystackp, yyk);
	      return yyok;
	    }
	  YYCHK (yyglrReduce (yystackp, yyk, yyrule, yyfalse, yyscanner, pcode, o, err, contexts));
	}
      else
	{
	  yySymbol yytoken;
	  yystackp->yytops.yylookaheadNeeds[yyk] = yytrue;
	  if (yychar == YYEMPTY)
	    {
	      YYDPRINTF ((stderr, "Reading a token: "));
	      yychar = YYLEX;
	    }

	  if (yychar <= YYEOF)
	    {
	      yychar = yytoken = YYEOF;
	      YYDPRINTF ((stderr, "Now at end of input.\n"));
	    }
	  else
	    {
	      yytoken = YYTRANSLATE (yychar);
	      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
	    }

	  yygetLRActions (yystate, yytoken, &yyaction, &yyconflicts);

	  while (*yyconflicts != 0)
	    {
	      size_t yynewStack = yysplitStack (yystackp, yyk);
	      YYDPRINTF ((stderr, "Splitting off stack %lu from %lu.\n",
			  (unsigned long int) yynewStack,
			  (unsigned long int) yyk));
	      YYCHK (yyglrReduce (yystackp, yynewStack,
				  *yyconflicts, yyfalse, yyscanner, pcode, o, err, contexts));
	      YYCHK (yyprocessOneStack (yystackp, yynewStack,
					yyposn, yylocp, yyscanner, pcode, o, err, contexts));
	      yyconflicts += 1;
	    }

	  if (yyisShiftAction (yyaction))
	    break;
	  else if (yyisErrorAction (yyaction))
	    {
	      YYDPRINTF ((stderr, "Stack %lu dies.\n",
			  (unsigned long int) yyk));
	      yymarkStackDeleted (yystackp, yyk);
	      break;
	    }
	  else
	    YYCHK (yyglrReduce (yystackp, yyk, -yyaction,
				yyfalse, yyscanner, pcode, o, err, contexts));
	}
    }
  return yyok;
}

/*ARGSUSED*/ static void
yyreportSyntaxError (yyGLRStack* yystackp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  if (yystackp->yyerrState == 0)
    {
#if YYERROR_VERBOSE
      int yyn;
      yyn = yypact[yystackp->yytops.yystates[0]->yylrState];
      if (YYPACT_NINF < yyn && yyn <= YYLAST)
	{
	  yySymbol yytoken = YYTRANSLATE (yychar);
	  size_t yysize0 = yytnamerr (NULL, yytokenName (yytoken));
	  size_t yysize = yysize0;
	  size_t yysize1;
	  yybool yysize_overflow = yyfalse;
	  char* yymsg = NULL;
	  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn + 1;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytokenName (yytoken);
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytokenName (yyx);
		yysize1 = yysize + yytnamerr (NULL, yytokenName (yyx));
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + strlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow)
	    yymsg = (char *) YYMALLOC (yysize);

	  if (yymsg)
	    {
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (&yylloc, yyscanner, pcode, o, err, contexts, yymsg);
	      YYFREE (yymsg);
	    }
	  else
	    {
	      yyerror (&yylloc, yyscanner, pcode, o, err, contexts, YY_("syntax error"));
	      yyMemoryExhausted (yystackp);
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (&yylloc, yyscanner, pcode, o, err, contexts, YY_("syntax error"));
      yynerrs += 1;
    }
}

/* Recover from a syntax error on *YYSTACKP, assuming that *YYSTACKP->YYTOKENP,
   yylval, and yylloc are the syntactic category, semantic value, and location
   of the lookahead.  */
/*ARGSUSED*/ static void
yyrecoverSyntaxError (yyGLRStack* yystackp, yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  size_t yyk;
  int yyj;

  if (yystackp->yyerrState == 3)
    /* We just shifted the error token and (perhaps) took some
       reductions.  Skip tokens until we can proceed.  */
    while (YYID (yytrue))
      {
	yySymbol yytoken;
	if (yychar == YYEOF)
	  yyFail (yystackp, &yylloc, yyscanner, pcode, o, err, contexts, NULL);
	if (yychar != YYEMPTY)
	  {
	    /* We throw away the lookahead, but the error range
	       of the shifted error token must take it into account.  */
	    yyGLRState *yys = yystackp->yytops.yystates[0];
	    yyGLRStackItem yyerror_range[3];
	    yyerror_range[1].yystate.yyloc = yys->yyloc;
	    yyerror_range[2].yystate.yyloc = yylloc;
	    YYLLOC_DEFAULT ((yys->yyloc), yyerror_range, 2);
	    yytoken = YYTRANSLATE (yychar);
	    yydestruct ("Error: discarding",
			yytoken, &yylval, &yylloc, yyscanner, pcode, o, err, contexts);
	  }
	YYDPRINTF ((stderr, "Reading a token: "));
	yychar = YYLEX;
	if (yychar <= YYEOF)
	  {
	    yychar = yytoken = YYEOF;
	    YYDPRINTF ((stderr, "Now at end of input.\n"));
	  }
	else
	  {
	    yytoken = YYTRANSLATE (yychar);
	    YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
	  }
	yyj = yypact[yystackp->yytops.yystates[0]->yylrState];
	if (yyis_pact_ninf (yyj))
	  return;
	yyj += yytoken;
	if (yyj < 0 || YYLAST < yyj || yycheck[yyj] != yytoken)
	  {
	    if (yydefact[yystackp->yytops.yystates[0]->yylrState] != 0)
	      return;
	  }
	else if (yytable[yyj] != 0 && ! yyis_table_ninf (yytable[yyj]))
	  return;
      }

  /* Reduce to one stack.  */
  for (yyk = 0; yyk < yystackp->yytops.yysize; yyk += 1)
    if (yystackp->yytops.yystates[yyk] != NULL)
      break;
  if (yyk >= yystackp->yytops.yysize)
    yyFail (yystackp, &yylloc, yyscanner, pcode, o, err, contexts, NULL);
  for (yyk += 1; yyk < yystackp->yytops.yysize; yyk += 1)
    yymarkStackDeleted (yystackp, yyk);
  yyremoveDeletes (yystackp);
  yycompressStack (yystackp);

  /* Now pop stack until we find a state that shifts the error token.  */
  yystackp->yyerrState = 3;
  while (yystackp->yytops.yystates[0] != NULL)
    {
      yyGLRState *yys = yystackp->yytops.yystates[0];
      yyj = yypact[yys->yylrState];
      if (! yyis_pact_ninf (yyj))
	{
	  yyj += YYTERROR;
	  if (0 <= yyj && yyj <= YYLAST && yycheck[yyj] == YYTERROR
	      && yyisShiftAction (yytable[yyj]))
	    {
	      /* Shift the error token having adjusted its location.  */
	      YYLTYPE yyerrloc;
	      yystackp->yyerror_range[2].yystate.yyloc = yylloc;
	      YYLLOC_DEFAULT (yyerrloc, (yystackp->yyerror_range), 2);
	      YY_SYMBOL_PRINT ("Shifting", yystos[yytable[yyj]],
			       &yylval, &yyerrloc);
	      yyglrShift (yystackp, 0, yytable[yyj],
			  yys->yyposn, &yylval, &yyerrloc);
	      yys = yystackp->yytops.yystates[0];
	      break;
	    }
	}
      yystackp->yyerror_range[1].yystate.yyloc = yys->yyloc;
      if (yys->yypred != NULL)
	yydestroyGLRState ("Error: popping", yys, yyscanner, pcode, o, err, contexts);
      yystackp->yytops.yystates[0] = yys->yypred;
      yystackp->yynextFree -= 1;
      yystackp->yyspaceLeft += 1;
    }
  if (yystackp->yytops.yystates[0] == NULL)
    yyFail (yystackp, &yylloc, yyscanner, pcode, o, err, contexts, NULL);
}

#define YYCHK1(YYE)							     \
  do {									     \
    switch (YYE) {							     \
    case yyok:								     \
      break;								     \
    case yyabort:							     \
      goto yyabortlab;							     \
    case yyaccept:							     \
      goto yyacceptlab;							     \
    case yyerr:								     \
      goto yyuser_error;						     \
    default:								     \
      goto yybuglab;							     \
    }									     \
  } while (YYID (0))


/*----------.
| yyparse.  |
`----------*/

int
yyparse (yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts)
{
  int yyresult;
  yyGLRStack yystack;
  yyGLRStack* const yystackp = &yystack;
  size_t yyposn;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY;
  yylval = yyval_default;

#if YYLTYPE_IS_TRIVIAL
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif


  if (! yyinitGLRStack (yystackp, YYINITDEPTH))
    goto yyexhaustedlab;
  switch (YYSETJMP (yystack.yyexception_buffer))
    {
    case 0: break;
    case 1: goto yyabortlab;
    case 2: goto yyexhaustedlab;
    default: goto yybuglab;
    }
  yyglrShift (&yystack, 0, 0, 0, &yylval, &yylloc);
  yyposn = 0;

  while (YYID (yytrue))
    {
      /* For efficiency, we have two loops, the first of which is
	 specialized to deterministic operation (single stack, no
	 potential ambiguity).  */
      /* Standard mode */
      while (YYID (yytrue))
	{
	  yyRuleNum yyrule;
	  int yyaction;
	  const short int* yyconflicts;

	  yyStateNum yystate = yystack.yytops.yystates[0]->yylrState;
	  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
	  if (yystate == YYFINAL)
	    goto yyacceptlab;
	  if (yyisDefaultedState (yystate))
	    {
	      yyrule = yydefaultAction (yystate);
	      if (yyrule == 0)
		{
		  yystack.yyerror_range[1].yystate.yyloc = yylloc;
		  yyreportSyntaxError (&yystack, yyscanner, pcode, o, err, contexts);
		  goto yyuser_error;
		}
	      YYCHK1 (yyglrReduce (&yystack, 0, yyrule, yytrue, yyscanner, pcode, o, err, contexts));
	    }
	  else
	    {
	      yySymbol yytoken;
	      if (yychar == YYEMPTY)
		{
		  YYDPRINTF ((stderr, "Reading a token: "));
		  yychar = YYLEX;
		}

	      if (yychar <= YYEOF)
		{
		  yychar = yytoken = YYEOF;
		  YYDPRINTF ((stderr, "Now at end of input.\n"));
		}
	      else
		{
		  yytoken = YYTRANSLATE (yychar);
		  YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
		}

	      yygetLRActions (yystate, yytoken, &yyaction, &yyconflicts);
	      if (*yyconflicts != 0)
		break;
	      if (yyisShiftAction (yyaction))
		{
		  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
		  yychar = YYEMPTY;
		  yyposn += 1;
		  yyglrShift (&yystack, 0, yyaction, yyposn, &yylval, &yylloc);
		  if (0 < yystack.yyerrState)
		    yystack.yyerrState -= 1;
		}
	      else if (yyisErrorAction (yyaction))
		{
		  yystack.yyerror_range[1].yystate.yyloc = yylloc;
		  yyreportSyntaxError (&yystack, yyscanner, pcode, o, err, contexts);
		  goto yyuser_error;
		}
	      else
		YYCHK1 (yyglrReduce (&yystack, 0, -yyaction, yytrue, yyscanner, pcode, o, err, contexts));
	    }
	}

      while (YYID (yytrue))
	{
	  yySymbol yytoken_to_shift;
	  size_t yys;

	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    yystackp->yytops.yylookaheadNeeds[yys] = yychar != YYEMPTY;

	  /* yyprocessOneStack returns one of three things:

	      - An error flag.  If the caller is yyprocessOneStack, it
		immediately returns as well.  When the caller is finally
		yyparse, it jumps to an error label via YYCHK1.

	      - yyok, but yyprocessOneStack has invoked yymarkStackDeleted
		(&yystack, yys), which sets the top state of yys to NULL.  Thus,
		yyparse's following invocation of yyremoveDeletes will remove
		the stack.

	      - yyok, when ready to shift a token.

	     Except in the first case, yyparse will invoke yyremoveDeletes and
	     then shift the next token onto all remaining stacks.  This
	     synchronization of the shift (that is, after all preceding
	     reductions on all stacks) helps prevent double destructor calls
	     on yylval in the event of memory exhaustion.  */

	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    YYCHK1 (yyprocessOneStack (&yystack, yys, yyposn, &yylloc, yyscanner, pcode, o, err, contexts));
	  yyremoveDeletes (&yystack);
	  if (yystack.yytops.yysize == 0)
	    {
	      yyundeleteLastStack (&yystack);
	      if (yystack.yytops.yysize == 0)
		yyFail (&yystack, &yylloc, yyscanner, pcode, o, err, contexts, YY_("syntax error"));
	      YYCHK1 (yyresolveStack (&yystack, yyscanner, pcode, o, err, contexts));
	      YYDPRINTF ((stderr, "Returning to deterministic operation.\n"));
	      yystack.yyerror_range[1].yystate.yyloc = yylloc;
	      yyreportSyntaxError (&yystack, yyscanner, pcode, o, err, contexts);
	      goto yyuser_error;
	    }

	  /* If any yyglrShift call fails, it will fail after shifting.  Thus,
	     a copy of yylval will already be on stack 0 in the event of a
	     failure in the following loop.  Thus, yychar is set to YYEMPTY
	     before the loop to make sure the user destructor for yylval isn't
	     called twice.  */
	  yytoken_to_shift = YYTRANSLATE (yychar);
	  yychar = YYEMPTY;
	  yyposn += 1;
	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    {
	      int yyaction;
	      const short int* yyconflicts;
	      yyStateNum yystate = yystack.yytops.yystates[yys]->yylrState;
	      yygetLRActions (yystate, yytoken_to_shift, &yyaction,
			      &yyconflicts);
	      /* Note that yyconflicts were handled by yyprocessOneStack.  */
	      YYDPRINTF ((stderr, "On stack %lu, ", (unsigned long int) yys));
	      YY_SYMBOL_PRINT ("shifting", yytoken_to_shift, &yylval, &yylloc);
	      yyglrShift (&yystack, yys, yyaction, yyposn,
			  &yylval, &yylloc);
	      YYDPRINTF ((stderr, "Stack %lu now in state #%d\n",
			  (unsigned long int) yys,
			  yystack.yytops.yystates[yys]->yylrState));
	    }

	  if (yystack.yytops.yysize == 1)
	    {
	      YYCHK1 (yyresolveStack (&yystack, yyscanner, pcode, o, err, contexts));
	      YYDPRINTF ((stderr, "Returning to deterministic operation.\n"));
	      yycompressStack (&yystack);
	      break;
	    }
	}
      continue;
    yyuser_error:
      yyrecoverSyntaxError (&yystack, yyscanner, pcode, o, err, contexts);
      yyposn = yystack.yytops.yystates[0]->yyposn;
    }

 yyacceptlab:
  yyresult = 0;
  goto yyreturn;

 yybuglab:
  YYASSERT (yyfalse);
  goto yyabortlab;

 yyabortlab:
  yyresult = 1;
  goto yyreturn;

 yyexhaustedlab:
  yyerror (&yylloc, yyscanner, pcode, o, err, contexts, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;

 yyreturn:
  if (yychar != YYEMPTY)
    yydestruct ("Cleanup: discarding lookahead",
		YYTRANSLATE (yychar),
		&yylval, &yylloc, yyscanner, pcode, o, err, contexts);

  /* If the stack is well-formed, pop the stack until it is empty,
     destroying its entries as we go.  But free the stack regardless
     of whether it is well-formed.  */
  if (yystack.yyitems)
    {
      yyGLRState** yystates = yystack.yytops.yystates;
      if (yystates)
	{
	  size_t yysize = yystack.yytops.yysize;
	  size_t yyk;
	  for (yyk = 0; yyk < yysize; yyk += 1)
	    if (yystates[yyk])
	      {
		while (yystates[yyk])
		  {
		    yyGLRState *yys = yystates[yyk];
		    yystack.yyerror_range[1].yystate.yyloc = yys->yyloc;
		    if (yys->yypred != NULL)
		      yydestroyGLRState ("Cleanup: popping", yys, yyscanner, pcode, o, err, contexts);
		    yystates[yyk] = yys->yypred;
		    yystack.yynextFree -= 1;
		    yystack.yyspaceLeft += 1;
		  }
		break;
	      }
	}
      yyfreeGLRStack (&yystack);
    }

  /* Make sure YYID is used.  */
  return YYID (yyresult);
}

/* DEBUGGING ONLY */
#if YYDEBUG
static void yypstack (yyGLRStack* yystackp, size_t yyk)
  __attribute__ ((__unused__));
static void yypdumpstack (yyGLRStack* yystackp) __attribute__ ((__unused__));

static void
yy_yypstack (yyGLRState* yys)
{
  if (yys->yypred)
    {
      yy_yypstack (yys->yypred);
      YYFPRINTF (stderr, " -> ");
    }
  YYFPRINTF (stderr, "%d@%lu", yys->yylrState,
             (unsigned long int) yys->yyposn);
}

static void
yypstates (yyGLRState* yyst)
{
  if (yyst == NULL)
    YYFPRINTF (stderr, "<null>");
  else
    yy_yypstack (yyst);
  YYFPRINTF (stderr, "\n");
}

static void
yypstack (yyGLRStack* yystackp, size_t yyk)
{
  yypstates (yystackp->yytops.yystates[yyk]);
}

#define YYINDEX(YYX)							     \
    ((YYX) == NULL ? -1 : (yyGLRStackItem*) (YYX) - yystackp->yyitems)


static void
yypdumpstack (yyGLRStack* yystackp)
{
  yyGLRStackItem* yyp;
  size_t yyi;
  for (yyp = yystackp->yyitems; yyp < yystackp->yynextFree; yyp += 1)
    {
      YYFPRINTF (stderr, "%3lu. ",
                 (unsigned long int) (yyp - yystackp->yyitems));
      if (*(yybool *) yyp)
	{
	  YYFPRINTF (stderr, "Res: %d, LR State: %d, posn: %lu, pred: %ld",
		     yyp->yystate.yyresolved, yyp->yystate.yylrState,
		     (unsigned long int) yyp->yystate.yyposn,
		     (long int) YYINDEX (yyp->yystate.yypred));
	  if (! yyp->yystate.yyresolved)
	    YYFPRINTF (stderr, ", firstVal: %ld",
		       (long int) YYINDEX (yyp->yystate
                                             .yysemantics.yyfirstVal));
	}
      else
	{
	  YYFPRINTF (stderr, "Option. rule: %d, state: %ld, next: %ld",
		     yyp->yyoption.yyrule - 1,
		     (long int) YYINDEX (yyp->yyoption.yystate),
		     (long int) YYINDEX (yyp->yyoption.yynext));
	}
      YYFPRINTF (stderr, "\n");
    }
  YYFPRINTF (stderr, "Tops:");
  for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
    YYFPRINTF (stderr, "%lu: %ld; ", (unsigned long int) yyi,
	       (long int) YYINDEX (yystackp->yytops.yystates[yyi]));
  YYFPRINTF (stderr, "\n");
}
#endif



/* Line 2634 of glr.c  */
// #line 540 "./parser.y"


/* pcode is necessary but can be ignored. (See Bison Manual, Ch. 4.7)
 * The exact arguments depend on the parser type (GLR here) the use of locations
 * and additional parser argument (%parser-param, see above). */
void kure_yyerror (YYLTYPE *ploc, yyscan_t scanner, GString * pcode,
		   const ParserObserver * o, GString * err, GQueue * stack ,
		   const char * fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);

  /* Write the errors to the buffer if non-NULL. Otherwise use the standard
  * error stream. */
  if (err) {
    if (ploc->first_line) {
      /* Use a simpler location if the error is on just one line. */
      if (ploc->first_line == ploc->last_line)
	g_string_append_printf (err, "L%d.c%d-%d: ", ploc->first_line,
		 ploc->first_column, ploc->last_column);
      else
	g_string_append_printf (err, "L%d.c%d-L%d.c%d: ", ploc->first_line,
		 ploc->first_column, ploc->last_line, ploc->last_column);
    }
    g_string_append_vprintf (err, fmt, ap);
    g_string_append_c (err, '\n');
  }
  else {
    if (ploc->first_line) {
      /* Use a simpler location if the error is on just one line. */
      if (ploc->first_line == ploc->last_line)
	fprintf (stderr, "L%d.c%d-%d: ", ploc->first_line,
		 ploc->first_column, ploc->last_column);
      else
	fprintf (stderr, "L%d.c%d-L%d.c%d: ", ploc->first_line,
		 ploc->first_column, ploc->last_line, ploc->last_column);
    }
    vfprintf(stderr, fmt, ap);
    fputc ('\n', stderr);
  }
}

void _destroy_decls (LocalDecl * self, void * dummy) {
  g_free (self->name);
  g_free (self);
}

Context * _new_context () {
  Context * self = g_new0 (Context, 1);
  g_queue_init (&self->decls);
  return self;
}

LocalDecl * _find_decl_with_name (GQueue/*Decl*/ * decls, const char * name) {
  GList * iter = decls->head;
  for ( ; iter ; iter = iter->next) {
    LocalDecl * cur = (LocalDecl*)iter->data;
    if (0 == strcmp (name, cur->name)) return cur;
  }
  return NULL;
}

void _destroy_context (Context * self) {
  g_queue_foreach (&self->decls, (GFunc)_destroy_decls, NULL);
}

int _is_declared (Context * c, const char * name, LocalDeclType type)
{
  LocalDecl * l = _find_decl_with_name (&c->decls, name);
  if (l && l->type == type) return TRUE; else return FALSE;
}

// Returns NULL on success and the already declared object otherwise.
LocalDecl * _declare(Context * c, const char * name, LocalDeclType type)
{
  LocalDecl * l = _find_decl_with_name (&c->decls, name);

  if (l) /* already declared. */
    return l;
  else {
    l = g_new0 (LocalDecl, 1);
    l->type = type;
    l->name = g_strdup (name);
    g_queue_push_tail (&c->decls, l);

#if 0
    printf ("Declared \"%s\" with name \"%s\".\n", (type == VAR)
	    ? "VAR" : ((type == DOM) ? "Dom" : (type == FUN)
		       ? "Fun" : "Unknown"), name);
#endif
    return NULL;
  }
}

void _g_free_all (void * p, ...)
{
  va_list ap;
  void * tmp;

  if (p) g_free (p);

  va_start (ap, p);
  while ((tmp = va_arg(ap, void*))) {
    g_free (tmp);
  }
  va_end (ap);
}

/*!
 * Returns the internal (mapped) function name for the given RelView function.
 * Frees fun or reuses it. Returns a newly alloced string which must be freed
 * using g_free. */
char * map_fun (char * fun)
{
  /* The following two patterns also match, e.g., "random10xyz" and
   * "randomcf10xyz" */
  if (g_str_has_prefix(fun, "random") && !g_str_equal(fun, "randomperm") && !g_str_equal(fun, "random")) {
    int prob;
    if (1 == sscanf (fun, "randomcf%d", &prob)) {
      g_free (fun);
      return g_strdup_printf ("kure.compat.randomcf_closure(%.3lf)", prob/100.0);
    }
    else if (1 == sscanf (fun, "random%d", &prob)) {
      g_free (fun);
      return g_strdup_printf ("kure.compat.random_closure(%.3lf)", prob/100.0);
    }
    else return fun;
  }
  else {
    struct fun_map_t *p = fun_map;
    for ( ; p->name ; ++p) {
      if (g_str_equal(p->name, fun)) {
	g_free (fun);
	return g_strdup (p->transl_name);
      }
    }

    return fun;
  }
}

/*!
 * Returns TRUE if the given function name is a RelView reserved function,
 * e.g. "p-1" or "random99".
 */
gboolean _is_reserved_fun (const gchar * name)
{
  gchar * mapped = map_fun (g_strdup(name));

  /* RelView's internal functions are always mapped to a name which is not
   * equal to the original name. */
  gboolean ret = (!g_str_equal (mapped, name));

  g_free (mapped);
  return ret;
}


const fun_map_t * _lookup_reserved_fun (const gchar * name)
{
  struct fun_map_t *p = fun_map;
  for ( ; p->name ; ++p) {
    if (g_str_equal(p->name, name)) return p;
  }

  return NULL;
}


/*!
 * Create debug code to check function arguments. The returnes string has to
 * be freed using g_free or similar.
 *
 * \param name The function's name.
 * \param params A comma separated list of arguments expected by the function.
 */
gchar * _check_args_code (const gchar * name, const gchar * params)
{
  /* Skip any leading whitespaces. */
  const gchar * paramsPtr = params;
  while (*paramsPtr != '\0' && isspace(*paramsPtr))
    paramsPtr ++;

  if ('\0' == *paramsPtr) {
    /* Empty parameter list. */
    return g_strdup ("");
  }
  else {
    gchar ** v = g_strsplit (paramsPtr, ",", 0), **vptr;
    GString * ret = g_string_new ("\tif not (");

    for (vptr = v ; *vptr ; vptr ++) {
      /* Remove leading and trailing whitespaces. */
      *vptr = g_strstrip (*vptr);

      if (vptr != v)
	g_string_append (ret, " and ");
      g_string_append (ret, *vptr);
    }
    g_string_append_printf
      (ret, ") then\n"
       "\t\tlocal err = 'Too few arguments to function \\'%s\\'.';\n", name);
    for (vptr = v ; *vptr ; vptr ++) {
      g_string_append_printf (ret, "\t\tif not %s then err "
			      "= err .. ' Argument \\'%s\\' is nil.' end\n", *vptr, *vptr);
    }
    g_string_append (ret, "\t\terror(err)\n"
		     "\tend\n");

    g_strfreev (v);
    return g_string_free (ret, FALSE);
  }
}
