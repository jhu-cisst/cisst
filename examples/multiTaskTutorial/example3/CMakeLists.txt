#
# $Id: CMakeLists.txt,v 1.4 2008/05/30 19:45:18 anton Exp $
#
# (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
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
PROJECT(multiTaskTutorialExample3)

# use CMake provided script to locate FLTK
FIND_PACKAGE(FLTK REQUIRED)

# add include paths
INCLUDE_DIRECTORIES(${multiTaskTutorialExample3_SOURCE_DIR}
                    ${FLTK_INCLUDE_DIR})

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(multiTaskTutorialExample3
               # code files to compile
               robotLowLevel.cpp
	       monitorTask.cpp
               appTask.cpp
               appTaskUI.cpp
               main.cpp
               # header files are provided for information
               robotLowLevel.h
	       monitorTask.h
               appTask.h
               appTaskUI.h
	       )

# this program 
TARGET_LINK_LIBRARIES(multiTaskTutorialExample3 ${FLTK_LIBRARIES})

# link with the cisst libraries
CISST_REQUIRES("multiTaskTutorialExample3" "cisstCommon;cisstOSAbstraction;cisstMultiTask")


IF(CISST_HAS_LINUX_RTAI)
  TARGET_LINK_LIBRARIES(multiTaskTutorialExample3 ${RTAI_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
ENDIF(CISST_HAS_LINUX_RTAI)

# For apple applications with GUI
IF(APPLE)
  FIND_PROGRAM(APPLE_REZ Rez /Developer/Tools)
  ADD_CUSTOM_COMMAND(TARGET multiTaskTutorialExample3
                     POST_BUILD
                     COMMAND ${APPLE_REZ}
                     ARGS ${FLTK_INCLUDE_DIR}/FL/mac.r Carbon.r -o ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/multiTaskTutorialExample3)
ENDIF(APPLE)
