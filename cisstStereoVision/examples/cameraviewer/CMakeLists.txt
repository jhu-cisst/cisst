#
# (C) Copyright 2006-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

cmake_minimum_required (VERSION 3.16)

# create a list of libraries needed for this project
set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision cisstStereoVisionQt cisstQt)

# find cisst and make sure the required libraries have been compiled
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND)

  if (CISST_SVL_HAS_X11 OR WIN32)

    if (CISST_HAS_QT4)
      set (USED_CISST_LIBRARIES cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision cisstStereoVisionQt)
    else (CISST_HAS_QT4)
      set (USED_CISST_LIBRARIES cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision)
    endif (CISST_HAS_QT4)
    list (TRANSFORM USED_CISST_LIBRARIES PREPEND "cisst::")

    add_executable (svlExCameraViewer cameraviewer.cpp)
    set_property (TARGET svlExCameraViewer PROPERTY FOLDER "cisstStereoVision/examples")
    target_link_libraries (svlExCameraViewer ${USED_CISST_LIBRARIES})

  else (CISST_SVL_HAS_X11 OR WIN32)
    message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires CISST_SVL_HAS_X11 or WIN32")
  endif (CISST_SVL_HAS_X11 OR WIN32)

else (cisst_FOUND)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND)

