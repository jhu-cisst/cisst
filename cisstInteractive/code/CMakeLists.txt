#
# (C) Copyright 2003-2025 Johns Hopkins University (JHU), All Rights Reserved.
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

project (cisstInteractive)

set (SOURCE_FILES ireFramework.cpp)
set (HEADER_FILES ireExport.h ireFramework.h)
set (ADDITIONAL_SOURCE_FILES "")
set (ADDITIONAL_HEADER_FILES "")
set (DEPENDENCIES cisstCommon)

if (CISST_cisstMultiTask)
  # create data type using the data generator
  cisst_data_generator (cisstInteractive
    "${cisst_BINARY_DIR}/include" # where to save the file
    "cisstInteractive/"           # sub directory for include
    ../ireTaskConstructorArg.cdg
    )

  # to compile cisst generated code, need to find header file
  include_directories (${CMAKE_CURRENT_BINARY_DIR})

  set (SOURCE_FILES ${SOURCE_FILES} ireTask.cpp)
  set (HEADER_FILES ${HEADER_FILES} ireTask.h)
  set (ADDITIONAL_SOURCE_FILES ${cisstInteractive_CISST_DG_SRCS})
  set (ADDITIONAL_HEADER_FILES ${cisstInteractive_CISST_DG_HDRS})
  set (DEPENDENCIES ${DEPENDENCIES} cisstOSAbstraction cisstMultiTask)
endif (CISST_cisstMultiTask)

# set directories for this project
cisst_set_directories (cisstPython)

cisst_add_library (LIBRARY cisstInteractive
                   FOLDER cisstInteractive
                   DEPENDENCIES ${DEPENDENCIES}
                   SETTINGS cisstPython
                   SOURCE_FILES ${SOURCE_FILES}
                   HEADER_FILES ${HEADER_FILES}
                   ADDITIONAL_SOURCE_FILES ${ADDITIONAL_SOURCE_FILES}
                   ADDITIONAL_HEADER_FILES_RELATIVE ${ADDITIONAL_HEADER_FILES}
                   )

install (FILES ${cisstInteractive_CISST_DG_HDRS_FULLPATH}
         DESTINATION "include/cisstInteractive")

# Call makeIrepy to compile the Python source files (*.py) and place
# the results (*.pyc files) in the destination directory.
add_custom_target (cisstInteractive_COMPILE_IREPY ALL
                   ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/makeIrepy.py
                   ${CMAKE_CURRENT_SOURCE_DIR}/irepy  # src-dir
                   ${LIBRARY_OUTPUT_PATH}/irepy)   # dest-dir
set_property (TARGET cisstInteractive_COMPILE_IREPY PROPERTY FOLDER "cisstInteractive")

install (DIRECTORY ${LIBRARY_OUTPUT_PATH}/irepy
         DESTINATION lib
         COMPONENT cisstInteractive)
