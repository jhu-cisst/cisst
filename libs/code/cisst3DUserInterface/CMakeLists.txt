#
# $Id$
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
    ui3CursorSphere.cpp
    ui3ImagePlane.cpp
    ui3InputDeviceBase.cpp
    ui3Manager.cpp
    ui3MasterArm.cpp
    ui3MenuBar.cpp
    ui3MenuButton.cpp
    ui3SceneManager.cpp
    ui3SlaveArm.cpp
    ui3VTKRenderer.cpp
    ui3VideoInterfaceFilter.cpp
    ui3VisibleList.cpp
    ui3VisibleObject.cpp
    )

# all header files
SET(HEADER_FILES
    ui3ForwardDeclarations.h
    ui3BehaviorBase.h
    ui3CursorBase.h
    ui3CursorSphere.h
    ui3ImagePlane.h
    ui3InputDeviceBase.h
    ui3Manager.h
    ui3MasterArm.h
    ui3SlaveArm.h
    ui3MenuBar.h
    ui3MenuButton.h
    ui3SceneManager.h
    ui3VTKRenderer.h
    ui3VTKForwardDeclarations.h
    ui3VideoInterfaceFilter.h
    ui3VisibleList.h
    ui3VisibleObject.h
    )

CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)


# find icons

set(CISST_UI3_ICONS_DIR "${cisst_SOURCE_DIR}/libs/etc/cisst3DUserInterface/icons")
file(GLOB CISST_UI3_ICONS RELATIVE ${CISST_UI3_ICONS_DIR} ${CISST_UI3_ICONS_DIR}/*.png)

# post build command to copy icons in build tree
foreach(icon ${CISST_UI3_ICONS})
  add_custom_command(TARGET cisst3DUserInterface
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy_if_different
                            ${CISST_UI3_ICONS_DIR}/${icon}
                            ${cisst_BINARY_DIR}/etc/cisst3DUserInterface/icons/${icon})
endforeach(icon)
