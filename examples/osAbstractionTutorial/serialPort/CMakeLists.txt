#
# $Id$
#
# (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

ADD_EXECUTABLE(serialPortServer server.cpp)
CISST_REQUIRES("serialPortServer" "cisstCommon;cisstVector;cisstOSAbstraction")

ADD_EXECUTABLE(serialPortClient client.cpp)
CISST_REQUIRES("serialPortClient" "cisstCommon;cisstVector;cisstOSAbstraction")

