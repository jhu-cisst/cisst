#
# $Id: CMakeLists.txt 2593 2011-05-25 22:41:14Z adeguet1 $
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


set( HEADER_FILES	../cisstOpenNI.h
					../cisstOpenNISkeleton.h
					cisstOpenNIData.h )
set( SOURCE_FILES	cisstOpenNI.cpp
					cisstOpenNISkeleton.cpp
					cisstOpenNIData.cpp )


# Finally create the library
add_library( cisstOpenNI ${HEADER_FILES} ${SOURCE_FILES} )

target_link_libraries( cisstOpenNI ${OPENNI_LIBRARIES} )

cisst_target_link_libraries ( 
  cisstOpenNI
  cisstCommon
  cisstVector
  cisstOSAbstraction
  cisstMultiTask )

set( cisstOpenNI_INCLUDE_DIR "${cisstComponents_SOURCE_DIR}" )
set( cisstOpenNI_LIBRARY_DIR ${LIBRARY_OUTPUT_PATH} )
set( cisstOpenNI_LIBRARIES cisstOpenNI 
                            ${OPENNI_LIBRARIES})

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/cisstOpenNIConfig.cmake.in"
                "${cisstComponents_BINARY_DIR}/cisstOpenNIConfig.cmake"
                 @ONLY@)
