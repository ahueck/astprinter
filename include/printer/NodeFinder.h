/*
 * NodeFinder.h
 *
 *  Created on: Aug 12, 2017
 *      Author: ahueck
 */

#ifndef INCLUDE_PRINTER_NODEFINDER_H_
#define INCLUDE_PRINTER_NODEFINDER_H_

#include <clang/AST/RecursiveASTVisitor.h>
#include <ClangUtil.h>

#include <utility>

namespace clang {
class ASTContext;
class Decl;
class Expr;
class Stmt;
class TranslationUnitDecl;
class SourceManager;
}

namespace astprinter {

using clang::ASTContext;
using clang::SourceLocation;
using clang::SourceRange;
using clang::Decl;
using clang::Expr;
using clang::Stmt;
using clang::TranslationUnitDecl;
using clang::SourceManager;

class NodeFinder;

namespace detail {

class NodeFindingASTVisitor: public clang::RecursiveASTVisitor<
    NodeFindingASTVisitor> {

  friend NodeFinder;
private:
  SourceLocation start_loc;
  SourceLocation end_loc;
  const ASTContext& ctx;
  bool found { false };
  bool print_whole { false };

public:
  explicit NodeFindingASTVisitor(const ASTContext& Context);

  bool shouldVisitImplicitCode() const;

  bool TraverseTranslationUnitDecl(TranslationUnitDecl* decl);

  bool TraverseDecl(Decl* decl);

  bool TraverseStmt(Stmt* stmt);

private:
  const SourceManager& sm();

  template<typename Node>
  bool within(Node n) {
    const auto& m = sm();
    auto loc = locOf(m, n);
    return within(loc);
  }

  template<typename Node>
  bool inMainFile(Node n) {
    const auto& m = sm();
    auto loc = locOf(m, n);
    return m.isInMainFile(loc.getBegin());
  }

  template<typename Node>
  bool isCandidate(Node n) {
    const auto& m = sm();
    auto loc = locOf(m, n);
    return inMainFile(n) && within(loc);
  }

  bool within(const SourceRange ast_range);

  bool isPointWithin(const SourceLocation Start, const SourceLocation End,
      SourceLocation Point);
};

} /* namespace detail */

class NodeFinder {
private:
  detail::NodeFindingASTVisitor visitor;

public:
  explicit NodeFinder(const ASTContext& Context);

  NodeFinder(const ASTContext& Context, const SourceLocation Point,
      const SourceLocation Point2);

  void find(TranslationUnitDecl* tu_decl, bool print_all_if_not_found = false);

  void setLocation(const SourceLocation& start, const SourceLocation& end =
      SourceLocation());
};

} /* namespace astprinter */

#endif /* INCLUDE_PRINTER_NODEFINDER_H_ */
