#
# $Id$
#
# (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

ADD_EXECUTABLE(socketServer server.cpp)
CISST_REQUIRES("socketServer" "cisstCommon;cisstOSAbstraction")

ADD_EXECUTABLE(socketClient client.cpp)
CISST_REQUIRES("socketClient" "cisstCommon;cisstOSAbstraction")
