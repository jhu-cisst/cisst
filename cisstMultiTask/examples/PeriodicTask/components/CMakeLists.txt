#
#
# (C) Copyright 2005-2012 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# List of cisst libraries needed
set (CISST_LIBRARIES_NEEDED cisstCommon cisstOSAbstraction cisstMultiTask)
cisst_set_directories (${CISST_LIBRARIES_NEEDED})

# create a library of components
add_library (mtsExPeriodicTaskComponents
             clockComponent.h
             clockComponent.cpp
             sineTask.h
             sineTask.cpp
             sineTaskWithDelay.cpp
             sineTaskWithDelay.h
             )
set_property (TARGET mtsExPeriodicTaskComponents PROPERTY FOLDER "cisstMultiTask/examples")

# link with the cisst libraries
cisst_target_link_libraries (mtsExPeriodicTaskComponents ${CISST_LIBRARIES_NEEDED})
