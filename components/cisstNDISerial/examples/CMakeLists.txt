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

if (NOT CURRENT_PROJECT_IS_CISST)
  find_package (cisst REQUIRED)
  find_package (cisstNDISerial REQUIRED)
endif (NOT CURRENT_PROJECT_IS_CISST)

if (cisst_FOUND)
  # cisst configuration
  include (${CISST_USE_FILE})
  set (CISST_LIBRARIES_NEEDED cisstCommon cisstCommonQt cisstCommonXML cisstOSAbstraction cisstMultiTask cisstParameterTypes cisstRobot)
  cisst_set_directories (${CISST_LIBRARIES_NEEDED})

  # cisstNDISerial configuration
  include_directories (${cisstNDISerial_INCLUDE_DIR})
  link_directories (${cisstNDISerial_LIBRARY_DIR})

  add_executable (cisstNDISerialQtExample mainQt.cpp)

  target_link_libraries (cisstNDISerialQtExample
                         cisstNDISerial
                         cisstNDISerialQt
                         ${QT_LIBRARIES}
                         )
  cisst_target_link_libraries (cisstNDISerialQtExample ${CISST_LIBRARIES_NEEDED})

endif (cisst_FOUND)