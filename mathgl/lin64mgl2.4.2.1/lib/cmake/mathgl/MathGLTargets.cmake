# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget mgl mgl-static mgl-qt5 mgl-qt5-static mgl-qt mgl-qt-static udav mglconv mgl.cgi mglview)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# The installation prefix configured by this project.
set(_IMPORT_PREFIX "/home/don_vanchos/capsys-legacy/mathgl/lin64")

# Create imported target mgl
add_library(mgl SHARED IMPORTED)

set_target_properties(mgl PROPERTIES
  INTERFACE_LINK_LIBRARIES "/usr/lib/x86_64-linux-gnu/libpng.so;/usr/lib/x86_64-linux-gnu/libz.so;/usr/lib/x86_64-linux-gnu/libz.so;m"
)

# Create imported target mgl-static
add_library(mgl-static STATIC IMPORTED)

set_target_properties(mgl-static PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "MGL_STATIC_DEFINE"
  INTERFACE_LINK_LIBRARIES "/usr/lib/x86_64-linux-gnu/libpng.so;/usr/lib/x86_64-linux-gnu/libz.so;/usr/lib/x86_64-linux-gnu/libz.so;m"
)

# Create imported target mgl-qt5
add_library(mgl-qt5 SHARED IMPORTED)

set_target_properties(mgl-qt5 PROPERTIES
  INTERFACE_LINK_LIBRARIES "mgl;Qt5::Core;Qt5::Gui;Qt5::Widgets;Qt5::PrintSupport;Qt5::OpenGL"
)

# Create imported target mgl-qt5-static
add_library(mgl-qt5-static STATIC IMPORTED)

set_target_properties(mgl-qt5-static PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "MGL_STATIC_DEFINE"
  INTERFACE_LINK_LIBRARIES "mgl-static;Qt5::Core;Qt5::Gui;Qt5::Widgets;Qt5::PrintSupport;Qt5::OpenGL"
)

# Create imported target mgl-qt
add_library(mgl-qt SHARED IMPORTED)

set_target_properties(mgl-qt PROPERTIES
  INTERFACE_LINK_LIBRARIES "mgl;Qt5::Core;Qt5::Gui;Qt5::Widgets;Qt5::PrintSupport;Qt5::OpenGL"
)

# Create imported target mgl-qt-static
add_library(mgl-qt-static STATIC IMPORTED)

set_target_properties(mgl-qt-static PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "MGL_STATIC_DEFINE"
  INTERFACE_LINK_LIBRARIES "mgl-static;Qt5::Core;Qt5::Gui;Qt5::Widgets;Qt5::PrintSupport;Qt5::OpenGL"
)

# Create imported target udav
add_executable(udav IMPORTED)

# Create imported target mglconv
add_executable(mglconv IMPORTED)

# Create imported target mgl.cgi
add_executable(mgl.cgi IMPORTED)

# Create imported target mglview
add_executable(mglview IMPORTED)

if(CMAKE_VERSION VERSION_LESS 2.8.12)
  message(FATAL_ERROR "This file relies on consumers using CMake 2.8.12 or greater.")
endif()

# Load information for each installed configuration.
get_filename_component(_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(GLOB CONFIG_FILES "${_DIR}/MathGLTargets-*.cmake")
foreach(f ${CONFIG_FILES})
  include(${f})
endforeach()

# Cleanup temporary variables.
set(_IMPORT_PREFIX)

# Loop over all imported files and verify that they actually exist
foreach(target ${_IMPORT_CHECK_TARGETS} )
  foreach(file ${_IMPORT_CHECK_FILES_FOR_${target}} )
    if(NOT EXISTS "${file}" )
      message(FATAL_ERROR "The imported target \"${target}\" references the file
   \"${file}\"
but this file does not exist.  Possible reasons include:
* The file was deleted, renamed, or moved to another location.
* An install or uninstall procedure did not complete successfully.
* The installation package was faulty and contained
   \"${CMAKE_CURRENT_LIST_FILE}\"
but not all the files it references.
")
    endif()
  endforeach()
  unset(_IMPORT_CHECK_FILES_FOR_${target})
endforeach()
unset(_IMPORT_CHECK_TARGETS)

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)
