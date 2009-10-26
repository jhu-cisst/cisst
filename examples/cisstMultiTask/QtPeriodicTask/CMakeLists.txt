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

qt4_wrap_cpp(cisstMultiTaskQtPeriodicTask_QT_UI_SRCS
             proxyQt.h
             )
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(cisstMultiTaskQtPeriodicTask
               ${cisstMultiTaskQtPeriodicTask_QT_UI_SRCS}
               main.cpp
               proxyQt.cpp
               proxyQt.h
               sineTask.cpp
               sineTask.h
               )

target_link_libraries(cisstMultiTaskQtPeriodicTask
                      ${QT_LIBRARIES}
                      )
cisst_requires(cisstMultiTaskQtPeriodicTask
               "cisstCommon;cisstOSAbstraction;cisstMultiTask"
               )
