#
# $Id: CMakeLists.txt 3331 2012-01-10 22:34:09Z adeguet1 $
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



# create a list of required cisst libraries
set (REQUIRED_CISST_LIBRARIES cisstCommon
  cisstVector
  cisstOSAbstraction
  cisstMultiTask
  cisstParameterTypes
  cisstRobot)

find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)
  include (${CISST_USE_FILE})

  add_executable (example example.cpp)
  set_property (TARGET example PROPERTY FOLDER "cisstRobot/examples")
  cisst_target_link_libraries (example ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires " ${REQUIRED_CISST_LIBRARIES})
endif (cisst_FOUND_AS_REQUIRED)
