#
# CMakeLists for cisstMultiTaskQt
#
# (C) Copyright 2003-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstMultiTaskQt)

# set directories for this project
cisst_set_directories (cisstQt)

set (QTNODES_SOURCE_FILES "")
set (QTNODES_HEADER_FILES "")
set (QTNODES_SETTINGS "")

if (CISST_HAS_QTNODES)
  cisst_set_directories (cisstQtNodes)
  cisst_set_directories (cisstGraphviz)
  add_definitions(-DNODE_EDITOR_SHARED)

  set (QTNODES_SOURCE_FILES
       mtsComponentModelQtNodes.cpp
       mtsComponentViewerQt.cpp)
  set (QTNODES_HEADER_FILES
       mtsComponentModelQtNodes.h
       mtsComponentViewerQt.h)
  set (QTNODES_SETTINGS
       cisstQtNodes
       cisstGraphviz)
endif (CISST_HAS_QTNODES)

# to include Qt generated files
include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (CMAKE_AUTOMOC ON)

# add the Qt library
cisst_add_library (LIBRARY cisstMultiTaskQt
                   LIBRARY_DIR cisstMultiTask
                   FOLDER cisstMultiTask
                   DEPENDENCIES cisstCommon
                                cisstOSAbstraction
                                cisstVector
                                cisstVectorQt
                                cisstMultiTask
                   SETTINGS     cisstQt
                                ${QTNODES_SETTINGS}
                   SOURCE_FILES mtsClassServicesQt.cpp
                                mtsCollectorFactoryQtWidget.cpp
                                mtsMessageQtWidget.cpp
                                mtsSystemQtWidget.cpp
                                mtsQtCommandSelector.cpp
                                mtsQtScaleAndSignalTool.cpp
                                mtsQtVisualizerShell.cpp
                                mtsIntervalStatisticsQtWidget.cpp
                                mtsQtWidgetFactory.cpp
                                mtsQtWidgetGenericObject.cpp
                                mtsQtWidgetGenericObjectProxy.cpp
                                mtsQtWidgetVectorTypes.cpp
                                mtsQtWidgetComponent.cpp
                                mtsQtWidgetInterfaceRequired.cpp
                                mtsQtWidgetFunction.cpp
                                mtsQtWidgetEvent.cpp
                                mtsQtApplication.cpp
                                ${QTNODES_SOURCE_FILES}
                   HEADER_FILES mtsForwardDeclarationsQt.h
                                mtsCollectorFactoryQtWidget.h
                                mtsMessageQtWidget.h
                                mtsSystemQtWidget.h
                                mtsQtCommandSelector.h
                                mtsQtScaleAndSignalTool.h
                                mtsQtVisualizerShell.h
                                mtsIntervalStatisticsQtWidget.h
                                mtsQtWidgetFactory.h
                                mtsQtWidgetGenericObject.h
                                mtsQtWidgetGenericObjectProxy.h
                                mtsQtWidgetVectorTypes.h
                                mtsQtWidgetComponent.h
                                mtsQtWidgetInterfaceRequired.h
                                mtsQtWidgetFunction.h
                                mtsQtWidgetEvent.h
                                mtsQtApplication.h
                                ${QTNODES_HEADER_FILES}
                                mtsExportQt.h)

# link against Qt libraries
target_link_libraries (cisstMultiTaskQt ${QT_LIBRARIES})
