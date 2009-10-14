#
#   $Id$
#
#   Author(s):  Ali Uneri
#   Created on: 2009-10-13
#
#   (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project(devicesTutorialExample6)

add_executable(devicesTutorialExample6
               main.cpp
               )

cisst_requires(devicesTutorialExample6
               "cisstCommon;cisstOSAbstraction;cisstMultiTask;cisstDevices"
               )
