// RUN: echo "p" | %clang-ast-printer --use-color=false %s -- | %filecheck %s

// CHECK: FunctionDecl {{.*}} main 'int ()'
int main() {
  return 0;
}
