#
# (C) Copyright 2005-2016 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# All cisstRobot libraries
project (cisstRobotLibs)

add_subdirectory (code)
add_subdirectory (applications)

# SWIG Python wrappers
cisst_offer_python (cisstRobot)

# build examples and tests, add an option to skip them
cisst_offer_examples (cisstRobot)
cisst_offer_tests (cisstRobot)
