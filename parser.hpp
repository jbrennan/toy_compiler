/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TIDENTIFIER = 258,
     TINTEGER = 259,
     TDOUBLE = 260,
     TYES = 261,
     TNO = 262,
     TCEQ = 263,
     TCNE = 264,
     TCLT = 265,
     TCLE = 266,
     TCGT = 267,
     TCGE = 268,
     TEQUAL = 269,
     TLPAREN = 270,
     TRPAREN = 271,
     TLBRACE = 272,
     TRBRACE = 273,
     TCOMMA = 274,
     TDOT = 275,
     TPLUS = 276,
     TMINUS = 277,
     TMUL = 278,
     TDIV = 279
   };
#endif
/* Tokens.  */
#define TIDENTIFIER 258
#define TINTEGER 259
#define TDOUBLE 260
#define TYES 261
#define TNO 262
#define TCEQ 263
#define TCNE 264
#define TCLT 265
#define TCLE 266
#define TCGT 267
#define TCGE 268
#define TEQUAL 269
#define TLPAREN 270
#define TRPAREN 271
#define TLBRACE 272
#define TRBRACE 273
#define TCOMMA 274
#define TDOT 275
#define TPLUS 276
#define TMINUS 277
#define TMUL 278
#define TDIV 279




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 10 "parser.y"
{
	Node *node;
	NBlock *block;
	NExpression *expr;
	NStatement *stmt;
	NIdentifier *ident;
	NVariableDeclaration *var_decl;
	std::vector<NVariableDeclaration *> *var_vector;
	std::vector<NExpression *> *expr_vector;
	std::string *string;
	int token;
}
/* Line 1529 of yacc.c.  */
#line 110 "parser.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

