#
# $Id: CMakeLists.txt,v 1.6 2009/02/17 22:00:10 anton Exp $
#
# CMakeLists for cisst3DUserInterface
#
# (C) Copyright 2003-2008 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# set the library name
SET(LIBRARY cisst3DUserInterface)

# set dependencies
SET(DEPENDENCIES
    cisstCommon
    cisstVector
    cisstOSAbstraction
    cisstStereoVision
    cisstMultiTask
    cisstParameterTypes
    )

# Find VTK settings
FIND_PACKAGE(VTK REQUIRED)
IF(NOT VTK_USE_RENDERING)
    MESSAGE(FATAL_ERROR "${LIBRARY} requires VTK_USE_RENDERING.")
ENDIF(NOT VTK_USE_RENDERING)
INCLUDE(${VTK_USE_FILE})

IF(CISST_SVL_HAS_OPENCV)
    # Link each example against the OpenCV library if enabled
    FIND_PACKAGE(OpenCV)
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
    #LINK_DIRECTORIES(${OpenCV_LINK_DIRECTORIES})
    #TARGET_LINK_LIBRARIES(stereoTutorialSimpleStream ${OpenCV_LIBRARIES})
ENDIF(CISST_SVL_HAS_OPENCV)

# all source files
SET(SOURCE_FILES
    ui3BehaviorBase.cpp
    ui3ImagePlane.cpp
    ui3Cursor.cpp
    ui3InputDeviceBase.cpp
    ui3Manager.cpp
    ui3MenuBar.cpp
    ui3MenuButton.cpp
    ui3SceneManager.cpp
    ui3VTKRenderer.cpp
    ui3VideoInterfaceFilter.cpp
    ui3VisibleObject.cpp
    )

# all header files
SET(HEADER_FILES
    ui3ForwardDeclarations.h
    ui3BehaviorBase.h
    ui3ImagePlane.h
    ui3Cursor.h
    ui3InputDeviceBase.h
    ui3Manager.h
    ui3MenuBar.h
    ui3MenuButton.h
    ui3SceneManager.h
    ui3VTKRenderer.h
    ui3VTKForwardDeclarations.h
    ui3VideoInterfaceFilter.h
    ui3VisibleObject.h
    )


CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)
