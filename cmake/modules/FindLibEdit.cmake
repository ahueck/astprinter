include(FindPackageHandleStandardArgs)

find_path(libedit_INCLUDE_DIRS
  editline/readline.h
  PATHS ${LIBEDIT_INCLUDE_DIR} /usr/include /usr/local/include
)

find_library(libedit_LIBRARIES
  NAMES edit
  PATHS ${LIBEDIT_LIBRARY_DIR} /usr/lib /usr/local/lib /usr/lib64 /usr/local/lib64
) 

find_package_handle_standard_args(libedit 
  FOUND_VAR libedit_FOUND
  REQUIRED_VARS
  	libedit_LIBRARIES
  	libedit_INCLUDE_DIRS 
)