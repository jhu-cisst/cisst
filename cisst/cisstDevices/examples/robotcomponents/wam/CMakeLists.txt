#
# $Id$
#
# (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---


project( wam )

add_executable( WAMtest WAMtest.cpp )
add_executable( WAMgc WAMgc.cpp )

cisst_target_link_libraries( WAMtest cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )

cisst_target_link_libraries( WAMgc cisstCommon cisstVector cisstOSAbstraction cisstDevices cisstMultiTask )





