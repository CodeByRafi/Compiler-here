# Mini Compiler Project

**Metropolitan University, Bangladesh**  
**Department of Computer Science and Engineering**  
**Compiler Construction Lab**

## Project Overview

This is a complete compiler front-end implementation for a custom mini-programming language. It demonstrates the integration of all compiler phases:

1. **Lexical Analysis** (Flex) - Tokenizes source code
2. **Syntax Analysis** (Bison) - Builds Abstract Syntax Tree
3. **Semantic Analysis** - Type checking and error detection
4. **Intermediate Code Generation** - Generates Three Address Code (TAC)

## Project Structure

```
project-root/
├── docs/                          # Documentation
├── src/
│   ├── lexer/
│   │   └── lexer.l               # Flex specification
│   ├── parser/
│   │   └── parser.y              # Bison grammar
│   ├── ast/
│   │   ├── ast.h                 # AST node definitions
│   │   └── ast.c                 # AST construction & printing
│   ├── semantic/
│   │   ├── semantic.h            # Semantic analyzer interface
│   │   ├── semantic.c            # Type checking & validation
│   │   ├── tacgen.h              # TAC generator interface
│   │   └── tacgen.c              # TAC code generation
│   ├── symbol_table/
│   │   ├── symbol_table.h        # Symbol table interface
│   │   └── symbol_table.c        # Variable tracking & scopes
│   └── main.c                    # Compiler driver
├── tests/                        # Test programs (valid & invalid)
├── examples/                     # Example programs
├── Makefile                      # Build configuration
└── README.md                     # This file
```

## Language Specification

### Data Types
- **int** - Signed integer
- **float** - Floating-point number
- **bool** - Boolean (true/false)

### Statements
- Variable declaration: `int x;`
- Assignment: `x = 5;`
- Control flow: `if`, `if-else`, `while`
- I/O: `print x;`
- Nested blocks: `{ ... }`

### Operators

| Category | Operators |
|----------|-----------|
| Arithmetic | `+`, `-`, `*`, `/`, `%` |
| Relational | `<`, `>`, `<=`, `>=`, `==`, `!=` |
| Logical | `&&`, `||`, `!` |

### Example Program

```c
int x;
int y;
bool flag;

x = 10;
y = 0;
flag = true;

while (x > 0) {
    y = y + x;
    x = x - 1;
}

if (flag == true) {
    print y;
} else {
    print x;
}
```

## Prerequisites

- **Linux/Unix environment** (Ubuntu/Debian recommended)
- **GCC** - C compiler
- **Flex** - Lexical analyzer generator
- **Bison** - Parser generator
- **Make** - Build tool

### Installation (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install build-essential flex bison
```

## Building the Compiler

```bash
make
```

This will:
1. Generate the lexer from `lexer.l` (Flex)
2. Generate the parser from `parser.y` (Bison)
3. Compile all C source files
4. Link into executable `./compiler`

### Cleaning Build Files

```bash
make clean        # Remove object files and generated files
make distclean    # Remove everything including executable
```

## Usage

### Basic Usage

```bash
./compiler <source-file>
```

### Example

```bash
./compiler examples/factorial.mc
```

The compiler will output:
- **AST Structure** - Visual representation of the parse tree
- **Semantic Analysis Results** - Type checking status
- **Three Address Code (TAC)** - Intermediate representation

### Running Tests

```bash
make test
```

This runs the compiler on all test programs in `tests/` directory.

## Compiler Output

### Successful Compilation

```
=== Compiler Frontend ===

Compiling: examples/factorial.mc

Phase 1: Lexical & Syntax Analysis
✓ Parsing successful

AST Structure:
[VarDecl] n : int
[VarDecl] result : int
[Assignment] n =
  [Const] 5 : int
...

Phase 2: Semantic Analysis
✓ Semantic analysis passed

=== Symbol Table ===
Scope 0:
  n : int (line 2)
  result : int (line 3)
...

Phase 3: Intermediate Code Generation

=== Three Address Code (TAC) ===
  n = 5
  result = 1
  L0:
  if n > 1 goto L1
  t1 = result * n
  result = t1
  t2 = n - 1
  n = t2
  goto L0
  L1:
  print result

✓ Code generation successful

=== Compilation Complete ===
```

### Error Reporting

The compiler detects and reports:

1. **Lexical Errors** - Invalid characters or tokens
2. **Syntax Errors** - Grammar violations with line numbers
3. **Semantic Errors** - Type mismatches, undeclared variables, scope violations

Example error output:
```
Semantic Error: Undeclared variable 'x' at line 5
Semantic Error: Variable 'y' redeclared at line 8 (first declared at line 6)
```

## Test Cases

### Valid Programs

- `tests/test_valid_basic.mc` - Basic arithmetic and print
- `tests/test_valid_control.mc` - If/while control flow
- `examples/factorial.mc` - Factorial computation
- `examples/scope_example.mc` - Nested scopes

### Error Cases

- `tests/test_error_undeclared.mc` - Using undefined variable
- `tests/test_error_type_mismatch.mc` - Invalid type assignment
- `tests/test_error_redeclaration.mc` - Duplicate variable declaration

## Compiler Phases Explained

### Phase 1: Lexical & Syntax Analysis

**Lexer** (Flex - `lexer.l`):
- Tokenizes input character-by-character
- Recognizes keywords, identifiers, operators, constants
- Discards comments and whitespace
- Reports lexical errors with line numbers

**Parser** (Bison - `parser.y`):
- Consumes token stream
- Validates grammar using Context-Free Grammar (CFG)
- Constructs Abstract Syntax Tree (AST) during parsing
- Performs error recovery for graceful compilation

### Phase 2: Semantic Analysis (`semantic.c`)

Walks the AST and checks:
- **Undeclared Variables** - Usage before declaration
- **Type Correctness** - Operand types match operations
- **Scope Violations** - Variables accessible in their scope
- **Redeclarations** - No duplicate variable names in same scope

### Phase 3: Intermediate Code Generation (`tacgen.c`)

Converts AST to Three Address Code (TAC):
- Simple assignment: `x = y`
- Binary operations: `t1 = a + b` (temp variables created as needed)
- Control flow: Labels and jumps for if/while
- Print statements: Direct output instructions

## Key Design Decisions

### 1. Nested Scopes
Symbol table supports hierarchical scoping:
- Each block `{ ... }` creates a new scope
- Variables in inner scopes shadow outer scopes
- Scope is popped upon block exit

### 2. Type System
- Implicit int-to-float conversion allowed
- Arithmetic operators on numeric types only
- Relational operators return bool
- Logical operators require bool operands

### 3. AST-Driven Compilation
All analysis happens by traversing the AST:
- Enables multi-pass compilation
- Supports error recovery
- Clear separation of phases

### 4. TAC Representation
- Linear instructions suitable for backend compilation
- Temporary variables created for complex expressions
- Labels for control flow (if/while jumps)

## Example Walkthrough

Let's trace `tests/test_valid_basic.mc`:

**Source Code:**
```c
int a;
int b;
int c;

a = 5;
b = 10;
c = a + b;

print c;
```

**Lexer Output (tokens):**
```
INT IDENTIFIER("a") SEMICOLON
INT IDENTIFIER("b") SEMICOLON
...
```

**Parser (builds AST):**
```
VarDecl(a, int)
VarDecl(b, int)
VarDecl(c, int)
Assignment(a, Const(5))
Assignment(b, Const(10))
Assignment(c, BinOp(VarRef(a), +, VarRef(b)))
Print(VarRef(c))
```

**Semantic Analysis:**
- ✓ All variables declared before use
- ✓ Types match in assignments
- ✓ No scope violations

**TAC Generation:**
```
a = 5
b = 10
t1 = a + b
c = t1
print c
```

## Extending the Compiler (Bonus Features)

Possible enhancements:
1. **Arrays** - Array declarations and indexing
2. **Functions** - Function calls and parameters
3. **for/do-while loops** - Additional control flow
4. **Operators** - Unary ++/-- or compound assignments
5. **Optimizations** - Constant folding, dead code elimination
6. **Visualization** - Graphviz AST rendering
7. **GUI** - Interactive compiler frontend

See Section 14 of the project manual for details.

## Common Issues & Solutions

### Issue: "flex: command not found"
**Solution:** Install Flex
```bash
sudo apt-get install flex
```

### Issue: "bison: command not found"
**Solution:** Install Bison
```bash
sudo apt-get install bison
```

### Issue: Undefined reference errors during linking
**Solution:** Ensure all `.c` files are compiled. Check Makefile dependencies.

### Issue: Parser conflicts (shift/reduce)
**Ensure precedence is properly defined in Bison (`%left`, `%right`, etc.)`

## Project Evaluation

The project is evaluated on:
1. **Correctness** (50%) - Correct compilation and error detection
2. **Code Quality** (20%) - Clean, modular, well-commented code
3. **Documentation** (20%) - Clear README and inline comments
4. **Testing** (10%) - Comprehensive test cases

## References

- **Flex Manual** - https://westes.github.io/flex/manual/
- **Bison Manual** - https://www.gnu.org/software/bison/manual/
- **Compiler Textbook** - "Compilers: Principles, Techniques, and Tools" (Dragon Book)

## Team Members

*Add team member names here*

## Submission

- **Deadline:** 31 July (no extensions)
- **Deliverables:**
  - Complete source code with all modules
  - GitHub repository with meaningful commits
  - This README with build/run instructions
  - Project report (details in docs/)
  - Test programs and outputs
  - Presentation slides

---

**Last Updated:** July 2026  
**University:** Metropolitan University, Bangladesh
