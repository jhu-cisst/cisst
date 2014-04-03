#
#
# (C) Copyright 2005-2012 Johns Hopkins University (JHU), All Rights
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

set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstPython)
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES} QUIET)

if (cisst_FOUND_AS_REQUIRED)
  include (${CISST_USE_FILE})

  # Create SineGenerator as a shared library
  add_library (SineGenerator SHARED SineGenerator.cpp SineGenerator.h)
  set_property (TARGET SineGenerator PROPERTY FOLDER "cisstInteractive/examples")
  cisst_target_link_libraries (SineGenerator ${REQUIRED_CISST_LIBRARIES})

  # If OS Abstraction is compiled, offer a choice
  # if (CISST_cisstOSAbstraction)
  #  option (EXAMPLE_IRE_USE_OSATHREAD "Use osaThread in pythonEmbeddedIRE example." OFF)
  #  mark_as_advanced (EXAMPLE_IRE_USE_OSATHREAD)
  #endif (CISST_cisstOSAbstraction)

  # Allow generated c++ code to find header files
  include_directories (${CMAKE_CURRENT_SOURCE_DIR})

  # To link with build libraries
  link_directories (${CMAKE_CURRENT_BINARY_DIR})

  # Run SWIG on the interface file
  cisst_add_swig_module (MODULE SineGenerator
                         INTERFACE_FILENAME SineGeneratorPython.i
                         FOLDER "cisstMultiTask/examples"
                         MODULE_LINK_LIBRARIES SineGenerator)

  add_executable (pythonEmbeddedIRE pythonEmbeddedIRE.cpp)
  set_property (TARGET pythonEmbeddedIRE PROPERTY FOLDER "cisstInteractive/examples")

  # if (EXAMPLE_IRE_USE_OSATHREAD)
  add_definitions (-DCISST_OSATHREAD)
  cisst_target_link_libraries (pythonEmbeddedIRE cisstCommon cisstInteractive cisstOSAbstraction)
  #else (EXAMPLE_IRE_USE_OSATHREAD)
  #cisst_target_link_libraries (pythonEmbeddedIRE cisstCommon cisstInteractive)
  #endif (EXAMPLE_IRE_USE_OSATHREAD)

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

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
