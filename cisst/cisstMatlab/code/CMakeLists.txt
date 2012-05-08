#
# $Id$
#
# (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstMatlab)

# all source files
set (SOURCE_FILES
     testComponent.cpp # for development only, to be removed asap
     mtlCreateComponent.cpp
     mtlCallFunction.cpp
     )

# all header files
set (HEADER_FILES
     mtlCreateComponent.h
     mtlCallFunction.h
     )

cisst_add_library (
  LIBRARY cisstMatlab
  FOLDER cisstMatlab
  DEPENDENCIES cisstCommon cisstOSAbstraction cisstMultiTask
  SOURCE_FILES ${SOURCE_FILES}
  HEADER_FILES ${HEADER_FILES})

# find matlab scripts
set (CISST_MTL_SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/matlab")
file (GLOB CISST_MTL_SCRIPTS RELATIVE ${CISST_MTL_SCRIPTS_DIR} ${CISST_MTL_SCRIPTS_DIR}/*.m)

# post build command to copy icons in build tree
unset (CISST_MTL_SCRIPTS_WITH_DIR)
foreach (script ${CISST_MTL_SCRIPTS})
  add_custom_command (TARGET cisstMatlab
                      POST_BUILD
                      COMMAND ${CMAKE_COMMAND}
                      ARGS -E copy_if_different
                             "${CISST_MTL_SCRIPTS_DIR}/${script}"
                             "${CISST_SHARE_DIR}/cisstMatlab/${script}")
  set (CISST_MTL_SCRIPTS_WITH_DIR ${CISST_MTL_SCRIPTS_WITH_DIR} "${CISST_MTL_SCRIPTS_DIR}/${script}")
endforeach (script)
install (FILES ${CISST_MTL_SCRIPTS_WITH_DIR} DESTINATION ${CISST_SHARE_INSTALL_SUFFIX}/cisstMatlab COMPONENT cisstMatlab)

# find header files that need to be copied to build tree for Matlab
# loadlibrary
foreach (header ${HEADER_FILES})
  add_custom_command (TARGET cisstMatlab
                      POST_BUILD
                      COMMAND ${CMAKE_COMMAND}
                      ARGS -E copy_if_different
                             "${cisstMatlabLibs_SOURCE_DIR}/${header}"
                             "${cisst_BINARY_DIR}/include/cisstMatlab/${header}")
endforeach (header)