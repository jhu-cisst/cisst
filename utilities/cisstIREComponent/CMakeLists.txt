#
# $Id$
#
# (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# name of project and executable
PROJECT(cisstIREComponent)

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(cisstIREComponent main.cpp)

# link with the cisst libraries
cisst_target_link_libraries(cisstIREComponent cisstCommon cisstOSAbstraction 
                                              cisstMultiTask cisstInteractive)

# PK: check whether the following is still needed
if(UNIX)
  if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(cisstIREComponent util)
  endif(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
endif(UNIX)
