# Project Report: Mini Compiler Implementation

**Project Title:** Design and Implement a Mini Programming Language Compiler using Flex and Bison

**University:** Metropolitan University, Bangladesh  
**Department:** Computer Science and Engineering  
**Course:** Compiler Construction Lab  
**Date:** [Submission Date]

**Team Members:**
- [Member 1 Name] (Student ID: [XXXX])
- [Member 2 Name] (Student ID: [XXXX])
- [Member 3 Name] (Student ID: [XXXX])

---

## 1. Introduction

### 1.1 Project Context
[Describe the purpose of implementing a compiler in an academic setting. What concepts does it reinforce?]

### 1.2 Motivation
[Why is building a compiler valuable? What real-world applications exist?]

### 1.3 Objectives
This project demonstrates:
- Integration of lexical, syntax, semantic, and code generation phases
- How language features are implemented in practice
- Communication between compiler phases
- Error detection and reporting

---

## 2. Language Specification

### 2.1 Data Types
| Type | Description | Example |
|------|-------------|---------|
| int | Signed integer | 42, -17 |
| float | Floating-point | 3.14, -0.5 |
| bool | Boolean | true, false |

### 2.2 Formal Context-Free Grammar (CFG)

```
program     : statement_list
            | /* empty */
            ;

statement_list
            : statement
            | statement_list statement
            ;

statement   : declaration SEMICOLON
            | assignment SEMICOLON
            | if_statement
            | while_statement
            | print_statement SEMICOLON
            | block
            ;

declaration : INT IDENTIFIER
            | FLOAT IDENTIFIER
            | BOOL IDENTIFIER
            ;

assignment  : IDENTIFIER ASSIGN expression
            ;

if_statement
            : IF LPAREN expression RPAREN statement
            | IF LPAREN expression RPAREN statement ELSE statement
            ;

while_statement
            : WHILE LPAREN expression RPAREN statement
            ;

print_statement
            : PRINT expression
            ;

block       : LBRACE statement_list RBRACE
            | LBRACE RBRACE
            ;

expression  : logical_expr
            ;

logical_expr
            : comparison_expr
            | logical_expr OR logical_expr
            | logical_expr AND logical_expr
            | NOT logical_expr
            ;

comparison_expr
            : arithmetic_expr
            | arithmetic_expr EQ arithmetic_expr
            | arithmetic_expr NEQ arithmetic_expr
            | arithmetic_expr LT arithmetic_expr
            | arithmetic_expr GT arithmetic_expr
            | arithmetic_expr LE arithmetic_expr
            | arithmetic_expr GE arithmetic_expr
            ;

arithmetic_expr
            : term
            | arithmetic_expr PLUS term
            | arithmetic_expr MINUS term
            ;

term        : factor
            | term MULTIPLY factor
            | term DIVIDE factor
            | term MODULO factor
            ;

factor      : primary
            | MINUS primary
            ;

primary     : INT_CONST
            | FLOAT_CONST
            | TRUE
            | FALSE
            | IDENTIFIER
            | LPAREN expression RPAREN
            ;
```

### 2.3 Operator Precedence & Associativity

| Precedence (Highest → Lowest) | Operators | Associativity |
|------|-----------|---|
| 1 | `()`, `[]` | Left |
| 2 | Unary `-`, `!` | Right |
| 3 | `*`, `/`, `%` | Left |
| 4 | `+`, `-` | Left |
| 5 | `<`, `>`, `<=`, `>=` | Left |
| 6 | `==`, `!=` | Left |
| 7 | `&&` | Left |
| 8 | `\|\|` | Left |

---

## 3. Compiler Architecture

### 3.1 Overall Pipeline

```
Source Code (.mc)
       ↓
[Lexical Analyzer] (Flex)
       ↓ Token Stream
[Syntax Analyzer] (Bison)
       ↓ Abstract Syntax Tree
[Semantic Analyzer]
       ↓ Validated AST
[Code Generator]
       ↓
Three Address Code (TAC)
```

### 3.2 Module Interaction

**Flow Description:**
1. **Lexer** reads source file character-by-character
2. **Parser** receives tokens from lexer, builds AST
3. **Semantic Analyzer** walks AST, validates types & scopes
4. **Code Generator** traverses validated AST, emits TAC

**Error Handling:**
- Lexical errors: Stop at invalid character
- Syntax errors: Report and attempt error recovery
- Semantic errors: Collect all errors before stopping

---

## 4. Lexer Design

### 4.1 Token Categories

| Category | Examples | Implementation |
|----------|----------|---|
| Keywords | `int`, `float`, `bool`, `if`, `while`, `print`, `true`, `false` | Exact string match (before identifiers) |
| Identifiers | `x`, `sum`, `_temp` | `[a-zA-Z_][a-zA-Z0-9_]*` |
| Integer Constants | `42`, `0`, `-17` | `[0-9]+` |
| Float Constants | `3.14`, `0.5` | `[0-9]+\.[0-9]+` |
| Operators | `+`, `-`, `*`, `/`, `%`, `==`, `!=`, etc. | Individual patterns |
| Delimiters | `{`, `}`, `(`, `)`, `;` | Literal characters |
| Comments | `// ...` or `/* ... */` | Discarded, not tokenized |
| Whitespace | Spaces, tabs, newlines | Discarded (except for line counting) |

### 4.2 Regular Expression Patterns

**Keywords (must come BEFORE identifiers):**
```regex
"int"               { return INT; }
"float"             { return FLOAT; }
"bool"              { return BOOL; }
"if"                { return IF; }
"else"              { return ELSE; }
...
```

**Identifiers:**
```regex
[a-zA-Z_][a-zA-Z0-9_]*    { yylval.str = strdup(yytext); return IDENTIFIER; }
```

**Numbers:**
```regex
[0-9]+\.[0-9]+            { yylval.float_val = atof(yytext); return FLOAT_CONST; }
[0-9]+                    { yylval.int_val = atoi(yytext); return INT_CONST; }
```

### 4.3 Design Decisions

**Longest Match Rule:**
- Flex always consumes the longest possible match
- Ensures `==` is recognized as single token, not two `=` tokens

**Keyword vs. Identifier:**
- Keywords checked first with exact patterns
- Generic identifier pattern acts as fallback
- Avoids mistaking `integer` for keyword `int` + identifier `eger`

**Comment Handling:**
- Both `//` (line) and `/* */` (block) comments supported
- Line counter incremented for block comments (for error reporting)
- Comments discarded (not passed to parser)

---

## 5. Parser Design

### 5.1 Grammar Conflict Resolution

**Shift/Reduce Conflicts:**
[Describe any conflicts detected by Bison and how they were resolved]

**Precedence Rules:**
```
%left OR                 // Lowest precedence
%left AND
%left EQ NEQ
%left LT GT LE GE
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO  // Highest precedence
%right NOT UMINUS
```

**Associativity:**
- Left-associative: `a + b + c` = `(a + b) + c`
- Right-associative: Unary operators are right-associative

### 5.2 LALR(1) Parser Properties

[Explain what LALR(1) means and why Bison generates it]

### 5.3 Error Recovery

Bison's `error` token allows the parser to:
- Skip to next semicolon after syntax error
- Continue parsing rest of file
- Report multiple errors in one run

Example:
```
if (a b c) { ... }  // Syntax error
^--error--^
// Parser skips to next semicolon, resumes parsing
```

---

## 6. Abstract Syntax Tree

### 6.1 Node Structure

**Generic AST Node:**
```c
struct ASTNode {
    NodeType node_type;
    DataType expr_type;        // Filled by semantic analyzer
    union {
        /* Type-specific data */
    } data;
    struct ASTNode *next;      // Link for statement sequences
};
```

### 6.2 Node Types

| Node Type | Represents | Example |
|-----------|-----------|---------|
| `NODE_VAR_DECL` | Variable declaration | `int x;` |
| `NODE_ASSIGNMENT` | Assignment statement | `x = 5;` |
| `NODE_BINOP` | Binary operation | `a + b` |
| `NODE_UNOP` | Unary operation | `-x`, `!b` |
| `NODE_CONST` | Literal value | `42`, `3.14` |
| `NODE_VAR_REF` | Variable usage | `x` (in expression) |
| `NODE_IF` | If/else statement | `if (c) s1 else s2` |
| `NODE_WHILE` | While loop | `while (c) s` |
| `NODE_PRINT` | Print statement | `print x;` |
| `NODE_BLOCK` | Scoped block | `{ ... }` |

### 6.3 Construction During Parsing

**Bison Actions Example:**
```c
arithmetic_expr
    : arithmetic_expr PLUS term
        {
            $$ = create_binop_node($1, PLUS_OP, $3);
        }
    ;
```

- `$1` = left operand (result of parsing first expression)
- `$3` = right operand (result of parsing term)
- `$$` = result of this rule (new BinOp node)

### 6.4 AST Printing

**Indented Text Format:**
```
[VarDecl] x : int
[Assignment] x =
  [Const] 5 : int
[Print]
  [VarRef] x
```

---

## 7. Semantic Analysis

### 7.1 Analysis Rules

**Variable Declaration:**
- ✓ Multiple declarations allowed in different scopes
- ✗ Redeclaration in same scope → Error

**Variable Usage:**
- ✓ Variable declared in current or parent scope
- ✗ Undeclared variable → Error
- ✗ Usage outside declaring scope → Error

**Type Checking:**
- ✓ Assignment: RHS type matches or converts to LHS type
- ✗ Type mismatch: `bool x = 5 + 3.2;` → Error

**Operations:**
- Arithmetic (`+`, `-`, `*`, `/`, `%`): Require `int` or `float`
- Comparison (`<`, `>`, `<=`, `>=`, `==`, `!=`): Require `int` or `float`, return `bool`
- Logical (`&&`, `||`, `!`): Require `bool`, return `bool`

**Control Flow:**
- ✓ If/while condition is `bool`
- ✗ Numeric condition in if/while → Error

### 7.2 Type Compatibility

**Implicit Conversions:**
- `int` → `float` (automatic in assignments)
- No other implicit conversions

**Type Promotion:**
- `int + int` → `int`
- `int + float` → `float`
- `float + float` → `float`

### 7.3 Error Messages

Examples of error detection:

```
Semantic Error: Undeclared variable 'x' at line 5
Semantic Error: Variable 'y' redeclared at line 8 (first declared at line 6)
Semantic Error: Type mismatch in assignment to 'flag': expected bool, got int
Semantic Error: If condition must be boolean
```

---

## 8. Symbol Table

### 8.1 Data Structure

**Symbol Entry:**
```c
struct Symbol {
    char *name;              // Variable name
    DataType type;           // int, float, or bool
    int scope_level;         // Which nested block
    int line_declared;       // For error reporting
    struct Symbol *next;     // Linked list
};
```

### 8.2 Scope Management

**Scope Hierarchy:**
- Scope 0: Global (top-level)
- Scope 1: First nested block
- Scope 2: Block inside block
- etc.

**Variable Visibility:**
- Variable visible in declaring scope and all nested scopes
- Not visible in sibling or parent scopes
- Shadows identically-named variables in outer scopes

**Example:**
```c
int x;          // Scope 0, visible everywhere

{
    int x;      // Scope 1, shadows outer x
    print x;    // Uses Scope 1 x
}

print x;        // Uses Scope 0 x
```

### 8.3 Operations

**Insert:**
- Check if already exists in current scope → Error if yes
- Add to linked list of current scope's symbols

**Lookup:**
- Search from current scope upward to global
- Return first match found
- Return NULL if not found anywhere

**Push/Pop Scope:**
- `push_scope()`: Enter a block, increment scope level
- `pop_scope()`: Exit a block, free symbols in that scope

---

## 9. Intermediate Code Generation

### 9.1 Three Address Code Format

**Basic Instruction:**
```
result = operand1 op operand2
```

Each instruction has at most 3 operands (1 result, 2 operands).

### 9.2 Instruction Types

| Type | Example | Purpose |
|------|---------|---------|
| `TAC_ASSIGN` | `x = y` | Simple copy |
| `TAC_BINOP` | `t1 = a + b` | Binary operation |
| `TAC_UNOP` | `t2 = !flag` | Unary operation |
| `TAC_LABEL` | `L0:` | Jump target |
| `TAC_JUMP` | `goto L1` | Unconditional jump |
| `TAC_COND_JUMP` | `if cond goto L2` | Conditional jump |
| `TAC_PRINT` | `print x` | Output statement |

### 9.3 Temporary Variables

**Generation:**
- New temporary created for each sub-expression
- Named `t0`, `t1`, `t2`, etc.
- Temporary counter incremented after each creation

**Example:**
```
c = a + b * 2;
```

**TAC:**
```
t1 = b * 2      // Multiplication first (precedence)
t2 = a + t1     // Addition
c = t2          // Assignment
```

### 9.4 Control Flow (If Statement)

**Source:**
```
if (x > 0) {
    print x;
} else {
    print 0;
}
```

**TAC:**
```
if x > 0 goto L1      // Jump if true
goto L2               // Skip to else

L1:
print x               // Then branch
goto L3               // Jump to end

L2:
print 0               // Else branch

L3:                   // End label
```

### 9.5 Control Flow (While Loop)

**Source:**
```
while (n > 0) {
    print n;
    n = n - 1;
}
```

**TAC:**
```
L0:
if n > 0 goto L1      // Loop condition
goto L2               // Exit loop

L1:
print n               // Loop body
t1 = n - 1
n = t1
goto L0               // Back to loop condition

L2:                   // Exit label
```

---

## 10. Testing

### 10.1 Test Coverage

**Valid Programs:**
- [x] Basic arithmetic and assignments
- [x] Control flow (if/else, while)
- [x] Multiple statements and expressions
- [x] Nested blocks and scopes
- [x] Boolean operations

**Lexical Errors:**
- [x] Invalid characters
- [x] Malformed tokens

**Syntax Errors:**
- [x] Missing semicolons
- [x] Mismatched braces
- [x] Invalid grammar

**Semantic Errors:**
- [x] Undeclared variables
- [x] Redeclarations
- [x] Type mismatches
- [x] Scope violations
- [x] Invalid operations

### 10.2 Test Results

| Test Program | Result | Notes |
|---|---|---|
| `test_valid_basic.mc` | ✓ Pass | Arithmetic and print |
| `test_valid_control.mc` | ✓ Pass | If/while statements |
| `test_error_undeclared.mc` | ✓ Correctly rejected | Error message clear |
| `test_error_type_mismatch.mc` | ✓ Correctly rejected | Type checking works |
| `test_error_redeclaration.mc` | ✓ Correctly rejected | Scope checking works |

---

## 11. Challenges & Solutions

### Challenge 1: Flex Longest Match
**Problem:** How to distinguish keywords from identifiers?

**Solution:** Define keyword patterns before the generic identifier pattern. Flex applies longest match, then first-match-wins for same-length patterns.

### Challenge 2: Bison Ambiguity
**Problem:** Expression grammar has reduce/reduce or shift/reduce conflicts.

**Solution:** Use precedence declarations (`%left`, `%right`) and associativity rules to disambiguate.

### Challenge 3: Nested Scopes
**Problem:** How to track variables in nested blocks?

**Solution:** Symbol table maintains scope stack. On block entry, push scope. On exit, pop scope and free symbols.

### Challenge 4: Control Flow Generation
**Problem:** How to generate jumps and labels for if/while?

**Solution:** Pre-allocate label names (L0, L1, ...). Use conditional jumps (if condition goto label) for branching.

---

## 12. Lessons Learned

### Technical Insights
1. **Separation of Concerns:** Each phase is independent; changes don't cascade
2. **Data Structures:** AST is powerful representation; easy to traverse multiple times
3. **Error Recovery:** Collecting all errors (vs. stopping at first) helps users
4. **Code Generation:** Simple TAC is foundation for backend optimization

### Development Process
1. **Incremental Building:** Build and test one phase at a time
2. **Test-Driven:** Write tests before implementation
3. **Documentation:** Clear comments prevent confusion later

---

## 13. References

### Tools & Documentation
- [Flex Manual](https://westes.github.io/flex/manual/)
- [Bison Manual](https://www.gnu.org/software/bison/manual/)
- [GCC Documentation](https://gcc.gnu.org/onlinedocs/)

### Textbooks
- Aho, Lam, Sethi, Ullman. "Compilers: Principles, Techniques, and Tools" (2nd Edition)
- Appel, Palsberg. "Modern Compiler Implementation in C"

### Online Resources
- [Dragon Book Official Site](https://www.compilers.org/)
- [Flex/Bison Tutorial](https://aquamentus.com/flex_bison.html)

---

## Appendix A: Grammar Summary

[Include complete, formal CFG here]

---

## Appendix B: Sample Programs

### B.1 Factorial
```c
int n;
int result;
n = 5;
result = 1;
while (n > 1) {
    result = result * n;
    n = n - 1;
}
print result;
```

### B.2 Sum Using Loop
```c
int sum;
int i;
sum = 0;
i = 1;
while (i <= 10) {
    sum = sum + i;
    i = i + 1;
}
print sum;
```

---

**End of Report**
