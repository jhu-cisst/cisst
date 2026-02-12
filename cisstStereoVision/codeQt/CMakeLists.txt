#
#
# CMakeLists for cisstStereoVision
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

project (cisstStereoVisionQt)

# set directories for this project
cisst_set_directories (cisstQt)

set (CMAKE_AUTOMOC ON)

set (HEADER_FILES
     svlQtObjectFactory.h
     svlQtDialog.h
     svlQtWidgetFileOpen.h
     svlQtWidgetFramerate.h
     svlQtWidgetVideoEncoder.h
     svlQtWidgetVidCapSrcImageProperties.h
     )

set (SOURCE_FILES
     svlQtObjectFactory.cpp
     svlQtDialog.cpp
     svlQtWidgetFileOpen.cpp
     svlQtWidgetFramerate.cpp
     svlQtWidgetVideoEncoder.cpp
     svlQtWidgetVidCapSrcImageProperties.cpp
     )

set (QT_UI_FILES
     svlQtWidgetFramerate.ui
     svlQtWidgetVideoEncoder.ui
     svlQtWidgetVidCapSrcImageProperties.ui
     )

if (CISST_HAS_OPENGL)
  set (HEADER_FILES
       ${HEADER_FILES}
       svlFilterImageWindowQt.h
       svlFilterImageWindowQtStereo.h
       svlFilterImageOpenGLQtWidget.h
       )
  set (SOURCE_FILES
       ${SOURCE_FILES}
       winQt4OpenGL.h          # private header
       winQt4OpenGL.cpp
       winQt4OpenGLStereo.h    # private header
       winQt4OpenGLStereo.cpp
       svlFilterImageWindowQt.cpp
       svlFilterImageWindowQtStereo.cpp
       svlFilterImageOpenGLQtWidget.cpp
       )
endif (CISST_HAS_OPENGL)

# Include Qt generated files
include_directories (${CMAKE_CURRENT_BINARY_DIR})

# The following two files are added after Qt wrapping
# because they have to Qt symbols to wrap
#if (CISST_HAS_OPENGL)
#  set (HEADER_FILES
#       ${HEADER_FILES}
#       svlFilterImageWindowQt.h
#       svlFilterImageWindowQtStereo.h
#       )
#endif (CISST_HAS_OPENGL)

cisst_add_library (LIBRARY cisstStereoVisionQt
                   LIBRARY_DIR cisstStereoVision
                   FOLDER cisstStereoVision
                   DEPENDENCIES cisstCommon
                                cisstVector
                                cisstMultiTask
                                cisstStereoVision
                   SETTINGS     cisstQt
                   SOURCE_FILES ${SOURCE_FILES}
                   HEADER_FILES ${HEADER_FILES} svlExportQt.h)

# link against Qt libraries
target_link_libraries (cisstStereoVisionQt ${QT_LIBRARIES})
