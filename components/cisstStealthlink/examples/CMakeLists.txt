#
# $Id$
#
# (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

set (CISST_LIBRARIES_NEEDED cisstCommon
                            cisstCommonXML
                            cisstVector
                            cisstOSAbstraction
                            cisstMultiTask
                            cisstParameterTypes)

# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # cisstStealthlink has been compiled within cisst, we should find it automatically
  find_package (cisstStealthlink REQUIRED HINTS "${CISST_BINARY_DIR}/components")

  if (cisstStealthlink_FOUND)

    # cisstStealthlink configuration
    include_directories (${cisstStealthlink_INCLUDE_DIR})
    link_directories (${cisstStealthlink_LIBRARY_DIR})

    add_executable (cisstStealthlinkExample
                    mtsStealthlinkExampleComponent.h
                    mtsStealthlinkExampleComponent.cpp
                    main.cpp)
    target_link_libraries (cisstStealthlinkExample
                           cisstStealthlink ${Stealthlink_LIBRARIES})
    cisst_target_link_libraries (cisstStealthlinkExample ${CISST_LIBRARIES_NEEDED})

    # compile distributed example is ZeroC Ice is available
    if (CISST_MTS_HAS_ICE)
      add_executable (cisstStealthlinkExampleServer
                      mainServer.cpp)
      target_link_libraries (cisstStealthlinkExampleServer
                             cisstStealthlink ${Stealthlink_LIBRARIES})
      cisst_target_link_libraries (cisstStealthlinkExampleServer ${CISST_LIBRARIES_NEEDED})

      add_executable (cisstStealthlinkExampleClient
                      mtsStealthlinkExampleComponent.h
                      mtsStealthlinkExampleComponent.cpp
                      mainClient.cpp)
      target_link_libraries (cisstStealthlinkExampleClient
                             cisstStealthlink ${Stealthlink_LIBRARIES})
      cisst_target_link_libraries (cisstStealthlinkExampleClient ${CISST_LIBRARIES_NEEDED})
    endif (CISST_MTS_HAS_ICE)

  endif (cisstStealthlink_FOUND)

endif (cisst_FOUND)
