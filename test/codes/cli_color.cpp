// RUN: echo "color" | %clang-ast-printer --use-color=true %s -- | %filecheck %s

// CHECK: Show color off.

int main() {
  return 0;
}