// RUN: printf "list [\\n1 2 3\\nquit\\n" | %clang-ast-printer %s -- | %filecheck %s

// CHECK: Invalid regex ({{.*}}): "["
// CHECK: Erroneous input of size: 3

int main() {
  return 0;
}