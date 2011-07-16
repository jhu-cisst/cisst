#
# $Id$
#
# (C) Copyright 2003-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# CMakeLists for cisstInteractive
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

set(SOURCE_FILES ireFramework.cpp)
set(HEADER_FILES ireExport.h ireFramework.h)
set(DEPENDENCIES cisstCommon)

if(BUILD_LIBS_cisstMultiTask)
   set(SOURCE_FILES ${SOURCE_FILES} ireTask.cpp)
   set(HEADER_FILES ${HEADER_FILES} ireTask.h)
   set(DEPENDENCIES ${DEPENDENCIES} cisstOSAbstraction cisstMultiTask)
endif(BUILD_LIBS_cisstMultiTask)

cisst_add_library (LIBRARY cisstInteractive
                   DEPENDENCIES ${DEPENDENCIES}
                   SOURCE_FILES ${SOURCE_FILES}
                   HEADER_FILES ${HEADER_FILES}
                   )

# Link against these libraries
target_link_libraries (cisstInteractive debug ${PYTHON_DEBUG_LIBRARIES} optimized ${PYTHON_LIBRARIES})

# Call makeIrepy to compile the Python source files (*.py) and place
# the results (*.pyc files) in the destination directory.
add_custom_target (cisstInteractive_COMPILE_IREPY ALL
                   ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/makeIrepy.py
                   ${CMAKE_CURRENT_SOURCE_DIR}/irepy  # src-dir
                   ${LIBRARY_OUTPUT_PATH}/irepy)   # dest-dir
