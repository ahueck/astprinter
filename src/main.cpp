#include <ClangUtil.h>
#include <printer/NodeFinder.h>

#include <clang/AST/ASTContext.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/LineEditor/LineEditor.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

using namespace astprinter;
using namespace llvm;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

llvm::cl::OptionCategory ASTPrinter("AST Printer Sample");

cl::opt<bool> colors("use-color", cl::init(true), cl::desc("Enable or disable color output"), cl::cat(ASTPrinter));
cl::opt<bool> sources("source", cl::init(false), cl::desc("Enable or disable source output"), cl::cat(ASTPrinter));

namespace {
StringRef lexWord(StringRef word) {
  StringRef::iterator begin{word.begin()};
  StringRef::iterator end{word.end()};
  while (true) {
    if (begin == end) {
      return StringRef(begin, 0);
    }
    if (!isWhitespace(*begin)) {
      break;
    }
    ++begin;
  }
  StringRef::iterator wordbegin = begin;
  while (true) {
    ++begin;

    if (begin == end || isWhitespace(*begin)) {
      return StringRef(wordbegin, begin - wordbegin);
    }
  }
}
}  // namespace

int main(int argc, const char** argv) {
#if LLVM_VERSION_MAJOR < 14
  CommonOptionsParser op(argc, argv, ASTPrinter);
  ClangTool tool(op.getCompilations(), op.getSourcePathList());
#else
  auto op = CommonOptionsParser::create(argc, argv, ASTPrinter);
  if (!op) {
    llvm::outs() << "Erroneous input";
    return 1;
  }
  ClangTool tool(op->getCompilations(), op->getSourcePathList());
#endif

  std::vector<std::unique_ptr<clang::ASTUnit>> av;
  tool.buildASTs(av);

  if (av.empty()) {
    return 0;
  }

  bool color{colors.getValue()};
  bool source{sources.getValue()};
  auto& ctx = av[0]->getASTContext();

  NodeFinder visitor(ctx, llvm::outs());
  visitor.showColor(color);

  llvm::LineEditor le("ast-printer");
  while (auto line = le.readLine()) {
    StringRef ref = *line;
    auto cmd      = lexWord(ref);

    if (cmd == "q" || cmd == "quit") {
      break;
    } else if (cmd == "c" || cmd == "color") {
      color = !color;
      llvm::outs() << "Show color " << (color ? "on.\n" : "off.\n");
      visitor.showColor(color);
      continue;
    } else if (cmd == "s" || cmd == "source") {
      source = !source;
      llvm::outs() << "Show source " << (source ? "on.\n" : "off.\n");
      visitor.showSource(source);
      continue;
    } else if (cmd == "l" || cmd == "list" || cmd == "p" || cmd == "print") {
      auto str = lexWord(StringRef(cmd.end(), ref.end() - cmd.end()));
      if (str == "") {
        str = ".*";
      } else {
        std::string error;
        llvm::Regex r(str);
        if (!r.isValid(error)) {
          llvm::outs() << "Invalid regex (" << error << "): \"" << str << "\"\n";
          continue;
        }
      }

      if (cmd == "p" || cmd == "print") {
        visitor.dumpFunctions(std::string{str});
      } else {
        visitor.printFunctionDecls(std::string{str});
      }

      continue;
    } else if (cmd == "d" || cmd == "demangle") {
      auto str            = lexWord(StringRef(cmd.end(), ref.end() - cmd.end()));
      auto demangled_name = NodeFinder::demangle(std::string{str});
      llvm::outs() << "Demangled name: " << demangled_name << "\n";

      continue;
    }

    std::istringstream is(*line);
    auto numbers    = std::vector<unsigned>(std::istream_iterator<unsigned>(is), {});
    const auto size = numbers.size();
    if (size == 0 || size > 2) {
      llvm::outs() << "Erroneous input of size: " << size << "\n";
      continue;
    }

    const auto start = getLocation(ctx.getSourceManager(), numbers[0], 1);
    const auto end   = size == 2 ? getLocation(ctx.getSourceManager(), numbers[1], 1) : SourceLocation();

    visitor.setLocation(start, end);
    visitor.find();

    llvm::outs().flush();
  }

  return 0;
}
