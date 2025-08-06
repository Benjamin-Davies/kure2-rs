/*
 * lexer.l
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

%option outfile="lexer.c" header-file="lexer.h"
%option prefix="kure_yy"

%option noyywrap
%option yylineno
/*%option case-insensitive*/

/* We use pure parser. For non-pure parsers the following two definitions
 * would cause mysterious errors. */
%option bison-bridge
%option bison-locations

/* Use a reentrant Flex Scanner. Notice that now the yylex function has
 * an additional argument of type yyscan_t which has to be indicated in
 * the parser file with %lex-param. (See Flex Manual, Ch. 19.4 and
 * Bison Manual, Ch. 4.6.4, p.85) */
%option reentrant

%option stack

%{
#include <stdio.h>

#include "Kure.h" // KURE_DOLLAR_SUBST
#include "parser.h"
//#include "lexer.h" // must not be included!

#define YY_EXTRA_TYPE LexerInfo*

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
   yylloc->first_column = yyextra->column; yylloc->last_column = yyextra->column + yyleng-1; \
   yyextra->column += yyleng;

%}

/* Pass the column number as extra argument. */
/*%option extra-type=int*/ 
/* Doesn't work! */

%x IN_COMMENT

%%
   
%{
/* See the start symbol in parser.y. This is used to provide different start
 * symbols as suggested in the Bison manual, Sec. 11.5. */
if (yyextra->start_symbol) {
   int t = yyextra->start_symbol;
   yyextra->start_symbol = 0;
   return t;
}
%}

"{"     yy_push_state(IN_COMMENT, yyscanner);

<IN_COMMENT>{
"}"     yy_pop_state(yyscanner);
[^}\n]*        
\n       { yyextra->column = 1; }
<<EOF>>  { 
GString * err = yyextra->err; // Necessary for the macro to work.
KURE_YYERROR (*yylloc, "Unclosed comment."); yyterminate(); 
}
}

"PROD"   { return (PROD);}
"SUM"    { return (SUM); }

"DECL"   { return (DECL); }
"BEG"    { return (BEG); }
"END"    { return (END); }
"RETURN" { return (RET); }
"WHILE"  { return (WHILE); }
"DO"     { return (DO); }
"OD"     { return (OD); }
"IF"     { return (IF); }
"THEN"   { return (THEN); }
"ELSE"   { return (ELSE); }
"FI"     { return (FI); }
"ASSERT" { return (ASSERT); }

"[|" {
	/* Used in the left tupling and cannot easily be recognized by the
	 * parser due to a collision with | in the OR operation. */
	return LLBRACKET;
}

"|]" {
	/* Similar to "[|" above. */
	return RRBRACKET;
}

"$" { 
  /* Dollar is not covered by our convention definition for an
   * IDENTIFIER. Furthermore, $ is no valid IDENTIFIER inside
   * Lua, so it has to be replaced by something valid, viz.
   * "__dollar". */
  yylval->code = g_strdup (KURE_DOLLAR_SUBST); 
  return IDENTIFIER; }

"1-st" |
"2-nd" |
[a-zA-Z_]([a-zA-Z0-9_-]*) {
  yylval->code = g_strdup (yytext);
  return (IDENTIFIER);
}

[\t \r]
\n { yyextra->column = 1; }

. { return *yytext; }

%%