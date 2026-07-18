/*
 * Main Compiler Driver
 *
 * This is the entry point of the compiler.
 * It orchestrates the full pipeline:
 *   Lexical Analysis (Flex)
 *     -> Syntax Analysis (Bison)
 *       -> AST Construction
 *         -> Semantic Analysis
 *           -> TAC Generation
 */

#include <stdio.h>
#include <stdlib.h>
#include "ast/ast.h"
#include "symbol_table/symbol_table.h"
#include "semantic/semantic.h"
#include "semantic/tacgen.h"

/* Forward declarations from Bison */
extern ASTNode *program_root;
extern int yyparse(void);
extern FILE *yyin;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source file>\n", argv[0]);
        fprintf(stderr, "Example: %s program.mc\n", argv[0]);
        return 1;
    }
    
    const char *filename = argv[1];
    
    /* ============ PHASE 1: LEXICAL & SYNTAX ANALYSIS ============ */
    printf("=== Compiler Frontend ===\n\n");
    printf("Compiling: %s\n\n", filename);
    
    yyin = fopen(filename, "r");
    if (yyin == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return 1;
    }
    
    printf("Phase 1: Lexical & Syntax Analysis\n");
    int parse_result = yyparse();
    fclose(yyin);
    
    if (parse_result != 0) {
        fprintf(stderr, "Parsing failed\n");
        return 1;
    }
    
    if (program_root == NULL) {
        printf("(Empty program)\n");
        return 0;
    }
    
    /* ============ PHASE 2: AST CONSTRUCTION & PRINTING ============ */
    printf("✓ Parsing successful\n\n");
    
    printf("AST Structure:\n");
    print_ast(program_root, 0);
    
    /* ============ PHASE 3: SEMANTIC ANALYSIS ============ */
    printf("\nPhase 2: Semantic Analysis\n");
    
    SymbolTable *symtab = symbol_table_create();
    
    if (!semantic_analysis(program_root, symtab)) {
        fprintf(stderr, "Semantic analysis failed\n");
        symbol_table_destroy(symtab);
        free_ast(program_root);
        return 1;
    }
    
    printf("✓ Semantic analysis passed\n");
    symbol_table_print(symtab);
    
    /* ============ PHASE 4: INTERMEDIATE CODE GENERATION ============ */
    printf("\nPhase 3: Intermediate Code Generation\n");
    
    int temp_count = 0;
    TACInstr *tac_code = generate_tac(program_root, &temp_count);
    
    if (tac_code == NULL) {
        printf("(No code generated)\n");
    } else {
        print_tac(tac_code);
    }
    
    printf("✓ Code generation successful\n");
    printf("\n=== Compilation Complete ===\n");
    
    /* ============ CLEANUP ============ */
    if (tac_code != NULL) {
        free_tac(tac_code);
    }
    symbol_table_destroy(symtab);
    free_ast(program_root);
    
    return 0;
}
