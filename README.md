# astprinter &middot; ![License](https://img.shields.io/github/license/ahueck/astprinter)


**astprinter** is a command-line tool for exploring the Clang abstract syntax tree (AST). It is designed to simplify retrieving specific AST nodes by specifying line numbers from C/C++ source code.


## Features
* Retrieve AST nodes corresponding to specific lines of C/C++ source code.
* Use regular expressions to match function names and display the AST nodes for their declarations or definitions.

## Usage
See [main.cpp](src/main.cpp) for all possible command-line arguments.

### Example of using *astprinter*
Assume *test.c* contains the code:

  ```c
  1  int foo () {
  2      return 2; 
  3  }
  4  int main() {
  5      int val;
  6      val = foo();    
  7      return 0;
  8  }
  ```
#### Using astprinter on test.c
In this example we
  1) load `test.c` with standard Clang flags,
  2) list all function declarations in `test.c`,
  3) print the AST definition of foo, and finally,
  4) print the AST of the return statement in line 2.

```console
ahueck@sys:~/astprint/install$ ./bin/astprinter ../test.c --
ast-printer> l
foo:~/astprint/install/../test.c:1:3->3:4
main:~/astprint/install/../test.c:4:3->8:4

ast-printer> p foo
FunctionDecl 0x3977120 <test.c:1:3, line:3:3> line:1:7 used foo 'int ()'
`-CompoundStmt 0x3977238 <col:14, line:3:3>
  `-ReturnStmt 0x3977220 <line:2:7, col:14>
    `-IntegerLiteral 0x3977200 <col:14> 'int' 2
~/astprint/install/../test.c:1:3->3:4

ast-printer> 2
ReturnStmt 0x1ba3220 <test.c:2:7, col:14>
`-IntegerLiteral 0x1ba3200 <col:14> 'int' 2
~/astprint/install/../test.c:2:7->2:15
```

## How to build

###### Requirements

- CMake >= 3.20
- Clang/LLVM 12, 14, 18 (CMake needs to find the installation, see
  the [LLVM CMake documentation](https://llvm.org/docs/CMake.html) or the [CI workflow](.github/workflows/basic-ci.yml))
- C++17 compiler

###### Build steps

In the root project folder, execute the following commands (see also [CI workflow](.github/workflows/basic-ci.yml))

  ```
  cmake -B build -DCMAKE_INSTALL_PREFIX=*your path*
  cmake --build build --target install --parallel
  ```
