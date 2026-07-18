/*
 * Parser Specification (Bison)
 *
 * This file defines the Context-Free Grammar (CFG) for the mini-language.
 * Bison converts this into a LALR(1) parser that consumes tokens from the lexer
 * and builds an Abstract Syntax Tree (AST).
 *
 * Key concepts:
 * - Grammar rules map to AST node construction
 * - Semantic actions (in {}) execute when a rule matches
 * - Precedence/associativity eliminate ambiguity
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ast/ast.h"
#include "../semantic/semantic.h"

/* Forward declaration of lexer function */
extern int yylex(void);

/* Global line counter (shared with lexer) */
extern int line_number;

/* Global AST root (filled by the parser) */
ASTNode *program_root = NULL;

/* Error callback */
void yyerror(const char *msg);

%}

/* ============ TOKEN DECLARATIONS ============ */
%union {
    int int_val;
    float float_val;
    int bool_val;
    char *str;
    ASTNode *node;
}

/* Keywords */
%token INT FLOAT BOOL
%token IF ELSE WHILE PRINT
%token TRUE FALSE

/* Operators */
%token PLUS MINUS MULTIPLY DIVIDE MODULO
%token ASSIGN EQ NEQ LT GT LE GE AND OR NOT

/* Delimiters */
%token LBRACE RBRACE LPAREN RPAREN SEMICOLON

/* Constants and Identifiers */
%token <int_val> INT_CONST
%token <float_val> FLOAT_CONST
%token <str> IDENTIFIER
%token UNKNOWN

/* AST node types */
%type <node> program
%type <node> statement_list
%type <node> statement
%type <node> declaration
%type <node> assignment
%type <node> if_statement
%type <node> while_statement
%type <node> print_statement
%type <node> block
%type <node> expression
%type <node> comparison_expr
%type <node> logical_expr
%type <node> arithmetic_expr
%type <node> term
%type <node> factor
%type <node> primary

/* ============ OPERATOR PRECEDENCE & ASSOCIATIVITY ============ */
%left OR
%left AND
%left EQ NEQ
%left LT GT LE GE
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT UMINUS

/* Entry point of grammar */
%start program

%%

/* ============ GRAMMAR RULES ============ */

/*
 * PROGRAM: sequence of statements (declarations, assignments, control flow, etc.)
 * The root of the AST is created here.
 */
program
    : statement_list
        {
            program_root = $1;
            $$ = $1;
        }
    | /* empty program */
        {
            program_root = NULL;
            $$ = NULL;
        }
    ;

/*
 * STATEMENT_LIST: one or more statements
 * Creates a sequence node that links all statements together.
 */
statement_list
    : statement
        {
            $$ = $1;
        }
    | statement_list statement
        {
            /* Link statements in sequence */
            if ($1 != NULL) {
                ASTNode *temp = $1;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = $2;
                $$ = $1;
            } else {
                $$ = $2;
            }
        }
    ;

/*
 * STATEMENT: any single statement
 */
statement
    : declaration SEMICOLON
        { $$ = $1; }
    | assignment SEMICOLON
        { $$ = $1; }
    | if_statement
        { $$ = $1; }
    | while_statement
        { $$ = $1; }
    | print_statement SEMICOLON
        { $$ = $1; }
    | block
        { $$ = $1; }
    | error SEMICOLON
        {
            fprintf(stderr, "Syntax Error at line %d\n", line_number);
            $$ = NULL;
        }
    ;

/*
 * DECLARATION: type keyword followed by identifier
 * Creates a VarDeclNode with type and name information.
 */
declaration
    : INT IDENTIFIER
        {
            $$ = create_var_decl_node(strdup($2), INT_TYPE);
        }
    | FLOAT IDENTIFIER
        {
            $$ = create_var_decl_node(strdup($2), FLOAT_TYPE);
        }
    | BOOL IDENTIFIER
        {
            $$ = create_var_decl_node(strdup($2), BOOL_TYPE);
        }
    ;

/*
 * ASSIGNMENT: identifier = expression
 * Creates an AssignNode linking variable to computed expression.
 */
assignment
    : IDENTIFIER ASSIGN expression
        {
            $$ = create_assign_node(strdup($1), $3);
        }
    ;

/*
 * IF_STATEMENT: if-else control flow
 */
if_statement
    : IF LPAREN expression RPAREN statement
        {
            $$ = create_if_node($3, $5, NULL);
        }
    | IF LPAREN expression RPAREN statement ELSE statement
        {
            $$ = create_if_node($3, $5, $7);
        }
    ;

/*
 * WHILE_STATEMENT: while loop
 */
while_statement
    : WHILE LPAREN expression RPAREN statement
        {
            $$ = create_while_node($3, $5);
        }
    ;

/*
 * PRINT_STATEMENT: print expression
 */
print_statement
    : PRINT expression
        {
            $$ = create_print_node($2);
        }
    ;

/*
 * BLOCK: nested scope with braces
 * Supports variable declarations local to the block.
 */
block
    : LBRACE statement_list RBRACE
        {
            $$ = create_block_node($2);
        }
    | LBRACE RBRACE
        {
            $$ = create_block_node(NULL);
        }
    ;

/* ============ EXPRESSION RULES ============ */
/*
 * Expression hierarchy (highest to lowest precedence):
 *   logical_expr (||, &&)
 *     comparison_expr (<, >, ==, !=, <=, >=)
 *       arithmetic_expr (+, -, *, /, %)
 *         primary (literals, identifiers, parentheses)
 */

expression
    : logical_expr
        { $$ = $1; }
    ;

logical_expr
    : comparison_expr
        { $$ = $1; }
    | logical_expr OR logical_expr
        {
            $$ = create_binop_node($1, OR_OP, $3);
        }
    | logical_expr AND logical_expr
        {
            $$ = create_binop_node($1, AND_OP, $3);
        }
    | NOT logical_expr
        {
            $$ = create_unop_node(NOT_OP, $2);
        }
    ;

comparison_expr
    : arithmetic_expr
        { $$ = $1; }
    | arithmetic_expr EQ arithmetic_expr
        {
            $$ = create_binop_node($1, EQ_OP, $3);
        }
    | arithmetic_expr NEQ arithmetic_expr
        {
            $$ = create_binop_node($1, NEQ_OP, $3);
        }
    | arithmetic_expr LT arithmetic_expr
        {
            $$ = create_binop_node($1, LT_OP, $3);
        }
    | arithmetic_expr GT arithmetic_expr
        {
            $$ = create_binop_node($1, GT_OP, $3);
        }
    | arithmetic_expr LE arithmetic_expr
        {
            $$ = create_binop_node($1, LE_OP, $3);
        }
    | arithmetic_expr GE arithmetic_expr
        {
            $$ = create_binop_node($1, GE_OP, $3);
        }
    ;

arithmetic_expr
    : term
        { $$ = $1; }
    | arithmetic_expr PLUS term
        {
            $$ = create_binop_node($1, PLUS_OP, $3);
        }
    | arithmetic_expr MINUS term
        {
            $$ = create_binop_node($1, MINUS_OP, $3);
        }
    ;

term
    : factor
        { $$ = $1; }
    | term MULTIPLY factor
        {
            $$ = create_binop_node($1, MULT_OP, $3);
        }
    | term DIVIDE factor
        {
            $$ = create_binop_node($1, DIV_OP, $3);
        }
    | term MODULO factor
        {
            $$ = create_binop_node($1, MOD_OP, $3);
        }
    ;

factor
    : primary
        { $$ = $1; }
    | MINUS primary %prec UMINUS
        {
            $$ = create_unop_node(NEG_OP, $2);
        }
    ;

primary
    : INT_CONST
        {
            $$ = create_const_node(INT_TYPE, $1, 0.0);
        }
    | FLOAT_CONST
        {
            $$ = create_const_node(FLOAT_TYPE, 0, $1);
        }
    | TRUE
        {
            $$ = create_const_node(BOOL_TYPE, 1, 0.0);
        }
    | FALSE
        {
            $$ = create_const_node(BOOL_TYPE, 0, 0.0);
        }
    | IDENTIFIER
        {
            $$ = create_var_ref_node(strdup($1));
        }
    | LPAREN expression RPAREN
        {
            $$ = $2;
        }
    ;

%%

/* ============ ERROR HANDLING ============ */
void yyerror(const char *msg) {
    fprintf(stderr, "Syntax Error at line %d: %s\n", line_number, msg);
}
