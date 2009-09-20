#
# $Id: CMakeLists.txt 679 2009-08-13 23:09:12Z gsevinc1 $
#
# (C) Copyright 2005-2008 Johns Hopkins University (JHU), All Rights
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
PROJECT(devicesTutorialExample5)

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(devicesTutorialExample5Client
               # code files to compile
               mainTask.cpp
               clientMain.cpp
               # header files are provided for information
               mainTask.h
               )

# link with the cisst libraries
CISST_REQUIRES("devicesTutorialExample5Client" "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices")

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(devicesTutorialExample5Server
               # code files to compile
               serverMain.cpp
               )

# link with the cisst libraries
CISST_REQUIRES("devicesTutorialExample5Server" "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices")