#
# $Id$
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

# find Sphinx 4
find_path (CISST_SPHINX4_DIST_DIR
           NAMES lib/sphinx4.jar
           DOC "Directory containing binary distribution of sphynx 4 (beta 6)")

# Java simplified wrapper for sphinx4
set (CISST_SPHINX4_JAVACLASS_DIR ${LIBRARY_OUTPUT_PATH})
set (CISST_SPHINX4_JAVA ${CISST_SPHINX4_JAVACLASS_DIR}/cisstSphinx4.class)
add_custom_command (OUTPUT ${CISST_SPHINX4_JAVA}
                    COMMAND ${JAVA_COMPILE}
                    ARGS -classpath "${CISST_SPHINX4_DIST_DIR}/lib/sphinx4.jar"
                         -d "${CISST_SPHINX4_JAVACLASS_DIR}"
                         "${CMAKE_CURRENT_SOURCE_DIR}/cisstSphinx4.java"
                    DEPENDS cisstSphinx4.java)

# compile all Java based targets
add_custom_target (cisstSphinx4Java ALL DEPENDS ${CISST_SPHINX4_JAVA})

# copy template file to binary dir
set (CISST_SPHINX4_TEMPLATE_DIR ${LIBRARY_OUTPUT_PATH})
file (COPY cisstSphinx4.xml.template DESTINATION ${CISST_SPHINX4_TEMPLATE_DIR})

# create configuration file with all wrappers
configure_file (mtsSphinx4Config.h.in "${CMAKE_CURRENT_BINARY_DIR}/../mtsSphinx4Config.h" @ONLY)

# core library
add_library (cisstSphinx4 SHARED
             ../mtsSphinx4Export.h
             ../mtsSphinx4.h
             mtsSphinx4Context.cpp
             mtsSphinx4WordActions.cpp
             mtsSphinx4JNI.h
             mtsSphinx4JNI.cpp
             mtsSphinx4.cpp)
add_dependencies (cisstSphinx4
                  cisstSphinx4Java)
target_link_libraries (cisstSphinx4
                       ${JNI_LIBRARIES})
cisst_target_link_libraries (cisstSphinx4
                             cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)

# Qt wrappers
if (CISST_HAS_QT)
  find_package (Qt4)
  include (${QT_USE_FILE})

  qt4_wrap_cpp (QT_WRAPPED_CPP
                ../mtsSphinx4QtComponent.h ../mtsSphinx4QtWidget.h)

  add_library (cisstSphinx4Qt SHARED
               ../mtsSphinx4ExportQt.h
               ../mtsSphinx4QtComponent.h
               ../mtsSphinx4QtWidget.h
               mtsSphinx4QtComponent.cpp
               mtsSphinx4QtWidget.cpp
               ${QT_WRAPPED_CPP})

target_link_libraries (cisstSphinx4Qt ${QT_LIBRARIES})
cisst_target_link_libraries (cisstSphinx4Qt
                             cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
endif (CISST_HAS_QT)
