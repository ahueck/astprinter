add_subdirectory(printer)

add_executable(clang-ast-printer${EXE_SUFFIX}
  main.cpp
)

target_define_file_basename(clang-ast-printer${EXE_SUFFIX})
target_project_compile_options(clang-ast-printer${EXE_SUFFIX})
target_project_compile_definitions(clang-ast-printer${EXE_SUFFIX}
  PRIVATE_DEFS
    LOG_LEVEL=${LOG_LEVEL}
)

target_link_libraries(clang-ast-printer${EXE_SUFFIX}
  clang-ast-print
  #${libedit_LIBRARIES}
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
