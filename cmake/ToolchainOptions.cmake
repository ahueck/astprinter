set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

# Setup of all necessary include/lib dirs for the development of a Clang based tool
find_package(LLVM 10 REQUIRED)
find_package(Clang REQUIRED)


message(STATUS "${CLANG_EXPORTED_TARGETS}")
#string(REGEX REPLACE "clang-format|clang-import-test|clang-rename|clang-refactor|clang-reorder-fields|modularize|clang-tidy|clangd" "" CLANG_EXPORTED_TARGETS "${CLANG_EXPORTED_TARGETS}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

include(AddLLVM)
include(clang-tidy)
include(clang-format)
include(log-util)
include(target-util)

set(LOG_LEVEL 0 CACHE STRING "Granularity of the logger. 3 ist most verbose, 0 is least.")

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
  message(STATUS "Building as debug (default)")
endif ()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${astprinter_SOURCE_DIR}/install/astprinter" CACHE PATH "Default install path" FORCE)
  message(STATUS "Installing to (default): ${CMAKE_INSTALL_PREFIX}")
endif ()
