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

find_package (cisst REQUIRED)

if (cisst_FOUND)

  # cisst configuration
  include (${CISST_USE_FILE})

  # cisstNDISerial has been compiled within cisst, we should find it automatically
  find_package (cisstNDISerial REQUIRED HINTS "${CISST_BINARY_DIR}/components")

  if (cisstNDISerial_FOUND)

    set (CISST_LIBRARIES_NEEDED cisstCommon cisstCommonQt cisstCommonXML
                                cisstVector cisstOSAbstraction cisstNumerical
                                cisstMultiTask cisstParameterTypes cisstRobot)
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

  endif (cisstNDISerial_FOUND)

endif (cisst_FOUND)
