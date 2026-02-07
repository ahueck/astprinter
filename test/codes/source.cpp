// RUN: printf "source\\nquit\\n" | %clang-ast-printer %s -- | %filecheck %s

// CHECK: Show source on.

int main() {
  return 0;
}