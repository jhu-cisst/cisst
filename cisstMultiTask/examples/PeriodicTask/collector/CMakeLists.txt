#
# (C) Copyright 2009-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# List cisst libraries needed
set (REQUIRED_CISST_LIBRARIES cisstCommon
                              cisstVector
                              cisstOSAbstraction
                              cisstMultiTask)

add_executable (mtsExPeriodicTaskCollector
                main.cpp)
set_property (TARGET mtsExPeriodicTaskCollector PROPERTY FOLDER "cisstMultiTask/examples")
target_link_libraries (mtsExPeriodicTaskCollector
                       mtsExPeriodicTaskComponents
                       ${cisst_LIBRARIES})
