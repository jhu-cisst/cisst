#
# $Id$
#
# (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


############################
# OPENNI - KINECT EXAMPLES #
############################

# find cisst with all available libraries
find_package (cisst REQUIRED)

if (cisst_FOUND)

    # load cisst configuration
    include (${CISST_USE_FILE})

    find_package (cisstOpenNI REQUIRED HINTS "${CISST_BINARY_DIR}/components")

    if (cisstOpenNI_FOUND)

        include_directories (${cisstOpenNI_INCLUDE_DIR})
        link_directories (${cisstOpenNI_LIBRARY_DIR})

        #####################
        # CISST KINECT TEST #
        #####################

        add_executable (oniExKinectTest
                        oniExKinectTest.cpp
                        )

        target_link_libraries (oniExKinectTest
                               cisstOpenNI
                               )

        cisst_target_link_libraries (oniExKinectTest
                                     cisstCommon
                                     cisstVector
                                     cisstOSAbstraction
                                     )

        ###################
        # SVL KINECT TEST #
        ###################

        add_executable (oniExKinectView
                        oniExKinectView.cpp
                        )

        target_link_libraries (oniExKinectView
                               cisstOpenNI
                               )

        cisst_target_link_libraries (oniExKinectView
                                     cisstCommon
                                     cisstVector
                                     cisstOSAbstraction
                                     cisstMultiTask
                                     cisstStereoVision
                                     )

    endif (cisstOpenNI_FOUND)

endif (cisst_FOUND)

