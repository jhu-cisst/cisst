#
# $Id$
#
# (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# create a list of libraries needed for this project
set (REQUIRED_CISST_LIBRARIES cisstCommon cisstCommonQt cisstCommonXML
                              cisstVector cisstOSAbstraction cisstNumerical
                              cisstMultiTask cisstParameterTypes cisstRobot
                              cisstStereoVision)

# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # cisstMicronTracker has been compiled within cisst, we should find it automatically
  find_package (cisstMicronTracker REQUIRED HINTS "${CISST_BINARY_DIR}/components")

  if (cisstMicronTracker_FOUND)

    # cisstMicronTracker configuration
    include_directories (${cisstMicronTracker_INCLUDE_DIR})
    link_directories (${cisstMicronTracker_LIBRARY_DIR})

    add_executable (cisstMicronTrackerQtExample mainQt.cpp)

    # link against non cisst libraries and cisst components
    target_link_libraries (cisstMicronTrackerQtExample
                           ${cisstMicronTracker_LIBRARIES}
                           ${cisstMicronTrackerQt_LIBRARIES}
                           ${QT_LIBRARIES}
                         )
    # link against cisst libraries (and dependencies)
    cisst_target_link_libraries (cisstMicronTrackerQtExample ${REQUIRED_CISST_LIBRARIES})

  endif (cisstMicronTracker_FOUND)

endif (cisst_FOUND)
