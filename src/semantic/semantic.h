/*
 * Semantic Analysis Header
 *
 * The semantic analyzer walks the AST and enforces rules that
 * the grammar alone cannot check:
 * - Variable declarations before use
 * - Type correctness
 * - Scope violations
 * - Invalid operations
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../ast/ast.h"
#include "../symbol_table/symbol_table.h"

/* ============ SEMANTIC ANALYSIS ============ */

/* Main semantic analysis function */
int semantic_analysis(ASTNode *program, SymbolTable *symtab);

/* Analyze and type-check a single node */
DataType analyze_node(ASTNode *node, SymbolTable *symtab);

/* Type-check an assignment (LHS type must match RHS type) */
int check_assignment_type(DataType lhs_type, DataType rhs_type);

/* Type-check binary operations */
DataType check_binop_types(ASTNode *node, SymbolTable *symtab);

/* Type-check unary operations */
DataType check_unop_types(ASTNode *node, SymbolTable *symtab);

/* Verify operand types for operators */
int is_valid_arithmetic_operand(DataType type);
int is_valid_relational_operand(DataType type);
int is_valid_logical_operand(DataType type);

#endif /* SEMANTIC_H */
