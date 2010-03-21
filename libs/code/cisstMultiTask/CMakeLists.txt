#
# $Id$
#
# CMakeLists for cisstMultiTask
#
# (C) Copyright 2003-2010 Johns Hopkins University (JHU), All Rights
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
SET(LIBRARY cisstMultiTask)

# set dependencies
SET(DEPENDENCIES
    cisstCommon
    cisstVector
    cisstOSAbstraction
    )

# all source files
SET(SOURCE_FILES
    mtsClassServices.cpp

    mtsGenericObject.cpp

    mtsMulticastCommandVoid.cpp
    mtsMulticastCommandWriteBase.cpp

    mtsFunctionVoid.cpp
    mtsFunctionReadOrWrite.cpp
    mtsFunctionQualifiedReadOrWrite.cpp

    mtsCommandQueuedVoidBase.cpp
    mtsCommandQueuedWriteBase.cpp

    mtsDevice.cpp

    mtsMailBox.cpp
    mtsStateIndex.cpp
    mtsStateTable.cpp

    mtsTaskInterface.cpp
    mtsRequiredInterface.cpp
    mtsTask.cpp
    mtsTaskContinuous.cpp
    mtsTaskPeriodic.cpp
    mtsTaskFromCallback.cpp
    mtsTaskFromSignal.cpp

    mtsDeviceInterface.cpp

    mtsCollectorBase.cpp
    mtsCollectorState.cpp
    mtsCollectorEvent.cpp
    
    mtsManagerLocal.cpp
    mtsManagerGlobal.cpp
    )

# all header files
SET(HEADER_FILES
    mtsMacros.h
    mtsGenericObject.h
    mtsGenericObjectProxy.h

    mtsFixedSizeVector.h
    mtsFixedSizeVectorTypes.h
    mtsVector.h
    mtsMatrix.h
    mtsTransformationTypes.h

    mtsCommandBase.h
    mtsFunctionBase.h

    mtsCommandVoid.h
    mtsCommandVoidBase.h
    mtsMulticastCommandVoid.h
    mtsCommandQueuedVoid.h
    mtsCommandQueuedVoidBase.h
    mtsFunctionVoid.h

    mtsCommandRead.h
    mtsCommandWrite.h
    mtsCommandReadOrWriteBase.h
    mtsMulticastCommandWrite.h
    mtsMulticastCommandWriteBase.h
    mtsCommandQueuedWrite.h
    mtsCommandQueuedWriteBase.h
    mtsFunctionReadOrWrite.h
    mtsEventWriteGeneratorProxy.h

    mtsCommandQualifiedRead.h
    mtsCommandQualifiedReadOrWriteBase.h
    mtsFunctionQualifiedReadOrWrite.h

    mtsExport.h
    mtsForwardDeclarations.h

    mtsDevice.h

    mtsQueue.h
    mtsMailBox.h
    mtsStateIndex.h

    mtsStateArray.h
    mtsStateArrayBase.h
    mtsStateTable.h
    mtsStateData.h

    mtsTaskInterface.h
    mtsRequiredInterface.h
    mtsTask.h
    mtsTaskContinuous.h
    mtsTaskPeriodic.h
    mtsTaskFromCallback.h
    mtsTaskFromSignal.h

    mtsTaskManager.h    # to be deleted

    mtsDeviceInterface.h
    mtsInterfaceCommon.h

    mtsCollectorBase.h
    mtsCollectorState.h
    mtsCollectorEvent.h

    mtsManagerLocalInterface.h
    mtsManagerLocal.h
    mtsManagerGlobalInterface.h    
    mtsManagerGlobal.h

    mtsHistory.h
    )

# make sure all options are set for all devices to OFF
option(CISST_MTS_HAS_ICE "Enable inter-process communication (IPC) feature of cisstMultiTask, requires ICE from ZeroC" OFF)

if(CISST_MTS_HAS_ICE)

  SET(SOURCE_FILES ${SOURCE_FILES}
      mtsComponentInterfaceProxyServer.cpp
      mtsComponentInterfaceProxyClient.cpp
      mtsManagerProxyServer.cpp
      mtsManagerProxyClient.cpp
      mtsComponentProxy.cpp
      )

  SET(HEADER_FILES ${HEADER_FILES}
      mtsComponentProxy.h

      mtsProxyBaseCommon.h
      mtsProxyBaseClient.h
      mtsProxyBaseServer.h
      mtsProxySerializer.h

      mtsComponentInterfaceProxyServer.h
      mtsComponentInterfaceProxyClient.h
      mtsManagerProxyServer.h
      mtsManagerProxyClient.h

      mtsCommandProxyBase.h
      mtsCommandVoidProxy.h
      mtsCommandWriteProxy.h
      mtsCommandReadProxy.h
      mtsCommandQualifiedReadProxy.h
      mtsFunctionReadOrWriteProxy.h
      mtsFunctionQualifiedReadOrWriteProxy.h
      mtsMulticastCommandWriteProxy.h
      )

  SET(ICE_RESOURCE_FILES
      ${cisst_SOURCE_DIR}/libs/etc/${LIBRARY}/Ice/config.client
      ${cisst_SOURCE_DIR}/libs/etc/${LIBRARY}/Ice/config.server
      ${cisst_SOURCE_DIR}/libs/etc/${LIBRARY}/Ice/config.LCM
      ${cisst_SOURCE_DIR}/libs/etc/${LIBRARY}/Ice/config.GCM
      ${cisst_SOURCE_DIR}/libs/etc/${LIBRARY}/Ice/mtsComponentInterfaceProxy.ice
      ${cisst_SOURCE_DIR}/libs/etc/${LIBRARY}/Ice/mtsManagerProxy.ice
      )

  #
  # ICE
  #
  SOURCE_GROUP(Resources FILES ${ICE_RESOURCE_FILES})

  SET(SLICE_FILES
      mtsComponentInterfaceProxy
      mtsManagerProxy
      )

  find_package(Ice)

  CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_INCLUDE_DIRECTORIES ICE_INCLUDE_DIR)
  CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LINK_DIRECTORIES ICE_LIBRARY_DIR)

  # create a list of include directories for slice
  foreach(ice_include_dir ${ICE_INCLUDE_DIR})
    set(SLICE_OPTIONS ${SLICE_OPTIONS} -I ${ice_include_dir})
  endforeach(ice_include_dir ${ICE_INCLUDE_DIR})

  foreach(slice_file ${SLICE_FILES})
    set(GENERATED_CPP_FILE "${CMAKE_CURRENT_BINARY_DIR}/${slice_file}.cpp")
    set(GENERATED_H_FILE "${CMAKE_CURRENT_BINARY_DIR}/${slice_file}.h")

    add_custom_command(
      OUTPUT ${GENERATED_CPP_FILE} ${GENERATED_H_FILE} ${cisst_BINARY_DIR}/libs/include/cisstMultiTask/${slice_file}.h
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      # first run slice2cpp
      COMMAND ${ICE_HOME}/bin/slice2cpp
      ARGS    -I ${ICE_SLICE_DIR} ${SLICE_OPTIONS} ${cisst_SOURCE_DIR}/libs/etc/cisstMultiTask/Ice/${slice_file}.ice
      # then create a directory to copy the header file
      COMMAND ${CMAKE_COMMAND}
      ARGS -E make_directory ${cisst_BINARY_DIR}/libs/include/cisstMultiTask
      # and finally copy the header file
      COMMAND ${CMAKE_COMMAND}
      ARGS -E copy_if_different ${GENERATED_H_FILE} ${cisst_BINARY_DIR}/libs/include/cisstMultiTask
      COMMENT "Compiling ${slice_file}.ice"
    )

    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    set(ADDITIONAL_SOURCE_FILES ${ADDITIONAL_SOURCE_FILES} ${GENERATED_CPP_FILE})

  endforeach(slice_file ${SLICE_FILES})

  set(ADDITIONAL_SOURCE_FILES ${ADDITIONAL_SOURCE_FILES} ${ICE_RESOURCE_FILES})

endif(CISST_MTS_HAS_ICE)


# Create the config file
SET(CISST_MTS_CONFIG_FILE ${cisst_BINARY_DIR}/libs/include/cisstMultiTask/mtsConfig.h)
CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/include/cisstMultiTask/mtsConfig.h.in
               ${CISST_MTS_CONFIG_FILE}
               @ONLY)
INSTALL_FILES(/include/cisstMultiTask
               ".h"
               ${CISST_MTS_CONFIG_FILE})

# Add the config file to the project
SET_SOURCE_FILES_PROPERTIES("${CISST_MTS_CONFIG_FILE}"
                            PROPERTIES GENERATED TRUE)
SET(ADDITIONAL_HEADER_FILES ${CISST_MTS_CONFIG_FILE})

# Create the CMake config file
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cisstMultiTaskBuild.cmake.in
               ${cisst_BINARY_DIR}/cisstMultiTaskBuild.cmake
               @ONLY)

# Finally add the library
CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

if(CISST_MTS_HAS_ICE)
  if(WIN32)
	set(ICE_LIBRARIES_OPTIMIZED Ice IceUtil)
	set(ICE_LIBRARIES_DEBUG Iced IceUtild)
    target_link_libraries(cisstMultiTask
                             optimized Ice
                             debug Iced
                             optimized IceUtil
                             debug IceUtild)
    CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES_DEBUG ICE_LIBRARIES_DEBUG)
    CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES_OPTIMIZED ICE_LIBRARIES_OPTIMIZED)
  else(WIN32)
    set(ICE_LIBRARIES ${ICE_LIBRARY_NAME} IceUtil)
    target_link_libraries(cisstMultiTask
                          ${ICE_LIBRARIES})
    CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES ICE_LIBRARIES)
  endif(WIN32)

endif(CISST_MTS_HAS_ICE)

