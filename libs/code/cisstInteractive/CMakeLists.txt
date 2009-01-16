#
# $Id: CMakeLists.txt,v 1.6 2007/04/26 19:33:56 anton Exp $
#
# (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# CMakeLists for cisstInteractive
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# set the library name
SET(LIBRARY cisstInteractive)

# set dependencies
SET(DEPENDENCIES
    cisstCommon)

# all source files
SET(SOURCE_FILES
    ireFramework.cpp
    )

# all header files
SET(HEADER_FILES
    ireExport.h
    ireFramework.h
    )

# Add library to project
CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

# Link against these libraries
TARGET_LINK_LIBRARIES(cisstInteractive debug ${PYTHON_DEBUG_LIBRARIES} optimized ${PYTHON_LIBRARIES}) 

# Call makeIrepy to compile the Python source files (*.py) and place
# the results (*.pyc files) in the destination directory.
ADD_CUSTOM_TARGET(cisstInteractive_COMPILE_IREPY ALL
                  ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/makeIrepy.py
                  ${CMAKE_CURRENT_SOURCE_DIR}/irepy  # src-dir
                  ${LIBRARY_OUTPUT_PATH}/irepy)   # dest-dir
