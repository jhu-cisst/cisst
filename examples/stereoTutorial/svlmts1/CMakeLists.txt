#
# $Id$
#
# (C) Copyright 2005-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

set (CISST_LIBRARIES_NEEDED cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision)
cisst_set_directories (${CISST_LIBRARIES_NEEDED})
ADD_EXECUTABLE(svlExMultitask1 exampleFilter.cpp exampleComponent.cpp main.cpp exampleFilter.h exampleComponent.h)
cisst_target_link_libraries (svlExMultitask1 ${CISST_LIBRARIES_NEEDED})
