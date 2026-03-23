# Python-like Language Compiled to C

WooLang is a simple educational programming language inspired by Python,  
designed to demonstrate how compilers work by translating high-level code into C.

Author: Damian Łuczak  
University: UTH Warsaw  
Major: Computer Science — Software Engineering  
Project Type: Engineering Thesis  

---

## Project Overview

The goal of this project is to design and implement a complete compiler pipeline for a custom programming language.

The system supports two execution modes:
- Interpreter — executes the program directly from AST
- Compiler (source-to-source) — translates code into C and compiles it using GCC

This approach allows direct comparison between interpreted and compiled execution models.

---

## Compiler Architecture

```
Source Code → Lexer → Parser → AST → Code Generator → C → GCC
                                ↓
                           Interpreter
```

### Components:
- Lexer — tokenizes input source code
- Parser — builds Abstract Syntax Tree (AST)
- AST — central representation of the program
- Interpreter (eval) — executes AST directly
- Code Generator (codegen) — converts AST into C code

---

## Language Features

Language supports a simplified Python-like syntax:

- indentation-based blocks (INDENT / DEDENT)
- variables (type: int)
- arithmetic operations (+, -, *, /, %)
- comparisons (<, >, ==, !=, etc.)
- control flow:
  - if, elif, else
  - while
- functions (def, return)
- output (print)

---

## Example

### .woo
```txt
x = 5 + 2
print x
```

### Generated C
```c
int x = 0;

int main() {
    x = (5 + 2);
    printf("%d\n", x);
    return 0;
}
```

---

## Testing

The project includes test cases for:
- lexer
- parser
- execution correctness (interpreter vs compiled)

---

## Technologies

- Language: C (C11)
- Compiler: GCC / Clang
- Build system: CMake
- Version control: Git / GitHub
- Testing: Bash / CTest

---

## Project Structure

```
woolang/
├── src/
│   ├── lexer.c
│   ├── parser.c
│   ├── ast.c
│   ├── eval.c
│   ├── codegen.c
│   └── main.c
│
├── include/
│   ├── lexer.h
│   ├── parser.h
│   ├── ast.h
│   ├── eval.h
│   └── codegen.h
│
├── tests/
│   ├── lexer/
│   ├── parser/
│   ├── interpreter/
│   └── codegen/
│
├── examples/
│   └── sample.woo
│
├── build/
├── CMakeLists.txt
├── README.md
└── .gitignore
```

---

## Future Improvements

- optimization stage (middle-end)
- additional data types (e.g., strings)
- better error handling
- direct machine code generation
- performance optimizations

---

## Notes

This project is intended for educational purposes and demonstrates  
the fundamental concepts of compiler construction, including parsing, AST design, and code generation.
