#
# $Id$
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

project (cisstVectorQt)

# To include Qt generated files
include_directories (${CMAKE_CURRENT_BINARY_DIR})
qt4_wrap_cpp (QT_WRAPPED_CPP
              ../vctPlot2DOpenGLQtWidget.h
              ../vctQtWidgetDynamicVector.h
              )

# cisstVectorQt relies on cisstQt settings (for future use)
cisst_library_use_settings (cisstVectorQt cisstQt cisstOpenGL)

# set directories for this project
cisst_set_directories (cisstQt)

# add the Qt library
cisst_add_library (
  LIBRARY cisstVectorQt
  LIBRARY_DIR cisstVector
  FOLDER cisstVector
  DEPENDENCIES cisstCommon
               cisstVector
  SOURCE_FILES vctPlot2DOpenGLQtWidget.cpp
               vctQtWidgetDynamicVector.cpp
  HEADER_FILES vctPlot2DOpenGLQtWidget.h
               vctQtWidgetDynamicVector.h
               vctExportQt.h
  ADDITIONAL_SOURCE_FILES ${QT_WRAPPED_CPP})

# link against Qt libraries
target_link_libraries (cisstVectorQt ${QT_LIBRARIES})
