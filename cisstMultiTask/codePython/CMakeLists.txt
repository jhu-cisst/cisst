#
#
# CMakeLists for cisstMultiTask
#
# (C) Copyright 2003-2012 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstMultiTaskPython)

# create wrappers using swig
cisst_add_swig_module (MODULE cisstMultiTask
                       INTERFACE_DIRECTORY ..
                       FOLDER cisstMultiTask
                       HEADER_FILES ../mtsPython.h
                       MODULE_LINK_LIBRARIES cisstMultiTask cisstOSAbstraction cisstVector cisstCommon
                       INSTALL_FILES 1)

if (WIN32 AND CMAKE_CL_64)
    message("cisstMultiTaskPython: added /bigobj flag")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif (WIN32 AND CMAKE_CL_64)
