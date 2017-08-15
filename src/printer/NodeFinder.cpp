/*
 * NodeFinder.cpp
 *
 *  Created on: Aug 12, 2017
 *      Author: ahueck
 */

#include <printer/NodeFinder.h>
#include <ClangUtil.h>

#include <utility>
#include <type_traits>

namespace astprinter {

namespace {

template<typename Node>
inline void debug(const ASTContext& ctx, Node node) {
  llvm::outs() << "Code: " << node2str(ctx, node) << "\n";
  node->dumpColor();
  auto& sm = ctx.getSourceManager();
  const auto loc = locOf(ctx.getSourceManager(), node);
  loc.getBegin().print(llvm::outs(), sm);
  llvm::outs() << " -> ";
  loc.getEnd().print(llvm::outs(), sm);
  llvm::outs() << "\n";
}
}

NodeFinder::NodeFinder(const ASTContext& Context) :
    visitor(Context) {

}

NodeFinder::NodeFinder(const ASTContext& Context, const SourceLocation Point,
    const SourceLocation Point2) :
    visitor(Context) {

}

void NodeFinder::find(clang::TranslationUnitDecl* tu_decl,
    bool print_all_if_not_found) {
  if (visitor.start_loc.isInvalid()) {
    return;
  }
  visitor.print_whole = print_all_if_not_found;
  visitor.TraverseTranslationUnitDecl(tu_decl);
}

void NodeFinder::setLocation(const SourceLocation& start,
    const SourceLocation& end) {
  visitor.start_loc = start;
  if (end.isValid()) {
    visitor.end_loc = end;
  }
}

namespace detail {

NodeFindingASTVisitor::NodeFindingASTVisitor(const ASTContext& Context) :
    ctx(Context) {
}

bool NodeFindingASTVisitor::shouldVisitImplicitCode() const {
  return false;
}

const SourceManager& NodeFindingASTVisitor::sm() {
  return ctx.getSourceManager();
}

bool NodeFindingASTVisitor::TraverseTranslationUnitDecl(
    TranslationUnitDecl* decl) {
  auto result =
      clang::RecursiveASTVisitor<NodeFindingASTVisitor>::TraverseTranslationUnitDecl(
          decl);

  if (!found && print_whole) {
    // by: 1. traverse all decl etc. 2. filter nodes not within the main file.
    llvm::outs() << "Print whole decl\n";
    for (auto* d : decl->decls()) {
      if (inMainFile(d)) {
        debug(ctx, d);
      }
    }
  }

  llvm::outs() << "-------------------------------------------\n";
  found = false;
  return result;
}

bool NodeFindingASTVisitor::TraverseDecl(Decl* decl) {
  if (decl && isCandidate(decl)) {
    debug(ctx, decl);
    found = true;
    return false;
  }
  return clang::RecursiveASTVisitor<NodeFindingASTVisitor>::TraverseDecl(decl);
}

bool NodeFindingASTVisitor::TraverseStmt(Stmt* expr) {
  if (expr && isCandidate(expr)) {
    debug(ctx, expr);
    found = true;
    return false;
  }
  return clang::RecursiveASTVisitor<NodeFindingASTVisitor>::TraverseStmt(expr);
}

bool NodeFindingASTVisitor::within(const SourceRange ast_range) {
  // FIXME only a specific cursor location is supported for now
  if (isPointWithin(ast_range.getBegin(), ast_range.getEnd(), start_loc)) {
    return true;
  }
  /*
   if (isPointWithin(ast_range.getBegin(), ast_range.getEnd(), end_loc)) {
   return true;

   */
  return false;
}

// \brief Determines if the Point is within Start and End.
bool NodeFindingASTVisitor::isPointWithin(const SourceLocation Start,
    const SourceLocation End, SourceLocation Point) {
  // FIXME: Add tests for Point == End.

  /*
   * || (ctx.getSourceManager().isBeforeInTranslationUnit(Start, Point)
   && ctx.getSourceManager().isBeforeInTranslationUnit(Point, End));
   */

  return Point == Start
      || sm().getPresumedLoc(Start).getLine()
          == sm().getPresumedLoc(Point).getLine();

}

}

} /* namespace astprinter */
