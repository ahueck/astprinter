set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

# Setup of all necessary include/lib dirs for the development of a Clang based tool
find_package(LLVM REQUIRED)
find_package(Clang REQUIRED)
#find_package(LibEdit REQUIRED)

string(REGEX REPLACE "clang-format|clang-import-test|clang-rename|clang-refactor|clang-reorder-fields|modularize|clang-tidy|clangd" "" CLANG_EXPORTED_TARGETS "${CLANG_EXPORTED_TARGETS}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

include(AddLLVM)
include(clang-tidy)
include(clang-format)
include(log-util)

set(LOG_LEVEL 0 CACHE STRING "Granularity of the logger. 3 ist most verbose, 0 is least.")

if(NOT CMAKE_BUILD_TYPE)
# set default build type
  set(CMAKE_BUILD_TYPE Debug)
endif()


function(target_project_compile_options target)
  cmake_parse_arguments(ARG "" "" "PRIVATE_FLAGS;PUBLIC_FLAGS" ${ARGN})

  target_compile_options(${target} PRIVATE
    -Wall -Wextra -pedantic
    -Wunreachable-code -Wwrite-strings
    -Wpointer-arith -Wcast-align
    -Wcast-qual -Wno-unused-parameter
    -fno-rtti
  )

  if(ARG_PRIVATE_FLAGS)
    target_compile_options(${target} PRIVATE
      "${ARG_PRIVATE_FLAGS}"
    )
  endif()

  if(ARG_PUBLIC_FLAGS)
    target_compile_options(${target} PUBLIC
      "${ARG_PUBLIC_FLAGS}"
    )
  endif()
endfunction()

function(target_project_compile_definitions target)
  cmake_parse_arguments(ARG "" "" "PRIVATE_DEFS;PUBLIC_DEFS" ${ARGN})

  if(ARG_PRIVATE_DEFS)
    target_compile_definitions(${target} PRIVATE
      "${ARG_PRIVATE_DEFS}"
    )
  endif()

  if(ARG_PUBLIC_DEFS)
    target_compile_definitions(${target} PUBLIC
      "${ARG_PUBLIC_DEFS}"
    )
  endif()
endfunction()
