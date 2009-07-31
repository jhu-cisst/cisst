#
# $Id: CMakeLists.txt 464 2009-06-13 05:36:07Z adeguet1 $
#

project(3DUserInterfaceExample2)

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

IF(CISST_SVL_HAS_SVS)
    INCLUDE_DIRECTORIES(${SVS_INCLUDE_DIR})
    LINK_LIBRARIES(${SVS_svs_LIBRARY} ${SVS_svsgrab_LIBRARY})
ENDIF(CISST_SVL_HAS_SVS)

IF(CISST_SVL_HAS_OPENCV)
    # Link each example against the OpenCV library if enabled
    FIND_PACKAGE(OpenCV)
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
    LINK_DIRECTORIES(${OpenCV_LINK_DIRECTORIES})
ENDIF(CISST_SVL_HAS_OPENCV)

OPTION(HAS_DAVINCI "Build using the daVinci API" OFF)
IF(HAS_DAVINCI)
   FIND_PATH(daVinci_INCLUDE_DIRS cisstDaVinciAPI.h)
   IF(daVinci_INCLUDE_DIRS)
       INCLUDE_DIRECTORIES(${daVinci_INCLUDE_DIRS})
   ENDIF(daVinci_INCLUDE_DIRS)
   FIND_LIBRARY(daVinci_LIBRARIES cisstDaVinciAPI)
ENDIF(HAS_DAVINCI)


add_executable(${PROJECT_NAME}
               main.cpp
               SimpleBehavior.cpp
               SimpleBehavior.h
               BehaviorWithSlave.cpp
               BehaviorWithSlave.h
               BehaviorLUS.cpp
               BehaviorLUS.h
               CSOpenGLStippleActor.cxx
               CSOpenGLStippleActor.h
               )

target_link_libraries(${PROJECT_NAME}
                      vtkRendering vtkHybrid
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${OpenCV_LIBRARIES}
                      ${daVinci_LIBRARIES}
                      )

IF(UNIX)
    # Link each executable against the X11 library
    FIND_PACKAGE(X11 REQUIRED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${X11_LIBRARIES})

    IF(CISST_SVL_HAS_LIBDC1394)
        # Also link the examples using live camera source against the DC1394 library if enabled
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} dc1394)
    ENDIF(CISST_SVL_HAS_LIBDC1394)
ENDIF(UNIX)


CISST_REQUIRES(${PROJECT_NAME} "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstStereoVision;cisstDevices;cisstParameterTypes;cisst3DUserInterface")
