#
# $Id$
#
# (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights
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

# Create myDerivedClass as a shared library
cisst_set_directories (cisstCommon cisstVector)
add_library (myDerivedClass SHARED myDerivedClass.cpp myDerivedClass.h)
cisst_target_link_libraries (myDerivedClass cisstCommon cisstVector)

# Create myReDerivedClass as a shared library
add_library (myReDerivedClass SHARED myReDerivedClass.cpp myReDerivedClass.h)
cisst_target_link_libraries (myReDerivedClass cisstCommon cisstVector)
target_link_libraries (myReDerivedClass myDerivedClass)

# Allow generated c++ code to find header files
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# To link with build libraries
link_directories (${CMAKE_CURRENT_BINARY_DIR})

# Run SWIG on the interface file
cisst_add_swig_module(MODULE myDerivedClass
                      INTERFACE_FILENAME myDerivedClassPython.i
                      INTERFACE_DIRECTORY .
                      CISST_LIBRARIES myDerivedClass)

cisst_add_swig_module(MODULE myReDerivedClass
                      INTERFACE_FILENAME myReDerivedClassPython.i
                      INTERFACE_DIRECTORY .
                      CISST_LIBRARIES myReDerivedClass myDerivedClass)

# Built executable with embedding
add_executable (pythonEmbedded pythonEmbedded.cpp)
cisst_target_link_libraries (pythonEmbedded cisstCommon cisstVector)
target_link_libraries (pythonEmbedded myDerivedClass myReDerivedClass ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (pythonEmbedded
                       debug ${PYTHON_DEBUG_LIBRARIES}
                       optimized ${PYTHON_LIBRARIES})
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
                    COMMENT "Copy of pythonEmbedded.py to the executable directory")
