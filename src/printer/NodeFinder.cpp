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

  static void do_print(T node, const ASTContext& ctx, std::true_type) {
    node->dumpColor();
  }

  static void do_print(T node, const ASTContext& ctx, std::false_type) {
    node->dump(llvm::outs());
  }

public:
  static void print(T node, const ASTContext& ctx) {
    llvm::outs() << "Code: " << node2str(ctx, node) << "\n";
    printer<T>::do_print(node, ctx, hasColor::test(node));
    auto& sm = ctx.getSourceManager();
    const auto loc = locOf(ctx.getSourceManager(), node);
    loc.getBegin().print(llvm::outs(), sm);
    llvm::outs() << " -> ";
    loc.getEnd().print(llvm::outs(), sm);
    llvm::outs() << "\n";
  }
};

template<typename Node>
inline void debug(const ASTContext& ctx, Node n) {
  printer<Node>::print(n, ctx);
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

  if (!found) { // not found in subtree
    // by: 1. traverse all decl etc. 2. filter nodes not within the main file.
    llvm::outs() << "Print whole decl\n";
    decl_printer_mode = true;
    result = clang::RecursiveASTVisitor<NodeFindingASTVisitor>::TraverseTranslationUnitDecl(
              decl);
    decl_printer_mode = false;
  }

  llvm::outs() << "-------------------------------------------\n";
  found = false;
  return result;
}

bool NodeFindingASTVisitor::VisitDecl(Decl* decl) {

  if (!llvm::isa<TranslationUnitDecl>(*decl) && (decl_printer_mode || isCandidate(decl))) {
    debug(ctx, decl);
    found = true;
    return true;
  }

  return clang::RecursiveASTVisitor<NodeFindingASTVisitor>::VisitDecl(decl);
}

bool NodeFindingASTVisitor::VisitExpr(Expr* expr) {
  if (isCandidate(expr)) {
    debug(ctx, expr);
    found = true;
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
