add_library(astprint
  NodeFinder.cpp
)

target_define_file_basename(astprint)
target_project_compile_options(astprint)
target_project_compile_definitions(astprint
  PRIVATE_DEFS
    LOG_LEVEL=${LOG_LEVEL}
)

target_include_directories(astprint
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

target_include_directories(astprint 
  SYSTEM
  PUBLIC
  ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(astprint
  ${CLANG_EXPORTED_TARGETS}
)

install(
  TARGETS astprint
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include
)