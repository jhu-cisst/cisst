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

# assumes automoc on Qt5
if (CISST_HAS_QT5)
  set (CMAKE_AUTOMOC ON)
endif (CISST_HAS_QT5)

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
  if (CISST_HAS_QT4)
    qt4_wrap_cpp (QT_WRAPPED_CPP
                  winQt4OpenGL.h
                  winQt4OpenGLStereo.h
                  )
  endif (CISST_HAS_QT4)

endif (CISST_HAS_OPENGL)

# Include Qt generated files
include_directories (${CMAKE_CURRENT_BINARY_DIR})

if (CISST_HAS_QT4)
  foreach (HEADER ${HEADER_FILES})
    qt4_wrap_cpp (QT_WRAPPED_CPP_TEMP
                  ../${HEADER}
                  )
    set (QT_WRAPPED_CPP
         ${QT_WRAPPED_CPP}
         ${QT_WRAPPED_CPP_TEMP}
         )
  endforeach (HEADER)
endif (CISST_HAS_QT4)

foreach (UI ${QT_UI_FILES})
  if (CISST_HAS_QT4)
    qt4_wrap_ui (QT_WRAPPED_UI
                 ${UI}
                 )
  else (CISST_HAS_QT4)
    qt5_wrap_ui (QT_WRAPPED_UI
                 ${UI}
                 )
  endif (CISST_HAS_QT4)
  set (QT_WRAPPED_UI
       ${QT_WRAPPED_UI}
       ${QT_WRAPPED_UI_TEMP}
       )
endforeach (UI)

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
                   HEADER_FILES ${HEADER_FILES} svlExportQt.h
                   ADDITIONAL_SOURCE_FILES ${QT_WRAPPED_CPP}
                                           ${QT_WRAPPED_UI}
                   )

# link against Qt libraries
target_link_libraries (cisstStereoVisionQt ${QT_LIBRARIES})
