##
## Script relies on llvm-config binary to gather the necessary information
## to compile a LLVM/Clang based tool.
##
## Note:  If llvm-config can not be located, use ${LLVM_ROOT_DIR} variable 
##        for the path to the respective bin/ folder.
##


function(setup_llvm)
## Setup for llvm-config executable
##
## Out:
##  Var: ${LLVM_ROOT_DIR} (Prefix of LLVM)
##  Exe: ${LLVM_CONFIG}
  set(llvm_config_names llvm-config-4.0
                        llvm-config)

  find_program(LLVM_CONFIG
      NAMES ${llvm_config_names}
      PATHS /usr/bin /usr/local/bin /opt/local/bin ${LLVM_ROOT_DIR}/bin
      DOC "llvm-config executable.")

  if(LLVM_CONFIG)
    message(STATUS "llvm-config found: ${LLVM_CONFIG}")
  else()
    message(FATAL_ERROR "llvm-config NOT found. Search failed: ${llvm_config_names}")
  endif()

  execute_process(
    COMMAND ${LLVM_CONFIG} --prefix
    OUTPUT_VARIABLE LLVM_ROOT_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(LLVM_ROOT_DIR ${LLVM_ROOT_DIR} PARENT_SCOPE)
  set(LLVM_CONFIG ${LLVM_CONFIG} PARENT_SCOPE)
endfunction()

function(set_llvm_cpp_flags)
## Compiler flags used for LLVM 
## Note:  Removes definitions (-D and -U), 
##        Optimization flags (-O, -g)
##        Include flags (-I)
## Out:
##  Var: ${LLVM_COMPILE_FLAGS}
  execute_process(
    COMMAND ${LLVM_CONFIG} --cxxflags
    OUTPUT_VARIABLE llvm_cxxflags
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  
  string(REGEX REPLACE "-D[^ ]*|-U[^ ]*|-I[^ ]*|-g[^ ]*|-[oO][^ ]*" "" llvm_cxxflags ${llvm_cxxflags})
  string(REGEX REPLACE "[ ]+" " " llvm_cxxflags ${llvm_cxxflags})
  set(LLVM_COMPILE_FLAGS ${llvm_cxxflags} PARENT_SCOPE)
endfunction()

function(set_llvm_lib_path)
## ld flags of LLVM (/usr/lib/...)
## Out:
##  Var: ${LLVM_LIBRARY_DIRS}
  execute_process(
    COMMAND ${LLVM_CONFIG} --libdir
    OUTPUT_VARIABLE llvm_ldflags
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  
  set(LLVM_LIBRARY_DIRS ${llvm_ldflags} PARENT_SCOPE)
endfunction()

function(set_llvm_system_libs)
## System libraries used by LLVM
## Out:
##  Var: ${LLVM_SYSTEM_LIBS}
  execute_process(
    COMMAND ${LLVM_CONFIG} --system-libs
    OUTPUT_VARIABLE llvm_system_libs
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  
  set(LLVM_SYSTEM_LIBS ${llvm_system_libs} PARENT_SCOPE)
endfunction()

function(set_llvm_include_dir)
## Include directory of LLVM (-I)
## Out:
##  Var: ${LLVM_INCLUDE_DIRS}
  execute_process(
    COMMAND ${LLVM_CONFIG} --includedir
    OUTPUT_VARIABLE llvm_includdir
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  
  set(LLVM_INCLUDE_DIRS ${llvm_includdir} PARENT_SCOPE)
endfunction()

function(set_llvm_definitions)
## Definitions used by LLVM (-D) except -DNDEBUG or -DDEBUG
## Out:
##  Var: ${LLVM_DEFINITIONS}
  execute_process(
    COMMAND ${LLVM_CONFIG} --cppflags
    OUTPUT_VARIABLE llvm_definitions
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  
  string(REGEX REPLACE "-I[^ ]*" "" llvm_definitions ${llvm_definitions})
  string(REGEX REPLACE "[ ]+" " " llvm_definitions ${llvm_definitions})
  string(REGEX REPLACE "-DN?DEBUG" " " llvm_definitions ${llvm_definitions})
  set(LLVM_DEFINITIONS ${llvm_definitions} PARENT_SCOPE)
endfunction()

function(set_llvm_libs)
## LLVM libraries to link against.
## Out:
##  Var: ${LLVM_LIBS}
  execute_process(
    COMMAND ${LLVM_CONFIG} --libs
    OUTPUT_VARIABLE llvm_libs
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REGEX REPLACE " -" ";-" llvm_libs ${llvm_libs})

  set(LLVM_LIBS ${llvm_libs} PARENT_SCOPE)
endfunction()

setup_llvm()
set_llvm_libs()
set_llvm_cpp_flags()
set_llvm_lib_path()
set_llvm_system_libs()
set_llvm_include_dir()
set_llvm_definitions()

