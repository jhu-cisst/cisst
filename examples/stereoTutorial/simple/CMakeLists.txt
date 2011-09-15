#
# $Id$
#
# (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

IF(CISST_SVL_HAS_X11 OR WIN32)

    set (CISST_LIBRARIES_NEEDED cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision)
    cisst_set_directories (${CISST_LIBRARIES_NEEDED})
    ADD_EXECUTABLE(svlExSimpleStream simplestream.cpp)
    cisst_target_link_libraries (svlExSimpleStream ${CISST_LIBRARIES_NEEDED})

ENDIF(CISST_SVL_HAS_X11 OR WIN32)

