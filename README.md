# astprinter

*astprinter* is a small commandline-based tool developed to explore the Clang abstract syntax tree (AST).
The main goal is to make it easier to get specific AST nodes by specifying line numbers of the C/C++ source code.
In conjunction with tools like `clang-query`, AST matchers can be written and tested more quickly.


## Main features
Explore the AST nodes of a translation unit
- Use the line(s) of the C/C++ source code to get the corresponding AST nodes
- Match function names (with regex) to print the AST nodes of the declaration/definition

## Usage
See [main.cpp](src/main.cpp) for all possible commandline arguments.

### Example of using *astprinter*
Assume *test.c* contains the code:

  ```c
  int foo () {
      return 2; 
  }
  int main() {
      int val;
      val = foo();    
      return 0;
  }
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

- cmake >= 3.14
- Clang/LLVM 10 (cmake needs to find the installation, see
  the [LLVM cmake documentation](https://llvm.org/docs/CMake.html#id14))
- C++ compiler with support for the C++17 standard, e.g., Clang-10

###### Build steps

In the root project folder, execute the following commands (see also [CI build file](.github/workflows/basic-ci.yml))

  ```
  cmake -B build -DCMAKE_INSTALL_PREFIX=*your path*
  cmake --build build --target install --parallel
  ```
