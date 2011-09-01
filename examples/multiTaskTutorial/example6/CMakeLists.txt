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

# name of project and executable
PROJECT(multiTaskTutorialExample6)

# Set include/link directories
cisst_set_directories (cisstCommon cisstOSAbstraction cisstMultiTask)

# add include paths
INCLUDE_DIRECTORIES(${multiTaskTutorialExample6_SOURCE_DIR})

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(multiTaskTutorialExample6
               # code files to compile
               robotLowLevel.cpp
               userInterface.cpp
               main.cpp
               # header files are provided for information
               userInterface.h
	       robotLowLevel.h
	       )

# link with the cisst libraries
cisst_target_link_libraries(multiTaskTutorialExample6 cisstCommon cisstOSAbstraction cisstMultiTask)
