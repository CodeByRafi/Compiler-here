/*
 * Three Address Code (TAC) Generator Implementation
 *
 * Converts the validated AST into TAC, a linear intermediate representation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tacgen.h"

static int label_count = 0;

/* ============ HELPER FUNCTIONS ============ */

TACInstr* create_tac_instr() {
    TACInstr *instr = (TACInstr*)malloc(sizeof(TACInstr));
    instr->next = NULL;
    return instr;
}

char* new_temp(int *temp_count) {
    char *temp = (char*)malloc(20);
    sprintf(temp, "t%d", (*temp_count)++);
    return temp;
}

char* new_label(int *label_count) {
    char *label = (char*)malloc(20);
    sprintf(label, "L%d", label_count++);
    return label;
}

/* Create an operand for a variable */
static TACOperand operand_var(const char *name) {
    TACOperand op;
    op.type = OPERAND_VAR;
    op.value.var_name = (char*)malloc(strlen(name) + 1);
    strcpy(op.value.var_name, name);
    return op;
}

/* Create an operand for an integer constant */
static TACOperand operand_int(int val) {
    TACOperand op;
    op.type = OPERAND_CONST_INT;
    op.value.int_val = val;
    return op;
}

/* Create an operand for a float constant */
static TACOperand operand_float(float val) {
    TACOperand op;
    op.type = OPERAND_CONST_FLOAT;
    op.value.float_val = val;
    return op;
}

/* Create an operand for a bool constant */
static TACOperand operand_bool(int val) {
    TACOperand op;
    op.type = OPERAND_CONST_BOOL;
    op.value.bool_val = val;
    return op;
}

/* Create an operand for a label */
static TACOperand operand_label(const char *name) {
    TACOperand op;
    op.type = OPERAND_LABEL;
    op.value.var_name = (char*)malloc(strlen(name) + 1);
    strcpy(op.value.var_name, name);
    return op;
}

/* Convert AST operand to TAC operand */
static TACOperand ast_node_to_operand(ASTNode *node) {
    if (node == NULL) {
        return operand_var("error");
    }
    
    if (node->node_type == NODE_CONST) {
        if (node->data.constant.type == INT_TYPE) {
            return operand_int(node->data.constant.int_val);
        } else if (node->data.constant.type == FLOAT_TYPE) {
            return operand_float(node->data.constant.float_val);
        } else if (node->data.constant.type == BOOL_TYPE) {
            return operand_bool(node->data.constant.int_val);
        }
    } else if (node->node_type == NODE_VAR_REF) {
        return operand_var(node->data.var_ref.var_name);
    }
    
    return operand_var("error");
}

/* Get operator string for printing */
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

/* ============ TAC CODE GENERATION ============ */

TACInstr* generate_tac_node(ASTNode *node, int *temp_count, TACInstr **last) {
    if (node == NULL) {
        return NULL;
    }
    
    TACInstr *program = NULL;
    TACInstr *current = NULL;
    
    switch (node->node_type) {
        case NODE_VAR_DECL: {
            /* Variable declarations don't generate TAC */
            break;
        }
        
        case NODE_ASSIGNMENT: {
            /* x = expr  ->  t1 = expr; x = t1 */
            TACInstr *expr_code = generate_tac_node(node->data.assignment.value, temp_count, &current);
            
            if (program == NULL) {
                program = expr_code;
            }
            
            if (expr_code != NULL) {
                current = expr_code;
                while (current->next != NULL) {
                    current = current->next;
                }
            }
            
            /* Create assignment instruction */
            TACInstr *assign = create_tac_instr();
            assign->op = TAC_ASSIGN;
            assign->result = operand_var(node->data.assignment.var_name);
            assign->operand1 = ast_node_to_operand(node->data.assignment.value);
            
            if (program == NULL) {
                program = assign;
            } else {
                current->next = assign;
            }
            current = assign;
            
            break;
        }
        
        case NODE_BINOP: {
            /* x = y op z */
            TACInstr *left_code = generate_tac_node(node->data.binop.left, temp_count, &current);
            TACInstr *right_code = generate_tac_node(node->data.binop.right, temp_count, &current);
            
            if (program == NULL) {
                program = left_code;
            }
            
            if (program != NULL) {
                current = program;
                while (current->next != NULL) {
                    current = current->next;
                }
            }
            
            if (right_code != NULL) {
                if (program == NULL) {
                    program = right_code;
                } else {
                    current->next = right_code;
                }
                current = right_code;
                while (current->next != NULL) {
                    current = current->next;
                }
            }
            
            /* Create binop instruction */
            TACInstr *binop = create_tac_instr();
            binop->op = TAC_BINOP;
            binop->result = operand_var(new_temp(temp_count));
            binop->operand1 = ast_node_to_operand(node->data.binop.left);
            binop->operand2 = ast_node_to_operand(node->data.binop.right);
            binop->binary_op = node->data.binop.op;
            
            if (program == NULL) {
                program = binop;
            } else {
                current->next = binop;
            }
            current = binop;
            
            break;
        }
        
        case NODE_UNOP: {
            /* x = op y */
            TACInstr *operand_code = generate_tac_node(node->data.unop.operand, temp_count, &current);
            
            if (program == NULL) {
                program = operand_code;
            }
            
            if (operand_code != NULL) {
                current = operand_code;
                while (current->next != NULL) {
                    current = current->next;
                }
            }
            
            /* Create unop instruction */
            TACInstr *unop = create_tac_instr();
            unop->op = TAC_UNOP;
            unop->result = operand_var(new_temp(temp_count));
            unop->operand1 = ast_node_to_operand(node->data.unop.operand);
            unop->binary_op = node->data.unop.op;
            
            if (program == NULL) {
                program = unop;
            } else {
                current->next = unop;
            }
            current = unop;
            
            break;
        }
        
        case NODE_PRINT: {
            /* print expr */
            TACInstr *expr_code = generate_tac_node(node->data.print_stmt.expression, temp_count, &current);
            
            if (program == NULL) {
                program = expr_code;
            }
            
            if (expr_code != NULL) {
                current = expr_code;
                while (current->next != NULL) {
                    current = current->next;
                }
            }
            
            /* Create print instruction */
            TACInstr *print = create_tac_instr();
            print->op = TAC_PRINT;
            print->operand1 = ast_node_to_operand(node->data.print_stmt.expression);
            
            if (program == NULL) {
                program = print;
            } else {
                current->next = print;
            }
            current = print;
            
            break;
        }
        
        case NODE_IF: {
            /* if (cond) then_stmt else else_stmt */
            char *else_label = new_label(&label_count);
            char *end_label = new_label(&label_count);
            
            /* Generate condition code */
            TACInstr *cond_code = generate_tac_node(node->data.if_stmt.condition, temp_count, &current);
            
            if (program == NULL) {
                program = cond_code;
            }
            
            if (cond_code != NULL) {
                current = cond_code;
                while (current->next != NULL) {
                    current = current->next;
                }
            }
            
            /* Jump if not true */
            TACInstr *jump_else = create_tac_instr();
            jump_else->op = TAC_COND_JUMP;
            jump_else->operand1 = ast_node_to_operand(node->data.if_stmt.condition);
            jump_else->operand2 = operand_label(else_label);
            
            if (program == NULL) {
                program = jump_else;
            } else {
                current->next = jump_else;
            }
            current = jump_else;
            
            /* Then code */
            TACInstr *then_code = generate_tac_node(node->data.if_stmt.then_stmt, temp_count, &current);
            if (then_code != NULL) {
                current->next = then_code;
                while (current->next != NULL) {
                    current = current->next;
                }
            }
            
            /* Jump to end */
            TACInstr *jump_end = create_tac_instr();
            jump_end->op = TAC_JUMP;
            jump_end->operand1 = operand_label(end_label);
            current->next = jump_end;
            current = jump_end;
            
            /* Else label */
            TACInstr *else_lbl = create_tac_instr();
            else_lbl->op = TAC_LABEL;
            else_lbl->result = operand_label(else_label);
            current->next = else_lbl;
            current = else_lbl;
            
            /* Else code (if exists) */
            if (node->data.if_stmt.else_stmt != NULL) {
                TACInstr *else_code = generate_tac_node(node->data.if_stmt.else_stmt, temp_count, &current);
                if (else_code != NULL) {
                    current->next = else_code;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                }
            }
            
            /* End label */
            TACInstr *end_lbl = create_tac_instr();
            end_lbl->op = TAC_LABEL;
            end_lbl->result = operand_label(end_label);
            current->next = end_lbl;
            current = end_lbl;
            
            free(else_label);
            free(end_label);
            
            break;
        }
        
        case NODE_WHILE: {
            /* while (cond) body */
            char *loop_label = new_label(&label_count);
            char *exit_label = new_label(&label_count);
            
            /* Loop label */
            TACInstr *loop_lbl = create_tac_instr();
            loop_lbl->op = TAC_LABEL;
            loop_lbl->result = operand_label(loop_label);
            
            program = loop_lbl;
            current = loop_lbl;
            
            /* Generate condition code */
            TACInstr *cond_code = generate_tac_node(node->data.while_stmt.condition, temp_count, &current);
            if (cond_code != NULL) {
                current->next = cond_code;
                while (current->next != NULL) {
                    current = current->next;
                }
            }
            
            /* Jump to exit if false */
            TACInstr *jump_exit = create_tac_instr();
            jump_exit->op = TAC_COND_JUMP;
            jump_exit->operand1 = ast_node_to_operand(node->data.while_stmt.condition);
            jump_exit->operand2 = operand_label(exit_label);
            current->next = jump_exit;
            current = jump_exit;
            
            /* Loop body */
            TACInstr *body_code = generate_tac_node(node->data.while_stmt.body, temp_count, &current);
            if (body_code != NULL) {
                current->next = body_code;
                while (current->next != NULL) {
                    current = current->next;
                }
            }
            
            /* Jump back to loop label */
            TACInstr *jump_back = create_tac_instr();
            jump_back->op = TAC_JUMP;
            jump_back->operand1 = operand_label(loop_label);
            current->next = jump_back;
            current = jump_back;
            
            /* Exit label */
            TACInstr *exit_lbl = create_tac_instr();
            exit_lbl->op = TAC_LABEL;
            exit_lbl->result = operand_label(exit_label);
            current->next = exit_lbl;
            
            free(loop_label);
            free(exit_label);
            
            break;
        }
        
        case NODE_BLOCK: {
            /* Generate code for all statements in block */
            program = generate_tac_node(node->data.block.statements, temp_count, &current);
            break;
        }
        
        default:
            break;
    }
    
    /* If this node has a next sibling, generate code for it too */
    if (node->next != NULL) {
        TACInstr *next_code = generate_tac_node(node->next, temp_count, &current);
        if (next_code != NULL) {
            if (program == NULL) {
                program = next_code;
            } else {
                if (current == NULL) {
                    current = program;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                }
                current->next = next_code;
            }
        }
    }
    
    if (last != NULL) {
        *last = current;
    }
    
    return program;
}

TACInstr* generate_tac(ASTNode *node, int *temp_count) {
    TACInstr *last = NULL;
    return generate_tac_node(node, temp_count, &last);
}

/* ============ TAC PRINTING ============ */

static void print_operand(TACOperand op) {
    switch (op.type) {
        case OPERAND_VAR:
            printf("%s", op.value.var_name);
            break;
        case OPERAND_CONST_INT:
            printf("%d", op.value.int_val);
            break;
        case OPERAND_CONST_FLOAT:
            printf("%.2f", op.value.float_val);
            break;
        case OPERAND_CONST_BOOL:
            printf("%s", op.value.bool_val ? "true" : "false");
            break;
        case OPERAND_LABEL:
            printf("%s", op.value.var_name);
            break;
    }
}

void print_tac(TACInstr *program) {
    printf("\n=== Three Address Code (TAC) ===\n");
    
    for (TACInstr *instr = program; instr != NULL; instr = instr->next) {
        switch (instr->op) {
            case TAC_ASSIGN:
                printf("  ");
                print_operand(instr->result);
                printf(" = ");
                print_operand(instr->operand1);
                printf("\n");
                break;
                
            case TAC_BINOP:
                printf("  ");
                print_operand(instr->result);
                printf(" = ");
                print_operand(instr->operand1);
                printf(" %s ", op_to_string(instr->binary_op));
                print_operand(instr->operand2);
                printf("\n");
                break;
                
            case TAC_UNOP:
                printf("  ");
                print_operand(instr->result);
                printf(" = %s", op_to_string(instr->binary_op));
                print_operand(instr->operand1);
                printf("\n");
                break;
                
            case TAC_LABEL:
                printf("%s:\n", instr->result.value.var_name);
                break;
                
            case TAC_JUMP:
                printf("  goto ");
                print_operand(instr->operand1);
                printf("\n");
                break;
                
            case TAC_COND_JUMP:
                printf("  if ");
                print_operand(instr->operand1);
                printf(" goto ");
                print_operand(instr->operand2);
                printf("\n");
                break;
                
            case TAC_PRINT:
                printf("  print ");
                print_operand(instr->operand1);
                printf("\n");
                break;
        }
    }
    
    printf("\n");
}

/* ============ TAC MEMORY MANAGEMENT ============ */

void free_tac(TACInstr *program) {
    while (program != NULL) {
        TACInstr *temp = program;
        program = program->next;
        
        /* Free operand strings */
        if (temp->result.type == OPERAND_VAR ||
            temp->result.type == OPERAND_LABEL) {
            free(temp->result.value.var_name);
        }
        if (temp->operand1.type == OPERAND_VAR ||
            temp->operand1.type == OPERAND_LABEL) {
            free(temp->operand1.value.var_name);
        }
        if (temp->operand2.type == OPERAND_VAR ||
            temp->operand2.type == OPERAND_LABEL) {
            free(temp->operand2.value.var_name);
        }
        
        free(temp);
    }
}
