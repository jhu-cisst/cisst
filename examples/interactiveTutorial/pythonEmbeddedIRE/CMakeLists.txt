#
# $Id$
#
# (C) Copyright 2005-2010 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


project (pythonEmbeddedIRE)

# Create SineGenerator as a shared library
add_library (SineGenerator SHARED SineGenerator.cpp SineGenerator.h)
cisst_target_link_libraries (SineGenerator cisstCommon)

# If OS Abstraction is compiled, offer a choice
if (BUILD_LIBS_cisstOSAbstraction)
  option (EXAMPLE_IRE_USE_OSATHREAD "Use osaThread in pythonEmbeddedIRE example." OFF)
  mark_as_advanced (EXAMPLE_IRE_USE_OSATHREAD)
endif (BUILD_LIBS_cisstOSAbstraction)


# Allow generated c++ code to find header files
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CISST_SWIG_INCLUDE_DIR})

# To link with build libraries
link_directories (${CMAKE_CURRENT_BINARY_DIR})


# Run swig for each interface file
set (MODULES SineGenerator)

foreach (module ${MODULES})
  set (interface ${CMAKE_CURRENT_SOURCE_DIR}/${module}Python.i)
  # We are using C++ code
  set_source_files_properties (${interface} PROPERTIES CPLUSPLUS ON)
  # Set the verbose flag
  set_source_files_properties (${interface}
                               PROPERTIES SWIG_FLAGS "-v;-modern;-fcompact;-fvirtual")
  swig_add_module (${module}Python python ${interface})
  swig_link_libraries (${module}Python ${module}
                       debug ${PYTHON_DEBUG_LIBRARIES}
                       optimized ${PYTHON_LIBRARIES})
  # uses .pyd instead of dll on Windows
  if (WIN32)
    set_target_properties (_${module}Python PROPERTIES SUFFIX .pyd)
    set_target_properties (_${module}Python PROPERTIES DEBUG_POSTFIX "_d")
  endif (WIN32)
  # Copy the .py file generated to wherever the libraries are
  add_custom_command (TARGET _${module}Python
                      POST_BUILD
                      COMMAND ${CMAKE_COMMAND}
                      ARGS -E copy
                              ${CMAKE_CURRENT_BINARY_DIR}/${module}Python.py
                              ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${module}Python.py)
  # Create a SineGenerator.py as CMake assumes one should be created
  # This is a bug that should be fixed in future releases of CMake.
  add_custom_command (TARGET _${module}Python
                      POST_BUILD
                      COMMAND ${CMAKE_COMMAND}
                      ARGS -E copy
                              ${CMAKE_CURRENT_BINARY_DIR}/${module}Python.py
                              ${CMAKE_CURRENT_BINARY_DIR}/${module}.py)
  # Build list of files to clean
  set (CISST_CLEAN_FILES ${CISST_CLEAN_FILES}
                         ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${module}Python.py
                         ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${module}Python.pyc
                         ${module}Python_wrap.cxx)
endforeach (module)

# Additional files to clean
set_directory_properties (PROPERTIES
                          ADDITIONAL_MAKE_CLEAN_FILES
                          "${CISST_CLEAN_FILES}")

add_executable (pythonEmbeddedIRE pythonEmbeddedIRE.cpp)

if (EXAMPLE_IRE_USE_OSATHREAD)
  add_definitions (-DCISST_OSATHREAD)
  cisst_target_link_libraries (pythonEmbeddedIRE cisstCommon cisstInteractive cisstOSAbstraction)
else (EXAMPLE_IRE_USE_OSATHREAD)
  cisst_target_link_libraries (pythonEmbeddedIRE cisstCommon cisstInteractive)
endif (EXAMPLE_IRE_USE_OSATHREAD)

target_link_libraries (pythonEmbeddedIRE SineGenerator ${CMAKE_THREAD_LIBS_INIT})
if (UNIX)
  if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries (pythonEmbeddedIRE util)
  endif (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
endif (UNIX)

# Add custom dependencies on dynamically loaded modules
add_dependencies (pythonEmbeddedIRE
                  _cisstCommonPython
                  _SineGeneratorPython)

# Copy the Python example to the binary tree
add_custom_command (TARGET pythonEmbeddedIRE
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND}
                    ARGS -E copy
                         ${CMAKE_CURRENT_SOURCE_DIR}/pythonEmbeddedIRE.py
                         ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/pythonEmbeddedIRE.py
                    COMMENT "Copy of pythonEmbeddedIRE.py to the library directory")
