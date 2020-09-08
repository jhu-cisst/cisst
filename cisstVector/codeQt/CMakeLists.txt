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

project (cisstVectorQt)

# To include Qt generated files
include_directories (${CMAKE_CURRENT_BINARY_DIR})

if (CISST_HAS_QT4)
  qt4_wrap_cpp (QT_WRAPPED_CPP
                ../vctPlot2DOpenGLQtWidget.h
                ../vctQtWidgetDynamicVector.h
                ../vctQtWidgetDynamicMatrix.h
                ../vctQtWidgetRotation.h
                ../vctQtWidgetFrame.h
                ../vctQtWidgetFrame4x4.h
                ../vctVector3DQtWidget.h
                ../vctPose3DQtWidget.h
                ../vctForceTorqueQtWidget.h
                ../vctForceTorque2DQtWidget.h
                ../vctForceTorque3DQtWidget.h
                )
else (CISST_HAS_QT4)
  # assumes Qt 5 Auto Moc
  set (CMAKE_AUTOMOC ON)
endif (CISST_HAS_QT4)

# set directories for this project
cisst_set_directories (cisstQt)

# add the Qt library
cisst_add_library (
  LIBRARY cisstVectorQt
  LIBRARY_DIR cisstVector
  FOLDER cisstVector
  DEPENDENCIES cisstCommon
               cisstVector
  SETTINGS     cisstQt
               cisstOpenGL
  SOURCE_FILES vctPlot2DOpenGLQtWidget.cpp
               vctQtWidgetDynamicVector.cpp
               vctQtWidgetDynamicMatrix.cpp
               vctQtWidgetRotation.cpp
               vctQtWidgetFrame.cpp
               vctQtWidgetFrame4x4.cpp
               vctVector3DQtWidget.cpp
               vctPose3DQtWidget.cpp
               vctForceTorqueQtWidget.cpp
               vctForceTorque2DQtWidget.cpp
               vctForceTorque3DQtWidget.cpp
  HEADER_FILES vctForwardDeclarationsQt.h
               vctPlot2DOpenGLQtWidget.h
               vctQtWidgetDynamicVector.h
               vctQtWidgetDynamicMatrix.h
               vctQtWidgetRotation.h
               vctQtWidgetFrame.h
               vctQtWidgetFrame4x4.h
               vctVector3DQtWidget.h
               vctPose3DQtWidget.h
               vctForceTorqueQtWidget.h
               vctForceTorque2DQtWidget.h
               vctForceTorque3DQtWidget.h
               vctExportQt.h
  ADDITIONAL_SOURCE_FILES ${QT_WRAPPED_CPP})

# link against Qt libraries
target_link_libraries (cisstVectorQt ${QT_LIBRARIES})
