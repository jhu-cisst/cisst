#
#
# (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
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
set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision cisstStereoVisionQt cisstQt)

# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND)

  # load cisst configuration
  include (${CISST_USE_FILE})

  if (CISST_SVL_HAS_X11 OR WIN32)

    if (CISST_HAS_QT4)
      set (USED_CISST_LIBRARIES cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision cisstStereoVisionQt)
    else (CISST_HAS_QT4)
      set (USED_CISST_LIBRARIES cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision)
    endif (CISST_HAS_QT4)

    add_executable (svlExVideoPlayer videoplayer.cpp)
    set_property (TARGET svlExVideoPlayer PROPERTY FOLDER "cisstStereoVision/examples")
    cisst_target_link_libraries (svlExVideoPlayer ${USED_CISST_LIBRARIES})

  else (CISST_SVL_HAS_X11 OR WIN32)
    message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires CISST_SVL_HAS_X11 or WIN32")
  endif (CISST_SVL_HAS_X11 OR WIN32)

else (cisst_FOUND)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND)
