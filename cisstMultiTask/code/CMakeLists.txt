#
#
# CMakeLists for cisstMultiTask
#
# (C) Copyright 2003-2014 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# create data type using the data generator
cisst_data_generator (cisstMultiTask
                      "${cisst_BINARY_DIR}/include" # where to save the file
                      "cisstMultiTask/"        # sub directory for include
                      ../mtsInterfaceCommon.cdg
                      ../mtsComponentState.cdg
                      ../mtsExecutionResult.cdg
                      ../mtsParameterTypes.cdg)
# to compile cisst generated code, need to find header file
include_directories (${CMAKE_CURRENT_BINARY_DIR})

# all source files
set (SOURCE_FILES

     mtsClassServices.cpp

     mtsCollectorBase.cpp
     mtsCollectorEvent.cpp
     mtsCollectorState.cpp
     mtsCollectorFactory.cpp

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

     mtsComponentState.cpp # see mtsComponentState.cdg
     mtsComponent.cpp
     mtsComponentAddLatency.cpp
     mtsComponentViewer.cpp
     mtsConnection.cpp

     mtsEventReceiver.cpp
     mtsExecutionResult.cpp  # see mtsExecutionResult.cdg

     mtsFunctionBase.cpp
     mtsFunctionQualifiedRead.cpp
     mtsFunctionRead.cpp
     mtsFunctionVoid.cpp
     mtsFunctionVoidReturn.cpp
     mtsFunctionWrite.cpp
     mtsFunctionWriteReturn.cpp

     mtsGenericObject.cpp

     mtsInterface.cpp
     mtsInterfaceInput.cpp
     mtsInterfaceOutput.cpp
     mtsInterfaceProvided.cpp
     mtsInterfaceRequired.cpp
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

     mtsParameterTypesOld.cpp

     mtsSocketProxyCommon.cpp
     mtsSocketProxyClient.cpp
     mtsSocketProxyServer.cpp

     mtsStateIndex.cpp
     mtsStateTable.cpp

     mtsTask.cpp
     mtsTaskContinuous.cpp
     mtsTaskFromCallback.cpp
     mtsTaskFromSignal.cpp
     mtsTaskPeriodic.cpp

     mtsWatchdogClient.cpp
     mtsWatchdogServer.cpp
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
     mtsCollectorFactory.h

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
     mtsComponentAddLatency.h
     mtsComponentGeneratorMacros.h
     mtsComponentViewer.h
     mtsConnection.h

     mtsEventReceiver.h
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

     mtsIntervalStatistics.h
     mtsInterface.h
     mtsInterfaceInput.h
     mtsInterfaceOutput.h
     mtsInterfaceProvided.h
     mtsInterfaceRequired.h

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

     mtsParameterTypesOld.h

     mtsQueue.h

     mtsSocketProxyCommon.h
     mtsSocketProxyClient.h
     mtsSocketProxyServer.h

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

     mtsWatchdogClient.h
     mtsWatchdogServer.h

     mtsTransformationTypes.h

     mtsVector.h
     )

# Header files in code directory
set (ADDITIONAL_HEADER_FILES
     mtsProxySerializer.h
     )

if (MINGW)
  message("Information: splitting mtsClassServices.cpp for MINGW")
  file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/mtsClassServices1.cpp
        "#define MTS_CLASS_SERVICES_PART1\n#include \"${CMAKE_CURRENT_SOURCE_DIR}/mtsClassServices.cpp\"\n")
  file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/mtsClassServices2.cpp
        "#define MTS_CLASS_SERVICES_PART2\n#include \"${CMAKE_CURRENT_SOURCE_DIR}/mtsClassServices.cpp\"\n")
  file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/mtsClassServices3.cpp
        "#define MTS_CLASS_SERVICES_PART3\n#include \"${CMAKE_CURRENT_SOURCE_DIR}/mtsClassServices.cpp\"\n")
  set (ADDITIONAL_SOURCE_FILES ${ADDITIONAL_SOURCE_FILES}
       ${CMAKE_CURRENT_BINARY_DIR}/mtsClassServices1.cpp
       ${CMAKE_CURRENT_BINARY_DIR}/mtsClassServices2.cpp
       ${CMAKE_CURRENT_BINARY_DIR}/mtsClassServices3.cpp)
endif (MINGW)

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
       mtsFunctionQualifiedReadProxy.h
       mtsFunctionReturnProxyBase.h
       mtsFunctionVoidReturnProxy.h
       mtsFunctionWriteReturnProxy.h
       mtsMulticastCommandVoidProxy.h
       mtsMulticastCommandWriteProxy.h
       mtsEventWriteGeneratorProxy.h
       )

  set (ICE_RESOURCE_DIR ${cisstMultiTaskLibs_SOURCE_DIR}/share/Ice)
  set (ICE_CONFIG_FILES
       ${ICE_RESOURCE_DIR}/config.client
       ${ICE_RESOURCE_DIR}/config.server
       ${ICE_RESOURCE_DIR}/config.LCM
       ${ICE_RESOURCE_DIR}/config.GCM)
  install (FILES ${ICE_CONFIG_FILES} DESTINATION ${CISST_SHARE_INSTALL_SUFFIX}/cisstMultiTask/Ice COMPONENT cisstMultiTask)

  set (ICE_RESOURCE_FILES
       ${ICE_CONFIG_FILES}
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
      OUTPUT ${GENERATED_CPP_FILE} ${GENERATED_H_FILE} ${cisst_BINARY_DIR}/include/cisstMultiTask/${slice_file}.h
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      # first run slice2cpp
      COMMAND ${ICE_SLICE2CPP}
      ARGS    -I ${ICE_SLICE_DIR} ${SLICE_OPTIONS} ${cisstMultiTaskLibs_SOURCE_DIR}/share/Ice/${slice_file}.ice
      # then create a directory to copy the header file
      COMMAND ${CMAKE_COMMAND}
      ARGS -E make_directory ${cisst_BINARY_DIR}/include/cisstMultiTask
      # and finally copy the header file
      COMMAND ${CMAKE_COMMAND}
      ARGS -E copy_if_different ${GENERATED_H_FILE} ${cisst_BINARY_DIR}/include/cisstMultiTask
      COMMENT "Compiling ${slice_file}.ice"
      MAIN_DEPENDENCY ${cisstMultiTaskLibs_SOURCE_DIR}/share/Ice/${slice_file}.ice
    )

    include_directories (${CMAKE_CURRENT_BINARY_DIR})
    set (ADDITIONAL_SOURCE_FILES ${ADDITIONAL_SOURCE_FILES} ${GENERATED_CPP_FILE})

  endforeach (slice_file ${SLICE_FILES})

  set (ADDITIONAL_SOURCE_FILES ${ADDITIONAL_SOURCE_FILES} ${ICE_RESOURCE_FILES})
endif (CISST_MTS_HAS_ICE)

# Create the config file
set (CISST_MTS_CONFIG_FILE ${cisst_BINARY_DIR}/include/cisstMultiTask/mtsConfig.h)
configure_file (${cisstMultiTaskLibs_SOURCE_DIR}/mtsConfig.h.in
                ${CISST_MTS_CONFIG_FILE}
                @ONLY)
install (FILES ${CISST_MTS_CONFIG_FILE}
         DESTINATION include/cisstMultiTask
         COMPONENT cisstMultiTask)

# Add the config file to the project
set_source_files_properties ("${CISST_MTS_CONFIG_FILE}"
                             PROPERTIES GENERATED TRUE)
set (ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES} ${CISST_MTS_CONFIG_FILE})

# Create the CMake config file
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cisstMultiTaskBuild.cmake.in
                ${CISST_CMAKE_BINARY_DIR}/cisstMultiTaskBuild.cmake
                @ONLY)
install (FILES ${CISST_CMAKE_BINARY_DIR}/cisstMultiTaskBuild.cmake
         DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
         COMPONENT cisstMultiTask)

# Finally add the library
cisst_add_library (LIBRARY cisstMultiTask
                   FOLDER cisstMultiTask
                   DEPENDENCIES cisstCommon cisstVector cisstOSAbstraction
                   SOURCE_FILES ${SOURCE_FILES}
                   HEADER_FILES ${HEADER_FILES}
                   ADDITIONAL_SOURCE_FILES ${ADDITIONAL_SOURCE_FILES}
                                           ${cisstMultiTask_CISST_DG_SRCS}
                   ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES}
                                           ${cisstMultiTask_CISST_DG_HDRS})

install (FILES ${cisstMultiTask_CISST_DG_HDRS_FULLPATH}
         DESTINATION "include/cisstMultiTask")

# Add linking to external libraries
if (CISST_MTS_HAS_ICE)
  if (WIN32)
    target_link_libraries (cisstMultiTask ${ICE_LIBRARIES_ALL})
  else (WIN32)
    target_link_libraries (cisstMultiTask ${ICE_LIBRARIES})
  endif (WIN32)

  # copy configuration files to build tree
  file (MAKE_DIRECTORY "${CISST_SHARE_DIR}/cisstMultiTask/Ice")
  foreach (configFile ${ICE_CONFIG_FILES})
    add_custom_command (TARGET cisstMultiTask
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy_if_different
                               ${configFile}
                               ${CISST_SHARE_DIR}/cisstMultiTask/Ice)

  endforeach (configFile)

endif (CISST_MTS_HAS_ICE)

# Add installer files (only tested on WIN32)
if (WIN32 AND CISST_MTS_HAS_ICE)
  set (ICE_BIN ${ICE_HOME}/bin)
  file (GLOB icedll "${ICE_BIN}/Ice??.dll")
  install (FILES ${icedll} DESTINATION bin COMPONENT cisstMultiTask)
  file (GLOB iceutildll "${ICE_BIN}/IceUtil??.dll")
  install (FILES ${iceutildll} DESTINATION bin COMPONENT cisstMultiTask)
  install (FILES ${ICE_BIN}/bzip2.dll DESTINATION bin COMPONENT cisstMultiTask)
endif (WIN32 AND CISST_MTS_HAS_ICE)

if (WIN32 AND CMAKE_CL_64)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif (WIN32 AND CMAKE_CL_64)
