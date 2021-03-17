/*
 * ClangUtil.h
 *
 *  Created on: Aug 12, 2017
 *      Author: ahueck
 */

#ifndef INCLUDE_CLANGUTIL_H_
#define INCLUDE_CLANGUTIL_H_

#include <clang/Basic/SourceLocation.h>
#include <clang/Lex/Lexer.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Demangle/Demangle.h>
#include <llvm/Support/raw_ostream.h>

#include <string>

namespace astprinter {

template <typename T>
inline clang::SourceRange locOf(const clang::SourceManager& sm, T node, unsigned int offset = 0) {
  // offset=1 includes ';' (assuming no whitespaces)
  clang::SourceLocation start(node->getBeginLoc());
  clang::SourceLocation end(clang::Lexer::getLocForEndOfToken(node->getEndLoc(), 0, sm, clang::LangOptions()));
  return {start, end.getLocWithOffset(offset)};
}

inline std::string node2str(const clang::ASTContext& ac, const clang::Stmt* node) {
  std::string exprStr;
  llvm::raw_string_ostream s(exprStr);
  node->printPretty(s, nullptr, ac.getPrintingPolicy());
  return s.str();
}

inline std::string node2str(const clang::ASTContext& ac, const clang::Decl* node) {
  std::string declStr;
  llvm::raw_string_ostream s(declStr);
  node->print(s, ac.getPrintingPolicy());
  return s.str();
}

inline clang::SourceLocation getLocation(const clang::ASTUnit* u, unsigned line, unsigned column) {
  // if line > source file lines, loc to last source line is returned instead
  if (line == 0 || column == 0) {
    return {};
  }
  const auto& sm = u->getSourceManager();
  const auto fileid_main = sm.getMainFileID();
  const auto fe = sm.getFileEntryForID(fileid_main);
  auto loc = u->getLocation(fe, line, column);
  return loc;
}

inline clang::SourceLocation getLocation(const clang::SourceManager& sm, unsigned line, unsigned column) {
  // if line > source file lines, loc to last source line is returned instead
  if (line == 0 || column == 0) {
    return {};
  }
  const auto fileid_main = sm.getMainFileID();
  auto loc = sm.translateLineCol(fileid_main, line, column);
  return loc;
}

inline std::string printToString(const clang::SourceManager& SM, clang::SourceLocation loc, bool with_file = true) {
  using namespace clang;
  std::string S;
  llvm::raw_string_ostream OS(S);

  if (!loc.isValid()) {
    OS << "<invalid loc>";
    return OS.str();
  }

  if (loc.isFileID()) {
    PresumedLoc PLoc = SM.getPresumedLoc(loc);

    if (PLoc.isInvalid()) {
      OS << "<invalid>";
      return OS.str();
    }
    // The macro expansion and spelling pos is identical for file locs.
    if (with_file) {
      OS << PLoc.getFilename() << ":" << PLoc.getLine() << ':' << PLoc.getColumn();
    } else {
      OS << PLoc.getLine() << ':' << PLoc.getColumn();
    }
    return OS.str();
  }

  // TODO test code path
  auto expansionLoc = SM.getExpansionLoc(loc);
  OS << printToString(SM, expansionLoc, with_file);

  OS << " <Spelling=";
  auto spellingloc = SM.getSpellingLoc(loc);
  OS << printToString(SM, spellingloc, with_file);
  OS << '>';

  return OS.str();
}

inline std::string printToString(const clang::SourceManager& SM, clang::SourceRange range) {
  return printToString(SM, range.getBegin()) + "->" + printToString(SM, range.getEnd(), false);
}

template <typename String>
inline std::string try_demangle(String s) {
  std::string name = s;
  auto demangle = llvm::itaniumDemangle(s.data(), nullptr, nullptr, nullptr);
  if (demangle && std::string(demangle) != "") {
    return std::string(demangle);
  }
  return name;
}

namespace detail {
template <typename Predicate>
inline llvm::SmallVector<clang::NamedDecl*, 32> find_decls(const clang::ASTContext& ac, Predicate p) {
  llvm::SmallVector<clang::NamedDecl*, 32> data;
  auto tu = ac.getTranslationUnitDecl();
  //  const auto& m = ac.getSourceManager();

  for (auto* decl : tu->decls()) {
    if (p(decl)) {
      data.emplace_back(llvm::dyn_cast<clang::NamedDecl>(decl));
    }
  }

  return data;
}

inline void printDecls(const clang::ASTContext& ac, llvm::SmallVector<clang::NamedDecl*, 32>& decls,
                       const std::string& regex, llvm::raw_ostream& out = llvm::outs()) {
  const auto& m = ac.getSourceManager();
  llvm::Regex r(regex);
  for (const auto* node : decls) {
    if (m.isInMainFile(node->getBeginLoc())) {
      const auto name = node->getNameAsString();
      if (r.match(name)) {
        auto loc = locOf(m, node);
        out << name << ":" << printToString(m, loc) << "\n";
      }
    }
  }
}

inline void dumpDecls(const clang::ASTContext& ac, llvm::SmallVector<clang::NamedDecl*, 32>& decls,
                      const std::string& regex, llvm::raw_ostream& out = llvm::outs()) {
  const auto& m = ac.getSourceManager();
  llvm::Regex r(regex);
  for (const auto* node : decls) {
    if (m.isInMainFile(node->getBeginLoc())) {
      const auto name = node->getNameAsString();
      if (r.match(name)) {
        node->dump(out);
        auto& sm = ac.getSourceManager();
        const auto loc = locOf(sm, node);
        out << printToString(sm, loc);
        out << "\n";
      }
    }
  }
}
}  // namespace detail

inline void listFunctionDecls(const clang::ASTContext& ac, std::string regex = ".*",
                              llvm::raw_ostream& out = llvm::outs()) {
  auto fDecls = detail::find_decls(ac, [](auto d) { return llvm::isa<clang::FunctionDecl>(d); });
  detail::printDecls(ac, fDecls, regex, out);
}

inline void listNamedDecls(const clang::ASTContext& ac, std::string regex = ".*",
                           llvm::raw_ostream& out = llvm::outs()) {
  auto nDecls = detail::find_decls(ac, [](auto d) { return llvm::isa<clang::NamedDecl>(d); });
  detail::printDecls(ac, nDecls, regex, out);
}

inline void dumpFunctionDecls(const clang::ASTContext& ac, std::string regex = ".*",
                              llvm::raw_ostream& out = llvm::outs()) {
  auto nDecls = detail::find_decls(ac, [](auto d) { return llvm::isa<clang::NamedDecl>(d); });
  detail::dumpDecls(ac, nDecls, regex, out);
}

} /* namespace astprinter */

#endif /* INCLUDE_CLANGUTIL_H_ */
