/*
 * Symbol Table Implementation
 * 
 * Manages variable declarations with support for nested scopes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

#define INITIAL_SCOPES 10

/* ============ SYMBOL TABLE CREATION ============ */

SymbolTable* symbol_table_create() {
    SymbolTable *table = (SymbolTable*)malloc(sizeof(SymbolTable));
    table->scopes = (Scope*)malloc(sizeof(Scope) * INITIAL_SCOPES);
    table->num_scopes = INITIAL_SCOPES;
    table->current_scope = 0;
    
    /* Initialize global scope (scope 0) */
    table->scopes[0].symbols = NULL;
    table->scopes[0].scope_level = 0;
    
    return table;
}

void symbol_table_destroy(SymbolTable *table) {
    if (table == NULL) {
        return;
    }
    
    /* Free all symbols in all scopes */
    for (int i = 0; i <= table->current_scope; i++) {
        Symbol *sym = table->scopes[i].symbols;
        while (sym != NULL) {
            Symbol *temp = sym;
            sym = sym->next;
            free(temp->name);
            free(temp);
        }
    }
    
    free(table->scopes);
    free(table);
}

/* ============ SCOPE MANAGEMENT ============ */

void push_scope(SymbolTable *table) {
    table->current_scope++;
    
    /* Resize scopes array if needed */
    if (table->current_scope >= table->num_scopes) {
        table->num_scopes *= 2;
        table->scopes = (Scope*)realloc(table->scopes, sizeof(Scope) * table->num_scopes);
    }
    
    /* Initialize new scope */
    table->scopes[table->current_scope].symbols = NULL;
    table->scopes[table->current_scope].scope_level = table->current_scope;
}

void pop_scope(SymbolTable *table) {
    if (table->current_scope == 0) {
        fprintf(stderr, "Error: Cannot pop global scope\n");
        return;
    }
    
    /* Free all symbols in current scope */
    Symbol *sym = table->scopes[table->current_scope].symbols;
    while (sym != NULL) {
        Symbol *temp = sym;
        sym = sym->next;
        free(temp->name);
        free(temp);
    }
    
    table->scopes[table->current_scope].symbols = NULL;
    table->current_scope--;
}

/* ============ SYMBOL INSERTION ============ */

int symbol_table_insert(SymbolTable *table, const char *name, DataType type, int line) {
    /* Check if already declared in current scope */
    Symbol *existing = symbol_table_lookup_current(table, name);
    if (existing != NULL) {
        fprintf(stderr, "Semantic Error: Variable '%s' redeclared at line %d (first declared at line %d)\n",
                name, line, existing->line_declared);
        return 0;  /* Insertion failed */
    }
    
    /* Create new symbol */
    Symbol *sym = (Symbol*)malloc(sizeof(Symbol));
    sym->name = (char*)malloc(strlen(name) + 1);
    strcpy(sym->name, name);
    sym->type = type;
    sym->scope_level = table->current_scope;
    sym->line_declared = line;
    
    /* Insert at head of current scope's symbol list */
    sym->next = table->scopes[table->current_scope].symbols;
    table->scopes[table->current_scope].symbols = sym;
    
    return 1;  /* Insertion succeeded */
}

/* ============ SYMBOL LOOKUP ============ */

Symbol* symbol_table_lookup(SymbolTable *table, const char *name) {
    /* Search from current scope up to global scope */
    for (int i = table->current_scope; i >= 0; i--) {
        Symbol *sym = table->scopes[i].symbols;
        while (sym != NULL) {
            if (strcmp(sym->name, name) == 0) {
                return sym;
            }
            sym = sym->next;
        }
    }
    return NULL;  /* Not found */
}

Symbol* symbol_table_lookup_current(SymbolTable *table, const char *name) {
    /* Search only in current scope */
    Symbol *sym = table->scopes[table->current_scope].symbols;
    while (sym != NULL) {
        if (strcmp(sym->name, name) == 0) {
            return sym;
        }
        sym = sym->next;
    }
    return NULL;
}

/* ============ DEBUGGING ============ */

void symbol_table_print(SymbolTable *table) {
    printf("\n=== Symbol Table ===\n");
    
    for (int i = 0; i <= table->current_scope; i++) {
        printf("Scope %d:\n", i);
        Symbol *sym = table->scopes[i].symbols;
        if (sym == NULL) {
            printf("  (empty)\n");
        } else {
            while (sym != NULL) {
                printf("  %s : %s (line %d)\n",
                       sym->name,
                       (sym->type == INT_TYPE) ? "int" :
                       (sym->type == FLOAT_TYPE) ? "float" : "bool",
                       sym->line_declared);
                sym = sym->next;
            }
        }
    }
    printf("\n");
}
