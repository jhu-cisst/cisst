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

  # sawSensablePhantom has been compiled within cisst, we should find it automatically
  find_package (sawSensablePhantom HINTS "${CISST_BINARY_DIR}/saw/components" QUIET)
  find_package (sawKeyboard HINTS "${CISST_BINARY_DIR}/saw/components" QUIET)

  if (sawSensablePhantom_FOUND AND sawKeyboard_FOUND)
    add_subdirectory(SensableNoVideo)
  endif (sawSensablePhantom_FOUND AND sawKeyboard_FOUND)

#     add_subdirectory(SensableStereoVideo)
#     add_subdirectory(SensableStereoVideoNetwork)
#   else (CISST_DEV_HAS_SENSABLEHD)
#     message ("cisst3DUserInterface examples SensableNoVideo, SensableStereoVideo, and SensableStereoVideoNetwork will not be compiled since they require CISST_DEV_HAS_SENSABLEHD")
#   endif (CISST_DEV_HAS_SENSABLEHD)

  # sawIntuitiveDaVinci has been compiled within cisst, we should find it automatically
  find_package (sawIntuitiveDaVinci HINTS "${CISST_BINARY_DIR}/saw/components" QUIET)

  if (sawIntuitiveDaVinci_FOUND)
    add_subdirectory(daVinciStereoVideo)
  endif (sawIntuitiveDaVinci_FOUND)

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
