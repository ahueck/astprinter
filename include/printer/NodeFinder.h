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
using clang::PresumedLoc;
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

//  using loc_pair_t = std::pair<SourceLocation, PresumedLoc>;
  struct loc_pair_t {
    SourceLocation location;
    PresumedLoc presumed;
  };

  friend NodeFinder;
private:
  const ASTContext& ctx;
  llvm::raw_ostream& os;
  loc_pair_t start_loc {};
  loc_pair_t end_loc {};
  bool found { false };
  bool print_whole { false };
  bool stop_recursing { false };

public:
  explicit NodeFindingASTVisitor(const ASTContext& Context,
      llvm::raw_ostream& os = llvm::outs());

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
  bool isInMainFile(Node n) {
    return sm().isInMainFile(n->getLocStart());
  }

  template<typename Node>
  bool isCandidate(Node n) {
    const auto& m = sm();
    auto loc = locOf(m, n);
    return isInMainFile(n) && within(loc);
  }

  bool within(const SourceRange& ast_range);

};

} /* namespace detail */

class NodeFinder {
private:
  detail::NodeFindingASTVisitor visitor;

public:
  explicit NodeFinder(ASTContext& Context, llvm::raw_ostream& os = llvm::outs());

  NodeFinder(ASTContext& Context, const SourceLocation Point,
      const SourceLocation Point2, llvm::raw_ostream& os = llvm::outs());

  void find(TranslationUnitDecl* tu_decl, bool print_all_if_not_found = false);

  void setLocation(const SourceLocation& start, const SourceLocation& end =
      SourceLocation());
};

} /* namespace astprinter */

#endif /* INCLUDE_PRINTER_NODEFINDER_H_ */
