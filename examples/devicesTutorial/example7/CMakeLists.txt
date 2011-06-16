#
#   $Id$
#
#   Author(s):  Ali Uneri
#   Created on: 2009-11-06
#
#   (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

qt4_wrap_cpp(QT_WRAPPED_CPP
             devMicronTrackerControllerQDevice.h
             devMicronTrackerToolQDevice.h
             )
qt4_wrap_ui(QT_WRAPPED_UI
            devMicronTrackerControllerQWidget.ui
            devMicronTrackerToolQWidget.ui
            )
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(devicesTutorialExample7
               ${QT_WRAPPED_CPP}
               ${QT_WRAPPED_UI}
               devMicronTrackerControllerQDevice.cpp
               devMicronTrackerControllerQDevice.h
               devMicronTrackerToolQDevice.cpp
               devMicronTrackerToolQDevice.h
               main.cpp
               )

target_link_libraries (devicesTutorialExample7
                       ${QT_LIBRARIES}
                       )

cisst_target_link_libraries (devicesTutorialExample7
                             cisstCommon cisstCommonXML
                             cisstOSAbstraction
                             cisstMultiTask
                             cisstDevices)
