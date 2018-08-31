add_subdirectory(printer)

add_executable(astprinter${EXE_SUFFIX}
  main.cpp
)

target_define_file_basename(astprinter${EXE_SUFFIX})
target_project_compile_options(astprinter${EXE_SUFFIX})
target_project_compile_definitions(astprinter${EXE_SUFFIX}
  PRIVATE_DEFS
    LOG_LEVEL=${LOG_LEVEL}
)

target_link_libraries(astprinter${EXE_SUFFIX}
  astprint
  #${libedit_LIBRARIES}
)

add_custom_target(executable
  COMMENT "Target to compile astprinter executable"
  DEPENDS astprinter${EXE_SUFFIX}
)

install(
  TARGETS astprinter${EXE_SUFFIX}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include
)
