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


ADD_EXECUTABLE(svlExMultitask1 exampleFilter.cpp exampleComponent.cpp main.cpp exampleFilter.h exampleComponent.h)

CISST_REQUIRES("svlExMultitask1" "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstStereoVision")
