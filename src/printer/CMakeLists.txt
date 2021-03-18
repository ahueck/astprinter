add_library(clang-ast-print STATIC
  NodeFinder.cpp
  )

target_define_file_basename(clang-ast-print)
target_project_compile_options(clang-ast-print)
target_project_compile_definitions(clang-ast-print
  PRIVATE_DEFS
  LOG_LEVEL=${LOG_LEVEL}
  )

target_include_directories(clang-ast-print
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include
  )

target_include_directories(clang-ast-print
  SYSTEM
  PUBLIC
  ${LLVM_INCLUDE_DIRS}
  )

target_link_libraries(clang-ast-print PUBLIC
  clangTooling
  )

install(
  TARGETS clang-ast-print
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include
)

install(
  FILES
  ${PROJECT_SOURCE_DIR}/include/printer/NodeFinder.h
  ${PROJECT_SOURCE_DIR}/include/ClangUtil.h
  DESTINATION include
)

# Hacky: Here, the install code is guaranteed to run last (after ${CMAKE_INSTALL_PREFIX}/lib folder was created)
#install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CLANG_INSTALL_PREFIX}/lib/clang ${CMAKE_INSTALL_PREFIX}/lib/clang)")
