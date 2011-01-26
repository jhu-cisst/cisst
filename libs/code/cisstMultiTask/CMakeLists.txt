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


# make sure all options are set for all devices to OFF
find_package (Ice)
if (ICE_FOUND)
  option (CISST_MTS_HAS_ICE "Enable inter-process communication (IPC) feature of cisstMultiTask, requires ICE from ZeroC" ON)
  mark_as_advanced (CISST_MTS_HAS_ICE)
else (ICE_FOUND)
  option (CISST_MTS_HAS_ICE "Enable inter-process communication (IPC) feature of cisstMultiTask, requires ICE from ZeroC" OFF)
endif (ICE_FOUND)

if (CISST_MTS_HAS_ICE)

  set (SOURCE_FILES ${SOURCE_FILES}
       mtsComponentInterfaceProxyServer.cpp
       mtsComponentInterfaceProxyClient.cpp
       mtsManagerProxyServer.cpp
       mtsManagerProxyClient.cpp
       mtsComponentProxy.cpp
       )

  set (PROXY_HEADER_FILES
       mtsComponentProxy.h

       mtsProxyConfig.h
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
       mtsFunctionReadProxy.h
       mtsFunctionWriteProxy.h
       mtsFunctionQualifiedReadProxy.h
       mtsMulticastCommandWriteProxy.h
       mtsEventWriteGeneratorProxy.h
       )

  # For now, consider the PROXY_HEADER_FILES to be additional header files; that way, they are not added
  # to the generated cisstMultiTask.h
  foreach (file ${PROXY_HEADER_FILES})
    set (ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES} ${cisst_SOURCE_DIR}/libs/include/cisstMultiTask/${file})
  endforeach (file)

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

  find_package (Ice REQUIRED)

  CISST_ADD_TO_INTERNAL (CISST_ADDITIONAL_INCLUDE_DIRECTORIES ICE_INCLUDE_DIR)
  CISST_ADD_TO_INTERNAL (CISST_ADDITIONAL_LINK_DIRECTORIES ICE_LIBRARY_DIR)

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
configure_file (${cisst_SOURCE_DIR}/libs/include/cisstMultiTask/mtsConfig.h.in
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
cisst_add_library(
  LIBRARY cisstMultiTask
  DEPENDENCIES cisstCommon cisstVector cisstOSAbstraction
  SOURCE_FILES ${SOURCE_FILES}
  HEADER_FILES ${HEADER_FILES}
  ADDITIONAL_SOURCE_FILES ${ADDITIONAL_SOURCE_FILES}
  ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES})


# Add linking to external libraries
if (CISST_MTS_HAS_ICE)
  if (WIN32)
    set (ICE_LIBRARIES_OPTIMIZED Ice IceUtil)
    set (ICE_LIBRARIES_DEBUG Iced IceUtild)
    target_link_libraries (cisstMultiTask
                           optimized Ice
                           debug Iced
                           optimized IceUtil
                           debug IceUtild)
    CISST_ADD_TO_INTERNAL (CISST_ADDITIONAL_LIBRARIES_DEBUG ICE_LIBRARIES_DEBUG)
    CISST_ADD_TO_INTERNAL (CISST_ADDITIONAL_LIBRARIES_OPTIMIZED ICE_LIBRARIES_OPTIMIZED)
  else (WIN32)
    set (ICE_LIBRARIES ${ICE_LIBRARY_NAME} IceUtil)
    target_link_libraries (cisstMultiTask
                           ${ICE_LIBRARIES})
    CISST_ADD_TO_INTERNAL (CISST_ADDITIONAL_LIBRARIES ICE_LIBRARIES)
  endif (WIN32)

endif (CISST_MTS_HAS_ICE)


# Qt widgets
if (CISST_HAS_QT)

  # To include Qt generated files
  include_directories (${CMAKE_CURRENT_BINARY_DIR})
  qt4_wrap_cpp (QT_WRAPPED_CPP
               ../../include/cisstMultiTask/mtsCollectorQtComponent.h
               ../../include/cisstMultiTask/mtsCollectorQtWidget.h)

  cisst_add_library (
    LIBRARY cisstMultiTaskQt
    LIBRARY_DIR cisstMultiTask
    DEPENDENCIES cisstCommon
                 cisstOSAbstraction
                 cisstVector
                 cisstMultiTask
    SOURCE_FILES mtsClassServicesQt.cpp
                 mtsCollectorQtComponent.cpp
                 mtsCollectorQtWidget.cpp
    HEADER_FILES mtsCollectorQtComponent.h
                 mtsCollectorQtWidget.h
                 mtsExportQt.h
    ADDITIONAL_SOURCE_FILES ${QT_WRAPPED_CPP})

  target_link_libraries (cisstMultiTaskQt ${QT_LIBRARIES})

endif (CISST_HAS_QT)

# Python wrappers
if (CISST_HAS_SWIG_PYTHON)
  cisst_add_swig_module(MODULE cisstMultiTask
                        INTERFACE_DIRECTORY ../../include/cisstMultiTask
			CISST_LIBRARIES cisstMultiTask cisstOSAbstraction cisstVector cisstCommon)
endif (CISST_HAS_SWIG_PYTHON)
