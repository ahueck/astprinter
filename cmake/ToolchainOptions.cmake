set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(LLVM CONFIG HINTS "${LLVM_DIR}")
if(NOT LLVM_FOUND)
  message(STATUS "LLVM not found at: ${LLVM_DIR}.")
  find_package(LLVM REQUIRED CONFIG)
endif()

set_package_properties(LLVM PROPERTIES
  URL https://llvm.org/
  TYPE REQUIRED
  PURPOSE
  "LLVM framework installation required to compile."
)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

find_package(Clang REQUIRED)

include(AddLLVM)
include(clang-tidy)
include(clang-format)
include(log-util)
include(target-util)

set(LOG_LEVEL 0 CACHE STRING "Granularity of the logger. 3 is most verbose, 0 is least.")

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
  message(STATUS "Building as debug (default)")
endif ()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${astprinter_SOURCE_DIR}/install/astprinter" CACHE PATH "Default install path" FORCE)
  message(STATUS "Installing to (default): ${CMAKE_INSTALL_PREFIX}")
endif ()
