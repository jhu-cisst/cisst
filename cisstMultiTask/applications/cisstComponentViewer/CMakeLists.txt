#
#
# Author(s): Min Yang Jung
# Created on: November 14, 2013
#
# (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstComponentViewer)

# set all libraries needed
set (REQUIRED_CISST_LIBRARIES cisstCommon cisstOSAbstraction cisstMultiTask)

# find cisst and make sure the required libraries have been compiled
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES} QUIET)

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  add_executable (cisstComponentViewer main.cpp)
  set_property (TARGET cisstComponentViewer PROPERTY FOLDER "cisstMultiTask/applications")
  cisst_target_link_libraries (cisstComponentViewer ${REQUIRED_CISST_LIBRARIES})

  install (TARGETS cisstComponentViewer
           COMPONENT cisstMultiTask
           RUNTIME DESTINATION bin)

else (cisst_FOUND_AS_REQUIRED)
  cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
endif (cisst_FOUND_AS_REQUIRED)
