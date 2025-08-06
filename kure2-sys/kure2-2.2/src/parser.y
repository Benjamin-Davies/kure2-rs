/*
 * parser.y
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

/* Uncomment this and set kure_yydebug (int) in KureLang.c to a non-0 value 
 * to enable debugging. (Bison Manual, Sec. 8.2) */
//%debug

/* TODO: - Clean up the context step in case of error.
 *       - Improve error recovery in general. */

/* Line tracking
 *
 * When domain-specific code is parsed and translated into Lua code,
 * line numbers are lost due to auxiliary code in Lua. For instance,
 * declaration of local variables. However, correct line numbers are
 * essential for the end user to identify error locations in DSL code.
 * In C, the C precompiler has a comparable problem which is solved
 * by using '#line <line>' directives which are used by the compiler
 * to print the original line numbers. We do something similar here.
 * Each time a DSL function or program is transformed into a Lua 
 * function, an additional local variable 'line' is generated which
 * stores the current line number in the DSL code and which is updated
 * accordingly. To give an example, consider the following DSL code:
 *    1: f(X,Y)
 *    2:  DECL A,B
 *    3:   BEG A=X|Y
 *    4:       B=X&Y
 *    5:       RETURN A*B
 *    6:   END
 * The generated Lua code now looks similar to this:
 *    function f(X,Y)
 *      local __line = 1
 *      local X
 *      local Y
 *      __line = 3; A = kure.or(X,Y);
 *      __line = 4; B = kure.and(X,Y);
 *      return kure.mult(A,B);
 *    end
 * As you can see, it is now possible to map line numbers back to the
 * original DSL code.
 *
 * The name of the local variable is defined in LINE_TRACKING_VAR.
 */


/* Tree Optimization for Transposed Relations
 *
 * There a four different variants to multiply (or compose) a pair of relations:
 *     mult(X,X)               = X * X
 *     mult_norm_transp(X,X)   = X * X^
 *     mult_transp_norm(X,X)   = X^ * X 
 *     mult_transp_transp(X,X) = X^ * X^
 * Each variant is dominant over the naive approach to apply mult(..) and 
 * tranpose(..) for the argument(s). Hence, a expression like
 *     R*(S|T)^
 * should be transformed into
 *     mult_norm_transp(R,(or(S,T)))
 * which can be considered an optimized version. In the previous version of
 * Kure, these optimization were applied while the DSL was interpreted. However,
 * Lua does not easily allow to change the executed commands at runtime, so we
 * we have to use another way here. Our approach is that we use the parser
 * to generate the specialized code. This does not work in every situation, but
 * works quite well in simple and some moderate cases.
 *
 * To this end we use a special token called 'transp_expr' along with the def.
 * 'expr' token. The difference is that 'transp_expr' represents a expressions
 * that has to be transposed before it can be used as an 'expr'. E.g. a rule like
 *     expr: '(' transp_expr ')' { $$ = "transpose($2)" }
 * has to create Lua code.
 *     (transpose($2))
 *
 * Do distinguish the variants of multiplication, we can now add rules like
 *     expr: transp_expr '*' expr { $$ = "mult_transp_norm($1,$3)" }
 * The serious disadvantage is that these rules cause ambiguity to the parser
 * that has to be solved manually. As an example consider the following
 * expression -(X^) and simplified rules:
 *     (1) transp_expr: '(' transp_expr ')' { $$=$2 }
 *     (2) | expr '^'
 *     (3) expr: '(' expr ')' { $$=$2 }
 *     (4) | transp_expr { $$ = "tranpose($1)" }
 *     (5) | '-' expr { $$ = "complement($2)" }
 * The last rule is mandatory because at some point in time we end up at a 
 * function for which there is no rule involving a transp_expr token. The
 * expression can be parsed as:
 *    X^    -> transp_expr;  rule (2)
 *    (X^)  -> transp_expr;  rule (1)
 *    (X^)  -> expr;         rule (4)
 *    -(X^) -> expr;         rule (5)
 * But it can also be parsed as:
 *    X^    -> transp_expr;  rule (2)
 *    X^    -> expr;         rule (4)
 *    (X^)  -> expr;         rule (3)
 *    -(X^) -> expr;         rule (5)
 *
 * Bison is unable to solve these ambiguities itself. Thus, they have to be
 * solved manually in advance using so called dynamic precedences (%dprec <n>).
 * See 1.5.2, p.23 and 2.4.2, p.100 in the Bison manual. Only the branch with
 * highest precedence is used after the parser has split itself.
 */

%code top {
#define _GNU_SOURCE // We need vasprintf
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <KureImpl.h>
#include <glib.h>

}

%code requires {
  #include <glib.h>

  typedef struct _Context Context;

  typedef struct Signature
  {
    gchar * name;
    gchar * params;
  } Signature;

  typedef struct {
    int n;
    gchar * s;
  } Args;
}

%union {
  gchar * code;
  Signature sig;
  Args args;
}


%code {
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
  static struct fun_map_t {
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
}

%code provides {
#include <glib.h>

  typedef struct _ParserObserver {
    void (*onProgram) (void * object, const char * gencode, const YYLTYPE*);
    void (*onFunction) (void * object, const char * gencode, const YYLTYPE*);

    void * object;
  } ParserObserver;

typedef struct
{
  int column;
  int start_symbol;
  GString * err;
} LexerInfo;

#ifndef YY_TYPEDEF_YY_SCANNER_T // lexer.h
  typedef void* yyscan_t;
#endif

  /* Depends on the parser arguments. See %parser-param below. */
#define KURE_YYERROR(loc, ...)						\
  kure_yyerror (&(loc), NULL/*scanner*/, NULL/*code*/, NULL/*observer*/, \
		err, NULL/*stack*/,  __VA_ARGS__)
  
  void kure_yyerror (YYLTYPE * ploc, yyscan_t, GString*, const ParserObserver*, 
		     GString * err, GQueue*, const char * fmt, ...);
}

/* Also specifies the operator's precedence. The topmost operator has the lowest
 * precedence. (Bison manual, Ch. 5.3) */
%left '/' '\\'
%left '|'
%left '&'
%left '*'
%left '+'
%left '^'
%right '-'
%nonassoc LLBRACKET RRBRACKET
%nonassoc PROG DECL BEG RET END PROD SUM
%nonassoc IF THEN ELSE FI
%nonassoc WHILE DO OD
%nonassoc ASSERT
%nonassoc <code> IDENTIFIER

%type <sig> declarator
%type <code> translation_unit translation_element bare_statement statement 
%type <code> statement_list fundef fundef_bare progbody progdef progdef_bare 
%type <code> expr transp_expr postfix_expr parameter_list real_parameter_list
%type <code> local_vardecl local_fundef local_domdef
%type <code> local_decls local_decl_list local_decl local_decl_bare
%type <args> arg_expr_list real_arg_expr_list

 /* Use locations, e.g. defines YYLTYPE (Bison Manual, Ch. 3.6) */
%locations

 /* Prefix for the Parser/Lever symbol. (Bison Manual, Ch. 3.8) */
%name-prefix "kure_yy"

%output "parser.c"

/* Create a header file with e.g. token definitions. */
%defines

 /* Use a pure parser without global variables. */
%define api.pure

/* Output more useful error messages instead of "syntax error". */
%error-verbose

/* Otherwise, our yyerror function has the wrong type (would have no 
 * YYLTYPE* parameter) */
%glr-parser

/* See %lex-param below. */
%parse-param {yyscan_t yyscanner}
%parse-param {GString * pcode}
%parse-param {const ParserObserver * o}
%parse-param {GString * err}
%parse-param {GQueue/*Context**/ * contexts}

/* See the 'reentrant' option in the .l file for details. */
%lex-param {yyscan_t yyscanner}

/* See the non-terminal symbol 'start'. */
%token <code> START_DEFAULT START_EXPR
%type <code> start

/* Initialization for the stack of contexts is necessary only
 * when a translation unit is parsed. Thus, we use the non-
 * terminal symbol 'start' below. */
//%initial-action { /* Initialize the context. */ }

/* See 3.7.7 in the Bison manual. */
%destructor { /* Code */ } START_DEFAULT

%start start
%%

/* We use the grammer to parse translation units (e.g. files) and simple
 * expressions. This would require two different start symbols which is
 * not allowed in Bison. To overcome this, the Bison manual Sec. 11.5,
 * p. 143 suggests to use the following construct. Which of the tokens
 * START_* is used depends on the first symbol returned by the lexer. */
start: START_DEFAULT  { g_queue_init(contexts); PUSH_CONTEXT; } translation_unit
| START_EXPR expr { g_string_assign(pcode, $2); }
| START_EXPR error { KURE_YYERROR(@$, "... in this expression."); YYERROR; }

translation_unit: translation_element { g_string_append (pcode, $1); g_free($1);  }
| translation_unit translation_element {  g_string_append (pcode, $2); g_free($2); }

/* We use callbacks to let the caller extract the original part of the
 * source code. */
translation_element: fundef { $$=$1; if(o && o->onFunction) o->onFunction(o->object, $$, &@$); }
| progdef { $$=$1; if(o && o->onProgram) o->onProgram(o->object, $$, &@$); }

/* Note: While Lua uses Boolean expression for conditions, Relview uses 
 * relations of dimension 1x1. kure.is_true provides this test. 
 * (TRUE/FALSE->Bool). */
bare_statement: IF expr THEN statement_list FI { $$ = CONV ("\tif (kure.is_true(%s))\tthen\t%s\tend", $2, $4); }
| IF expr THEN statement_list ELSE statement_list FI { $$ = CONV ("\tif (kure.is_true(%s))\tthen\t%s\telse %s\tend", $2, $4, $6); }
| WHILE expr DO statement_list OD { $$ = CONV("\twhile (kure.is_true(%s)) do\t%s\tend", $2, $4); }
| ASSERT '(' IDENTIFIER ',' expr ')' { $$ = CONV("\tkure.compat.assert(\"%s\",%s)", $3,$5); }
  /* Only left-hand sides in assignment have to be checked whether 
   * they are defined locally or not. Furthermore, assignments can only appear
   * in program bodies. */
| IDENTIFIER { if( !_is_declared(CONTEXT, $1, VAR)) 
      { KURE_YYERROR(@$, "Left hand side \"%s\" in assignment is not a local variable.", $1); YYERROR; } } 
  '=' expr { $$ = CONV("\t%s = %s", $1,$4); }

statement: bare_statement { $$ = g_strdup_printf (" "LINE_TRACKING_VAR"=%d; %s", @$.first_line, $1); g_free ($1); }
| bare_statement ';' { $$ = g_strdup_printf (" "LINE_TRACKING_VAR"=%d; %s", @$.first_line, $1); g_free ($1); }


statement_list: { $$=g_strdup(""); }
| statement_list statement {  $$ = CONV("\t%s\n%s\n", $1,$2) }

declarator: IDENTIFIER '(' parameter_list ')' { 
  /* Check if the identifier is a RelView reserved function name. */
  if (_is_reserved_fun ($1)) {
    KURE_YYERROR(@$, "Identifier \"%s\" in declarator is a reserved function name.", $1);
    _g_free_all ($1, $3, NULL);
    YYERROR;
  }
  else  { $$.name = $1; $$.params = $3; }
  }

fundef: fundef_bare '.'
| fundef_bare END

fundef_bare: declarator '=' expr {
	/* Check arguments and throw an error if at least one argument is missing. */
	gchar * s = _check_args_code ($1.name, $1.params);
	gchar * line = g_strdup_printf ("%d", @$.first_line);
	$$ = CONV("function %s (%s) local "LINE_TRACKING_VAR"=%s\n%s return %s end\n", $1.name, $1.params, line, s, $3);
}
| declarator '=' error { KURE_YYERROR(@$, "... in the expression of function \"%s\".", $1.name); g_free($1.params); YYERROR; } 

/* YYERROR is used to propagate the error to the program level, so the user 
 * gets the program name. */
progbody: BEG statement_list RET expr END { $$ = CONV("%s\n\treturn %s\nend\n", $2,$4); }
| BEG error RET expr END { KURE_YYERROR(@$, "... in the statement(s) ..."); YYERROR;}
| BEG statement_list RET error END { KURE_YYERROR(@$, "... in the RETURN expression ..."); YYERROR; }

progdef_bare: declarator progbody {
	/* Check arguments and throw an error if at least one argument is missing. */
	gchar * s = _check_args_code ($1.name, $1.params);
	gchar * line = g_strdup_printf ("%d", @$.first_line);
	$$ = CONV("function %s (%s) local "LINE_TRACKING_VAR"=%d %s %s\n", $1.name, $1.params, line, s, $2);
}
| declarator DECL { PUSH_CONTEXT } local_decls progbody { 
	/* Check arguments and throw an error if at least one argument is missing. */
	gchar * s = _check_args_code ($1.name, $1.params);
	gchar * line = g_strdup_printf ("%d", @$.first_line);
	$$ = CONV("function %s (%s)\nlocal "LINE_TRACKING_VAR"=%s\n%s\n%s\n%s\n", $1.name,$1.params,line,s,$4,$5); POP_CONTEXT 
}
| declarator DECL { PUSH_CONTEXT } error progbody { KURE_YYERROR(@$, "... in the DECL part of program \"%s\".", $1.name); g_free($1.params); POP_CONTEXT; YYERROR; }
| declarator error { KURE_YYERROR(@$, "... of program \"%s\".", $1.name); g_free($1.params); YYERROR; }
| declarator DECL { PUSH_CONTEXT } local_decls error { KURE_YYERROR(@$, "... of program \"%s\".", $1.name); g_free($1.params); POP_CONTEXT; YYERROR; }

/* The trailing dot it optional now. */
progdef: progdef_bare | progdef_bare '.'

parameter_list: { $$ = strdup(""); }
| real_parameter_list

real_parameter_list: IDENTIFIER
| real_parameter_list ',' IDENTIFIER { $$ = CONV("%s, %s", $1, $3); }

/* Explained at the beginning of the file. */
transp_expr: expr '^'                                                   %dprec 3
| '(' transp_expr ')'    { $$ = CONV("(%s)", $2) } %dprec 3

expr: transp_expr '^' %dprec 2
| transp_expr            { $$=CONV("kure.transpose(%s)", $1) }           %dprec 1
| expr '*' transp_expr   { $$=CONV("kure.mult_norm_transp(%s,%s)", $1, $3) } %dprec 4
| transp_expr '*' expr   { $$=CONV("kure.mult_transp_norm(%s,%s)", $1, $3) } %dprec 6
| transp_expr '*' transp_expr { $$=CONV("kure.mult_transp_transp(%s,%s)", $1, $3) } %dprec 8
| expr '*' expr          { $$=CONV("kure.mult(%s,%s)", $1, $3) } %dprec 1

expr:
'(' expr ')'            { $$=CONV("(%s)", $2) }                        %dprec 2
| '[' expr ',' expr RRBRACKET { $$=CONV("kure.right_tupling(%s,%s)", $2, $4) }
| LLBRACKET expr ',' expr ']' { $$=CONV("kure.left_tupling(%s,%s)", $2, $4) }
| '[' expr ',' expr ']'  { $$=CONV("kure.tupling(%s,%s)", $2, $4) }
| expr '&' expr          { $$=CONV("kure.land(%s,%s)", $1, $3) }
| expr '|' expr          { $$=CONV("kure.lor(%s,%s)", $1, $3) }
| expr '\\' expr         { $$=CONV("kure.right_residue(%s,%s)", $1, $3) }
| expr '/' expr          { $$=CONV("kure.left_residue(%s,%s)", $1, $3) }
| expr '+' expr          { $$=CONV("kure.direct_sum(%s,%s)", $1, $3) }
| '-' expr               { $$=CONV("kure.complement(%s)", $2) }
| postfix_expr
| IDENTIFIER

/* Check the number of arguments if it is a RelView function. */
postfix_expr: IDENTIFIER '(' arg_expr_list ')' { 
  const fun_map_t * ent = _lookup_reserved_fun ($1);
  if (ent) {
    if ($3.n != ent->arg_count) {
      KURE_YYERROR(@$, "Reserved function \"%s\" expects %d argument(s). Got %d.", 
		   $1, ent->arg_count, $3.n);
      _g_free_all ($1,$3.s,NULL);
      YYERROR;
    }
    else { 
      $$=g_strdup_printf("%s(%s)", ent->transl_name,$3.s);
      _g_free_all ($1,$3.s,NULL);
    }
  }
  else /* Non reserved function, number of args unknown, or
        * something like "random20". */ { 
    gchar * mapped = map_fun ($1);
    $$ = CONV("%s(%s)", mapped, $3.s);
  }
  }

arg_expr_list: { $$.n = 0; $$.s = g_strdup (""); }
| real_arg_expr_list 

real_arg_expr_list: expr { $$.n = 1, $$.s = $1; }
| real_arg_expr_list ',' expr { $$.n = $1.n + 1; 
    $$.s = g_strconcat($1.s, ", ", $3, NULL); 
    _g_free_all($1.s, $3, NULL);
  }

local_decls: local_decl_list
| local_decl_list ';'
| local_decls local_decl_list     { $$ = CONV("%s\n%s", $1,$2); }
| local_decls local_decl_list ';' { $$ = CONV("%s\n%s", $1,$2); }

local_decl_list: local_decl
| local_decl_list ',' local_decl  { $$ = CONV("%s\n%s", $1,$3); }

local_decl: local_decl_bare       { $$ = CONV("\tlocal %s", $1); }

local_decl_bare: local_vardecl
| local_fundef 
| local_domdef 

local_vardecl: IDENTIFIER { DECLARE($1, VAR); $$=$1; }
/* To use "<name> = function <param> ... return ... end" would have been 
 * possible for every function, yet we need the function/program name for 
 * debugging purposes. */
local_fundef: declarator '=' expr {
	 /* Check arguments and throw an error if at least one argument is missing. */
	gchar * s = _check_args_code ($1.name, $1.params);
	
	DECLARE($1.name, FUN); 
	$$ = CONV("%s = function (%s) %s return %s end", $1.name,$1.params,s,$3); 
}
local_domdef: IDENTIFIER '=' PROD '(' expr ',' expr ')' { DECLARE($1, DOM); $$ = CONV("%s = kure.direct_product_new(%s,%s)", $1,$5,$7); }
| IDENTIFIER '=' SUM '(' expr ',' expr ')' { DECLARE($1, DOM); $$ = CONV("%s = kure.direct_sum_new(%s,%s)", $1,$5,$7); }

%%

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
