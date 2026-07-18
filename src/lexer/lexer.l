/* 
 * Lexer Specification (Flex)
 * 
 * This file defines regular expressions and actions for tokenizing source code.
 * Flex scans the input character-by-character and groups characters into tokens.
 * 
 * Key concepts:
 * - Patterns are matched in order; longest match wins
 * - Keywords must be defined BEFORE the generic identifier pattern
 * - Comments and whitespace are discarded (not tokenized)
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ast/ast.h"
#include "../parser/parser.tab.h"  /* Bison-generated header with token definitions */

/* Declare strdup to avoid implicit declaration warning and pointer truncation on 64-bit Windows */
char *strdup(const char *s);

/* Global line counter for error reporting */
int line_number = 1;

%}

 /* Options for Flex */
%option noyywrap

 /* Patterns (regular expressions) are defined below the %% marker */
%%

 /* ============ WHITESPACE (discarded) ============ */
[ \t]+              { /* Skip spaces and tabs */ }
\n                  { line_number++; /* Count newlines for error reporting */ }

 /* ============ COMMENTS (discarded) ============ */
"//".*              { /* Skip single-line comments */ }
"/*"([^*]|\*[^/])*"*/" { 
                      /* Skip block comments; count newlines inside them */
                      int i;
                      for (i = 0; yytext[i]; i++) {
                          if (yytext[i] == '\n') line_number++;
                      }
                    }

 /* ============ KEYWORDS (must come before IDENTIFIER) ============ */
"int"               { return INT; }
"float"             { return FLOAT; }
"bool"              { return BOOL; }
"if"                { return IF; }
"else"              { return ELSE; }
"while"             { return WHILE; }
"print"             { return PRINT; }
"true"              { yylval.bool_val = 1; return TRUE; }
"false"             { yylval.bool_val = 0; return FALSE; }

 /* ============ OPERATORS ============ */
"+"                 { return PLUS; }
"-"                 { return MINUS; }
"*"                 { return MULTIPLY; }
"/"                 { return DIVIDE; }
"%"                 { return MODULO; }
"="                 { return ASSIGN; }
"=="                { return EQ; }
"!="                { return NEQ; }
"<"                 { return LT; }
">"                 { return GT; }
"<="                { return LE; }
">="                { return GE; }
"&&"                { return AND; }
"||"                { return OR; }
"!"                 { return NOT; }

 /* ============ DELIMITERS ============ */
"{"                 { return LBRACE; }
"}"                 { return RBRACE; }
"("                 { return LPAREN; }
")"                 { return RPAREN; }
";"                 { return SEMICOLON; }

 /* ============ IDENTIFIERS (generic pattern; keywords matched first) ============ */
[a-zA-Z_][a-zA-Z0-9_]* {
                      yylval.str = strdup(yytext);
                      return IDENTIFIER;
                    }

 /* ============ NUMERIC CONSTANTS ============ */
[0-9]+\.[0-9]+      {
                      yylval.float_val = atof(yytext);
                      return FLOAT_CONST;
                    }
[0-9]+              {
                      yylval.int_val = atoi(yytext);
                      return INT_CONST;
                    }

 /* ============ ERROR HANDLING ============ */
.                   {
                      fprintf(stderr, "Lexical Error: Invalid character '%c' at line %d\n", 
                              yytext[0], line_number);
                      return UNKNOWN;
                    }

%%

 /* 
  * This section contains C functions that can be called from the parser.
  * In this case, we don't need any additional functions for basic lexing.
  */
