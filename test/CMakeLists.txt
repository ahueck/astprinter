set(LLVM_AST_PRINTER_BINARY $<TARGET_FILE:clang-ast-printer${EXE_SUFFIX}>)

astprinter_find_llvm_progs(ASTPRINTER_FILECHECK_EXEC "FileCheck-${LLVM_VERSION_MAJOR};FileCheck")

if(LLVM_EXTERNAL_LIT)
  cmake_path(GET LLVM_EXTERNAL_LIT PARENT_PATH LLVM_EXTERNAL_LIT_DIR)
endif()
astprinter_find_llvm_progs(ASTPRINTER_LIT_EXEC
  "llvm-lit;lit;lit.py"
  HINTS ${LLVM_EXTERNAL_LIT_DIR} /usr/lib/llvm-${LLVM_VERSION_MAJOR}/build/utils/lit
  ABORT_IF_MISSING
)

add_custom_target(check-astprinter
  COMMAND ${ASTPRINTER_LIT_EXEC} -vv ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS clang-ast-printer${EXE_SUFFIX}
  COMMENT "Running astprinter regression tests"
  USES_TERMINAL
)

astprinter_add_format_target(astprinter-format-tests
  "Formats project source files"
  TARGETS codes/*.cpp
          codes/*.c
          codes/*.h
)

if(ASTPRINTER_ENABLE_COVERAGE)
  set(COVERAGE_DIR ${CMAKE_BINARY_DIR}/coverage_report)
  astprinter_find_llvm_progs(ASTPRINTER_PROFDATA_EXEC "llvm-profdata-${LLVM_VERSION_MAJOR};llvm-profdata")
  astprinter_find_llvm_progs(ASTPRINTER_COV_EXEC "llvm-cov-${LLVM_VERSION_MAJOR};llvm-cov")
  
  set(IGNORE_REGEX ".*(test|build|build_cov|external).*")

  add_custom_target(coverage-astprinter
    COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E rm -f "${CMAKE_CURRENT_BINARY_DIR}/*.profraw" "${CMAKE_CURRENT_BINARY_DIR}/merged.profdata"
    # Execute tests
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target check-astprinter
    # Merge all profraw files
    COMMAND ${CMAKE_COMMAND} -E env bash -c "${ASTPRINTER_PROFDATA_EXEC} merge -sparse ${CMAKE_CURRENT_BINARY_DIR}/*.profraw -o ${CMAKE_CURRENT_BINARY_DIR}/merged.profdata"
    # Generate HTML
    COMMAND ${ASTPRINTER_COV_EXEC} show "${LLVM_AST_PRINTER_BINARY}"
            "-instr-profile=${CMAKE_CURRENT_BINARY_DIR}/merged.profdata"
            -format=html
            "-output-dir=${COVERAGE_DIR}"
            -show-line-counts-or-regions
            "-ignore-filename-regex=${IGNORE_REGEX}"
    # Show summary
    COMMAND ${ASTPRINTER_COV_EXEC} report "${LLVM_AST_PRINTER_BINARY}"
            "-instr-profile=${CMAKE_CURRENT_BINARY_DIR}/merged.profdata"
            "-ignore-filename-regex=${IGNORE_REGEX}"
    COMMENT "Generating coverage report"
    DEPENDS check-astprinter
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    VERBATIM
  )
endif()

configure_file(lit.site.cfg.py.in lit.site.cfg.py.tmp @ONLY)
file(GENERATE OUTPUT lit.site.cfg.py INPUT ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg.py.tmp)
