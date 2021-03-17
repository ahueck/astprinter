CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(astprinter)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH 
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

include(ToolchainOptions)
include(CMakePackageConfigHelpers)

add_format_target(format-sources
  "Formats project source files"
  TARGETS src/*.cpp
          src/*.h
)

add_subdirectory(src)