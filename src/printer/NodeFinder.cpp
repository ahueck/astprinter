/*
 * NodeFinder.cpp
 *
 *  Created on: Aug 12, 2017
 *      Author: ahueck
 */

#include <printer/NodeFinder.h>
#include <ClangUtil.h>

namespace astprinter {

NodeFindingASTVisitor::NodeFindingASTVisitor(const ASTContext& Context) :
    ctx(Context) {
}

NodeFindingASTVisitor::NodeFindingASTVisitor(const ASTContext& Context,
    const SourceLocation Point, const SourceLocation Point2) :
    start_loc(Point), end_loc(Point2), ctx(Context) {
}

void NodeFindingASTVisitor::setLocation(const SourceLocation& start,
    const SourceLocation& end) {
  this->start_loc = start;
  if (end.isValid()) {
    this->end_loc = end;
  }
}

bool NodeFindingASTVisitor::TraverseDecl(Decl *D) {
  if (start_loc.isInvalid()) {
    return false;
  }
  return clang::RecursiveASTVisitor<NodeFindingASTVisitor>::TraverseDecl(D);
}

bool NodeFindingASTVisitor::VisitDecl(Decl* decl) {
  auto loc = locOf(ctx.getSourceManager(), decl);
  if (ctx.getSourceManager().isInMainFile(loc.getBegin()) && within(loc)) {
    llvm::outs() << "Decl: " << node2str(ctx, decl) << " Is in point: "
        << isPointWithin(loc.getBegin(), loc.getEnd()) << "\n";
    auto& sm = ctx.getSourceManager();
    loc.getBegin().print(llvm::outs(), sm);
    llvm::outs() << " until ";
    loc.getEnd().print(llvm::outs(), sm);
    llvm::outs() << " <:> ";
    start_loc.print(llvm::outs(), sm);
    llvm::outs() << "->";
    end_loc.print(llvm::outs(), sm);
    llvm::outs() << "\n";
    llvm::outs() << "\n";

  }
  return clang::RecursiveASTVisitor<NodeFindingASTVisitor>::VisitDecl(decl);
}

bool NodeFindingASTVisitor::VisitExpr(Expr* expr) {
  auto loc = locOf(ctx.getSourceManager(), expr);
  if (ctx.getSourceManager().isInMainFile(loc.getBegin())) {
    llvm::outs() << "Expr: " << node2str(ctx, expr) << " Is in point: "
        << isPointWithin(loc.getBegin(), loc.getEnd()) << "\n";
    auto& sm = ctx.getSourceManager();
    loc.getBegin().print(llvm::outs(), sm);
    llvm::outs() << "->";
    loc.getEnd().print(llvm::outs(), sm);
    llvm::outs() << " <:> ";
    start_loc.print(llvm::outs(), sm);
    llvm::outs() << "->";
    end_loc.print(llvm::outs(), sm);
    llvm::outs() << "\n";
    llvm::outs() << "\n";
  }
  return clang::RecursiveASTVisitor<NodeFindingASTVisitor>::VisitExpr(expr);
}

bool NodeFindingASTVisitor::within(const SourceRange ast_range) {
  if (isPointWithin(ast_range.getBegin(), ast_range.getEnd(), start_loc)) {
    return true;
  }
  if (isPointWithin(ast_range.getBegin(), ast_range.getEnd(), end_loc)) {
    return true;
  }
  return false;
}

// \brief Determines if the Point is within Start and End.
bool NodeFindingASTVisitor::isPointWithin(const SourceLocation Start,
    const SourceLocation End, SourceLocation Point) {
  // FIXME: Add tests for Point == End.

  return Point == Start || Point == End
      || (ctx.getSourceManager().isBeforeInTranslationUnit(Start, Point)
          && ctx.getSourceManager().isBeforeInTranslationUnit(Point, End));
}

// \brief Determines if the Point is within Start and End.
bool NodeFindingASTVisitor::isPointWithin(const SourceLocation Start,
    const SourceLocation End) {
  // FIXME: Add tests for Point == End.
  return start_loc == Start || start_loc == End
      || (ctx.getSourceManager().isBeforeInTranslationUnit(Start, start_loc)
          && ctx.getSourceManager().isBeforeInTranslationUnit(start_loc, End));
}

} /* namespace astprinter */
