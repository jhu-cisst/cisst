#
# $Id$
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

# cisstStereoVisionQt relies on cisstQt settings (for future use)
cisst_library_use_settings (cisstStereoVisionQt cisstQt)

# set directories for this project
cisst_set_directories (cisstQt)


set (HEADER_FILES
     svlExportQt.h
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
       svlFilterImageOpenGLQtWidget.h
       )
  set (SOURCE_FILES
       ${SOURCE_FILES}
       winQt4OpenGL.h          # private header
       winQt4OpenGL.cpp
       svlFilterImageWindowQt.cpp
       svlFilterImageOpenGLQtWidget.cpp
       )
  QT4_WRAP_CPP(QT_WRAPPED_CPP
               winQt4OpenGL.h
               )

endif (CISST_HAS_OPENGL)

# Include Qt generated files
include_directories (${CMAKE_CURRENT_BINARY_DIR})

foreach (HEADER ${HEADER_FILES})
  QT4_WRAP_CPP(QT_WRAPPED_CPP_TEMP
               ../${HEADER}
               )
  set (QT_WRAPPED_CPP
       ${QT_WRAPPED_CPP}
       ${QT_WRAPPED_CPP_TEMP}
       )
endforeach (HEADER)

foreach (UI ${QT_UI_FILES})
  QT4_WRAP_UI(QT_WRAPPED_UI
              ${UI}
              )
  set (QT_WRAPPED_UI
       ${QT_WRAPPED_UI}
       ${QT_WRAPPED_UI_TEMP}
       )
endforeach (UI)

cisst_add_library (LIBRARY cisstStereoVisionQt
                   LIBRARY_DIR cisstStereoVision
                   DEPENDENCIES cisstCommon
                                cisstVector
                                cisstMultiTask
                                cisstStereoVision
                   SOURCE_FILES ${SOURCE_FILES}
                   HEADER_FILES ${HEADER_FILES}
                   ADDITIONAL_SOURCE_FILES ${QT_WRAPPED_CPP}
                                           ${QT_WRAPPED_UI}
                   )

target_link_libraries (cisstStereoVisionQt ${QT_LIBRARIES})
