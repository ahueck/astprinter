// RUN: printf "demangle _Z4mainiPPc\\nquit\\n" | %clang-ast-printer %s -- | %filecheck %s

// CHECK: Demangled name: main

int main() {
  return 0;
}