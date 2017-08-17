set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# Setup of all necessary include/lib dirs for the development of a Clang based tool
find_package(LLVM REQUIRED)
find_package(Clang REQUIRED)
find_package(LibEdit REQUIRED)
include(AddLLVM)

if(NOT CLANG_FOUND)
  message(FATAL_ERROR "Detection of Clang failed.")
else()
  include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
  add_definitions(${LLVM_DEFINITIONS})
  link_directories(${LLVM_LIBRARY_DIRS})
  set(TOOLCHAIN_LINK_LIB ${CLANG_LIBS} ${LLVM_LIBS} ${LLVM_SYSTEM_LIBS}) 
endif()

if(NOT DEFINED LOG_LEVEL)
  set(LOG_LEVEL 1)
endif()
add_definitions(-DLOG_LEVEL=${LOG_LEVEL})

# set default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Misc. find out if clang compiler is used
if(CMAKE_CXX_COMPILER MATCHES "clang\\+\\+.*")
  set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()

# Setup general compiler options
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
  # Ignore for now: ${LLVM_COMPILE_FLAGS}
  set(CXX_ONLY_FLAGS "-std=c++11 -Wall -Wextra -pedantic -Wunreachable-code -Wwrite-strings -Wpointer-arith -Wcast-align -Wcast-qual -Wno-unused-parameter -fno-rtti ${CXX_ONLY_FLAGS}")

  # Release build
  set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
  # Release build with debug symbols
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2")
  # Debug Build
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  # Release build optimized for size
  set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os")

  if(USE_GOLD)
    execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -fuse-ld=gold -Wl,--version
    ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
    if("${LD_VERSION}" MATCHES "GNU gold")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=gold")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    else()
      message(WARNING "GNU gold linker not available.")
    endif()
  endif()
endif()

if(USE_CCACHE)
  find_program(CCACHE_EXISTS ccache)
  if(CCACHE_EXISTS)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
  else()
    message(WARNING "CCache not available.")
  endif()
endif()

# Sanitizer support only with Clang (for now)
if(CMAKE_COMPILER_IS_CLANGXX AND SANITIZE)
  # A debug build configuration is expected. We override for performance reasons:
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O1")
  string(TOUPPER ${SANITIZE} SANITIZE_UPPER)
  if(SANITIZE_UPPER STREQUAL "ASAN")
    set(CXX_ONLY_FLAGS "${CXX_ONLY_FLAGS} -fsanitize=address -fno-optimize-sibling-calls -fno-omit-frame-pointer")
  elseif(SANITIZE_UPPER STREQUAL "MSAN")
    set(CXX_ONLY_FLAGS "${CXX_ONLY_FLAGS} -fsanitize=memory -fno-optimize-sibling-calls -fno-omit-frame-pointer -fsanitize-memory-track-origins=2")
  elseif(SANITIZE_UPPER STREQUAL "UBSAN")
    set(CXX_ONLY_FLAGS "${CXX_ONLY_FLAGS} -fsanitize=undefined -fsanitize=unsigned-integer-overflow -fno-omit-frame-pointer")
  endif()
endif()

# COMMON_FLAGS is unsued for now
set(CMAKE_CXX_FLAGS  "${CXX_ONLY_FLAGS} ${COMMON_FLAGS} ${CMAKE_CXX_FLAGS}")

string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UPPER)

if(CMAKE_BUILD_TYPE_UPPER STREQUAL "DEBUG")
  set(EXE_SUFFIX "Debug")
elseif(CMAKE_BUILD_TYPE_UPPER STREQUAL "RELEASE")
  set(EXE_SUFFIX "")
elseif(CMAKE_BUILD_TYPE_UPPER STREQUAL "RELWITHDEBINFO")
  set(EXE_SUFFIX "RelDebg")
elseif(CMAKE_BUILD_TYPE_UPPER STREQUAL "MINSIZEREL")
  set(EXE_SUFFIX "RelMin")
else()
  message(WARNING "Unknown exe suffix found.")
  set(EXE_SUFFIX "Unknown")
endif()

if(CMAKE_BUILD_TYPE_UPPER STREQUAL "RELWITHDEBINFO")
  set(ALL_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
elseif(CMAKE_BUILD_TYPE_UPPER STREQUAL "RELEASE")
  set(ALL_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})
elseif(CMAKE_BUILD_TYPE_UPPER STREQUAL "MINSIZEREL")
  set(ALL_CXX_FLAGS ${CMAKE_CXX_FLAGS_MINSIZEREL})
else()
  # Fallback to Debug
  set(ALL_CXX_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
endif()

message(STATUS "Build Type: "             ${CMAKE_BUILD_TYPE})
if(SANITIZE)
  message(STATUS "Requested Sanitizer: "  ${SANITIZE})
else()
  message(STATUS "Requested Sanitizer: None")
endif()
if(CCACHE_EXISTS)
  message(STATUS "CCache is active.")
endif()
message(STATUS "Logging Level: "          ${LOG_LEVEL})
message(STATUS "Compiler Flags (Modus): " ${ALL_CXX_FLAGS})
message(STATUS "Compiler Flags (cmake): " ${CMAKE_CXX_FLAGS})
message(STATUS "Found LLVM DIR: "         ${LLVM_ROOT_DIR})
message(STATUS "Found Clang: "            ${CLANG_FOUND})
message(STATUS "LLVM Include Dir: "       ${LLVM_INCLUDE_DIRS})
message(STATUS "LLVM CXX Flags: "         ${LLVM_COMPILE_FLAGS})
message(STATUS "LLVM LD Flags: "          ${LLVM_LIBRARY_DIRS})
message(STATUS "LLVM Definitions: "       ${LLVM_DEFINITIONS})
message(STATUS "LLVM System Libs: "       ${LLVM_SYSTEM_LIBS})
message(STATUS "LLVM Libs: "              ${LLVM_LIBS})
message(STATUS "Clang Libs: "             ${CLANG_LIBS})
