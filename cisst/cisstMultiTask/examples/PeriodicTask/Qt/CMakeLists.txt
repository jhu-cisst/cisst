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

# List cisst libraries needed
set (REQUIRED_CISST_LIBRARIES cisstCommon
                              cisstCommonQt
                              cisstVector
                              cisstVectorQt
                              cisstOSAbstraction
                              cisstMultiTask
                              cisstMultiTaskQt)

# Set include/link directories
cisst_set_directories (${REQUIRED_CISST_LIBRARIES})

qt4_wrap_cpp (QT_WRAPPED_CPP
              displayQtComponent.h
              displayQtWidget.h)

include_directories (${CMAKE_CURRENT_BINARY_DIR})

add_library (mtsExPeriodicTaskGuiQt ${IS_SHARED}
                ${QT_WRAPPED_CPP}
                mainQtComponent.cpp
                mainQtComponent.h
                displayQtComponent.cpp
                displayQtComponent.h
                displayQtWidget.cpp
                displayQtWidget.h)
set_property (TARGET mtsExPeriodicTaskGuiQt PROPERTY FOLDER "cisstMultiTask/examples")

add_executable (mtsExPeriodicTaskQt main.cpp)
set_property (TARGET mtsExPeriodicTaskQt PROPERTY FOLDER "cisstMultiTask/examples")

target_link_libraries (mtsExPeriodicTaskQt
                       mtsExPeriodicTaskGuiQt
                       mtsExPeriodicTaskComponents)
cisst_target_link_libraries (mtsExPeriodicTaskGuiQt ${REQUIRED_CISST_LIBRARIES})
cisst_target_link_libraries (mtsExPeriodicTaskQt ${REQUIRED_CISST_LIBRARIES})
