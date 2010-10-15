#
# $Id: CMakeLists.txt 1236 2010-02-26 20:38:21Z adeguet1 $
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

project (multiTaskGlobalComponentManagerEx)

# Create GCM with LCM that manages sineTask

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(multiTaskGlobalComponentManagerEx main.cpp)

# link with the cisst libraries
CISST_REQUIRES("multiTaskGlobalComponentManagerEx" "cisstCommon;cisstOSAbstraction;cisstMultiTask")

