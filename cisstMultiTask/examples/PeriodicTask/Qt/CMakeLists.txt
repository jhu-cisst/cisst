#
#
# (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights Reserved.
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

if (CISST_HAS_QT4)
  qt4_wrap_cpp (QT_WRAPPED_CPP
                displayQtComponent.h
                displayQtWidget.h)
else (CISST_HAS_QT4)
  # assumes Qt 5 Auto Moc
  set (CMAKE_AUTOMOC ON)
endif (CISST_HAS_QT4)

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

add_custom_target (mtsExPeriodicTaskQt_Scripts ALL
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                           ${CMAKE_CURRENT_SOURCE_DIR}/mtsExPeriodicTaskQt.cisst
                           ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/mtsExPeriodicTaskQt.cisst
                   SOURCES mtsExPeriodicTaskQt.cisst
                   COMMENT "Copying cisst scripts to the binary directory")
set_property (TARGET mtsExPeriodicTaskQt_Scripts PROPERTY FOLDER "cisstMultiTask/examples")

add_custom_target (mtsExPeriodicTaskQt_Configs ALL
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                           ${CMAKE_CURRENT_SOURCE_DIR}/mtsExPeriodicTaskQtCollectors.json
                           ${CISST_SHARE_DIR}/cisstMultiTask/examples/mtsExPeriodicTaskQtCollectors.json
                   SOURCES mtsExPeriodicTaskQtCollectors.json
                   COMMENT "Copying json config file to the cisst share directory")
set_property (TARGET mtsExPeriodicTaskQt_Configs PROPERTY FOLDER "cisstMultiTask/examples")
