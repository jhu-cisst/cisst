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

qt4_wrap_cpp (QT_WRAPPED_CPP
              displayQtComponent.h
              displayQtWidget.h
              )
include_directories (${CMAKE_CURRENT_BINARY_DIR})

add_executable (cisstMultiTaskPeriodicTaskQt
                ${QT_WRAPPED_CPP}
                displayQtComponent.cpp
                displayQtComponent.h
                displayQtWidget.cpp
                displayQtWidget.h
                main.cpp
                )

target_link_libraries (cisstMultiTaskPeriodicTaskQt
                       cisstMultiTaskPeriodicTaskComponents
                       ${QT_LIBRARIES}
                       )
cisst_target_link_libraries (cisstMultiTaskPeriodicTaskQt
                             cisstCommon
                             cisstCommonQt
                             cisstVector
                             cisstVectorQt
                             cisstOSAbstraction
                             cisstMultiTask
                             cisstMultiTaskQt
                             )
