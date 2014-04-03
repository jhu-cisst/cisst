#
#
# (C) Copyright 2004-2012 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


project (pythonEmbedded)

set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstPython)
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES} QUIET)

if (cisst_FOUND_AS_REQUIRED)
  include (${CISST_USE_FILE})

  # Create myDerivedClass as a shared library
  add_library (myDerivedClass SHARED myDerivedClass.cpp myDerivedClass.h)
  set_property (TARGET myDerivedClass PROPERTY FOLDER "cisstInteractive/examples")
  cisst_target_link_libraries (myDerivedClass ${REQUIRED_CISST_LIBRARIES})

  # Create myReDerivedClass as a shared library
  add_library (myReDerivedClass SHARED myReDerivedClass.cpp myReDerivedClass.h)
  set_property (TARGET myReDerivedClass PROPERTY FOLDER "cisstInteractive/examples")
  cisst_target_link_libraries (myReDerivedClass ${REQUIRED_CISST_LIBRARIES})
  target_link_libraries (myReDerivedClass myDerivedClass)

  # Allow generated c++ code to find header files
  include_directories (${CMAKE_CURRENT_SOURCE_DIR})

  # To link with build libraries
  link_directories (${CMAKE_CURRENT_BINARY_DIR})

  # Run SWIG on the interface file
  cisst_add_swig_module (MODULE myDerivedClass
                         INTERFACE_FILENAME myDerivedClassPython.i
                         FOLDER "cisstInteractive/examples"
                         MODULE_LINK_LIBRARIES myDerivedClass)

  cisst_add_swig_module (MODULE myReDerivedClass
                         INTERFACE_FILENAME myReDerivedClassPython.i
                         FOLDER "cisstInteractive/examples"
                         MODULE_LINK_LIBRARIES myReDerivedClass myDerivedClass)

  # Built executable with embedding
  add_executable (pythonEmbedded pythonEmbedded.cpp)
  set_property (TARGET pythonEmbedded PROPERTY FOLDER "cisstInteractive/examples")
  cisst_target_link_libraries (pythonEmbedded ${REQUIRED_CISST_LIBRARIES})
  target_link_libraries (pythonEmbedded myDerivedClass myReDerivedClass ${CMAKE_THREAD_LIBS_INIT})

  if (UNIX)
    if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      target_link_libraries (pythonEmbedded util)
    endif (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  endif (UNIX)

  # Copy the Python example to the binary tree
  add_custom_command (TARGET pythonEmbedded
                      POST_BUILD
                      COMMAND ${CMAKE_COMMAND}
                      ARGS -E copy
                           ${CMAKE_CURRENT_SOURCE_DIR}/pythonEmbedded.py
                           ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/pythonEmbedded.py
                      COMMENT "pythonEmbedded: copy pythonEmbedded.py to the executable directory")

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
