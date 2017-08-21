/*
 * ClangUtil.h
 *
 *  Created on: Aug 12, 2017
 *      Author: ahueck
 */

#ifndef INCLUDE_CLANGUTIL_H_
#define INCLUDE_CLANGUTIL_H_

#include <clang/Tooling/Tooling.h>
#include <clang/Lex/Lexer.h>
#include <llvm/Support/raw_ostream.h>

#include <string>

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

inline clang::SourceLocation getLocation(const clang::ASTUnit* u, unsigned line,
    unsigned column) {
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

inline clang::SourceLocation getLocation(const clang::SourceManager& sm, unsigned line,
    unsigned column) {
  // if line > source file lines, loc to last source line is returned instead
  if (line == 0 || column == 0) {
    return {};
  }
  const auto fileid_main = sm.getMainFileID();
  auto loc = sm.translateLineCol(fileid_main, line, column);
  return loc;
}



#endif /* INCLUDE_CLANGUTIL_H_ */
