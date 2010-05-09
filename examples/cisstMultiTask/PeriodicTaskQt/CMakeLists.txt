#
# $Id$
#
# (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# we need OpenGL
find_package(Qt4 COMPONENTS QtGUI QtCore QtOpenGL REQUIRED)
include(${QT_USE_FILE})

qt4_wrap_cpp(QT_WRAPPED_CPP
             displayQComponent.h
             displayQWidget.h
	     vctPlot2DQWidget.h
             )
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(cisstMultiTaskPeriodicTaskQt
               ${QT_WRAPPED_CPP}
               displayQComponent.cpp
               displayQComponent.h
               displayQWidget.cpp
               displayQWidget.h
               main.cpp
               sineTask.cpp
               sineTask.h
	       vctPlot2DQWidget.h
	       vctPlot2DQWidget.cpp
               )

target_link_libraries(cisstMultiTaskPeriodicTaskQt
                      ${QT_LIBRARIES}
                      )
cisst_requires(cisstMultiTaskPeriodicTaskQt
               "cisstCommon;cisstOSAbstraction;cisstMultiTask"
               )
