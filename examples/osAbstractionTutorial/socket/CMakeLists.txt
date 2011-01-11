#
#   $Id$
#
#   Author(s):  Peter Kazanzides
#   Created on: 2009
#
#   (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

add_executable (socketUDPServer UDPServer.cpp)
cisst_target_link_libraries (socketUDPServer  cisstCommon cisstOSAbstraction)

add_executable (socketUDPClient UDPClient.cpp)
cisst_target_link_libraries (socketUDPClient cisstCommon cisstOSAbstraction)

add_executable (socketTCPServer TCPServer.cpp)
cisst_target_link_libraries (socketTCPServer cisstCommon cisstOSAbstraction)

add_executable (socketTCPClient TCPClient.cpp)
cisst_target_link_libraries (socketTCPClient cisstCommon cisstOSAbstraction)
