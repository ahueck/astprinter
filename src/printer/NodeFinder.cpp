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

template<typename T>
class printer {
  struct hasColor {
    template<class U>
    static auto test(U* p) -> decltype(p->dumpColor(), std::true_type()) {
      return {};
    }
    ;

    template<class >
    static auto test(...) -> std::false_type {
      return {};
    }
    ;
  };

  static void do_print(T node, std::true_type) {
    node->dumpColor();
  }

  static void do_print(T node, std::false_type) {
    node->dump(llvm::outs());
  }

public:
  static void print(T node) {
    printer<T>::do_print(node, hasColor::test(node));
  }
};

template<typename Node>
inline void debug(const ASTContext& ctx, Node n) {
  auto loc = locOf(ctx.getSourceManager(), n);
  llvm::outs() << "Node: " << node2str(ctx, n) << "\n";
  auto& sm = ctx.getSourceManager();
  printer<Node>::print(n);
  loc.getBegin().print(llvm::outs(), sm);
  llvm::outs() << " -> ";
  loc.getEnd().print(llvm::outs(), sm);
  llvm::outs() << "\n";
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

void NodeFinder::find(clang::TranslationUnitDecl* tu_decl) {
  if (visitor.start_loc.isInvalid()) {
    return;
  }
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

const SourceManager& NodeFindingASTVisitor::sm() {
  return ctx.getSourceManager();
}

bool NodeFindingASTVisitor::TraverseTranslationUnitDecl(
    TranslationUnitDecl* decl) {
  const auto result =
      clang::RecursiveASTVisitor<NodeFindingASTVisitor>::TraverseTranslationUnitDecl(
          decl);
  llvm::outs() << "Print whole decl? " << within(decl) << "\n";
  if (!found && within(decl)) { // not found in subtree but cursor is in TU decl
    llvm::outs() << "Print whole decl\n";
    // by: 1. traverse all decl etc. 2. filter nodes not within the main file.
  }
  found = false;
  return result;
}

bool NodeFindingASTVisitor::VisitDecl(Decl* decl) {
  if (isCandidate(decl)) {
    debug(ctx, decl);
  }
  return clang::RecursiveASTVisitor<NodeFindingASTVisitor>::VisitDecl(decl);
}

bool NodeFindingASTVisitor::VisitExpr(Expr* expr) {
  if (isCandidate(expr)) {
    debug(ctx, expr);
  }
  return clang::RecursiveASTVisitor<NodeFindingASTVisitor>::VisitExpr(expr);
}

bool NodeFindingASTVisitor::within(const SourceRange ast_range) {
  // FIXME only a specific cursor location is supported for now
  if (isPointWithin(ast_range.getBegin(), ast_range.getEnd(), start_loc)) {
    return true;
  }
  /*
   if (isPointWithin(ast_range.getBegin(), ast_range.getEnd(), end_loc)) {
   return true;
   }
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
  return Point == Start;

}

}

} /* namespace astprinter */
