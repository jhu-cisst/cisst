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

set (QTNODES_SOURCE_FILES "")
set (QTNODES_HEADER_FILES "")

if (CISST_HAS_QTNODES)
  add_definitions(-DNODE_EDITOR_SHARED)

  set (QTNODES_SOURCE_FILES
       mtsComponentModelQtNodes.cpp
       mtsComponentViewerQt.cpp)
  set (QTNODES_HEADER_FILES
       mtsComponentModelQtNodes.h
       mtsComponentViewerQt.h)
endif (CISST_HAS_QTNODES)

# to include Qt generated files
include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (CMAKE_AUTOMOC ON)

# add the Qt library
cisst_add_library (LIBRARY cisstMultiTaskQt
                   LIBRARY_DIR cisstMultiTask
                   FOLDER cisstMultiTask
                   DEPENDENCIES cisstCommon
                                cisstCommonQt
                                cisstOSAbstraction
                                cisstVector
                                cisstVectorQt
                                cisstMultiTask
                   SOURCE_FILES mtsClassServicesQt.cpp
                                mtsCollectorFactoryQtWidget.cpp
                                mtsCommandLineOptionsQt.cpp
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
                                mtsCommandLineOptionsQt.h
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

target_link_libraries (cisstMultiTaskQt ${CISST_QT_LIBRARIES})

# Ensure the QtNodes ExternalProject is fully built before cisstMultiTaskQt
if (CISST_HAS_QTNODES)
  target_compile_definitions (cisstMultiTaskQt PUBLIC NODE_EDITOR_SHARED)
  target_include_directories (cisstMultiTaskQt PUBLIC
    $<BUILD_INTERFACE:${QTNODES_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
    ${GRAPHVIZ_INCLUDE_DIRS})
  if (QTNODES_LIBRARY_DIRS)
    target_link_directories (cisstMultiTaskQt PUBLIC
      $<BUILD_INTERFACE:${QTNODES_LIBRARY_DIRS}>
      $<INSTALL_INTERFACE:lib>)
  endif ()
  if (GRAPHVIZ_LIBRARY_DIRS)
    target_link_directories (cisstMultiTaskQt PUBLIC ${GRAPHVIZ_LIBRARY_DIRS})
  endif ()
  target_link_libraries (cisstMultiTaskQt
                         ${QTNODES_LIBRARIES}
                         ${GRAPHVIZ_LIBRARIES})
  add_dependencies (cisstMultiTaskQt cisstQtNodesExternal)
endif (CISST_HAS_QTNODES)
