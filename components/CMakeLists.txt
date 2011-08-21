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

# cisstNDISerial, wrappers for serial port based trackers from NDI, i.e. Polaris, Aurora, Vicra, ...
option (CISST_BUILD_COMP_cisstNDISerial "Build component cisstNDISerial" OFF)
if (CISST_BUILD_COMP_cisstNDISerial)
  add_subdirectory (cisstNDISerial)
endif (CISST_BUILD_COMP_cisstNDISerial)

# cisstOpenNI, wrappers for OpenNI Kinect
option (CISST_BUILD_COMP_cisstOpenNI "Build component cisstOpenNI (requires OpenNI)" OFF)
if (CISST_BUILD_COMP_cisstOpenNI)
  add_subdirectory (cisstOpenNI)
endif (CISST_BUILD_COMP_cisstOpenNI)

# cisstSphinx4, wrapper for CMU Sphinx 4 to generate commands from speech
if (CISST_BUILD_LIBS_cisstMultiTask)
  option (CISST_BUILD_COMP_cisstSphinx4 "Build component cisstSphinx4 (requires JNI and CMU Sphinx 4)" OFF)
  if (CISST_BUILD_COMP_cisstSphinx4)
    add_subdirectory (cisstSphinx4)
  endif (CISST_BUILD_COMP_cisstSphinx4)
endif (CISST_BUILD_LIBS_cisstMultiTask)

# cisstStealthlink, wrappers for Medtronic StealthLink
option (CISST_BUILD_COMP_cisstStealthlink "Build component cisstStealthlink (requires Stealthlink from Medtronic)" OFF)
if (CISST_BUILD_COMP_cisstStealthlink)
  add_subdirectory (cisstStealthlink)
endif (CISST_BUILD_COMP_cisstStealthlink)

# cisstCAN wrappers for various CAN devices
option(CISST_BUILD_COMP_cisstCAN "Build CAN support" OFF)
if( CISST_BUILD_COMP_cisstCAN )
  add_subdirectory( cisstCAN )
endif( CISST_BUILD_COMP_cisstCAN )

