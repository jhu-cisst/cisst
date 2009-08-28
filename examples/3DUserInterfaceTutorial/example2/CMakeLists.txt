#
# $Id: CMakeLists.txt 464 2009-06-13 05:36:07Z adeguet1 $
#

project(3DUserInterfaceExample2Project)

OPTION(HAS_DAVINCI "Build using the daVinci API" OFF)
IF(HAS_DAVINCI)
   FIND_PATH(daVinci_INCLUDE_DIRS cisstDaVinciAPI.h)
   IF(daVinci_INCLUDE_DIRS)
       INCLUDE_DIRECTORIES(${daVinci_INCLUDE_DIRS})
   ENDIF(daVinci_INCLUDE_DIRS)
   FIND_LIBRARY(daVinci_LIBRARIES cisstDaVinciAPI)
ENDIF(HAS_DAVINCI)

add_executable(3DUserInterfaceExample2
               main.cpp
               SimpleBehavior.cpp
               SimpleBehavior.h
               BehaviorWithSlave.cpp
               BehaviorWithSlave.h
               BehaviorLUS.cpp
               BehaviorLUS.h
               CSOpenGLStippleActor.cxx
               CSOpenGLStippleActor.h
               MeasurementBehavior.h
               MeasurementBehavior.cpp
               MapBehavior.h
               MapBehavior.cpp
               ToyBehavior.h
               ToyBehavior.cpp
               cs3DObjectViewer.h
               cs3DObjectViewer.cpp
               )

target_link_libraries(3DUserInterfaceExample2
                      ${daVinci_LIBRARIES}
                      )

# Copy VTK models to the build tree
IF(WIN32)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/probeEnd.STL
            ${CMAKE_BINARY_DIR}/examples/3DUserInterfaceTutorial/example2/probeEnd.STL COPYONLY)
ELSE(WIN32)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/probeEnd.STL
            ${CMAKE_BINARY_DIR}/examples/bin/probeEnd.STL COPYONLY)
ENDIF(WIN32)

CISST_REQUIRES(3DUserInterfaceExample2 "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstStereoVision;cisstDevices;cisstParameterTypes;cisst3DUserInterface")


