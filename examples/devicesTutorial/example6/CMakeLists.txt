#
#   $Id$
#
#   Author(s):  Ali Uneri
#   Created on: 2009-10-13
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
             proxyQt.h
             )
qt4_wrap_ui(QT_WRAPPED_UI
            devNDISerial.ui
            )
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(devicesTutorialExample6
               ${QT_WRAPPED_CPP}
               ${QT_WRAPPED_UI}
               main.cpp
               proxyQt.cpp
               proxyQt.h
               )

target_link_libraries(devicesTutorialExample6
                      ${QT_LIBRARIES}
                      )

cisst_requires(devicesTutorialExample6
               "cisstCommon;cisstOSAbstraction;cisstMultiTask;cisstDevices"
               )
