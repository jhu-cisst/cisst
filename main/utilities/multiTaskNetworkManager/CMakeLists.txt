#
# $Id: CMakeLists.txt 682 2009-08-14 03:18:33Z adeguet1 $
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
project (multiTaskNetworkManager)

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(multiTaskNetworkManager
               main.cpp
	       )

# link with the cisst libraries
CISST_REQUIRES("multiTaskNetworkManager" "cisstCommon;cisstOSAbstraction;cisstMultiTask")
