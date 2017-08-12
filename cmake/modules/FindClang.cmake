# Detect Clang libraries
#
# IN: 
#  - ${LLVM_LIBRARY_DIRS}: Required path to detect clang libs
#  - Alternative: ${CLANG_LIB_DIR}
# OUT:
#  - ${CLANG_LIBS}: contains all found Clang libraries for linking
#  - ${CLANG_FOUND}: true if (all) clang libraries were found
#
if(NOT LLVM_LIBRARY_DIRS AND NOT CLANG_LIB_DIR)
  message(FATAL_ERROR "No LLVM/Clang library directory set.")
else()  
  macro(find_and_add_clang_lib _libname_)
    find_library(CLANG_${_libname_}_LIB 
      ${_libname_} 
      PATHS ${LLVM_LIBRARY_DIRS} ${CLANG_LIB_DIR}
      NO_DEFAULT_PATH
    )

    if(CLANG_${_libname_}_LIB)
      set(CLANG_LIBS ${CLANG_LIBS} ${CLANG_${_libname_}_LIB})
    else()
      message(WARNING "Could not find Clang library: " ${_libname_})
    endif()
  endmacro()

  set(CLANG_LIB_NAMES 
    clangAST
    clangASTMatchers
    clangBasic
    clangDynamicASTMatchers
    clangFrontend
    clangQuery
    clangTooling
    clangDynamicASTMatchers
    clangASTMatchers
    clangFrontend
    clangParse
    clangSerialization
    clangSema
    clangEdit
    clangAnalysis
    clangDriver
    clangFormat
    clangToolingCore
    clangAST
    clangRewrite
    clangLex
    clangBasic
  )
  


  
  foreach(clang_lib ${CLANG_LIB_NAMES})
    find_and_add_clang_lib(${clang_lib})
  endforeach()

  if(CLANG_LIBS)
    set(CLANG_FOUND TRUE)
  else()
    set(CLANG_FOUND FALSE)
  endif()
endif()
