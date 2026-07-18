/*
 * Three Address Code (TAC) Generator Header
 *
 * Generates TAC from the validated AST.
 * TAC is a linear intermediate representation where each instruction
 * has at most 3 operands: result = operand1 op operand2
 */

#ifndef TACGEN_H
#define TACGEN_H

#include "../ast/ast.h"
#include "../symbol_table/symbol_table.h"

/* ============ TAC INSTRUCTION TYPES ============ */
typedef enum {
    TAC_ASSIGN,      /* x = y */
    TAC_BINOP,       /* x = y op z */
    TAC_UNOP,        /* x = op y */
    TAC_LABEL,       /* label: */
    TAC_JUMP,        /* goto label */
    TAC_COND_JUMP,   /* if (cond) goto label */
    TAC_PRINT        /* print x */
} TACOpType;

/* ============ TAC OPERAND ============ */
typedef struct {
    enum { OPERAND_VAR, OPERAND_CONST_INT, OPERAND_CONST_FLOAT, OPERAND_CONST_BOOL, OPERAND_LABEL } type;
    union {
        char *var_name;      /* Variable name or temp name */
        int int_val;
        float float_val;
        int bool_val;
    } value;
} TACOperand;

/* ============ TAC INSTRUCTION ============ */
typedef struct TACInstr {
    TACOpType op;
    TACOperand result;     /* Where result is stored (usually temp or variable) */
    TACOperand operand1;   /* First operand */
    TACOperand operand2;   /* Second operand (for binary ops) */
    OperatorType binary_op;/* Which binary operator (for binop instructions) */
    struct TACInstr *next;
} TACInstr;

/* ============ TAC GENERATOR ============ */

/* Create empty TAC instruction */
TACInstr* create_tac_instr();

/* Generate TAC for entire AST */
TACInstr* generate_tac(ASTNode *node, int *temp_count);

/* Generate TAC for a single node */
TACInstr* generate_tac_node(ASTNode *node, int *temp_count, TACInstr **last);

/* Generate a new temporary variable name */
char* new_temp(int *temp_count);

/* Generate a new label name */
char* new_label(int *label_count);

/* Print all TAC instructions */
void print_tac(TACInstr *program);

/* Free TAC instructions */
void free_tac(TACInstr *program);

#endif /* TACGEN_H */
