#
# $Id$
#
# (C) Copyright 2005-2010 Johns Hopkins University (JHU), All Rights
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
PROJECT(svlExMultitask1)

# add include paths
INCLUDE_DIRECTORIES(${svlExMultitask1_SOURCE_DIR})

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(svlExMultitask1
               exampleFilter.cpp
               exampleComponent.cpp
               main.cpp
               exampleFilter.h
               exampleComponent.h
	       )

# link with the cisst libraries
CISST_REQUIRES("svlExMultitask1" "cisstCommon;cisstOSAbstraction;cisstMultiTask;cisstStereoVision")
