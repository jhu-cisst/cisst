#
#   $Id$
#
#   Author(s):  Peter Kazanzides
#   Created on: 2009
#
#   (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

ADD_EXECUTABLE(socketUDPServer UDPServer.cpp)
CISST_REQUIRES(socketUDPServer "cisstCommon;cisstOSAbstraction")

ADD_EXECUTABLE(socketUDPClient UDPClient.cpp)
CISST_REQUIRES(socketUDPClient "cisstCommon;cisstOSAbstraction")

ADD_EXECUTABLE(socketTCPServer TCPServer.cpp)
CISST_REQUIRES(socketTCPServer "cisstCommon;cisstOSAbstraction")

ADD_EXECUTABLE(socketTCPClient TCPClient.cpp)
CISST_REQUIRES(socketTCPClient "cisstCommon;cisstOSAbstraction")
