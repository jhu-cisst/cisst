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

project (cisstComponents)

# Set the ouptut path for the libraries and executables
set (LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/lib")
set (EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin")

# cisstCAN wrappers for various CAN devices
option (CISST_BUILD_COMP_cisstCAN "Build controller area network (CAN) support" OFF)
if (CISST_BUILD_COMP_cisstCAN)
  add_subdirectory (cisstCAN)
endif (CISST_BUILD_COMP_cisstCAN)

# cisstISIAPI
option (CISST_BUILD_COMP_cisstISIAPI "Wrappers for Intuitive Surgical ISI API (read only API for daVinci), requires isi-api binaries from ISI" OFF)
if (CISST_BUILD_COMP_cisstISIAPI)
  add_subdirectory (cisstISIAPI)
endif (CISST_BUILD_COMP_cisstISIAPI)

# cisstNDISerial
option (CISST_BUILD_COMP_cisstNDISerial "Wrappers for serial port based trackers from NDI, i.e. Polaris, Aurora, Vicra ..." OFF)
if (CISST_BUILD_COMP_cisstNDISerial)
  add_subdirectory (cisstNDISerial)
endif (CISST_BUILD_COMP_cisstNDISerial)

# cisstOpenNI
option (CISST_BUILD_COMP_cisstOpenNI "Wrappers for OpenNI Kinect, requires OpenNI" OFF)
if (CISST_BUILD_COMP_cisstOpenNI)
  add_subdirectory (cisstOpenNI)
endif (CISST_BUILD_COMP_cisstOpenNI)

# cisstSphinx4
if (CISST_BUILD_LIBS_cisstMultiTask)
  option (CISST_BUILD_COMP_cisstSphinx4 "Wrappers for CMU Sphinx 4 to generate commands from speech, requires JNI and CMU Sphinx 4" OFF)
  if (CISST_BUILD_COMP_cisstSphinx4)
    add_subdirectory (cisstSphinx4)
  endif (CISST_BUILD_COMP_cisstSphinx4)
endif (CISST_BUILD_LIBS_cisstMultiTask)

# cisstStealthlink
option (CISST_BUILD_COMP_cisstStealthlink "Wrappers for Medtronic StealthLink, requires binaries from Medtronic" OFF)
if (CISST_BUILD_COMP_cisstStealthlink)
  add_subdirectory (cisstStealthlink)
endif (CISST_BUILD_COMP_cisstStealthlink)

# cisstBarrett wrappers for Barrett WAM and BH
if(CISST_BUILD_COMP_cisstCAN)
  option(CISST_BUILD_COMP_cisstBarrett "Build Barrett support" OFF)
  if( CISST_BUILD_COMP_cisstBarrett )
    add_subdirectory( cisstBarrett )
  endif( CISST_BUILD_COMP_cisstBarrett )
endif(CISST_BUILD_COMP_cisstCAN)
  


# cisstODE wrappers and mts component for osg
option(CISST_BUILD_COMP_cisstOSG "Build OpenSceneGraph (OSG) support" OFF)
if( CISST_BUILD_COMP_cisstOSG )
  add_subdirectory( cisstOSG )
endif( CISST_BUILD_COMP_cisstOSG )


# cisstODE wrappers and mts component for ode
option(CISST_BUILD_COMP_cisstODE "Build Open Dynamic Engine (ODE) support" OFF)
if( CISST_BUILD_COMP_cisstODE )
  add_subdirectory( cisstODE )
endif( CISST_BUILD_COMP_cisstODE )

