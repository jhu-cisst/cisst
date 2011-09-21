## OPENNI - KINECT EXAMPLES

project (OpenNI)

set (CISST_LIBRARIES_NEEDED cisstCommon
                            cisstVector
                            cisstOSAbstraction)

# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${CISST_LIBRARIES_REQUIRED})

if (cisst_FOUND)

    # load cisst configuration
    include (${CISST_USE_FILE})

    find_package (cisstOpenNI REQUIRED HINTS "${CISST_BINARY_DIR}/components")

    if (cisstOpenNI_FOUND)

        include_directories (${cisstOpenNI_INCLUDE_DIR})
        link_directories (${cisstOpenNI_LIBRARY_DIR})

        # KINECT_TEST #################################################################
        add_executable (cisstOpenNIExKinectTest cisstOpenNIExKinectTest.cpp)
        target_link_libraries (cisstOpenNIExKinectTest cisstOpenNI)
        cisst_target_link_libraries (cisstOpenNIExKinectTest ${CISST_LIBRARIES_NEEDED})

    endif (cisstOpenNI_FOUND)

endif (cisst_FOUND)

