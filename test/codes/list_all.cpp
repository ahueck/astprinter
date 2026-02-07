// RUN: printf "list\\nquit\\n" | %clang-ast-printer %s -- | %filecheck %s

// CHECK: foo:{{.*}}
// CHECK: main:{{.*}}

void foo() {}
int main() {
  return 0;
}