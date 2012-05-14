#
# $Id$
#
# (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# create alias for code and cmake configured file
option (CISST_BUILD_COMP_cisstStealthlink_SIMULATOR "Wrappers for Medtronic StealthLink simulator (build instead of real wrapper which requires binaries from Medtronic" OFF)
mark_as_advanced (CISST_BUILD_COMP_cisstStealthlink_SIMULATOR)
set (cisstStealthlink_IS_SIMULATOR ${CISST_BUILD_COMP_cisstStealthlink_SIMULATOR})

# add definition for code
if (cisstStealthlink_IS_SIMULATOR)
  add_definitions (-DcisstStealthlink_IS_SIMULATOR)
endif (cisstStealthlink_IS_SIMULATOR)

set (CISST_LIBRARIES_NEEDED cisstCommon
                            cisstCommonXML
                            cisstVector
                            cisstOSAbstraction
                            cisstMultiTask
                            cisstParameterTypes)

cisst_set_directories (${CISST_LIBRARIES_NEEDED})

add_library (cisstStealthlink ${IS_SHARED}
             ../cisstStealthlinkExport.h
             mtsStealthlink.cpp
             ../mtsStealthlink.h
             ../mtsStealthlinkTypes.h
	     ../mtsStealthlinkControllerComponent.h
             mtsStealthlinkControllerComponent.cpp
             mtsStealthlinkTypes.cpp
             mtsStealthlink_AsCL_Stuff.cpp
             mtsStealthlink_AsCL_Stuff.h
             )

target_link_libraries (cisstStealthlink ${Stealthlink_LIBRARIES})

if(WIN32)
  target_link_libraries(cisstStealthlink ws2_32.lib)
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /NODEFAULTLIB:msvcprt.lib /NODEFAULTLIB:LIBCMT.lib")
endif(WIN32)

cisst_target_link_libraries (cisstStealthlink ${CISST_LIBRARIES_NEEDED})

set (cisstStealthlink_INCLUDE_DIR "${cisstComponents_SOURCE_DIR}")
set (cisstStealthlink_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}")
set (cisstStealthlink_LIBRARIES cisstStealthlink ${Stealthlink_LIBRARIES})

  # Python wrappers
  if (CISST_HAS_SWIG_PYTHON)
    cisst_add_swig_module(MODULE cisstStealthlink
                        INTERFACE_DIRECTORY ../
			CISST_LIBRARIES cisstStealthlink
                                        ${Stealthlink_LIBRARIES})
  endif (CISST_HAS_SWIG_PYTHON)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/cisstStealthlinkConfig.cmake.in"
                "${cisstComponents_BINARY_DIR}/cisstStealthlinkConfig.cmake"
                 @ONLY@)
