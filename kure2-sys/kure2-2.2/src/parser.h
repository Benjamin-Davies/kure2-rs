
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison GLR parsers in C

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

/* "%code requires" blocks.  */

/* Line 2638 of glr.c  */
// #line 129 "./parser.y"

#pragma once

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



/* Line 2638 of glr.c  */
// #line 58 "parser.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     RRBRACKET = 258,
     LLBRACKET = 259,
     SUM = 260,
     PROD = 261,
     END = 262,
     RET = 263,
     BEG = 264,
     DECL = 265,
     PROG = 266,
     FI = 267,
     ELSE = 268,
     THEN = 269,
     IF = 270,
     OD = 271,
     DO = 272,
     WHILE = 273,
     ASSERT = 274,
     IDENTIFIER = 275,
     START_DEFAULT = 276,
     START_EXPR = 277
   };
#endif


#ifndef YYSTYPE
typedef union YYSTYPE
{

/* Line 2638 of glr.c  */
// #line 146 "./parser.y"

  gchar * code;
  Signature sig;
  Args args;



/* Line 2638 of glr.c  */
// #line 104 "parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{

  int first_line;
  int first_column;
  int last_line;
  int last_column;

} YYLTYPE;
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* "%code provides" blocks.  */

/* Line 2638 of glr.c  */
// #line 266 "./parser.y"

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



/* Line 2638 of glr.c  */
// #line 159 "parser.h"


int kure_yyparse (yyscan_t yyscanner, GString * pcode, const ParserObserver * o, GString * err, GQueue/*Context**/ * contexts);
