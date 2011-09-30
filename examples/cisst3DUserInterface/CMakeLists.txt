#
# $Id$
#
# (C) Copyright 2005-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

cmake_minimum_required (VERSION 2.8)

# create a list of libraries needed for this project
set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstOSAbstraction
                              cisstMultiTask cisstStereoVision cisst3DUserInterface)

# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} QUIET)

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  include_directories (behaviors)
  add_subdirectory (behaviors)

#   # Sensable specific examples, requires cisstDevices
#   if (CISST_DEV_HAS_SENSABLEHD)
#     add_subdirectory(SensableNoVideo)
#     add_subdirectory(SensableStereoVideo)
#     add_subdirectory(SensableStereoVideoNetwork)
#   else (CISST_DEV_HAS_SENSABLEHD)
#     message ("cisst3DUserInterface examples SensableNoVideo, SensableStereoVideo, and SensableStereoVideoNetwork will not be compiled since they require CISST_DEV_HAS_SENSABLEHD")
#   endif (CISST_DEV_HAS_SENSABLEHD)

#   # Intuitive Surgical Inc daVinci - requires Read Only API.  Contact cisst devel group.
#   option (CISST_UI3_HAS_DAVINCI "Build 3DUserInterface example using the daVinci API" OFF)
#   if (CISST_UI3_HAS_DAVINCI)
#     option (CISST_UI3_HAS_DAVINCI_BBAPI "Use the daVinci BBAPI" OFF)
#     if (CISST_UI3_HAS_DAVINCI_BBAPI)
#       find_package (cisstDaVinci-isi-bbapi)
#       include_directories (${cisstDaVinci-isi-bbapi_INCLUDE_DIR})
#       link_directories (${cisstDaVinci-isi-bbapi_LIBRARY_DIR})
#     endif (CISST_UI3_HAS_DAVINCI_BBAPI)
#     set(daVinci_ALL_LIBRARIES ${daVinci_LIBRARIES} ${BBAPI_LIBRARIES} ${cisstDaVinci-isi-bbapi_LIBRARIES})
#     add_subdirectory(daVinciStereoVideo)
#   else (CISST_UI3_HAS_DAVINCI)
#     message ("cisst3DUserInterface example daVinciStereoVideo will not be compiled since it requires CISST_UI3_HAS_DAVINCI")
#   endif (CISST_UI3_HAS_DAVINCI)

# else (CISST_BUILD_cisst3DUserInterface)
#   message ("cisst3DUserInterface examples will not be compiled since they require CISST_BUILD_3DUserInterface")
# endif (CISST_BUILD_cisst3DUserInterface)

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
