# Makefile for Mini Compiler Project
# Builds Flex lexer, Bison parser, and C source files

# Compiler and tools
CC = gcc
LEX = flex
YACC = bison

# Flags
CFLAGS = -Wall -g -I. -std=c99
LDFLAGS = -lm

# Output
TARGET = compiler

# Source files
FLEX_SRC = src/lexer/lexer.l
BISON_SRC = src/parser/parser.y

# Generated files
LEX_GEN = src/lexer/lex.yy.c
BISON_GEN_C = src/parser/parser.tab.c
BISON_GEN_H = src/parser/parser.tab.h

# Hand-written source files
C_SOURCES = \
	src/main.c \
	src/ast/ast.c \
	src/symbol_table/symbol_table.c \
	src/semantic/semantic.c \
	src/semantic/tacgen.c \
	$(LEX_GEN) \
	$(BISON_GEN_C)

# Object files
OBJECTS = $(C_SOURCES:.c=.o)

# ============ BUILD RULES ============

all: $(TARGET)

# Link final executable
$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET)
	@echo "✓ Build complete: ./$(TARGET) <source-file>"

# Compile C source to object files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Generate lexer from Flex spec
$(LEX_GEN): $(FLEX_SRC)
	@echo "Generating lexer from Flex spec..."
	$(LEX) -o $(LEX_GEN) $(FLEX_SRC)

# Generate parser from Bison spec
$(BISON_GEN_C): $(BISON_SRC) $(BISON_GEN_H)
	@echo "Generating parser from Bison spec..."
	$(YACC) -d -o $(BISON_GEN_C) $(BISON_SRC)

# Dependency: parser header must exist before lexer compilation
$(BISON_GEN_H): $(BISON_SRC)
	@echo "Generating Bison header..."
	$(YACC) -d -o $(BISON_GEN_C) $(BISON_SRC)

# ============ TESTING ============

test: $(TARGET)
	@echo "Running test cases..."
	@for test in tests/*.mc; do \
		if [ -f "$$test" ]; then \
			echo "\n--- Testing $$test ---"; \
			./$(TARGET) $$test; \
		fi \
	done

# ============ CLEANUP ============

clean:
	@echo "Cleaning..."
	rm -f $(OBJECTS) $(TARGET)
	rm -f $(LEX_GEN) $(BISON_GEN_C) $(BISON_GEN_H)
	rm -f src/parser/parser.output
	@echo "✓ Clean complete"

# Clean everything including built files
distclean: clean
	rm -f $(TARGET)

# ============ PHONY TARGETS ============

.PHONY: all test clean distclean

# Show help
help:
	@echo "Mini Compiler Project - Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build the compiler"
	@echo "  make test     - Run test programs"
	@echo "  make clean    - Remove object files and generated files"
	@echo "  make distclean- Remove everything"
	@echo "  make help     - Show this help"
	@echo ""
	@echo "Usage:"
	@echo "  ./compiler <source-file.mc>"
