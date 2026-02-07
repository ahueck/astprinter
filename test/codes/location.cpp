// RUN: printf "7\\nquit\\n" | %clang-ast-printer --source=false --use-color=false %s -- | %filecheck %s

// CHECK: VarDecl {{.*}} x 'int'
// CHECK: IntegerLiteral {{.*}} 'int' 10

void foo() {
  int x = 10;
}

int main() {
  return 0;
}
