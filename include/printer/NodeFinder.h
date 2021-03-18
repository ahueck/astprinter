/*
 * NodeFinder.h
 *
 *  Created on: Aug 12, 2017
 *      Author: ahueck
 */

#ifndef INCLUDE_PRINTER_NODEFINDER_H_
#define INCLUDE_PRINTER_NODEFINDER_H_

#include "ClangUtil.h"

#include <clang/AST/AST.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <llvm/Support/raw_ostream.h>

namespace clang {
class ASTContext;
class Decl;
class Expr;
class Stmt;
class TranslationUnitDecl;
}  // namespace clang
using namespace clang;
namespace astprinter {

using clang::ASTContext;
using clang::Decl;
using clang::Expr;
using clang::PresumedLoc;
using clang::SourceLocation;
using clang::SourceManager;
using clang::SourceRange;
using clang::Stmt;
using clang::TranslationUnitDecl;

class NodeFinder;

namespace detail {

class NodeFindingASTVisitor : public clang::RecursiveASTVisitor<NodeFindingASTVisitor> {
  //  using loc_pair_t = std::pair<SourceLocation, PresumedLoc>;
  struct loc_pair_t {
    SourceLocation location;
    unsigned line{};
  };

  friend NodeFinder;

 private:
  ASTContext& ctx;
  llvm::raw_ostream& os;
  loc_pair_t start_loc{};
  loc_pair_t end_loc{};
  bool found{false};
  bool print_whole{false};
  bool print_source{false};
  bool stop_recursing{false};

 public:
  explicit NodeFindingASTVisitor(ASTContext& Context, llvm::raw_ostream& os = llvm::outs());

  bool shouldVisitImplicitCode() const;

  bool TraverseTranslationUnitDecl(TranslationUnitDecl* decl);

  bool TraverseDecl(Decl* decl);

  bool TraverseStmt(Stmt* stmt);

 private:
  [[nodiscard]] const SourceManager& sm() const;

  template <typename Node>
  bool isCandidate(Node n) {
    const auto& m = sm();
    auto loc      = locOf(m, n);
    return m.isInMainFile(loc.getBegin()) && within(loc);
  }

  bool within(const SourceRange& ast_range);
};

} /* namespace detail */

class NodeFinder {
 private:
  detail::NodeFindingASTVisitor visitor;

 public:
  explicit NodeFinder(ASTContext& Context, llvm::raw_ostream& os = llvm::outs());

  NodeFinder(ASTContext& Context, const SourceLocation Point, const SourceLocation Point2,
             llvm::raw_ostream& os = llvm::outs());

  void showColor(bool color = true);

  void showSource(bool show_source = true);

  void find(bool print_all_if_not_found = false);

  void printFunctionDecls(const std::string& regex = ".*", bool add_mangle = false) const;

  void dumpFunctions(const std::string& regex = ".*", bool add_mangle = false) const;

  void setLocation(const SourceLocation& start, const SourceLocation& end = SourceLocation());

  void setLocation(unsigned line_start, unsigned line_end);

  static std::string demangle(const std::string& name);
};

} /* namespace astprinter */

#endif /* INCLUDE_PRINTER_NODEFINDER_H_ */
