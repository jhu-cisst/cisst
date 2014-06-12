#
# $Id: CMakeLists.txt 3331 2012-01-10 22:34:09Z adeguet1 $
#
# (C) Copyright 2014 Johns Hopkins University (JHU), All Rights
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
set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision)

# find cisst and make sure the required libraries have been compiled
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES} QUIET)

if (cisst_FOUND)

  # load cisst configuration
  include (${CISST_USE_FILE})

  if (CISST_SVL_HAS_X11 OR WIN32)
    add_executable (cisstVideoConverter main.cpp)
    set_property (TARGET cisstVideoConverter PROPERTY FOLDER "cisstStereoVision/applications")
    cisst_target_link_libraries (cisstVideoConverter ${REQUIRED_CISST_LIBRARIES})

  else (CISST_SVL_HAS_X11 OR WIN32)
    message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires CISST_SVL_HAS_X11 or WIN32")
  endif (CISST_SVL_HAS_X11 OR WIN32)

else (cisst_FOUND)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND)
