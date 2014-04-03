#
#
# (C) Copyright 2011-2012 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# name of project and executable
project (cisstIREComponent)

# cisst libraries used for this project
set (REQUIRED_CISST_LIBRARIES cisstCommon cisstOSAbstraction
                              cisstMultiTask cisstInteractive)

# find cisst and make sure the required libraries have been compiled
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES} QUIET)

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # name the main executable and specifies with source files to use
  add_executable (cisstIREComponent main.cpp)
  set_property (TARGET cisstIREComponent PROPERTY FOLDER "cisstMultiTask/applications")

  # link with the cisst libraries
  cisst_target_link_libraries (cisstIREComponent ${REQUIRED_CISST_LIBRARIES})

  install (TARGETS cisstIREComponent
           COMPONENT cisstMultiTask
           RUNTIME DESTINATION bin)

else (cisst_FOUND_AS_REQUIRED)
  cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
endif (cisst_FOUND_AS_REQUIRED)
