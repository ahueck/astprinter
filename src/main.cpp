#include <printer/NodeFinder.h>

#include <ClangUtil.h>

#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/LineEditor/LineEditor.h>
#include <llvm/Support/CommandLine.h>

#include <clang/Frontend/ASTConsumers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>

#include <sstream>
#include <string>
#include <vector>

using namespace astprinter;
using namespace llvm;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ASTPrinter("AST Printer Sample");

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ASTPrinter);
  ClangTool tool(op.getCompilations(), op.getSourcePathList());

  std::vector<std::unique_ptr<clang::ASTUnit>> av;
  tool.buildASTs(av);
  ASTUnit* u = av[0].get();

  NodeFinder visitor(u->getASTContext());
  llvm::LineEditor le("ast-printer");
  while (llvm::Optional<std::string> line = le.readLine()) {
    if (*line == "q" || *line == "quit") {
      break;
    }
    std::istringstream is(*line);
    auto numbers =
        std::vector<unsigned>(std::istream_iterator<unsigned>(is), { });
    const auto size = numbers.size();
    if (size == 0 || size > 4) {
      llvm::outs() << "Erroneous input of size:" << size << "\n";
      continue;
    }
    unsigned locs[4] = { 0, 1, 0, 0 };
    std::copy(std::begin(numbers), std::end(numbers), std::begin(locs));

    const auto start = getLocation(u, locs[0], locs[1]);
    const auto end = getLocation(u, locs[2], locs[3]);

    visitor.setLocation(start, end);
    visitor.find(u->getASTContext().getTranslationUnitDecl());

    llvm::outs().flush();
  }

  return 0;
}
