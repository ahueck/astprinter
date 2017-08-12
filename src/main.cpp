#include <sstream>
#include <string>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"
#include <clang/Lex/Lexer.h>
#include "llvm/LineEditor/LineEditor.h"
#include "llvm/Support/CommandLine.h"

#include <vector>

using namespace llvm;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ASTPrinter("AST Printer Sample");

static llvm::cl::opt<int> r("row", llvm::cl::desc("Row in file."),
		llvm::cl::init(0), llvm::cl::cat(ASTPrinter));
static llvm::cl::opt<int> c("column", llvm::cl::desc("Column in file."),
		llvm::cl::init(0), llvm::cl::cat(ASTPrinter));

template<typename T>
inline clang::SourceRange locOf(const clang::SourceManager& sm, T node,
		unsigned int offset = 0) {
	// offset=1 includes ';' (assuming no whitespaces)
	clang::SourceLocation start(node->getLocStart());
	clang::SourceLocation end(
			clang::Lexer::getLocForEndOfToken(node->getLocEnd(), 0, sm,
					clang::LangOptions()));
	return {start, end.getLocWithOffset(offset)};
}

inline std::string node2str(const clang::ASTContext& ac,
		const clang::Stmt* node) {
	std::string exprStr;
	llvm::raw_string_ostream s(exprStr);
	node->printPretty(s, nullptr, ac.getPrintingPolicy());
	return s.str();
}

inline std::string node2str(const clang::ASTContext& ac,
		const clang::Decl* node) {
	std::string declStr;
	llvm::raw_string_ostream s(declStr);
	node->print(s, ac.getPrintingPolicy());
	return s.str();
}

class NodeFindingASTVisitor: public clang::RecursiveASTVisitor<
		NodeFindingASTVisitor> {
private:
	SourceLocation start_loc;
	SourceLocation end_loc;
	const ASTContext& ctx;

public:
	explicit NodeFindingASTVisitor(const ASTContext& Context) :
			ctx(Context) {

	}
	explicit NodeFindingASTVisitor(const ASTContext& Context,
			const SourceLocation Point, const SourceLocation Point2) :
			start_loc(Point), end_loc(Point2), ctx(Context) {
	}

	void setLocation(const SourceLocation& start, const SourceLocation& end =
			SourceLocation()) {
		this->start_loc = start;
		if (end.isValid()) {
			this->end_loc = end;
		}
	}

	bool TraverseDecl(Decl *D) {
		if (start_loc.isInvalid()) {
			return false;
		}
		return clang::RecursiveASTVisitor<NodeFindingASTVisitor>::TraverseDecl(
				D);
	}

	bool VisitDecl(Decl* decl) {
		auto loc = locOf(ctx.getSourceManager(), decl);
		if (ctx.getSourceManager().isInMainFile(loc.getBegin())
				&& within(loc)) {
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
		return clang::RecursiveASTVisitor<NodeFindingASTVisitor>::VisitDecl(
				decl);
	}

	bool VisitExpr(Expr* expr) {
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
		return clang::RecursiveASTVisitor<NodeFindingASTVisitor>::VisitExpr(
				expr);
	}

private:
	bool within(const SourceRange ast_range) {
		if (isPointWithin(ast_range.getBegin(), ast_range.getEnd(),
				start_loc)) {
			return true;
		}
		if (isPointWithin(ast_range.getBegin(), ast_range.getEnd(), end_loc)) {
			return true;
		}
		return false;
	}

	// \brief Determines if the Point is within Start and End.
	bool isPointWithin(const SourceLocation Start, const SourceLocation End,
			SourceLocation Point) {
		// FIXME: Add tests for Point == End.

		return Point == Start || Point == End
				|| (ctx.getSourceManager().isBeforeInTranslationUnit(Start,
						Point)
						&& ctx.getSourceManager().isBeforeInTranslationUnit(
								Point, End));
	}

	// \brief Determines if the Point is within Start and End.
	bool isPointWithin(const SourceLocation Start, const SourceLocation End) {
		// FIXME: Add tests for Point == End.
		return start_loc == Start || start_loc == End
				|| (ctx.getSourceManager().isBeforeInTranslationUnit(Start,
						start_loc)
						&& ctx.getSourceManager().isBeforeInTranslationUnit(
								start_loc, End));
	}
};

inline SourceLocation getLocation(const ASTUnit* u, unsigned line,
		unsigned column) {
	if (line == 0 || column == 0) {
		return {};
	}
	const auto& sm = u->getSourceManager();
	const auto fileid_main = sm.getMainFileID();
	const auto fe = sm.getFileEntryForID(fileid_main);
	auto loc = u->getLocation(fe, line, column);
	return loc;
}

int main(int argc, const char **argv) {
	CommonOptionsParser op(argc, argv, ASTPrinter);
	ClangTool tool(op.getCompilations(), op.getSourcePathList());

	std::vector<std::unique_ptr<clang::ASTUnit>> av;
	tool.buildASTs(av);
	ASTUnit* u = av[0].get();

	NodeFindingASTVisitor visitor(u->getASTContext());
	llvm::LineEditor le("ast-printer");
	while (llvm::Optional<std::string> line = le.readLine()) {
		if (*line == "q" || *line == "quit") {
			break;
		}
		std::istringstream is(*line);
		auto numbers = std::vector<unsigned>(
				std::istream_iterator<unsigned>(is), { });
		const auto size = numbers.size();
		if (size == 0 || size > 4) {
			llvm::outs() << "Erroneous input.\n";
			continue;
		}
		unsigned locs[4] = { 0, 1, 0, 0 };
		std::copy(std::begin(numbers), std::end(numbers), std::begin(locs));

		const auto start = getLocation(u, locs[0], locs[1]);
		const auto end = getLocation(u, locs[2], locs[3]);

		visitor.setLocation(start, end);
		visitor.TraverseDecl(u->getASTContext().getTranslationUnitDecl());

		llvm::outs().flush();
	}

	return 0;
}
