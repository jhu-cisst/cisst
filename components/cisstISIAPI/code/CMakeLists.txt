#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2011-01-20
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


add_library (cisstISIAPI ${IS_SHARED}
             ../cisstISIAPIExport.h
             mtsISIAPI.cpp
             ../mtsISIAPI.h)

target_link_libraries (cisstISIAPI
                       ${isi_api_LIBRARIES})

cisst_target_link_libraries (cisstISIAPI
                             cisstCommon
                             cisstVector
                             cisstOSAbstraction
                             cisstMultiTask
                             cisstRobot
                             cisstParameterTypes)

set (cisstISIAPI_INCLUDE_DIR "${cisstComponents_SOURCE_DIR}")
set (cisstISIAPI_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}")
set (cisstISIAPI_LIBRARIES cisstISIAPI ${isi_api_LIBRARIES})

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/cisstISIAPIConfig.cmake.in"
                "${cisstComponents_BINARY_DIR}/cisstISIAPIConfig.cmake"
                 @ONLY@)
