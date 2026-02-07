// RUN: printf "color\\ncolor\\nquit\\n" | %clang-ast-printer --use-color=false %s -- | %filecheck %s

// CHECK: Show color on.
// CHECK: Show color off.

int main() {
  return 0;
}