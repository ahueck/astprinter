/*
 * NodeFinder.h
 *
 *  Created on: Aug 12, 2017
 *      Author: ahueck
 */

#ifndef INCLUDE_PRINTER_NODEFINDER_H_
#define INCLUDE_PRINTER_NODEFINDER_H_

#include <clang/AST/RecursiveASTVisitor.h>

namespace clang {
class ASTContext;
class Decl;
class Expr;
}

namespace astprinter {

using clang::ASTContext;
using clang::SourceLocation;
using clang::SourceRange;
using clang::Decl;
using clang::Expr;

class NodeFindingASTVisitor: public clang::RecursiveASTVisitor<
    NodeFindingASTVisitor> {
private:
  SourceLocation start_loc;
  SourceLocation end_loc;
  const ASTContext& ctx;

public:
  explicit NodeFindingASTVisitor(const ASTContext& Context);

  NodeFindingASTVisitor(const ASTContext& Context, const SourceLocation Point,
      const SourceLocation Point2);

  void setLocation(const SourceLocation& start, const SourceLocation& end =
      SourceLocation());

  bool TraverseDecl(Decl *D);

  bool VisitDecl(Decl* decl);

  bool VisitExpr(Expr* expr);

private:
  bool within(const SourceRange ast_range);

  bool isPointWithin(const SourceLocation Start, const SourceLocation End,
      SourceLocation Point);

  bool isPointWithin(const SourceLocation Start, const SourceLocation End);
};

} /* namespace astprinter */

#endif /* INCLUDE_PRINTER_NODEFINDER_H_ */
