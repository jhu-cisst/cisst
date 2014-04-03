#
#
#   Author(s):  Peter Kazanzides
#   Created on: 2009
#
#   (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights Reserved.
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

  add_executable (osaExSocketUDPServer UDPServer.cpp)
  set_property (TARGET osaExSocketUDPServer PROPERTY FOLDER "cisstOSAbstraction/examples")
  cisst_target_link_libraries (osaExSocketUDPServer ${REQUIRED_CISST_LIBRARIES})

  add_executable (osaExSocketUDPClient UDPClient.cpp)
  set_property (TARGET osaExSocketUDPClient PROPERTY FOLDER "cisstOSAbstraction/examples")
  cisst_target_link_libraries (osaExSocketUDPClient ${REQUIRED_CISST_LIBRARIES})

  add_executable (osaExSocketTCPServer TCPServer.cpp)
  set_property (TARGET osaExSocketTCPServer PROPERTY FOLDER "cisstOSAbstraction/examples")
  cisst_target_link_libraries (osaExSocketTCPServer ${REQUIRED_CISST_LIBRARIES})

  add_executable (osaExSocketTCPClient TCPClient.cpp)
  set_property (TARGET osaExSocketTCPClient PROPERTY FOLDER "cisstOSAbstraction/examples")
  cisst_target_link_libraries (osaExSocketTCPClient ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
