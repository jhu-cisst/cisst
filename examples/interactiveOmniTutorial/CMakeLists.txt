#
# $Id: CMakeLists.txt 75 2009-02-24 16:47:20Z adeguet1 $
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
PROJECT(interactiveOmniExample)

find_package(Sensable REQUIRED)
include_directories(${SENSABLE_INCLUDE_DIR})

find_package (FLTK REQUIRED)
# add include paths
INCLUDE_DIRECTORIES(${interactiveOmniExample_SOURCE_DIR}
                    ${FLTK_INCLUDE_DIR})

fltk_wrap_ui (${PROJECT_NAME}
               mainGUI.fl
)

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(${PROJECT_NAME}
               displayTask.cpp
               displayTask.h
               main.cpp
	       )

ADD_EXECUTABLE(OmniExample
	       MainTeleop.cpp
	      )

target_link_libraries(OmniExample ${SENSABLE_LIBRARIES})

TARGET_LINK_LIBRARIES(interactiveOmniExample ${FLTK_LIBRARIES})

# link with the cisst libraries
CISST_REQUIRES(${PROJECT_NAME} "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices")
