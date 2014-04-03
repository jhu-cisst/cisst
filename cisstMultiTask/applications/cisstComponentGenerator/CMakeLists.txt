#
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

cmake_minimum_required (VERSION 2.6)

# create a list of libraries needed for this project
set (REQUIRED_CISST_LIBRARIES cisstCommon)

# find cisst and make sure the required libraries have been compiled
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES} QUIET)

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  include_directories (${CMAKE_CURRENT_SOURCE_DIR})

  add_executable (cisstComponentGenerator
                  cisstComponentGenerator.cpp
                  ccgComponent.h
                  ccgParser.h
                  ccgComponent.cpp
                  ccgParser.cpp)

  set_property (TARGET cisstComponentGenerator PROPERTY FOLDER "cisstMultiTask/applications")

  cisst_target_link_libraries (cisstComponentGenerator ${REQUIRED_CISST_LIBRARIES})

  install (TARGETS cisstComponentGenerator
           COMPONENT cisstMultiTask
           RUNTIME DESTINATION bin)

else (cisst_FOUND_AS_REQUIRED)
  cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
endif (cisst_FOUND_AS_REQUIRED)
