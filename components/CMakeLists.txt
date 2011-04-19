#
# $Id: CMakeLists.txt 2448 2011-04-09 15:31:41Z adeguet1 $
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

# cisstSpeechToCommands
if (BUILD_LIBS_cisstMultiTask)
  option (CISST_BUILD_COMP_cisstSpeechToCommands "Build component cisstSpeechToCommands (requires JNI and CMU Sphinx)" OFF)
  mark_as_advanced (CISST_BUILD_COMP_cisstSpeechToCommands)
  if (CISST_BUILD_COMP_cisstSpeechToCommands)
    add_subdirectory (cisstSpeechToCommands)
  endif (CISST_BUILD_COMP_cisstSpeechToCommands)
endif (BUILD_LIBS_cisstMultiTask)

