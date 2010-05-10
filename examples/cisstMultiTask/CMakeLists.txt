#
# $Id$
#
# (C) Copyright 2005-2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


# this code will ultimately moved to the libs directory - adeguet1
if (CISST_HAS_FLTK OR CISST_HAS_QT)
  # we need OpenGL for base classes
  find_package (OpenGL REQUIRED)
  include_directories (${OPENGL_INCLUDE_DIR})
  link_libraries (${OPENGL_LIBRARIES})

  # to find includes
  include_directories (${CMAKE_CURRENT_SOURCE_DIR})

  set (PLOT_SOURCES
       vctPlot2DBase.h
       vctPlot2DBase.cpp
       vctPlot2DGLBase.h
       vctPlot2DGLBase.cpp)

  # Qt widget extra code
  if (CISST_HAS_QT)
    find_package (Qt4 COMPONENTS QtGUI QtCore QtOpenGL REQUIRED)
    include (${QT_USE_FILE})

    qt4_wrap_cpp (PLOT_QT_WRAPPED_CPP
                  vctPlot2DQWidget.h)

    set (PLOT_SOURCES ${PLOT_SOURCES}
         ${PLOT_QT_WRAPPED_CPP}
	 vctPlot2DQWidget.h
	 vctPlot2DQWidget.cpp)
  endif (CISST_HAS_QT)

  # FLTK extra code
  if (CISST_HAS_FLTK)
    set (PLOT_SOURCES ${PLOT_SOURCES}
	 vctPlot2DFLTK.h
	 vctPlot2DFLTK.cpp)
  endif (CISST_HAS_FLTK)

  # create library
  add_library(vctPlot ${PLOT_SOURCES})

endif (CISST_HAS_FLTK OR CISST_HAS_QT)

# examples
if (CISST_HAS_FLTK)
  add_subdirectory (PeriodicTaskFLTK)
  add_subdirectory (CommandsAndEventsFLTK)
else (CISST_HAS_FLTK)
  message (WARNING " cisstMultiTask examples PeriodicTaskFLTK and CommandsAndEventsFLTK will not be compiled since they require CISST_HAS_FLTK")
endif (CISST_HAS_FLTK)

if (CISST_HAS_QT)
  add_subdirectory (PeriodicTaskQt)
  add_subdirectory (CommandsAndEventsQt)
else (CISST_HAS_QT)
  message (WARNING " cisstMultiTask examples PeriodicTaskQt and CommandsAndEventsQt will not be compiled since they require CISST_HAS_QT")
endif (CISST_HAS_QT)
