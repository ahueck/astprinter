// RUN: printf "list main\\nquit\\n" | %clang-ast-printer %s -- | %filecheck %s

// CHECK: main:{{.*}}

int main() {
  return 0;
}