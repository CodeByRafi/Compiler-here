@echo off
echo Building the Mini Compiler...

echo Generating lexer...
.\tools\win_flex.exe -o src\lexer\lex.yy.c src\lexer\lexer.l

echo Generating parser...
.\tools\win_bison.exe -d -o src\parser\parser.tab.c src\parser\parser.y

echo Compiling C code...
gcc -Wall -g -I. -std=c99 src\main.c src\ast\ast.c src\symbol_table\symbol_table.c src\semantic\semantic.c src\semantic\tacgen.c src\lexer\lex.yy.c src\parser\parser.tab.c -o compiler.exe

if %errorlevel% neq 0 (
    echo.
    echo Build failed!
) else (
    echo.
    echo Build complete! Run it with: compiler.exe tests\test_valid_basic.mc
)
