#
#
# (C) Copyright 2003-2012 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisst3DUserInterface)

# all source files
set (SOURCE_FILES
     ui3BehaviorBase.cpp
     ui3CursorSphere.cpp
     ui3ImagePlane.cpp
     ui3InputDeviceBase.cpp
     ui3Manager.cpp
     ui3MasterArm.cpp
     ui3MenuBar.cpp
     ui3MenuButton.cpp
     ui3SceneManager.cpp
     ui3Selectable.cpp
     ui3SlaveArm.cpp
     ui3VTKRenderer.cpp
     ui3VTKStippleActor.cpp
     ui3VideoInterfaceFilter.cpp
     ui3VisibleList.cpp
     ui3VisibleObject.cpp
     ui3VisibleAxes.cpp
     ui3Widget3D.cpp
     )

# all header files
set (HEADER_FILES
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
     ui3Selectable.h
     ui3VTKRenderer.h
     ui3VTKStippleActor.h
     ui3VTKForwardDeclarations.h
     ui3VideoInterfaceFilter.h
     ui3VisibleList.h
     ui3VisibleObject.h
     ui3VisibleAxes.h
     ui3Widget3D.h
     )


# VTK Dependency
find_package (VTK REQUIRED)
if (VTK_FOUND)
    mark_as_advanced (FORCE VTK_DIR)

    if (VTK_USE_RENDERING)
        include (${VTK_USE_FILE})

        cisst_set_package_settings (cisst3DUserInterface VTK INCLUDE_DIRECTORIES VTK_INCLUDE_DIRS)
        cisst_set_package_settings (cisst3DUserInterface VTK LINK_DIRECTORIES    VTK_LIBRARY_DIRS)
        set (VTK_LIBRARY_USED vtkRendering vtkHybrid)
        cisst_set_package_settings (cisst3DUserInterface VTK LIBRARIES           VTK_LIBRARY_USED)
        set (VTK_PACKAGE_NAME VTK)
        cisst_set_package_settings (cisst3DUserInterface VTK PACKAGES            VTK_PACKAGE_NAME)
        cisst_set_package_settings (cisst3DUserInterface VTK CMAKE_FILES         VTK_USE_FILE)
        message (STATUS "VTK found - Switch to advanced view for details")

    else (VTK_USE_RENDERING)
        message (FATAL_ERROR "${LIBRARY} requires vtkRendering to be compiled in VTK.")
    endif (VTK_USE_RENDERING)
else (VTK_FOUND)
    mark_as_advanced (CLEAR VTK_DIR)
    message (FATAL_ERROR "${LIBRARY} requires VTK.")
    cisst_unset_all_package_settings (cisst3DUserInterface VTK)
endif (VTK_FOUND)


cisst_add_library (LIBRARY cisst3DUserInterface
                   FOLDER cisst3DUserInterface
                   DEPENDENCIES cisstCommon cisstVector cisstOSAbstraction cisstStereoVision cisstMultiTask cisstParameterTypes
                   SOURCE_FILES ${SOURCE_FILES}
                   HEADER_FILES ${HEADER_FILES}
                   )

# find icons
set (CISST_UI3_ICONS_DIR "${cisst3DUserInterfaceLibs_SOURCE_DIR}/share/icons")
file (GLOB CISST_UI3_ICONS RELATIVE ${CISST_UI3_ICONS_DIR} ${CISST_UI3_ICONS_DIR}/*.png)

# post build command to copy icons in build tree
unset (CISST_UI3_ICONS_WITH_DIR)
foreach (icon ${CISST_UI3_ICONS})
  add_custom_command (TARGET cisst3DUserInterface
                      POST_BUILD
                      COMMAND ${CMAKE_COMMAND}
                      ARGS -E copy_if_different
                             ${CISST_UI3_ICONS_DIR}/${icon}
                             ${CISST_SHARE_DIR}/cisst3DUserInterface/icons/${icon})
  set (CISST_UI3_ICONS_WITH_DIR ${CISST_UI3_ICONS_WITH_DIR} "${CISST_UI3_ICONS_DIR}/${icon}")
endforeach (icon)
install (FILES ${CISST_UI3_ICONS_WITH_DIR} DESTINATION ${CISST_SHARE_INSTALL_SUFFIX}/cisst3DUserInterface/icons COMPONENT cisst3DUserInterface)
