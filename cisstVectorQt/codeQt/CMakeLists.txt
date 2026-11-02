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

project (cisstVectorQt)

find_package (Qt${CISST_QT_VERSION} COMPONENTS Core REQUIRED)

find_package (Eigen3 REQUIRED)

# To include Qt generated files
include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (CMAKE_AUTOMOC ON)

# add the Qt library
cisst_add_library (
  LIBRARY cisstVectorQt
  LIBRARY_DIR cisstVectorQt
  FOLDER cisstVectorQt
  DEPENDENCIES cisstCommon
  SOURCE_FILES vctPlot2DBase.cpp
               vctPlot2DOpenGL.cpp
               vctPlot2DOpenGLQtWidget.cpp
               vctQtWidgetDynamicVector.cpp
               vctQtWidgetDynamicMatrix.cpp
               vctQtWidgetRotation.cpp
               vctQtWidgetFrame.cpp
               vctVector3DQtWidget.cpp
               vctPose3DQtWidget.cpp
               vctForceTorqueQtWidget.cpp
               vctForceTorque2DQtWidget.cpp
               vctForceTorque3DQtWidget.cpp
  HEADER_FILES vctForwardDeclarationsQt.h
               vctPlot2DBase.h
               vctPlot2DOpenGL.h
               vctPlot2DOpenGLQtWidget.h
               vctQtWidgetDynamicVector.h
               vctQtWidgetDynamicMatrix.h
               vctQtWidgetRotation.h
               vctQtWidgetFrame.h
               vctVector3DQtWidget.h
               vctPose3DQtWidget.h
               vctForceTorqueQtWidget.h
               vctForceTorque2DQtWidget.h
               vctForceTorque3DQtWidget.h
               vctExportQt.h)

target_link_libraries (cisstVectorQt
                       ${CISST_QT_LIBRARIES}
                       ${CISST_OPENGL_LIBRARIES})
