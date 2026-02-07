// RUN: echo "source" | %clang-ast-printer --source=true %s -- | %filecheck %s

// CHECK: Show source off.

int main() {
  return 0;
}