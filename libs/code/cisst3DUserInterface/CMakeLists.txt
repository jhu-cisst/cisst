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
    ui3VisibleAxes.cpp
    )

# all header files
SET(HEADER_FILES
    ui3ForwardDeclarations.h
    ui3BehaviorBase.h
    ui3CursorBase.h
    ui3CursorSphere.h
    ui3Export.h
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
    ui3VisibleAxes.h
    )


# VTK Dependency
FIND_PACKAGE(VTK REQUIRED)
IF(VTK_FOUND)
    MARK_AS_ADVANCED(FORCE VTK_DIR)

    IF(VTK_USE_RENDERING)
        INCLUDE(${VTK_USE_FILE})

        SET(UI3_ADDITIONAL_INCLUDE_DIRECTORIES
            "${UI3_ADDITIONAL_INCLUDE_DIRECTORIES}"
            "${VTK_INCLUDE_DIRS}"
            )
        SET(UI3_ADDITIONAL_LINK_DIRECTORIES
            "${UI3_ADDITIONAL_LINK_DIRECTORIES}"
            "${VTK_LIBRARY_DIRS}"
            )
        SET(UI3_ADDITIONAL_LIBRARIES
            "${UI3_ADDITIONAL_LIBRARIES}"
            vtkRendering
            vtkHybrid
            )
        MESSAGE(STATUS "VTK found - Switch to advanced view for details")

    ELSE(VTK_USE_RENDERING)
        MESSAGE(FATAL_ERROR "${LIBRARY} requires vtkRendering to be compiled in VTK.")
    ENDIF(VTK_USE_RENDERING)
ELSE(VTK_FOUND)
    MARK_AS_ADVANCED(CLEAR VTK_DIR)
    MESSAGE(FATAL_ERROR "${LIBRARY} requires VTK.")
ENDIF(VTK_FOUND)


# Exporting settings to applications that link against cisst
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_INCLUDE_DIRECTORIES UI3_ADDITIONAL_INCLUDE_DIRECTORIES)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LINK_DIRECTORIES UI3_ADDITIONAL_LINK_DIRECTORIES)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES UI3_ADDITIONAL_LIBRARIES)


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


CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

