CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(astprinter
  VERSION 0.3
)

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

astprinter_add_format_target(astprinter-format-sources
  "Formats project source files"
  TARGETS src/*.cpp
          src/*.h
          include/*.h
)

add_subdirectory(src)

if(PROJECT_IS_TOP_LEVEL)
  add_subdirectory(test)
endif()
