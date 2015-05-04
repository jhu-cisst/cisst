#
# (C) Copyright 2003-2014 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

cmake_minimum_required (VERSION 2.6)

# create a list of libraries needed for this project
set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstVectorQt)

# find cisst and make sure the required libraries have been compiled
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  include_directories (${CMAKE_CURRENT_BINARY_DIR})

  if (CISST_HAS_QT4)
    qt4_wrap_cpp (QT_WRAPPED_CPP
                  ExampleClass.h)
  else (CISST_HAS_QT4)
    set (CMAKE_AUTOMOC ON)
  endif (CISST_HAS_QT4)

  add_executable (vctExQtWidgets
                  main.cpp
                  ExampleClass.h
                  ExampleClass.cpp
                  ${QT_WRAPPED_CPP}
                  )
  set_property (TARGET vctExQtWidgets PROPERTY FOLDER "cisstVector/examples")

  # link against cisst libraries (and dependencies)
  cisst_target_link_libraries (vctExQtWidgets ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
