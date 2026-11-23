#
# (C) Copyright 2004-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (pythonEmbedded)

set (REQUIRED_CISST_LIBRARIES cisstCommon)
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES} QUIET)

if (cisst_FOUND AND CISST_HAS_PYTHON)
  # Create myDerivedClass as a shared library
  add_library (myDerivedClass SHARED myDerivedClass.cpp myDerivedClass.h)
  set_property (TARGET myDerivedClass PROPERTY FOLDER "cisstInteractive/examples")
  target_link_libraries (myDerivedClass ${REQUIRED_CISST_LIBRARIES})

  # Create myReDerivedClass as a shared library
  add_library (myReDerivedClass SHARED myReDerivedClass.cpp myReDerivedClass.h)
  set_property (TARGET myReDerivedClass PROPERTY FOLDER "cisstInteractive/examples")
  target_link_libraries (myReDerivedClass
    ${REQUIRED_CISST_LIBRARIES}
    myDerivedClass)

  cisst_add_pybind11_module (MODULE myDerivedClassPython
                             SOURCES myDerivedClassPybind.cpp
                             FOLDER "cisstInteractive/examples"
                             MODULE_LINK_LIBRARIES myDerivedClass)

  cisst_add_pybind11_module (MODULE myReDerivedClassPython
                             SOURCES myReDerivedClassPybind.cpp
                             FOLDER "cisstInteractive/examples"
                             MODULE_LINK_LIBRARIES myReDerivedClass myDerivedClass)

  # Built executable with embedding
  add_executable (pythonEmbedded pythonEmbedded.cpp)
  set_property (TARGET pythonEmbedded PROPERTY FOLDER "cisstInteractive/examples")
  target_include_directories (pythonEmbedded PRIVATE ${PYTHON_ALL_INCLUDE_DIRS})
  target_link_libraries (pythonEmbedded
    ${REQUIRED_CISST_LIBRARIES}
    myDerivedClass
    myReDerivedClass
    ${CMAKE_THREAD_LIBS_INIT}
    ${PYTHON_ALL_LIBRARIES}
    util)
  add_dependencies (pythonEmbedded
                    cisstCommonPython
                    myDerivedClassPython
                    myReDerivedClassPython)

  if (UNIX)
    if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    endif (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  endif (UNIX)

  # Copy the Python example to the binary tree
  add_custom_command (TARGET pythonEmbedded
                      POST_BUILD
                      COMMAND ${CMAKE_COMMAND}
                      ARGS -E copy
                           ${CMAKE_CURRENT_SOURCE_DIR}/pythonEmbedded.py
                           ${EXECUTABLE_OUTPUT_PATH}/pythonEmbedded.py
                      COMMENT "pythonEmbedded: copy pythonEmbedded.py to the executable directory")

else ()
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES} and cisst pybind11 support")
endif ()
