CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(astprinter)

include(${PROJECT_SOURCE_DIR}/cmake/ToolchainOptions.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

#add_subdirectory(${PROJECT_SOURCE_DIR}/externalpackages)

add_format_target(format-all
  "Formats astprinter source files"
  EXCLUDES "external"
)

set(printer_sources
  src/printer/NodeFinder.cpp
)

add_library(astprint
	${printer_sources}
)

set_target_properties(astprint
  PROPERTIES PUBLIC_HEADER "include/printer/NodeFinder.h;include/ClangUtil.h")

target_link_libraries(astprint
  ${CLANG_LIBS}
  -lLLVMSupport
  -lLLVMMCParser
  -lLLVMBitReader -lLLVMProfileData
  -lLLVMOption -lLLVMCore
  -lLLVMMC -lLLVMSupport
  -lLLVMDemangle
  ${LLVM_SYSTEM_LIBS}
)

add_executable(astprinter${EXE_SUFFIX}
  src/main.cpp
)

target_link_libraries(astprinter${EXE_SUFFIX}
  astprint
  ${CLANG_LIBS}
  ${libedit_LIBRARIES}
  -lLLVMLineEditor -lLLVMSupport
  -lLLVMLineEditor -lLLVMMCParser
  -lLLVMBitReader -lLLVMProfileData
  -lLLVMOption -lLLVMCore
  -lLLVMMC -lLLVMSupport
  -lLLVMDemangle
  ${LLVM_SYSTEM_LIBS}
)

add_custom_target(executable
  COMMENT "Target to compile astprinter executable"
  DEPENDS astprinter${EXE_SUFFIX}
)

install(
  TARGETS astprinter${EXE_SUFFIX} astprint
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include
)
