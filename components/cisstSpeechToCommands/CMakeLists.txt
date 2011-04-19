#
# $Id: CMakeLists.txt 2771 2011-03-11 19:45:58Z adeguet1 $
#
# (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# find and use JNI
find_package (JNI REQUIRED)
include_directories (${JNI_INCLUDE_DIRS})

# find Java
find_package (Java REQUIRED)

# find and use Sphinx 4
find_path (CISST_CSC_SPHINX4_DIR
           NAMES lib/sphinx4.jar)
configure_file (cscConfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/cscConfig.h" @ONLY)
include_directories (${CMAKE_CURRENT_BINARY_DIR})

# Java simplified wrapper for sphinx4
set (CISST_CSC_SPHINX4_JAVA ${CMAKE_CURRENT_BINARY_DIR}/cscSphinx4.class)
add_custom_command (OUTPUT ${CISST_CSC_SPHINX4_JAVA}
                    COMMAND ${JAVA_COMPILE}
                    ARGS -classpath "${CISST_CSC_SPHINX4_DIR}/lib/sphinx4.jar"
                         -d "${CMAKE_CURRENT_BINARY_DIR}"
                         "${CMAKE_CURRENT_SOURCE_DIR}/cscSphinx4.java"
                    DEPENDS cscSphinx4.java)

# compile all Java based targets
add_custom_target (java ALL DEPENDS ${CISST_CSC_SPHINX4_JAVA})

# core library
add_library (cisstSpeechToCommands SHARED
             cscContext.h
             cscContext.cpp
             cscWordActions.h
             cscWordActions.cpp
             cscSphinx4JNI.h
             cscSphinx4JNI.cpp
             cscExport.h
             cscSpeechToCommands.h
             cscSpeechToCommands.cpp)
target_link_libraries (cisstSpeechToCommands ${JNI_LIBRARIES})
cisst_target_link_libraries (cisstSpeechToCommands
                             cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)

# Qt wrappers
if (CISST_HAS_QT)
  find_package (Qt4)
  include (${QT_USE_FILE})

  qt4_wrap_cpp (QT_WRAPPED_CPP
                cscSpeechToCommandsQtComponent.h cscSpeechToCommandsQtWidget.h)

  add_library (cisstSpeechToCommandsQt SHARED
               cscExportQt.h
               cscSpeechToCommandsQtComponent.h
               cscSpeechToCommandsQtComponent.cpp
               cscSpeechToCommandsQtWidget.h
               cscSpeechToCommandsQtWidget.cpp
               ${QT_WRAPPED_CPP})

target_link_libraries (cisstSpeechToCommandsQt ${QT_LIBRARIES})
cisst_target_link_libraries (cisstSpeechToCommandsQt
                             cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
endif (CISST_HAS_QT)

# example with Qt
get_filename_component (JAVA_LIBRARY_DIR ${JAVA_AWT_LIBRARY} PATH)
link_directories (${JAVA_LIBRARY_DIR}/xawt)

add_executable (cisstSpeechToCommandsExample main.cpp)
target_link_libraries (cisstSpeechToCommandsExample
                       cisstSpeechToCommands cisstSpeechToCommandsQt ${QT_LIBRARIES})
cisst_target_link_libraries (cisstSpeechToCommandsExample
                             cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
