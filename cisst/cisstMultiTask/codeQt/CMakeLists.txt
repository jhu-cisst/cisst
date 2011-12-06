#
# $Id$
#
# CMakeLists for cisstMultiTask
#
# (C) Copyright 2003-2011 Johns Hopkins University (JHU), All Rights
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

# cisstMultiTaskQt relies on cisstQt settings (for future use)
cisst_library_use_settings (cisstMultiTaskQt cisstQt)

# set directories for this project
cisst_set_directories (cisstQt)

# to include Qt generated files
include_directories (${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp (QT_WRAPPED_CPP
              ../mtsCollectorQtComponent.h
              ../mtsCollectorQtWidget.h
              ../mtsQtWidgetGenericObject.h
              ../mtsQtWidgetComponent.h
              ../mtsQtWidgetInterfaceRequired.h
              ../mtsQtWidgetFunction.h
              ../mtsQtWidgetEvent.h)

cisst_add_library (LIBRARY cisstMultiTaskQt
                   LIBRARY_DIR cisstMultiTask
                   DEPENDENCIES cisstCommon
                                cisstOSAbstraction
                                cisstVector
                                cisstMultiTask
                   SOURCE_FILES mtsClassServicesQt.cpp
                                mtsCollectorQtComponent.cpp
                                mtsCollectorQtWidget.cpp
                                mtsQtWidgetFactory.cpp
                                mtsQtWidgetGenericObject.cpp
                                mtsQtWidgetComponent.cpp
                                mtsQtWidgetInterfaceRequired.cpp
                                mtsQtWidgetFunction.cpp
                                mtsQtWidgetEvent.cpp
                   HEADER_FILES mtsCollectorQtComponent.h
                                mtsCollectorQtWidget.h
                                mtsQtWidgetFactory.h
                                mtsQtWidgetGenericObject.h
                                mtsQtWidgetComponent.h
                                mtsQtWidgetInterfaceRequired.h
                                mtsQtWidgetFunction.h
                                mtsQtWidgetEvent.h
                                mtsExportQt.h
                   ADDITIONAL_SOURCE_FILES ${QT_WRAPPED_CPP})

target_link_libraries (cisstMultiTaskQt ${QT_LIBRARIES})
