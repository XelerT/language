# Binary translator

## Overview

## Introduction

This work was made as learning project during first semester at the university. This is C-like high level programming language which uses recursive descent to create abstract syntax tree (AST) and generate assembly code for my virtual stack [CPU](https://github.com/XelerT/cpu).

---

## Install & Use
Install and compile:

To compile use:

        $ make

To run you need to run drive, giving it name for output file and input file:

        $ ./drive output.txt input.txt

## Compiling

### Frontend

Frontend is the first step of compiling, it creates AST and write it into "tree.dreva" file.

Firstly, frontend takes input file and divides it into tokens, during this process key words are checked, punctuations etc. Then tokens are handled by recursive descent, it creates AST and checks for punctuation errors. Finally, created tree is written into "tree.dreva" file.

Some rules of language:

- int main () - is start point of execution
- Punctuation rules are the same as in C

<p align="center">
<img
  src="output_utils/tree.png"
  alt="cpu"
  style="display: block; margin: auto; width: 400px">
</p>

### Backend

Backend is the execution part of language. During it AST is transformed into assembly code for my virtual CPU and this code is executed.
Some assembly generator rules:

- rbx - contains RAM offset where will be contained variables.
- At the start of program rbx gets it's value (default is 100).

Default functions and their representation:

- print var => push [rbx + 1]; out
- scan  var => scan; pop [rbx + 1]
- sqrt  var => push [rbx + 1]; sqrt

After generation of assembly code, it's executed by my virtual stack [CPU](https://github.com/XelerT/cpu).
