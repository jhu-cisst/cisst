#
# $Id$
#
# (C) Copyright 2005-2008 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# name of project and executable
PROJECT(multiTaskTutorialExample6)

# use CMake provided script to locate FLTK
FIND_PACKAGE(FLTK REQUIRED)

# add include paths
INCLUDE_DIRECTORIES(${multiTaskTutorialExample6_SOURCE_DIR}
                    ${FLTK_INCLUDE_DIR})

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(multiTaskTutorialExample6
               # code files to compile
               robotLowLevel.cpp
               userInterface.cpp
               main.cpp
               # header files are provided for information
               userInterface.h
	       robotLowLevel.h
	       )

# this program 
TARGET_LINK_LIBRARIES(multiTaskTutorialExample6 ${FLTK_LIBRARIES})

# link with the cisst libraries
CISST_REQUIRES("multiTaskTutorialExample6" "cisstCommon;cisstOSAbstraction;cisstMultiTask")

# For apple applications with GUI
IF(APPLE)
  FIND_PROGRAM(APPLE_REZ Rez /Developer/Tools)
  ADD_CUSTOM_COMMAND(TARGET multiTaskTutorialExample6
                     POST_BUILD
                     COMMAND ${APPLE_REZ}
                     ARGS ${FLTK_INCLUDE_DIR}/FL/mac.r Carbon.r -o ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/multiTaskTutorialExample6)
ENDIF(APPLE)
