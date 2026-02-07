function(astprinter_project_compile_options target)
  cmake_parse_arguments(ARG "" "" "PRIVATE_FLAGS;PUBLIC_FLAGS" ${ARGN})

  target_compile_options(${target} PRIVATE
    -Wall -Wextra -pedantic
    -Wunreachable-code -Wwrite-strings
    -Wpointer-arith -Wcast-align
    -Wcast-qual -Wno-unused-parameter
    -fno-rtti
    )

  if (ARG_PRIVATE_FLAGS)
    target_compile_options(${target} PRIVATE
      "${ARG_PRIVATE_FLAGS}"
      )
  endif ()

  if (ARG_PUBLIC_FLAGS)
    target_compile_options(${target} PUBLIC
      "${ARG_PUBLIC_FLAGS}"
      )
  endif ()
endfunction()

function(astprinter_project_compile_definitions target)
  cmake_parse_arguments(ARG "" "" "PRIVATE_DEFS;PUBLIC_DEFS" ${ARGN})

  target_compile_definitions(${target} PRIVATE "LLVM_VERSION_MAJOR=${LLVM_VERSION_MAJOR}")

  if (ARG_PRIVATE_DEFS)
    target_compile_definitions(${target} PRIVATE
      "${ARG_PRIVATE_DEFS}"
      )
  endif ()

  if (ARG_PUBLIC_DEFS)
    target_compile_definitions(${target} PUBLIC
      "${ARG_PUBLIC_DEFS}"
      )
  endif ()
endfunction()

function(astprinter_find_llvm_progs target names)
  cmake_parse_arguments(ARG "ABORT_IF_MISSING;SHOW_VAR" "DEFAULT_EXE" "HINTS" ${ARGN})
  set(TARGET_TMP ${target})

  find_program(
    ${target}
    NAMES ${names}
    PATHS ${LLVM_TOOLS_BINARY_DIR}
    NO_DEFAULT_PATH
  )
  if(NOT ${target})
    find_program(
      ${target}
      NAMES ${names}
      HINTS ${ARG_HINTS}
    )
  endif()

  if(NOT ${target})
    set(target_missing_message "")
    if(ARG_DEFAULT_EXE)
      unset(${target} CACHE)
      set(${target}
          ${ARG_DEFAULT_EXE}
          CACHE
          STRING
          "Default value for ${TARGET_TMP}."
      )
      set(target_missing_message "Using default: ${ARG_DEFAULT_EXE}")
    endif()

    set(message_status STATUS)
    if(ARG_ABORT_IF_MISSING AND NOT ARG_DEFAULT_EXE)
      set(message_status SEND_ERROR)
    endif()
    message(${message_status}
      "Did find LLVM program " "${names}"
      " in ${LLVM_TOOLS_BINARY_DIR}, in system path or hints " "\"${ARG_HINTS}\"" ". "
      ${target_missing_message}
    )
  endif()

  set(${TARGET_TMP} "${${TARGET_TMP}}" PARENT_SCOPE)

  if(ARG_SHOW_VAR)
    mark_as_advanced(CLEAR ${target})
  else()
    mark_as_advanced(${target})
  endif()
endfunction()