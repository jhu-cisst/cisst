#
# CMakeLists for cisstMultiTaskQt
#
# (C) Copyright 2003-2020 Johns Hopkins University (JHU), All Rights
# Reserved.
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

# to include Qt generated files
include_directories (${CMAKE_CURRENT_BINARY_DIR})

if (CISST_HAS_QT4)
  qt4_wrap_cpp (QT_WRAPPED_CPP
                ../mtsCollectorQtComponent.h
                ../mtsCollectorQtWidget.h
                ../mtsMessageQtWidget.h
                ../mtsSystemQtWidget.h
                ../mtsQtCommandSelector.h
                ../mtsQtScaleAndSignalTool.h
                ../mtsQtVisualizerShell.h
                ../mtsIntervalStatisticsQtWidget.h
                ../mtsQtWidgetGenericObject.h
                ../mtsQtWidgetGenericObjectProxy.h
                ../mtsQtWidgetVectorTypes.h
                ../mtsQtWidgetComponent.h
                ../mtsQtWidgetInterfaceRequired.h
                ../mtsQtWidgetFunction.h
                ../mtsQtWidgetEvent.h)
else (CISST_HAS_QT4)
  # assumes Qt 5 Auto Moc
  set (CMAKE_AUTOMOC ON)
endif (CISST_HAS_QT4)

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
                   SOURCE_FILES mtsClassServicesQt.cpp
                                mtsCollectorQtComponent.cpp
                                mtsCollectorQtWidget.cpp
                                mtsCollectorQtFactory.cpp
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
                   HEADER_FILES mtsForwardDeclarationsQt.h
                                mtsCollectorQtComponent.h
                                mtsCollectorQtWidget.h
                                mtsCollectorQtFactory.h
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
                                mtsExportQt.h
                   ADDITIONAL_SOURCE_FILES ${QT_WRAPPED_CPP})

# link against Qt libraries
target_link_libraries (cisstMultiTaskQt ${QT_LIBRARIES})
