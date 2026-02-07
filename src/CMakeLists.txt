add_subdirectory(printer)

add_executable(clang-ast-printer${EXE_SUFFIX}
  main.cpp
  )

astprinter_define_file_basename(clang-ast-printer${EXE_SUFFIX})
astprinter_project_compile_options(clang-ast-printer${EXE_SUFFIX})
astprinter_project_compile_definitions(clang-ast-printer${EXE_SUFFIX}
  PRIVATE_DEFS
  LOG_LEVEL=${LOG_LEVEL}
  )

target_include_directories(clang-ast-printer${EXE_SUFFIX}
  SYSTEM
  PRIVATE
  ${LLVM_INCLUDE_DIRS}
  )

target_include_directories(clang-ast-printer${EXE_SUFFIX} PRIVATE
  ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(clang-ast-printer${EXE_SUFFIX}
  LLVMLineEditor
  clang-ast-print
  )

add_custom_target(executable
  COMMENT "Target to compile astprinter executable"
  DEPENDS clang-ast-printer${EXE_SUFFIX}
  )

install(
  TARGETS clang-ast-printer${EXE_SUFFIX}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include
)
