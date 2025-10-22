# 🧠 PyC Compiler — A Simple Programming Language Compiler in C

> **Author:** Damian Łuczak  
> **University:** Uczelnia Techniczno-Handlowa im. Heleny Chodkowskiej in Warsaw
> **Major:** Computer Science — Software Engineering  
> **Type of Work:** Engineering Thesis  

---

## 🎯 Project Goal

The goal of this engineering thesis is to design and implement a **compiler for a simple, custom programming language** whose syntax resembles Python, and whose output is **C source code**.

The project has an **educational and practical** character — combining theoretical aspects with implementation, allowing a deeper understanding of how compilers work by creating one from scratch.

---

## 🧩 PyC Language Overview

The **PyC** (Python-to-C) language has a simplified, Python-inspired syntax:
- Indentation (`INDENT` / `DEDENT`) defines code blocks.
- Comments: `# comment`
- Supported statements: `if`, `elif`, `else`, `while`, `def`, `return`, `print`
- All variables are of type `int` (in the MVP version)
- The compiler translates `.pyc` source files into equivalent C code that can be compiled with GCC.

---

## 🧰 Technologies and Tools
- Implementation language: C (C11)
- Compiler: GCC / Clang
- Build system: CMake
- Version control: Git + GitHub
- Environment: macOS (VS Code)
- Testing: Bash / CTest

---
'''
## Project Structure 
pyc-compiler/
├── src/            # compiler source code
│   ├── main.c
├── runtime/        # small standard library (e.g., print)
│   └── io.c
├── samples/        # example PyC programs
│   └── hello.pyc
├── docs/           # documentation and thesis notes
├── .gitignore
└── README.md
'''
---
