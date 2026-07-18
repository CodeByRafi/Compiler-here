/*
 * Symbol Table Header
 *
 * The symbol table maintains information about all declared identifiers.
 * It supports nested scopes (variables declared in blocks).
 * 
 * Key operations:
 * - Insert: Add a new symbol (variable declaration)
 * - Lookup: Find a symbol's type and scope
 * - Push/Pop scope: Enter/exit blocks
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "../ast/ast.h"

/* ============ SYMBOL ENTRY STRUCTURE ============ */
typedef struct Symbol {
    char *name;
    DataType type;
    int scope_level;
    int line_declared;
    struct Symbol *next;
} Symbol;

/* ============ SCOPE LEVEL MANAGEMENT ============ */
/* Each nested block increases scope level */
typedef struct {
    Symbol *symbols;
    int scope_level;
} Scope;

/* ============ SYMBOL TABLE STRUCTURE ============ */
typedef struct {
    Scope *scopes;
    int num_scopes;
    int current_scope;
} SymbolTable;

/* ============ PUBLIC OPERATIONS ============ */

/* Initialize global symbol table */
SymbolTable* symbol_table_create();

/* Free symbol table */
void symbol_table_destroy(SymbolTable *table);

/* Push a new scope (entering a block) */
void push_scope(SymbolTable *table);

/* Pop current scope (exiting a block) */
void pop_scope(SymbolTable *table);

/* Insert a variable declaration */
int symbol_table_insert(SymbolTable *table, const char *name, DataType type, int line);

/* Lookup a variable (searches current and parent scopes) */
Symbol* symbol_table_lookup(SymbolTable *table, const char *name);

/* Lookup in current scope only (for redeclaration check) */
Symbol* symbol_table_lookup_current(SymbolTable *table, const char *name);

/* Print symbol table for debugging */
void symbol_table_print(SymbolTable *table);

#endif /* SYMBOL_TABLE_H */
