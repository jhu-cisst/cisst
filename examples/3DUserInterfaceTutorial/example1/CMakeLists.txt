#
# $Id$
#

project(3DUserInterfaceExample1)

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
               )

target_link_libraries(${PROJECT_NAME}
                      ${daVinci_LIBRARIES}
                      )

CISST_REQUIRES(${PROJECT_NAME} "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstStereoVision;cisstDevices;cisstParameterTypes;cisst3DUserInterface")
