#
# $Id$
#
# CMakeLists for cisstDevice
#
# (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


# set the library name
set(LIBRARY cisstDevices)
set(LIBRARY_INCLUDE_DIR ${cisstLibs_SOURCE_DIR}/include/${LIBRARY})

# make sure all options are set for all devices to OFF
option(CISST_DEV_HAS_SENSABLEHD "Build Sensable Device using HD library" OFF)
option(CISST_DEV_HAS_LOPOMOCO "Build LoPoMoCo card extension" OFF)
option(CISST_DEV_HAS_OPENIGTLINK "Open IGT Link protocol" OFF)
option(CISST_DEV_HAS_NOVINTHDL "Build Novint Device using HDL library" OFF)
option(CISST_DEV_HAS_MICRONTRACKER "Claron Micron Tracker libraries" OFF)
option( CISST_DEV_HAS_PEAKCAN "Use support PEAKCAN." OFF )
option( CISST_DEV_HAS_RTSOCKETCAN "Use support RT Socket CAN." OFF )
option( CISST_DEV_HAS_ACCELEGLOVE "Use support for AcceleGlove." OFF )
option( CISST_DEV_HAS_ODE "Use support for ODE." OFF )
option( CISST_DEV_HAS_GLUT "Use support for GLUT." OFF )

# set dependencies
set(DEPENDENCIES
    cisstCommon
    cisstVector
    cisstOSAbstraction
    cisstMultiTask
    cisstParameterTypes
    cisstRobot
    )

set(HEADER_FILES
    devNull.h
    devSartoriusSerial.h
    devExport.h
    devKeyboard.h
    manipulators/devManipulator.h
#    controllers/devControlLoop.h
#    controllers/devGravityCompensation.h
#    controllers/devJointsPD.h
#     can/devCAN.hpp
#     wam/devGroup.hpp
#     wam/devPuck.hpp
#     wam/devWAM.hpp
    )

set(SOURCE_FILES
    devNull.cpp
    devSartoriusSerial.cpp
    devKeyboard.cpp
    manipulators/devManipulator.cpp
#    controllers/devControlLoop.cpp
#    controllers/devGravityCompensation.cpp
#    controllers/devJointsPD.cpp
#    can/devCAN.cpp
#    wam/devGroup.cpp
#    wam/devPuck.cpp
#    wam/devWAM.cpp
    )

# Check for GLUT
if( CISST_DEV_HAS_GLUT )

  include( FindOpenGL )
  if( OPENGL_FOUND )
    cisst_add_to_internal( CISST_ADDITIONAL_LIBRARIES OPENGL_LIBRARIES )
  endif( OPENGL_FOUND )

  include( FindGLUT )
  if( GLUT_FOUND )
    cisst_add_to_internal( CISST_ADDITIONAL_LIBRARIES GLUT_LIBRARIES )
  endif( GLUT_FOUND )

  SET( HEADER_FILES
    ${HEADER_FILES}
    glut/devGeometry.h
    glut/devGLUT.h
    glut/devGLUTManipulator.h
    glut/devMeshTriangular.h
    )

    SET( SOURCE_FILES
      ${SOURCE_FILES}
      glut/devGeometry.cpp
      glut/devGLUT.cpp
      glut/devGLUTManipulator.cpp
      glut/devMeshTriangular.cpp
      )

endif( CISST_DEV_HAS_GLUT )

# Check for ODE
if( CISST_DEV_HAS_ODE )
  find_package( ODE )
  if( ODE_FOUND )

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCISST_ODE_SUPPORT -DdDOUBLE")
    cisst_add_to_internal( CISST_ADDITIONAL_LIBRARIES ODE_LIBRARIES )

    SET( HEADER_FILES
      ${HEADER_FILES}
      ode/devODEBody.h
      ode/devODEJoint.h
      ode/devODEManipulator.h
      ode/devODEWorld.h
      )

    SET( SOURCE_FILES
      ${SOURCE_FILES}
      ode/devODEBody.cpp
      ode/devODEJoint.cpp
      ode/devODEManipulator.cpp
      ode/devODEWorld.cpp
      )

    endif( ODE_FOUND )

endif( CISST_DEV_HAS_ODE )

if(CISST_DEV_HAS_SENSABLEHD)
  find_package(Sensable REQUIRED)
  include_directories(${SENSABLE_INCLUDE_DIR})
  set(SOURCE_FILES ${SOURCE_FILES}
      devSensableHD.cpp
      devSensableHDMasterSlave.cpp)
  set(HEADER_FILES ${HEADER_FILES}
      devSensableHD.h
      devSensableHDMasterSlave.h)
endif(CISST_DEV_HAS_SENSABLEHD)

# Check for PEAK CAN
if( CISST_DEV_HAS_PEAKCAN )
  find_package( PEAKCAN )
  set( SOURCE_FILES ${SOURCE_FILES} can/devPEAKCAN.cpp )
  set( HEADER_FILES ${HEADER_FILES} can/devPEAKCAN.hpp )
endif( CISST_DEV_HAS_PEAKCAN )

# Check for RT Socket CAN
if( CISST_DEV_HAS_RTSOCKETCAN )
  set( SOURCE_FILES ${SOURCE_FILES} can/devRTSocketCAN.cpp )
  set( HEADER_FILES ${HEADER_FILES} can/devRTSocketCAN.hpp )
endif( CISST_DEV_HAS_RTSOCKETCAN )

if( CISST_DEV_HAS_ACCELEGLOVE )
  set(SOURCE_FILES ${SOURCE_FILES} devAcceleGlove.cpp)
  set(HEADER_FILES ${HEADER_FILES} devAcceleGlove.h)
endif( CISST_DEV_HAS_ACCELEGLOVE )

if(CISST_DEV_HAS_LOPOMOCO)
  set(SOURCE_FILES ${SOURCE_FILES}
      devLoPoMoCo.cpp
      devLoPoMoCoOffsets.h
      devLoPoMoCoBoardIO.h)
  set(HEADER_FILES ${HEADER_FILES}
      devLoPoMoCo.h)
endif(CISST_DEV_HAS_LOPOMOCO)

if(CISST_DEV_HAS_OPENIGTLINK)
  find_package(OpenIGTLink REQUIRED)
  include(${OpenIGTLink_USE_FILE})
  set(SOURCE_FILES
      ${SOURCE_FILES}
      devOpenIGTLink.cpp)
  set(HEADER_FILES
      ${HEADER_FILES}
      devOpenIGTLink.h)
  set(DEV_ADDITIONAL_CMAKE_FILES
      "${DEV_ADDITIONAL_CMAKE_FILES}"
      ${OpenIGTLink_USE_FILE})
  set(DEV_ADDITIONAL_PACKAGES
      "${DEV_ADDITIONAL_PACKAGES}"
      OpenIGTLink)
  set(DEV_ADDITIONAL_LINK_DIRECTORIES
      "${DEV_ADDITIONAL_LINK_DIRECTORIES}"
      ${OpenIGTLink_LIBRARY_DIRS})
  set(DEV_ADDITIONAL_LIBRARIES
      "${DEV_ADDITIONAL_LIBRARIES}"
      OpenIGTLink)
endif(CISST_DEV_HAS_OPENIGTLINK)

if(CISST_DEV_HAS_NOVINTHDL)
  find_package(NovintHDL REQUIRED)
  include_directories(${NOVINT_INCLUDE_DIR})
  set(SOURCE_FILES ${SOURCE_FILES}
      devNovintHDL.cpp
      devNovintHDLMasterSlave.cpp)
  set(HEADER_FILES ${HEADER_FILES}
      devNovintHDL.h
      devNovintHDLMasterSlave.h)
endif(CISST_DEV_HAS_NOVINTHDL)

if(CISST_HAS_XML)
  set(SOURCE_FILES
      ${SOURCE_FILES}
      devNDISerial.cpp)
  set(HEADER_FILES
      ${HEADER_FILES}
      devNDISerial.h)

  if(CISST_DEV_HAS_MICRONTRACKER)
    find_path(MicronTracker_INCLUDE_DIR
              NAMES MTC.h
              HINTS "C:/Program Files/Claron Technology/MicronTracker/Dist")
    find_library(MicronTracker_LIBRARIES
                 NAMES MTC
                 HINTS ${MicronTracker_INCLUDE_DIR}
                 )
    include_directories(${MicronTracker_INCLUDE_DIR})
    set(SOURCE_FILES
        ${SOURCE_FILES}
        devMicronTracker.cpp)
    set(HEADER_FILES
        ${HEADER_FILES}
        devMicronTracker.h)
    set(DEV_ADDITIONAL_INCLUDE_DIR
        "${DEV_ADDITIONAL_INCLUDE_DIR}"
        ${MicronTracker_INCLUDE_DIR})
    set(DEV_ADDITIONAL_LIBRARIES
        "${DEV_ADDITIONAL_LIBRARIES}"
        ${MicronTracker_LIBRARIES})
  endif(CISST_DEV_HAS_MICRONTRACKER)
endif(CISST_HAS_XML)

  MACRO (CISST_QT4_WRAP_UI outfiles )
    QT4_EXTRACT_OPTIONS(ui_files ui_options ${ARGN})

    FOREACH (it ${ui_files})
      GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)
      GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
# the only thing we modified ...
      SET(outfile ${cisst_BINARY_DIR}/libs/include/cisstDevices/${outfile}.h)
      ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
        COMMAND ${QT_UIC_EXECUTABLE}
        ARGS ${ui_options} -o ${outfile} ${infile}
        MAIN_DEPENDENCY ${infile})
      SET(${outfiles} ${${outfiles}} ${outfile})
    ENDFOREACH (it)

  ENDMACRO (CISST_QT4_WRAP_UI)

# Qt widgets
if(CISST_HAS_QT)
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  qt4_wrap_cpp(CISST_DEV_NDISERIAL_QDEVICES_GENERATED
               ${LIBRARY_INCLUDE_DIR}/devNDISerialControllerQDevice.h
               ${LIBRARY_INCLUDE_DIR}/devNDISerialToolQDevice.h
               )
  set(CISST_DEV_NDISERIAL_QWIDGETS_UI
      devNDISerialControllerQWidget.ui
      devNDISerialToolQWidget.ui)
  cisst_qt4_wrap_ui(CISST_DEV_NDISERIAL_QWIDGETS_GENERATED
                    ${CISST_DEV_NDISERIAL_QWIDGETS_UI}
                    )

  set(ADDITIONAL_SOURCE_FILES
      ${ADDITIONAL_SOURCE_FILES}
      ${CISST_DEV_NDISERIAL_QDEVICES_GENERATED}
      ${CISST_DEV_NDISERIAL_QWIDGETS_GENERATED})
  set(SOURCE_FILES
      ${SOURCE_FILES}
      devNDISerialControllerQDevice.cpp
      devNDISerialToolQDevice.cpp)
  set(HEADER_FILES
      ${HEADER_FILES}
      devNDISerialControllerQDevice.h
      devNDISerialToolQDevice.h)

endif(CISST_HAS_QT)

# Exporting settings to applications that link against cisst
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_PACKAGES DEV_ADDITIONAL_PACKAGES)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_CMAKE_FILES DEV_ADDITIONAL_CMAKE_FILES)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LINK_DIRECTORIES DEV_ADDITIONAL_LINK_DIRECTORIES)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES DEV_ADDITIONAL_LIBRARIES)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_INCLUDE_DIRECTORIES DEV_ADDITIONAL_INCLUDE_DIR)

# Create the config file
SET(CISST_DEV_CONFIG_FILE ${cisst_BINARY_DIR}/libs/include/cisstDevices/devConfig.h)
CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/include/cisstDevices/devConfig.h.in
               ${CISST_DEV_CONFIG_FILE}
               @ONLY)
INSTALL_FILES(/include/cisstDevices
               ".h"
               ${CISST_DEV_CONFIG_FILE})

# Add the config file to the project
SET_SOURCE_FILES_PROPERTIES("${CISST_DEV_CONFIG_FILE}"
                            PROPERTIES GENERATED TRUE)
SET(ADDITIONAL_HEADER_FILES ${CISST_DEV_CONFIG_FILE})

# Create the CMake config file
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cisstDevicesBuild.cmake.in
               ${cisst_BINARY_DIR}/cisstDevicesBuild.cmake
               @ONLY)

# Finally create the library
CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

# Add libraries to link with based on options
if(CISST_DEV_HAS_SENSABLEHD)
  target_link_libraries(cisstDevices ${SENSABLE_LIBRARIES})
endif(CISST_DEV_HAS_SENSABLEHD)

if(CISST_DEV_HAS_NOVINTHDL)
  target_link_libraries(cisstDevices ${NOVINT_LIBRARIES})
endif(CISST_DEV_HAS_NOVINTHDL)

if(CISST_DEV_HAS_MICRONTRACKER)
  target_link_libraries(cisstDevices ${MicronTracker_LIBRARIES})
endif(CISST_DEV_HAS_MICRONTRACKER)

if(CISST_HAS_QT)
  target_link_libraries(cisstDevices ${QT_LIBRARIES})
endif(CISST_HAS_QT)

# Python wrappers
if (CISST_HAS_SWIG_PYTHON)
  cisst_add_swig_module(MODULE cisstDevices
                        INTERFACE_DIRECTORY ../../include/cisstDevices
			CISST_LIBRARIES cisstDevices
                                        cisstParameterTypes
					cisstMultiTask
					cisstOSAbstraction
					cisstVector
					cisstCommon)
endif (CISST_HAS_SWIG_PYTHON)
