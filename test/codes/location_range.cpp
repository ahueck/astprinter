// RUN: printf "8 10\\nquit\\n" | %clang-ast-printer --source=false --use-color=false %s -- | %filecheck %s

// CHECK: ForStmt {{.*}}
// CHECK: BinaryOperator {{.*}} '<'
// CHECK: UnaryOperator {{.*}} '++'

void foo() {
  for (int i = 0; i < 10; ++i) {
  }
}

int main() {
  return 0;
}