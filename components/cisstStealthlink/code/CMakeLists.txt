#
# $Id$
#
# (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

add_library (cisstStealthlink ${IS_SHARED}
             ../cisstStealthlinkExport.h
             mtsStealthlink.cpp
             ../mtsStealthlink.h
             ../mtsStealthlinkTypes.h
             mtsStealthlinkTypes.cpp
             mtsStealthlink_AsCL_Stuff.cpp
             mtsStealthlink_AsCL_Stuff.h
             )

target_link_libraries (cisstStealthlink ${Stealthlink_LIBRARIES})

if(WIN32)
  target_link_libraries(cisstStealthlink ws2_32.lib)
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /NODEFAULTLIB:msvcprt.lib /NODEFAULTLIB:LIBCMT.lib")
endif(WIN32)

cisst_target_link_libraries (cisstStealthlink
                             cisstCommon
                             cisstCommonXML
                             cisstVector
                             cisstOSAbstraction
                             cisstMultiTask
                             cisstParameterTypes
                             )

set (cisstStealthlink_INCLUDE_DIR "${cisstComponents_SOURCE_DIR}")
set (cisstStealthlink_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}")
set (cisstStealthlink_LIBRARIES cisstStealthlink ${Stealthlink_LIBRARIES})

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/cisstStealthlinkConfig.cmake.in"
                "${cisstComponents_BINARY_DIR}/cisstStealthlinkConfig.cmake"
                 @ONLY@)
