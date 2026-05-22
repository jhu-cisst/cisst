#
#   Author(s):  Anton Deguet
#   Created on: 2009-11-16
#
#   (C) Copyright 2009-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

set (REQUIRED_CISST_LIBRARIES cisstCommon cisstCommonQt)
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND)
  add_executable (cmnExLoggerQt main.cpp)
  set_property (TARGET cmnExLoggerQt PROPERTY FOLDER "cisstCommon/examples")
  target_link_libraries (cmnExLoggerQt ${cisst_LIBRARIES})
else (cisst_FOUND)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND)
