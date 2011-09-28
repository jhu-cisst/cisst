#
# $Id$
#
# CMakeLists for cisstMultiTask
#
# (C) Copyright 2003-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# all source files
set (SOURCE_FILES

     mtsClassServices.cpp

     mtsCollectorBase.cpp
     mtsCollectorEvent.cpp
     mtsCollectorState.cpp

     mtsCommandFilteredQueuedWrite.cpp
     mtsCommandFilteredWrite.cpp
     mtsCommandQualifiedRead.cpp
     mtsCommandQueuedVoid.cpp
     mtsCommandQueuedVoidReturn.cpp
     mtsCommandQueuedWriteBase.cpp
     mtsCommandQueuedWriteReturn.cpp
     mtsCommandRead.cpp
     mtsCommandVoid.cpp
     mtsCommandVoidReturn.cpp
     mtsCommandWriteReturn.cpp

     mtsComponent.cpp
     mtsComponentState.cpp
     mtsComponentViewer.cpp
     mtsConnection.cpp

     mtsEventReceiver.cpp
     mtsExecutionResult.cpp

     mtsFunctionBase.cpp
     mtsFunctionQualifiedRead.cpp
     mtsFunctionRead.cpp
     mtsFunctionVoid.cpp
     mtsFunctionVoidReturn.cpp
     mtsFunctionWrite.cpp
     mtsFunctionWriteReturn.cpp

     mtsGenericObject.cpp

     mtsInterfaceInput.cpp
     mtsInterfaceOutput.cpp
     mtsInterfaceProvided.cpp
     mtsInterfaceProvidedOrOutput.cpp
     mtsInterfaceRequired.cpp
     mtsInterfaceRequiredOrInput.cpp
     mtsIntervalStatistics.cpp

     mtsLODMultiplexerStreambuf.cpp

     mtsMailBox.cpp
     mtsManagerLocal.cpp
     mtsManagerGlobal.cpp
     mtsManagerComponentBase.cpp
     mtsManagerComponentServer.cpp
     mtsManagerComponentClient.cpp
     mtsManagerComponentServices.cpp

     mtsMulticastCommandVoid.cpp
     mtsMulticastCommandWriteBase.cpp

     mtsParameterTypes.cpp

     mtsStateIndex.cpp
     mtsStateTable.cpp

     mtsTask.cpp
     mtsTaskContinuous.cpp
     mtsTaskFromCallback.cpp
     mtsTaskFromSignal.cpp
     mtsTaskPeriodic.cpp
     )

# all header files
set (HEADER_FILES
     mtsCallableQualifiedReadBase.h
     mtsCallableQualifiedReadMethod.h
     mtsCallableQualifiedReadReturnVoidMethod.h
     mtsCallableReadBase.h
     mtsCallableReadMethod.h
     mtsCallableReadReturnVoidMethod.h
     mtsCallableVoidBase.h
     mtsCallableVoidFunction.h
     mtsCallableVoidMethod.h
     mtsCallableVoidReturnBase.h
     mtsCallableVoidReturnMethod.h
     mtsCallableWriteReturnBase.h
     mtsCallableWriteReturnMethod.h

     mtsCollectorBase.h
     mtsCollectorEvent.h
     mtsCollectorState.h

     mtsCommandBase.h
     mtsCommandFilteredQueuedWrite.h
     mtsCommandFilteredWrite.h
     mtsCommandQualifiedRead.h
     mtsCommandQueuedVoid.h
     mtsCommandQueuedVoidReturn.h
     mtsCommandQueuedWrite.h
     mtsCommandQueuedWriteBase.h
     mtsCommandQueuedWriteReturn.h
     mtsCommandRead.h
     mtsCommandVoid.h
     mtsCommandVoidReturn.h
     mtsCommandWrite.h
     mtsCommandWriteBase.h
     mtsCommandWriteReturn.h

     mtsComponent.h
     mtsComponentDispatcher.h
     mtsComponentGeneratorMacros.h
     mtsComponentState.h
     mtsComponentViewer.h
     mtsConnection.h

     mtsEventReceiver.h
     mtsExecutionResult.h
     mtsExport.h

     mtsFixedSizeVector.h
     mtsFixedSizeVectorTypes.h
     mtsFunctionBase.h
     mtsFunctionQualifiedRead.h
     mtsFunctionRead.h
     mtsFunctionVoid.h
     mtsFunctionVoidReturn.h
     mtsFunctionWrite.h
     mtsFunctionWriteReturn.h
     mtsForwardDeclarations.h

     mtsGenericObject.h
     mtsGenericObjectProxy.h

     mtsHistory.h

     mtsIntervalStatistics.h
     mtsInterfaceCommon.h
     mtsInterfaceInput.h
     mtsInterfaceOutput.h
     mtsInterfaceProvided.h
     mtsInterfaceProvidedOrOutput.h
     mtsInterfaceRequired.h
     mtsInterfaceRequiredOrInput.h

     mtsLODMultiplexerStreambuf.h

     mtsMacros.h
     mtsMailBox.h
     mtsManagerComponentBase.h
     mtsManagerComponentServer.h
     mtsManagerComponentClient.h
     mtsManagerComponentServices.h
     mtsManagerGlobal.h
     mtsManagerGlobalInterface.h
     mtsManagerLocal.h
     mtsManagerLocalInterface.h
     mtsMatrix.h
     mtsMulticastCommandVoid.h
     mtsMulticastCommandWrite.h
     mtsMulticastCommandWriteBase.h

     mtsParameterTypes.h

     mtsQueue.h

     mtsStateArray.h
     mtsStateArrayBase.h
     mtsStateData.h
     mtsStateIndex.h
     mtsStateTable.h

     mtsTask.h
     mtsTaskContinuous.h
     mtsTaskFromCallback.h
     mtsTaskFromSignal.h
     mtsTaskPeriodic.h
     mtsTaskManager.h    # to be deleted

     mtsTransformationTypes.h

     mtsVector.h
     )


if (CISST_MTS_HAS_ICE)

  set (SOURCE_FILES ${SOURCE_FILES}
       mtsInterfaceRequiredProxy.cpp
       mtsComponentInterfaceProxyServer.cpp
       mtsComponentInterfaceProxyClient.cpp
       mtsManagerProxyServer.cpp
       mtsManagerProxyClient.cpp
       mtsComponentProxy.cpp
       )

  set (ADDITIONAL_HEADER_FILES
       ${ADDITIONAL_HEADER_FILES}

       mtsComponentProxy.h

       mtsProxyConfig.h
       mtsProxyBaseCommon.h
       mtsProxyBaseClient.h
       mtsProxyBaseServer.h
       mtsProxySerializer.h

       mtsInterfaceRequiredProxy.h
       mtsComponentInterfaceProxyServer.h
       mtsComponentInterfaceProxyClient.h
       mtsManagerProxyServer.h
       mtsManagerProxyClient.h

       mtsCommandProxyBase.h
       mtsCommandVoidProxy.h
       mtsCommandVoidProxyForReturnEvent.h
       mtsCommandVoidReturnProxy.h
       mtsCommandWriteProxy.h
       mtsCommandWriteReturnProxy.h
       mtsCommandReadProxy.h
       mtsCommandQualifiedReadProxy.h

       mtsFunctionReadProxy.h
       mtsFunctionWriteProxy.h
       mtsFunctionReturnProxyBase.h
       mtsFunctionVoidReturnProxy.h
       mtsFunctionWriteReturnProxy.h
       mtsMulticastCommandVoidProxy.h
       mtsMulticastCommandWriteProxy.h
       mtsEventWriteGeneratorProxy.h
       )

  set (ICE_RESOURCE_DIR ${cisst_SOURCE_DIR}/libs/etc/cisstMultiTask/Ice)
  set (ICE_RESOURCE_FILES
       ${ICE_RESOURCE_DIR}/config.client
       ${ICE_RESOURCE_DIR}/config.server
       ${ICE_RESOURCE_DIR}/config.LCM
       ${ICE_RESOURCE_DIR}/config.GCM
       ${ICE_RESOURCE_DIR}/mtsComponentInterfaceProxy.ice
       ${ICE_RESOURCE_DIR}/mtsManagerProxy.ice
       )

  source_group (Resources FILES ${ICE_RESOURCE_FILES})

  set (SLICE_FILES
       mtsComponentInterfaceProxy
       mtsManagerProxy
       )


  # create a list of include directories for slice
  foreach (ice_include_dir ${ICE_INCLUDE_DIR})
    set (SLICE_OPTIONS ${SLICE_OPTIONS} -I ${ice_include_dir})
  endforeach (ice_include_dir ${ICE_INCLUDE_DIR})

  foreach (slice_file ${SLICE_FILES})
    set (GENERATED_CPP_FILE "${CMAKE_CURRENT_BINARY_DIR}/${slice_file}.cpp")
    set (GENERATED_H_FILE "${CMAKE_CURRENT_BINARY_DIR}/${slice_file}.h")

    add_custom_command (
      OUTPUT ${GENERATED_CPP_FILE} ${GENERATED_H_FILE} ${cisst_BINARY_DIR}/libs/include/cisstMultiTask/${slice_file}.h
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      # first run slice2cpp
      COMMAND ${ICE_SLICE2CPP}
      ARGS    -I ${ICE_SLICE_DIR} ${SLICE_OPTIONS} ${cisst_SOURCE_DIR}/libs/etc/cisstMultiTask/Ice/${slice_file}.ice
      # then create a directory to copy the header file
      COMMAND ${CMAKE_COMMAND}
      ARGS -E make_directory ${cisst_BINARY_DIR}/libs/include/cisstMultiTask
      # and finally copy the header file
      COMMAND ${CMAKE_COMMAND}
      ARGS -E copy_if_different ${GENERATED_H_FILE} ${cisst_BINARY_DIR}/libs/include/cisstMultiTask
      COMMENT "Compiling ${slice_file}.ice"
    )

    include_directories (${CMAKE_CURRENT_BINARY_DIR})
    set (ADDITIONAL_SOURCE_FILES ${ADDITIONAL_SOURCE_FILES} ${GENERATED_CPP_FILE})

  endforeach (slice_file ${SLICE_FILES})

  set (ADDITIONAL_SOURCE_FILES ${ADDITIONAL_SOURCE_FILES} ${ICE_RESOURCE_FILES})
endif (CISST_MTS_HAS_ICE)

# Create the config file
set (CISST_MTS_CONFIG_FILE ${cisst_BINARY_DIR}/libs/include/cisstMultiTask/mtsConfig.h)
configure_file (${cisstMultiTaskLibs_SOURCE_DIR}/mtsConfig.h.in
                ${CISST_MTS_CONFIG_FILE}
                @ONLY)
install_files (/include/cisstMultiTask
               ".h"
               ${CISST_MTS_CONFIG_FILE})

# Add the config file to the project
set_source_files_properties ("${CISST_MTS_CONFIG_FILE}"
                             PROPERTIES GENERATED TRUE)
set (ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES} ${CISST_MTS_CONFIG_FILE})

# Create the CMake config file
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cisstMultiTaskBuild.cmake.in
                ${cisst_BINARY_DIR}/cisstMultiTaskBuild.cmake
                @ONLY)

# Finally add the library
cisst_add_library (LIBRARY cisstMultiTask
                   DEPENDENCIES cisstCommon cisstVector cisstOSAbstraction
                   SOURCE_FILES ${SOURCE_FILES}
                   HEADER_FILES ${HEADER_FILES}
                   ADDITIONAL_SOURCE_FILES ${ADDITIONAL_SOURCE_FILES}
                   ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES})


# Add linking to external libraries
if (CISST_MTS_HAS_ICE)
  if (WIN32)
    target_link_libraries (cisstMultiTask ${ICE_LIBRARIES_ALL})
  else (WIN32)
    target_link_libraries (cisstMultiTask ${ICE_LIBRARIES})
  endif (WIN32)
endif (CISST_MTS_HAS_ICE)
