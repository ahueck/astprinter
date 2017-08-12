function(add_lit_target target comment)
  cmake_parse_arguments(ARG "" "" "PARAMS;DEPENDS;ARGS" ${ARGN})
  #message(STATUS "Adding llvm-lit target '${target}' depends on '${ARG_DEPENDS}'")
  set(LIT_ARGS "${ARG_ARGS} ${LLVM_LIT_ARGS}")
  separate_arguments(LIT_ARGS)
  if (NOT CMAKE_CFG_INTDIR STREQUAL ".")
    list(APPEND LIT_ARGS --param build_mode=${CMAKE_CFG_INTDIR})
  endif ()
  find_program(LIT_COMMAND
               NAMES llvm-lit llvm-lit-4.0 llvm-lit-3.8 llvm-lit-3.7 llvm-lit-3.6)
  if(NOT LIT_COMMAND)
    message(STATUS "Could not find llvm-lit executable.")
  endif()
  list(APPEND LIT_COMMAND ${LIT_ARGS})
  foreach(param ${ARG_PARAMS})
    list(APPEND LIT_COMMAND --param ${param})
  endforeach()
  if (ARG_UNPARSED_ARGUMENTS AND LIT_COMMAND)
    add_custom_target(${target}
      COMMAND ${LIT_COMMAND} ${ARG_UNPARSED_ARGUMENTS}
      COMMENT "${comment}"
      USES_TERMINAL
      )
  else()
    add_custom_target(${target}
      COMMAND ${CMAKE_COMMAND} -E echo "${target} does nothing, no tools built.")
    message(STATUS "${target} does nothing.")
  endif()
  if (ARG_DEPENDS AND LIT_COMMAND)
    add_dependencies(${target} ${ARG_DEPENDS})
  endif()

  # Tests should be excluded from "Build Solution".
  set_target_properties(${target} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD ON)
endfunction()

# A function to add a set of lit test suites to be driven through 'check-*' targets.
function(add_lit_testsuite target comment)
  cmake_parse_arguments(ARG "" "" "PARAMS;DEPENDS;ARGS" ${ARGN})

  # EXCLUDE_FROM_ALL excludes the test ${target} out of check-all.
  if(NOT EXCLUDE_FROM_ALL)
    # Register the testsuites, params and depends for the global check rule, if any.
    set_property(GLOBAL APPEND PROPERTY OPOV_LIT_TESTSUITES ${ARG_UNPARSED_ARGUMENTS})
    set_property(GLOBAL APPEND PROPERTY OPOV_LIT_PARAMS ${ARG_PARAMS})
    set_property(GLOBAL APPEND PROPERTY OPOV_LIT_DEPENDS ${ARG_DEPENDS})
    set_property(GLOBAL APPEND PROPERTY OPOV_LIT_EXTRA_ARGS ${ARG_ARGS})
  endif()

  # Produce a specific suffixed check rule.
  add_lit_target(${target} ${comment}
    ${ARG_UNPARSED_ARGUMENTS}
    PARAMS ${ARG_PARAMS}
    DEPENDS ${ARG_DEPENDS}
    ARGS ${ARG_ARGS}
    )
endfunction()

function(add_format_target target comment)
  macro(filter_dir _dir_name_)
    foreach (SOURCE_FILE ${ALL_CXX_FILES})
        string(FIND ${SOURCE_FILE} ${_dir_name_} EXCLUDE_FOUND)
        if (NOT ${EXCLUDE_FOUND} EQUAL -1)
            list(REMOVE_ITEM ALL_CXX_FILES ${SOURCE_FILE})
        endif()
    endforeach()
  endmacro()

  cmake_parse_arguments(ARG "" "" "EXCLUDES;OTHER" ${ARGN})
  file(GLOB_RECURSE
    ALL_CXX_FILES
    src/*.cpp
    include/*.h
    include/*.hpp
  )

  foreach(exclude ${ARG_EXCLUDES})
    filter_dir(${exclude})
  endforeach()

  find_program(FORMAT_COMMAND
               NAMES clang-format clang-format-4.0 clang-format-3.8 clang-format-3.7 clang-format-3.6)
  if(FORMAT_COMMAND)
    add_custom_target(${target}
      COMMAND ${FORMAT_COMMAND} -i -style=file -fallback-style=none ${ARG_OTHER} ${ARG_UNPARSED_ARGUMENTS}
              ${ALL_CXX_FILES}
      COMMENT "${comment}"
      USES_TERMINAL
    )
  else()
    message(WARNING "Could not find clang-format executable.")
    add_custom_target(${target}
      COMMAND ${CMAKE_COMMAND} -E echo "${target} does nothing, no tools built.")
  endif()
endfunction()


function(add_tidy_target target comment)
  macro(filter_dir _name_)
    foreach (SOURCE_FILE ${ARG_SOURCES})
        string(FIND ${SOURCE_FILE} ${_name_} EXCLUDE_FOUND)
        if (NOT ${EXCLUDE_FOUND} EQUAL -1)
            list(REMOVE_ITEM ARG_SOURCES ${SOURCE_FILE})
        endif()
    endforeach()
  endmacro()

  cmake_parse_arguments(ARG "" "" "SOURCES;EXCLUDES;OTHER" ${ARGN})

  foreach(exclude ${ARG_EXCLUDES})
    filter_dir(${exclude})
  endforeach()

  find_program(TIDY_COMMAND
               NAMES clang-tidy clang-tidy-4.0 clang-tidy-3.8 clang-tidy-3.7 clang-tidy-3.6)
  if(TIDY_COMMAND)
    add_custom_target(${target}
      COMMAND ${TIDY_COMMAND} -p ${CMAKE_BINARY_DIR}
              ${ARG_OTHER}
              ${ARG_UNPARSED_ARGUMENTS}
              ${ARG_SOURCES}
      COMMENT "${comment}"
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      USES_TERMINAL
    )
  else()
    message(WARNING "Could not find clang-tidy executable.")
    add_custom_target(${target}
      COMMAND ${CMAKE_COMMAND} -E echo "${target} does nothing, no tools built.")
  endif()
endfunction()

function(add_tidy_fix_target target comment)
  cmake_parse_arguments(ARG "" "" "SOURCES;EXCLUDES;OTHER" ${ARGN})
  add_tidy_target(${target} "${comment}"
    SOURCES ${ARG_SOURCES}
    EXCLUDES ${ARG_EXCLUDES}
    OTHER ${ARG_OTHER} -fix
  )
endfunction()
