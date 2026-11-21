#
#
# CMakeLists for cisstMultiTask Python wrapping
#
# (C) Copyright 2003-2025 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstMultiTaskPython)

# create wrappers using pybind11
# Generated in the sibling code directory by cisstDataGenerator.
set_source_files_properties (${cisstMultiTask_CISST_DG_PYTHON_SRCS} PROPERTIES GENERATED TRUE)

cisst_add_pybind11_module (MODULE _cisstMultiTaskPython
                           FOLDER cisstMultiTask
                           SOURCES cisstMultiTaskPybind.cpp ${cisstMultiTask_CISST_DG_PYTHON_SRCS}
                           MODULE_LINK_LIBRARIES cisstMultiTask cisstOSAbstraction cisstCommon
                           INSTALL_FILES)

add_custom_command (OUTPUT "${LIBRARY_OUTPUT_PATH}/cisstMultiTaskPython.py"
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            "${CMAKE_CURRENT_SOURCE_DIR}/cisstMultiTaskPython.py"
                            "${LIBRARY_OUTPUT_PATH}/cisstMultiTaskPython.py"
                    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/cisstMultiTaskPython.py")

add_custom_target (cisstMultiTaskPython ALL
                   DEPENDS _cisstMultiTaskPython "${LIBRARY_OUTPUT_PATH}/cisstMultiTaskPython.py")

install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/cisstMultiTaskPython.py"
         DESTINATION lib
         COMPONENT cisstMultiTaskPython)

if (WIN32 AND CMAKE_CL_64)
    message("cisstMultiTaskPython: added /bigobj flag")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif (WIN32 AND CMAKE_CL_64)
