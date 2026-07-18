# Quick Start Guide

Get your compiler up and running in 5 minutes!

## 1. Prerequisites

```bash
sudo apt-get install build-essential flex bison
```

## 2. Build

```bash
cd compiler-project
make
```

Expected output:
```
Generating lexer from Flex spec...
Generating parser from Bison spec...
Compiling src/main.c...
Compiling src/ast/ast.c...
[... more files ...]
Linking...
✓ Build complete: ./compiler <source-file>
```

## 3. Test It

```bash
./compiler tests/test_valid_basic.mc
```

You should see output like:
```
=== Compiler Frontend ===

Compiling: tests/test_valid_basic.mc

Phase 1: Lexical & Syntax Analysis
✓ Parsing successful

AST Structure:
[VarDecl] a : int
[VarDecl] b : int
[VarDecl] c : int
[Assignment] a =
  [Const] 5 : int
...
```

## 4. Run All Tests

```bash
make test
```

This compiles and tests all programs in `tests/` directory.

## 5. Clean Up

```bash
make clean
```

---

## Project Structure at a Glance

```
src/
  ├── lexer/lexer.l              ← Token definitions (Flex)
  ├── parser/parser.y            ← Grammar rules (Bison)
  ├── ast/                       ← AST node structures
  ├── semantic/                  ← Type checking & TAC generation
  ├── symbol_table/              ← Variable tracking
  └── main.c                     ← Compiler driver

tests/                           ← Test programs
  ├── test_valid_basic.mc
  ├── test_valid_control.mc
  ├── test_error_*.mc
  └── ...

examples/                        ← Sample programs
  ├── factorial.mc
  └── scope_example.mc

docs/
  └── PROJECT_REPORT_TEMPLATE.md ← Fill this in!

README.md                        ← Full documentation
QUICKSTART.md                    ← This file
Makefile                         ← Build instructions
```

## Key Files to Edit

1. **Grammar Modifications**: Edit `src/parser/parser.y`
2. **Token Additions**: Edit `src/lexer/lexer.l`
3. **New Test Cases**: Add `.mc` files to `tests/`
4. **Project Report**: Fill in `docs/PROJECT_REPORT_TEMPLATE.md`

## Common Commands

```bash
make           # Build compiler
make test      # Run all tests on test programs
make clean     # Remove generated files
make help      # Show build options

./compiler program.mc     # Compile a single file
```

## Example Programs

### Arithmetic
```c
// save to: my_program.mc
int x;
int y;
x = 10;
y = 20;
print x + y;
```

### Control Flow
```c
int n;
n = 5;
if (n > 0) {
    print 1;
} else {
    print 0;
}
```

### Loops
```c
int i;
i = 1;
while (i <= 10) {
    print i;
    i = i + 1;
}
```

## Error Examples

Try these to see error detection:

```c
// Error 1: Undeclared variable
int x;
print y;  // y not declared!
```

```c
// Error 2: Type mismatch
bool flag;
flag = 5 + 3;  // Can't assign int to bool
```

```c
// Error 3: Redeclaration
int x;
int x;  // Already declared!
```

## Compiler Output Explained

### ✓ Successful Compilation
```
Phase 1: Lexical & Syntax Analysis
✓ Parsing successful

AST Structure:
[nodes here]

Phase 2: Semantic Analysis
✓ Semantic analysis passed

=== Symbol Table ===
Scope 0:
  x : int (line 2)
  ...

Phase 3: Intermediate Code Generation

=== Three Address Code (TAC) ===
  [code here]

✓ Code generation successful

=== Compilation Complete ===
```

### ✗ Error
```
Semantic Error: Undeclared variable 'y' at line 5
Semantic analysis failed with 1 error(s)
```

## Next Steps

1. **Build & Test** - Run all examples
2. **Read README.md** - Understand full architecture
3. **Study the Code** - Examine each phase
4. **Create Tests** - Write your own test programs
5. **Add Features** - Try bonus features (arrays, functions, etc.)
6. **Write Report** - Fill in `PROJECT_REPORT_TEMPLATE.md`

## Troubleshooting

### Build fails with "flex: command not found"
```bash
sudo apt-get install flex
```

### Build fails with "bison: command not found"
```bash
sudo apt-get install bison
```

### Weird "undefined reference" errors
```bash
make clean
make
```

### Parser won't recognize my keywords
- Check that keywords come BEFORE the identifier pattern in `lexer.l`
- Keywords must exactly match (case-sensitive)

## Timeline Suggestion

- **Week 1**: Understand lexer/parser (Flex/Bison)
- **Week 2**: Build AST and semantic analyzer
- **Week 3**: Implement TAC generation
- **Week 4**: Testing, documentation, report
- **Week 5**: Presentation preparation

---

**Happy Compiling!** 🚀
