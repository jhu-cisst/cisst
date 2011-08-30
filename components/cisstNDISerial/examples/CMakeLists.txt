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

# create a list of libraries needed for this project
set (CISST_LIBRARIES_REQUIRED cisstCommon cisstCommonQt cisstCommonXML
                              cisstVector cisstOSAbstraction cisstNumerical
                              cisstMultiTask cisstParameterTypes cisstRobot)

# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${CISST_LIBRARIES_REQUIRED})

if (cisst_FOUND)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # cisstNDISerial has been compiled within cisst, we should find it automatically
  find_package (cisstNDISerial REQUIRED HINTS "${CISST_BINARY_DIR}/components")

  if (cisstNDISerial_FOUND)

    # cisstNDISerial configuration
    include_directories (${cisstNDISerial_INCLUDE_DIR})
    link_directories (${cisstNDISerial_LIBRARY_DIR})

    add_executable (cisstNDISerialQtExample mainQt.cpp)

    # link against non cisst libraries and cisst components
    target_link_libraries (cisstNDISerialQtExample
                           cisstNDISerial
                           cisstNDISerialQt
                           ${QT_LIBRARIES}
                         )
    # link against cisst libraries (and dependencies)
    cisst_target_link_libraries (cisstNDISerialQtExample ${CISST_LIBRARIES_REQUIRED})

  endif (cisstNDISerial_FOUND)

endif (cisst_FOUND)
