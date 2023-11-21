#
# CMakeLists for cisstParameterTypesQt
#
# (C) Copyright 2003-2022 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstParameterTypesQt)

# set directories for this project
cisst_set_directories (cisstQt)

# to include Qt generated files
include_directories (${CMAKE_CURRENT_BINARY_DIR})


if (CISST_HAS_QT4)
  qt4_wrap_cpp (QT_WRAPPED_CPP
                  ../prmEventButtonQtWidget.h
                  ../prmInputDataQtWidget.h
                  ../prmOperatingStateQtWidget.h
                  ../prmForceCartesianGetQtWidget.h
                  ../prmPositionCartesianGetQtWidget.h
                  ../prmPositionCartesianGetQtWidgetFactory.h
                  ../prmPositionCartesianArrayGetQtWidget.h
                  ../prmPositionJointSetQtWidget.h
                  ../prmStateJointQtWidget.h
                  ../prmStateRobotQtWidget.h)
else (CISST_HAS_QT4)
  # assumes Qt 5 Auto Moc
  set (CMAKE_AUTOMOC ON)
endif (CISST_HAS_QT4)

cisst_add_library (LIBRARY cisstParameterTypesQt
                   LIBRARY_DIR cisstParameterTypes
                   FOLDER cisstParameterTypes
                   DEPENDENCIES cisstCommon
                                cisstOSAbstraction
                                cisstVector
                                cisstVectorQt
                                cisstMultiTask
                                cisstParameterTypes
                   SETTINGS     cisstQt
                   SOURCE_FILES prmEventButtonQtWidget.cpp
                                prmInputDataQtWidget.cpp
                                prmOperatingStateQtWidget.cpp
                                prmForceCartesianGetQtWidget.cpp
                                prmPositionCartesianGetQtWidget.cpp
                                prmPositionCartesianGetQtWidgetFactory.cpp
                                prmPositionCartesianArrayGetQtWidget.cpp
                                prmPositionJointSetQtWidget.cpp
                                prmStateJointQtWidget.cpp
                                prmStateRobotQtWidget.cpp
                   HEADER_FILES prmForwardDeclarationsQt.h
                                prmEventButtonQtWidget.h
                                prmInputDataQtWidget.h
                                prmOperatingStateQtWidget.h
                                prmForceCartesianGetQtWidget.h
                                prmPositionCartesianGetQtWidget.h
                                prmPositionCartesianGetQtWidgetFactory.h
                                prmPositionCartesianArrayGetQtWidget.h
                                prmPositionJointSetQtWidget.h
                                prmStateJointQtWidget.h
                                prmStateRobotQtWidget.h
                                prmExportQt.h
                   ADDITIONAL_SOURCE_FILES ${QT_WRAPPED_CPP})

# link against Qt libraries
target_link_libraries (cisstParameterTypesQt ${QT_LIBRARIES})
