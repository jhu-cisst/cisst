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

set (REQUIRED_CISST_LIBRARIES cisstCommon cisstOSAbstraction)
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)
  include (${CISST_USE_FILE})

  add_executable (osaExSerialPortServer server.cpp)
  set_property (TARGET osaExSerialPortServer PROPERTY FOLDER "cisstOSAbstraction/examples")
  cisst_target_link_libraries (osaExSerialPortServer ${REQUIRED_CISST_LIBRARIES})

  add_executable (osaExSerialPortClient client.cpp)
  set_property (TARGET osaExSerialPortClient PROPERTY FOLDER "cisstOSAbstraction/examples")
  cisst_target_link_libraries (osaExSerialPortClient ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
