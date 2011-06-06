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

# Set the ouptut path for the libraries and executables
set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

# cisstSphinx4, wrapper for CMU Sphinx 4 to generate commands from speech
if (BUILD_LIBS_cisstMultiTask)
  option (CISST_BUILD_COMP_cisstSphinx4 "Build component cisstSphinx4 (requires JNI and CMU Sphinx 4)" OFF)
  if (CISST_BUILD_COMP_cisstSphinx4)
    add_subdirectory (cisstSphinx4)
  endif (CISST_BUILD_COMP_cisstSphinx4)
endif (BUILD_LIBS_cisstMultiTask)

add_subdirectory( cisstOpenNI )
