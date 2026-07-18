/*
 * Abstract Syntax Tree (AST) Implementation
 * 
 * Implements AST node creation, printing, and memory management.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* Helper function to create a generic AST node */
static ASTNode* create_node(NodeType type) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->node_type = type;
    node->next = NULL;
    node->line_number = 0;
    node->expr_type = VOID_TYPE;
    node->is_lvalue = 0;
    return node;
}

/* ============ AST CONSTRUCTION FUNCTIONS ============ */

ASTNode* create_var_decl_node(char *var_name, DataType type) {
    ASTNode *node = create_node(NODE_VAR_DECL);
    node->data.var_decl.var_name = var_name;
    node->data.var_decl.type = type;
    node->expr_type = type;
    return node;
}

ASTNode* create_assign_node(char *var_name, ASTNode *value) {
    ASTNode *node = create_node(NODE_ASSIGNMENT);
    node->data.assignment.var_name = var_name;
    node->data.assignment.value = value;
    node->is_lvalue = 1;
    return node;
}

ASTNode* create_binop_node(ASTNode *left, OperatorType op, ASTNode *right) {
    ASTNode *node = create_node(NODE_BINOP);
    node->data.binop.left = left;
    node->data.binop.op = op;
    node->data.binop.right = right;
    return node;
}

ASTNode* create_unop_node(OperatorType op, ASTNode *operand) {
    ASTNode *node = create_node(NODE_UNOP);
    node->data.unop.op = op;
    node->data.unop.operand = operand;
    return node;
}

ASTNode* create_const_node(DataType type, int int_val, float float_val) {
    ASTNode *node = create_node(NODE_CONST);
    node->data.constant.type = type;
    node->data.constant.int_val = int_val;
    node->data.constant.float_val = float_val;
    node->expr_type = type;
    return node;
}

ASTNode* create_var_ref_node(char *var_name) {
    ASTNode *node = create_node(NODE_VAR_REF);
    node->data.var_ref.var_name = var_name;
    node->is_lvalue = 1;
    return node;
}

ASTNode* create_if_node(ASTNode *cond, ASTNode *then_stmt, ASTNode *else_stmt) {
    ASTNode *node = create_node(NODE_IF);
    node->data.if_stmt.condition = cond;
    node->data.if_stmt.then_stmt = then_stmt;
    node->data.if_stmt.else_stmt = else_stmt;
    return node;
}

ASTNode* create_while_node(ASTNode *cond, ASTNode *body) {
    ASTNode *node = create_node(NODE_WHILE);
    node->data.while_stmt.condition = cond;
    node->data.while_stmt.body = body;
    return node;
}

ASTNode* create_print_node(ASTNode *expr) {
    ASTNode *node = create_node(NODE_PRINT);
    node->data.print_stmt.expression = expr;
    return node;
}

ASTNode* create_block_node(ASTNode *statements) {
    ASTNode *node = create_node(NODE_BLOCK);
    node->data.block.statements = statements;
    return node;
}

/* ============ OPERATOR NAME HELPER ============ */
static const char* op_to_string(OperatorType op) {
    switch(op) {
        case PLUS_OP: return "+";
        case MINUS_OP: return "-";
        case MULT_OP: return "*";
        case DIV_OP: return "/";
        case MOD_OP: return "%";
        case LT_OP: return "<";
        case GT_OP: return ">";
        case LE_OP: return "<=";
        case GE_OP: return ">=";
        case EQ_OP: return "==";
        case NEQ_OP: return "!=";
        case AND_OP: return "&&";
        case OR_OP: return "||";
        case NOT_OP: return "!";
        case NEG_OP: return "-";
        default: return "?";
    }
}

static const char* type_to_string(DataType type) {
    switch(type) {
        case INT_TYPE: return "int";
        case FLOAT_TYPE: return "float";
        case BOOL_TYPE: return "bool";
        case VOID_TYPE: return "void";
        default: return "unknown";
    }
}

/* ============ AST PRINTING ============ */

void print_ast(ASTNode *node, int indent) {
    if (node == NULL) {
        return;
    }
    
    char *prefix = (char*)malloc(indent + 1);
    for (int i = 0; i < indent; i++) {
        prefix[i] = ' ';
    }
    prefix[indent] = '\0';
    
    switch (node->node_type) {
        case NODE_VAR_DECL:
            printf("%s[VarDecl] %s : %s\n", prefix,
                   node->data.var_decl.var_name,
                   type_to_string(node->data.var_decl.type));
            break;
            
        case NODE_ASSIGNMENT:
            printf("%s[Assignment] %s =\n", prefix,
                   node->data.assignment.var_name);
            print_ast(node->data.assignment.value, indent + 2);
            break;
            
        case NODE_BINOP:
            printf("%s[BinOp] %s\n", prefix,
                   op_to_string(node->data.binop.op));
            print_ast(node->data.binop.left, indent + 2);
            print_ast(node->data.binop.right, indent + 2);
            break;
            
        case NODE_UNOP:
            printf("%s[UnOp] %s\n", prefix,
                   op_to_string(node->data.unop.op));
            print_ast(node->data.unop.operand, indent + 2);
            break;
            
        case NODE_CONST:
            if (node->data.constant.type == INT_TYPE) {
                printf("%s[Const] %d : int\n", prefix,
                       node->data.constant.int_val);
            } else if (node->data.constant.type == FLOAT_TYPE) {
                printf("%s[Const] %.2f : float\n", prefix,
                       node->data.constant.float_val);
            } else if (node->data.constant.type == BOOL_TYPE) {
                printf("%s[Const] %s : bool\n", prefix,
                       node->data.constant.int_val ? "true" : "false");
            }
            break;
            
        case NODE_VAR_REF:
            printf("%s[VarRef] %s\n", prefix,
                   node->data.var_ref.var_name);
            break;
            
        case NODE_IF:
            printf("%s[If]\n", prefix);
            printf("%s  Condition:\n", prefix);
            print_ast(node->data.if_stmt.condition, indent + 4);
            printf("%s  Then:\n", prefix);
            print_ast(node->data.if_stmt.then_stmt, indent + 4);
            if (node->data.if_stmt.else_stmt != NULL) {
                printf("%s  Else:\n", prefix);
                print_ast(node->data.if_stmt.else_stmt, indent + 4);
            }
            break;
            
        case NODE_WHILE:
            printf("%s[While]\n", prefix);
            printf("%s  Condition:\n", prefix);
            print_ast(node->data.while_stmt.condition, indent + 4);
            printf("%s  Body:\n", prefix);
            print_ast(node->data.while_stmt.body, indent + 4);
            break;
            
        case NODE_PRINT:
            printf("%s[Print]\n", prefix);
            print_ast(node->data.print_stmt.expression, indent + 2);
            break;
            
        case NODE_BLOCK:
            printf("%s[Block]\n", prefix);
            print_ast(node->data.block.statements, indent + 2);
            break;
            
        default:
            printf("%s[Unknown]\n", prefix);
    }
    
    /* Print next statement in sequence */
    if (node->next != NULL) {
        print_ast(node->next, indent);
    }
    
    free(prefix);
}

/* ============ AST MEMORY MANAGEMENT ============ */

void free_ast(ASTNode *node) {
    if (node == NULL) {
        return;
    }
    
    /* Recursively free child nodes based on node type */
    switch (node->node_type) {
        case NODE_VAR_DECL:
            if (node->data.var_decl.var_name) {
                free(node->data.var_decl.var_name);
            }
            break;
            
        case NODE_ASSIGNMENT:
            if (node->data.assignment.var_name) {
                free(node->data.assignment.var_name);
            }
            free_ast(node->data.assignment.value);
            break;
            
        case NODE_BINOP:
            free_ast(node->data.binop.left);
            free_ast(node->data.binop.right);
            break;
            
        case NODE_UNOP:
            free_ast(node->data.unop.operand);
            break;
            
        case NODE_VAR_REF:
            if (node->data.var_ref.var_name) {
                free(node->data.var_ref.var_name);
            }
            break;
            
        case NODE_IF:
            free_ast(node->data.if_stmt.condition);
            free_ast(node->data.if_stmt.then_stmt);
            free_ast(node->data.if_stmt.else_stmt);
            break;
            
        case NODE_WHILE:
            free_ast(node->data.while_stmt.condition);
            free_ast(node->data.while_stmt.body);
            break;
            
        case NODE_PRINT:
            free_ast(node->data.print_stmt.expression);
            break;
            
        case NODE_BLOCK:
            free_ast(node->data.block.statements);
            break;
            
        default:
            break;
    }
    
    /* Free next node in sequence */
    if (node->next != NULL) {
        free_ast(node->next);
    }
    
    free(node);
}
