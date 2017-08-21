#include <printer/NodeFinder.h>

#include <ClangUtil.h>

#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/LineEditor/LineEditor.h>
#include <llvm/Support/CommandLine.h>

#include <clang/Frontend/ASTConsumers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

using namespace astprinter;
using namespace llvm;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ASTPrinter("AST Printer Sample");

static cl::opt<bool> colors("color", cl::init(true),
    cl::desc("Enable or disable color output"), cl::cat(ASTPrinter));

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ASTPrinter);
  ClangTool tool(op.getCompilations(), op.getSourcePathList());

  std::vector<std::unique_ptr<clang::ASTUnit>> av;
  tool.buildASTs(av);

  if(av.empty()) {
    return 0;
  }

  bool color { colors.getValue() };
  auto& ctx = av[0]->getASTContext();

  NodeFinder visitor(ctx, llvm::outs());
  visitor.showColor(color);

  llvm::LineEditor le("ast-printer");
  while (llvm::Optional<std::string> line = le.readLine()) {
    if (*line == "q" || *line == "quit") {
      break;
    } else if (*line == "c" || *line == "color") {
      color = !color;
      llvm::outs() << "Show color " << (color ? "on.\n" : "off.\n");
      visitor.showColor(color);
      continue;
    }
    std::istringstream is(*line);
    auto numbers = std::vector<unsigned>(std::istream_iterator<unsigned>(is),
        { });
    const auto size = numbers.size();
    if (size == 0 || size > 2) {
      llvm::outs() << "Erroneous input of size: " << size << "\n";
      continue;
    }
    unsigned locs[2] = { 0, 0 };
    std::copy(std::begin(numbers), std::end(numbers), std::begin(locs));

    const auto start = getLocation(ctx.getSourceManager(), locs[0], 1);
    const auto end = getLocation(ctx.getSourceManager(), locs[1], 1);

    visitor.setLocation(start, end);
    visitor.find();

    llvm::outs().flush();
  }

  return 0;
}
