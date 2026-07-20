/*
 * Semantic Analysis Implementation
 *
 * Walks the AST and checks:
 * - Undeclared variable usage
 * - Type mismatches
 * - Scope violations
 * - Invalid operations
 */

#include <stdio.h>
#include <stdlib.h>
#include "semantic.h"

/* Global error counter */
static int semantic_errors = 0;

/* ============ TYPE CHECKING HELPERS ============ */

int is_valid_arithmetic_operand(DataType type) {
    return (type == INT_TYPE || type == FLOAT_TYPE);
}

int is_valid_relational_operand(DataType type) {
    return (type == INT_TYPE || type == FLOAT_TYPE);
}

int is_valid_logical_operand(DataType type) {
    return (type == BOOL_TYPE);
}

/* ============ ASSIGNMENT TYPE CHECKING ============ */

int check_assignment_type(DataType lhs_type, DataType rhs_type) {
    /* Allow implicit int-to-float conversion */
    if (lhs_type == FLOAT_TYPE && rhs_type == INT_TYPE) {
        return 1;
    }
    
    /* Otherwise types must match exactly */
    return (lhs_type == rhs_type);
}

/* ============ BINARY OPERATION TYPE CHECKING ============ */

DataType check_binop_types(ASTNode *node, SymbolTable *symtab) {
    if (node == NULL) {
        return VOID_TYPE;
    }
    
    ASTNode *left = node->data.binop.left;
    ASTNode *right = node->data.binop.right;
    OperatorType op = node->data.binop.op;
    
    DataType left_type = analyze_node(left, symtab);
    DataType right_type = analyze_node(right, symtab);
    
    /* Arithmetic operators: need numeric operands */
    if (op == PLUS_OP || op == MINUS_OP || op == MULT_OP ||
        op == DIV_OP || op == MOD_OP) {
        
        if (!is_valid_arithmetic_operand(left_type)) {
            fprintf(stderr, "Semantic Error: Invalid operand type for arithmetic operator\n");
            semantic_errors++;
            return VOID_TYPE;
        }
        if (!is_valid_arithmetic_operand(right_type)) {
            fprintf(stderr, "Semantic Error: Invalid operand type for arithmetic operator\n");
            semantic_errors++;
            return VOID_TYPE;
        }
        
        /* Result is float if either operand is float, else int */
        if (left_type == FLOAT_TYPE || right_type == FLOAT_TYPE) {
            return FLOAT_TYPE;
        }
        return INT_TYPE;
    }
    
    /* Relational operators: need numeric operands, return bool */
    /* For == and !=, also allow bool comparisons (e.g. flag == true) */
    if (op == LT_OP || op == GT_OP || op == LE_OP || op == GE_OP ||
        op == EQ_OP || op == NEQ_OP) {
        
        int eq_op = (op == EQ_OP || op == NEQ_OP);
        int left_ok  = is_valid_relational_operand(left_type)  || (eq_op && left_type  == BOOL_TYPE);
        int right_ok = is_valid_relational_operand(right_type) || (eq_op && right_type == BOOL_TYPE);

        if (!left_ok) {
            fprintf(stderr, "Semantic Error: Invalid operand type for relational operator\n");
            semantic_errors++;
            return VOID_TYPE;
        }
        if (!right_ok) {
            fprintf(stderr, "Semantic Error: Invalid operand type for relational operator\n");
            semantic_errors++;
            return VOID_TYPE;
        }
        
        return BOOL_TYPE;
    }
    
    /* Logical operators: need bool operands, return bool */
    if (op == AND_OP || op == OR_OP) {
        if (!is_valid_logical_operand(left_type)) {
            fprintf(stderr, "Semantic Error: Invalid operand type for logical operator\n");
            semantic_errors++;
            return VOID_TYPE;
        }
        if (!is_valid_logical_operand(right_type)) {
            fprintf(stderr, "Semantic Error: Invalid operand type for logical operator\n");
            semantic_errors++;
            return VOID_TYPE;
        }
        
        return BOOL_TYPE;
    }
    
    return VOID_TYPE;
}

/* ============ UNARY OPERATION TYPE CHECKING ============ */

DataType check_unop_types(ASTNode *node, SymbolTable *symtab) {
    if (node == NULL) {
        return VOID_TYPE;
    }
    
    ASTNode *operand = node->data.unop.operand;
    OperatorType op = node->data.unop.op;
    
    DataType operand_type = analyze_node(operand, symtab);
    
    /* Negation: need numeric operand */
    if (op == NEG_OP) {
        if (!is_valid_arithmetic_operand(operand_type)) {
            fprintf(stderr, "Semantic Error: Invalid operand type for negation\n");
            semantic_errors++;
            return VOID_TYPE;
        }
        return operand_type;
    }
    
    /* Logical NOT: need bool operand */
    if (op == NOT_OP) {
        if (!is_valid_logical_operand(operand_type)) {
            fprintf(stderr, "Semantic Error: Invalid operand type for logical NOT\n");
            semantic_errors++;
            return VOID_TYPE;
        }
        return BOOL_TYPE;
    }
    
    return VOID_TYPE;
}

/* ============ NODE ANALYSIS ============ */

DataType analyze_node(ASTNode *node, SymbolTable *symtab) {
    if (node == NULL) {
        return VOID_TYPE;
    }
    
    DataType result_type = VOID_TYPE;
    
    switch (node->node_type) {
        case NODE_VAR_DECL: {
            /* Variable declaration: register in symbol table */
            int line = node->line_number;
            symbol_table_insert(symtab, 
                              node->data.var_decl.var_name,
                              node->data.var_decl.type,
                              line);
            result_type = node->data.var_decl.type;
            break;
        }
        
        case NODE_ASSIGNMENT: {
            /* Check: variable must be declared */
            Symbol *sym = symbol_table_lookup(symtab, node->data.assignment.var_name);
            if (sym == NULL) {
                fprintf(stderr, "Semantic Error: Undeclared variable '%s'\n",
                        node->data.assignment.var_name);
                semantic_errors++;
            } else {
                /* Check: assigned value's type must match variable's type */
                DataType rhs_type = analyze_node(node->data.assignment.value, symtab);
                if (!check_assignment_type(sym->type, rhs_type)) {
                    fprintf(stderr, "Semantic Error: Type mismatch in assignment to '%s'\n",
                            node->data.assignment.var_name);
                    semantic_errors++;
                }
                
                node->expr_type = sym->type;
                result_type = sym->type;
            }
            break;
        }
        
        case NODE_BINOP: {
            result_type = check_binop_types(node, symtab);
            break;
        }
        
        case NODE_UNOP: {
            result_type = check_unop_types(node, symtab);
            break;
        }
        
        case NODE_CONST: {
            /* Constants are always valid; type is already set */
            result_type = node->data.constant.type;
            break;
        }
        
        case NODE_VAR_REF: {
            /* Check: variable must be declared */
            Symbol *sym = symbol_table_lookup(symtab, node->data.var_ref.var_name);
            if (sym == NULL) {
                fprintf(stderr, "Semantic Error: Undeclared variable '%s'\n",
                        node->data.var_ref.var_name);
                semantic_errors++;
            } else {
                node->expr_type = sym->type;
                result_type = sym->type;
            }
            break;
        }
        
        case NODE_IF: {
            /* Check: condition must be bool */
            DataType cond_type = analyze_node(node->data.if_stmt.condition, symtab);
            if (cond_type != BOOL_TYPE) {
                fprintf(stderr, "Semantic Error: If condition must be boolean\n");
                semantic_errors++;
            }
            
            /* Analyze then and else branches */
            analyze_node(node->data.if_stmt.then_stmt, symtab);
            analyze_node(node->data.if_stmt.else_stmt, symtab);
            
            result_type = VOID_TYPE;
            break;
        }
        
        case NODE_WHILE: {
            /* Check: condition must be bool */
            DataType cond_type = analyze_node(node->data.while_stmt.condition, symtab);
            if (cond_type != BOOL_TYPE) {
                fprintf(stderr, "Semantic Error: While condition must be boolean\n");
                semantic_errors++;
            }
            
            /* Analyze loop body */
            analyze_node(node->data.while_stmt.body, symtab);
            
            result_type = VOID_TYPE;
            break;
        }
        
        case NODE_PRINT: {
            /* Print can accept any type */
            analyze_node(node->data.print_stmt.expression, symtab);
            result_type = VOID_TYPE;
            break;
        }
        
        case NODE_BLOCK: {
            /* Push new scope for the block */
            push_scope(symtab);
            
            /* Analyze all statements in block */
            analyze_node(node->data.block.statements, symtab);
            
            /* Pop scope when exiting block */
            pop_scope(symtab);
            
            result_type = VOID_TYPE;
            break;
        }
        
        default:
            break;
    }
    
    /* If node has a next sibling, analyze it too */
    if (node->next != NULL) {
        analyze_node(node->next, symtab);
    }
    
    return result_type;
}

/* ============ MAIN SEMANTIC ANALYSIS ============ */

int semantic_analysis(ASTNode *program, SymbolTable *symtab) {
    if (program == NULL) {
        printf("No program to analyze\n");
        return 1;
    }
    
    semantic_errors = 0;
    analyze_node(program, symtab);
    
    if (semantic_errors > 0) {
        fprintf(stderr, "\nSemantic analysis failed with %d error(s)\n", semantic_errors);
        return 0;
    }
    
    printf("Semantic analysis passed\n");
    return 1;
}
