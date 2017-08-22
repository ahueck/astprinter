/*
 * NodeFinder.cpp
 *
 *  Created on: Aug 12, 2017
 *      Author: ahueck
 */

#include <printer/NodeFinder.h>
#include <ClangUtil.h>

#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclBase.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/Diagnostic.h>

namespace astprinter {

namespace {

inline void print(ASTContext& ctx, const Stmt* node, llvm::raw_ostream& os) {
  // non-const SM for printing in color to "os" needed
  auto& sm = ctx.getSourceManager();
#ifdef NDEBUG
  node->dump(os, sm);
#else
  os << "Code: " << node2str(ctx, node) << "\n";
  node->dump(os, sm);
  const auto loc = locOf(sm, node);
  loc.getBegin().print(os, sm);
  os << " -> ";
  loc.getEnd().print(os, sm);
  os << "\n";
#endif
}

inline void print(ASTContext& ctx, const Decl* node, llvm::raw_ostream& os) {
#ifdef NDEBUG
  node->dump(os);
#else
  os << "Code: " << node2str(ctx, node) << "\n";
  node->dump(os);
  auto& sm = ctx.getSourceManager();
  const auto loc = locOf(sm, node);
  loc.getBegin().print(os, sm);
  os << " -> ";
  loc.getEnd().print(os, sm);
  os << "\n";
#endif
}

} /* namespace */

NodeFinder::NodeFinder(ASTContext& Context, llvm::raw_ostream& os) :
    visitor(Context, os) {

}

NodeFinder::NodeFinder(ASTContext& Context, const SourceLocation Point,
    const SourceLocation Point2, llvm::raw_ostream& os) :
    visitor(Context, os) {

}

void NodeFinder::showColor(bool color) {
  visitor.sm().getDiagnostics().setShowColors(color);
}

void NodeFinder::find(bool print_all_if_not_found) {
  if (visitor.start_loc.location.isInvalid()) {
    return;
  }
  auto tu_decl = visitor.ctx.getTranslationUnitDecl();
  visitor.print_whole = print_all_if_not_found;
  visitor.TraverseTranslationUnitDecl(tu_decl);
}

void NodeFinder::setLocation(const SourceLocation& start,
    const SourceLocation& end) {
  visitor.start_loc = {start, visitor.sm().getExpansionLineNumber(start)};
  if (end.isValid()) {
    visitor.end_loc = {end, visitor.sm().getExpansionLineNumber(end)};
  }
}

namespace detail {

NodeFindingASTVisitor::NodeFindingASTVisitor(ASTContext& Context,
    llvm::raw_ostream& os) :
    ctx(Context), os(os) {
}

bool NodeFindingASTVisitor::shouldVisitImplicitCode() const {
  return false;
}

const SourceManager& NodeFindingASTVisitor::sm() {
  return ctx.getSourceManager();
}

bool NodeFindingASTVisitor::TraverseTranslationUnitDecl(
    TranslationUnitDecl* decl) {
  stop_recursing = false;
  found = false;

  const auto result =
      clang::RecursiveASTVisitor<NodeFindingASTVisitor>::TraverseTranslationUnitDecl(
          decl);

  if (!found && print_whole) {
    // by: 1. traverse all decl etc. 2. filter nodes not within the main file.
    os << "Print whole decl\n";

    for (auto* node : decl->decls()) {
      if (isInMainFile(node)) {
        print(ctx, node, os);
      }
    }
  }

  return result;
}

#define traverseNode(node_t)                                                            \
    bool NodeFindingASTVisitor::Traverse##node_t(node_t* node) {                        \
      if (node) {                                                                       \
        if (isCandidate(node)) {                                                        \
          print(ctx, node, os);                                                         \
          found = true;                                                                 \
          return true;                                                                  \
        }                                                                               \
        if (stop_recursing) {                                                           \
          return false;                                                                 \
        }                                                                               \
      }                                                                                 \
      return clang::RecursiveASTVisitor<NodeFindingASTVisitor>::Traverse##node_t(node); \
    }
traverseNode(Decl)
traverseNode(Stmt)
#undef traverseNode

bool NodeFindingASTVisitor::within(const SourceRange& ast_range) {
  auto& s = sm();
  const auto p1 = start_loc.line;
  const auto p2 = end_loc.location.isValid() ? end_loc.line : p1;
  const auto d1 = s.getExpansionLineNumber(ast_range.getBegin());

  if (d1 > p2) {
    this->stop_recursing = true;
    return false;
  }

  const auto d2 = s.getExpansionLineNumber(ast_range.getEnd());
  const auto enclosed = (p1 <= d1 && p2 >= d1) || (p1 <= d2 && p2 >= d2);

//  if (enclosed) {
//    os << p1 << "<=" << d1 << "&&" << p2 << ">=" << d1 << " || " << p1 <<
//    "<=" << d2 << "&&" << p2 << ">=" << d2 << "\n";
//  }
  return enclosed;
}

} /* namespace detail */

} /* namespace astprinter */

