#
# (C) Copyright 2005-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# name of project
PROJECT(mtsExBenchmark2)

set(REQUIRED_CISST_LIBRARIES cisstCommon cisstOSAbstraction cisstMultiTask cisstParameterTypes)

# find cisst and make sure the required libraries have been compiled
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # name the main executable and specifies with source files to use
  add_executable(mtsExBenchmark2
                 clientTask.cpp
                 serverTask.cpp
                 localMain.cpp
                 clientTask.h
                 serverTask.h
                 configuration.h
                 )
  set_property (TARGET mtsExBenchmark2 PROPERTY FOLDER "cisstMultiTask/examples")

  # link with the cisst libraries
  cisst_target_link_libraries(mtsExBenchmark2 ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
