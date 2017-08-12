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
class TranslationUnitDecl;
}

namespace astprinter {

using clang::ASTContext;
using clang::SourceLocation;
using clang::SourceRange;
using clang::Decl;
using clang::Expr;
using clang::TranslationUnitDecl;

class NodeFinder;

namespace detail {

class NodeFindingASTVisitor: public clang::RecursiveASTVisitor<
    NodeFindingASTVisitor> {

  friend NodeFinder;
private:
  SourceLocation start_loc;
  SourceLocation end_loc;
  const ASTContext& ctx;

public:
  explicit NodeFindingASTVisitor(const ASTContext& Context);

  bool VisitDecl(Decl* decl);

  bool VisitExpr(Expr* expr);

private:
  bool within(const SourceRange ast_range);

  bool isPointWithin(const SourceLocation Start, const SourceLocation End,
      SourceLocation Point);

  bool isPointWithin(const SourceLocation Start, const SourceLocation End);
};

} /* namespace detail */

class NodeFinder {
private:
  detail::NodeFindingASTVisitor visitor;

public:
  explicit NodeFinder(const ASTContext& Context);

  NodeFinder(const ASTContext& Context, const SourceLocation Point,
      const SourceLocation Point2);

  void find(clang::TranslationUnitDecl* tu_decl);

  void setLocation(const SourceLocation& start, const SourceLocation& end =
      SourceLocation());
};

} /* namespace astprinter */

#endif /* INCLUDE_PRINTER_NODEFINDER_H_ */
