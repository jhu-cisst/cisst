#
# $Id: CMakeLists.txt,v 1.3 2009/02/03 21:09:06 anton Exp $
#

project(3DUserInterfaceExample)

# Find VTK settings
find_package(VTK REQUIRED)
if (VTK_FOUND)
    if (NOT VTK_USE_RENDERING)
        message (FATAL_ERROR "${PROJECT_NAME} requires VTK_USE_RENDERING.")
    endif (NOT VTK_USE_RENDERING)
    include (${VTK_USE_FILE})
else (VTK_FOUND)

    message (FATAL ERROR "VTK not found")
endif (VTK_FOUND)

IF(CISST_SVL_HAS_OPENCV)
    # Link each example against the OpenCV library if enabled
    FIND_PACKAGE(OpenCV)
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
    LINK_DIRECTORIES(${OpenCV_LINK_DIRECTORIES})
ENDIF(CISST_SVL_HAS_OPENCV)

add_executable(${PROJECT_NAME}
               main.cpp
               example1.cpp
               example1.h
               )

target_link_libraries(${PROJECT_NAME}
                      vtkRendering
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${OpenCV_LIBRARIES}
                      )

CISST_REQUIRES(${PROJECT_NAME} "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstImage;cisstStereoVision;cisstDevices;cisstParameterTypes;cisst3DUserInterface")
