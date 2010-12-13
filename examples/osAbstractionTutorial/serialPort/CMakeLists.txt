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

add_executable (serialPortServer server.cpp)
cisst_target_link_libraries (serialPortServer cisstCommon cisstOSAbstraction)

add_executable (serialPortClient client.cpp)
cisst_target_link_libraries (serialPortClient cisstCommon cisstOSAbstraction)

