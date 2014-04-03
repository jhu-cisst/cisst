#
#
# (C) Copyright 2012-2013 Johns Hopkins University (JHU), All Rights Reserved.
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
                              cisstCommonQt
                              cisstVector
                              cisstVectorQt
                              cisstOSAbstraction
                              cisstMultiTask
                              cisstMultiTaskQt
                              cisstParameterTypes)

# Set include/link directories
cisst_set_directories (${REQUIRED_CISST_LIBRARIES})

add_executable (mtsExQt2DPlotVisualizerQt
                ../TestComponent.h
                ../TestComponent.cpp
                main.cpp)
set_property (TARGET mtsExQt2DPlotVisualizerQt PROPERTY FOLDER "cisstMultiTask/examples")
cisst_target_link_libraries (mtsExQt2DPlotVisualizerQt ${REQUIRED_CISST_LIBRARIES})
