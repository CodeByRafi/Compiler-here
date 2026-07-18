/*
 * Abstract Syntax Tree (AST) Definitions
 *
 * This file defines the structure of all AST node types.
 * Each node represents a language construct (statement, expression, etc.)
 * and contains pointers to child nodes.
 */

#ifndef AST_H
#define AST_H

/* ============ DATA TYPE ENUMERATION ============ */
typedef enum {
    INT_TYPE,
    FLOAT_TYPE,
    BOOL_TYPE,
    VOID_TYPE
} DataType;

/* ============ NODE TYPE ENUMERATION ============ */
typedef enum {
    NODE_PROGRAM,
    NODE_VAR_DECL,
    NODE_ASSIGNMENT,
    NODE_BINOP,
    NODE_UNOP,
    NODE_CONST,
    NODE_VAR_REF,
    NODE_IF,
    NODE_WHILE,
    NODE_PRINT,
    NODE_BLOCK
} NodeType;

/* ============ OPERATOR ENUMERATION ============ */
typedef enum {
    /* Arithmetic */
    PLUS_OP, MINUS_OP, MULT_OP, DIV_OP, MOD_OP,
    /* Relational */
    LT_OP, GT_OP, LE_OP, GE_OP, EQ_OP, NEQ_OP,
    /* Logical */
    AND_OP, OR_OP, NOT_OP,
    /* Unary */
    NEG_OP
} OperatorType;

/* ============ AST NODE STRUCTURE ============ */
typedef struct ASTNode {
    NodeType node_type;
    int line_number;
    
    /* For linking statements in sequence */
    struct ASTNode *next;
    
    /* Type annotation (filled by semantic analyzer) */
    DataType expr_type;
    int is_lvalue;  /* Can this node be assigned to? */
    char *tac_place; /* Where the result is stored in TAC */
    
    /* Node-specific data */
    union {
        /* VAR_DECL: int x; */
        struct {
            char *var_name;
            DataType type;
        } var_decl;
        
        /* ASSIGNMENT: x = expr */
        struct {
            char *var_name;
            struct ASTNode *value;
        } assignment;
        
        /* BINOP: left OP right (e.g., a + b) */
        struct {
            struct ASTNode *left;
            OperatorType op;
            struct ASTNode *right;
        } binop;
        
        /* UNOP: OP operand (e.g., !x, -5) */
        struct {
            OperatorType op;
            struct ASTNode *operand;
        } unop;
        
        /* CONST: integer/float/bool literal */
        struct {
            DataType type;
            int int_val;
            float float_val;
        } constant;
        
        /* VAR_REF: identifier use (e.g., x in x + 1) */
        struct {
            char *var_name;
        } var_ref;
        
        /* IF: if (cond) stmt [else stmt] */
        struct {
            struct ASTNode *condition;
            struct ASTNode *then_stmt;
            struct ASTNode *else_stmt;  /* NULL if no else */
        } if_stmt;
        
        /* WHILE: while (cond) stmt */
        struct {
            struct ASTNode *condition;
            struct ASTNode *body;
        } while_stmt;
        
        /* PRINT: print expr */
        struct {
            struct ASTNode *expression;
        } print_stmt;
        
        /* BLOCK: { statements } */
        struct {
            struct ASTNode *statements;
        } block;
        
    } data;
    
} ASTNode;

/* ============ AST CONSTRUCTION FUNCTIONS ============ */

/* Create a variable declaration node: int x; */
ASTNode* create_var_decl_node(char *var_name, DataType type);

/* Create an assignment node: x = expr */
ASTNode* create_assign_node(char *var_name, ASTNode *value);

/* Create a binary operation node: left OP right */
ASTNode* create_binop_node(ASTNode *left, OperatorType op, ASTNode *right);

/* Create a unary operation node: OP operand */
ASTNode* create_unop_node(OperatorType op, ASTNode *operand);

/* Create a constant node: 42, 3.14, true */
ASTNode* create_const_node(DataType type, int int_val, float float_val);

/* Create a variable reference node: x (when used in expression) */
ASTNode* create_var_ref_node(char *var_name);

/* Create an if statement node */
ASTNode* create_if_node(ASTNode *cond, ASTNode *then_stmt, ASTNode *else_stmt);

/* Create a while statement node */
ASTNode* create_while_node(ASTNode *cond, ASTNode *body);

/* Create a print statement node */
ASTNode* create_print_node(ASTNode *expr);

/* Create a block node */
ASTNode* create_block_node(ASTNode *statements);

/* ============ AST UTILITY FUNCTIONS ============ */

/* Print AST in readable format (indented text) */
void print_ast(ASTNode *node, int indent);

/* Free all AST memory */
void free_ast(ASTNode *node);

#endif /* AST_H */
