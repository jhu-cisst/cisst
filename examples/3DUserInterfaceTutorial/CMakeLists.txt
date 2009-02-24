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


add_executable(${PROJECT_NAME}
               example1.cpp
               example1.h
               )

target_link_libraries(${PROJECT_NAME}
                      vtkRendering
                      ${CMAKE_THREAD_LIBS_INIT}
                      )

CISST_REQUIRES(${PROJECT_NAME} "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstImage;cisstStereoVision;cisstDevices;cisstParameterTypes;cisst3DUserInterface")

# IF(WIN32)
#  set(CMAKE_EXE_LINKER_FLAGS
#      "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:libc.lib /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:libcd.lib /NODEFAULTLIB:libcmtd.lib")
# ENDIF(WIN32)
