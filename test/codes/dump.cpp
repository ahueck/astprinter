// RUN: printf "print main\nquit\n" | %clang-ast-printer --use-color=false %s -- | %filecheck %s

// CHECK: FunctionDecl {{.*}} main 'int ()'
// CHECK: CompoundStmt {{.*}}
// CHECK: ReturnStmt {{.*}}

int main() {
  return 0;
}
