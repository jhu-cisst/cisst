#
# $Id$
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

# Set the ouptut path for the libraries and executables
set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Lists all examples (one per subdirectory)
set (CISST_EXAMPLES
     3DUserInterfaceTutorial
     cisstMultiTask
     commonTutorial
     devicesTutorial
     interactiveTutorial
     multiTaskTutorial
     nArrayBenchmark
     numericalTutorial
     osAbstractionTutorial
     registrationTest
     stereoTutorial
     vectorTutorial
    )

# Which examples to build
foreach (example ${CISST_EXAMPLES})
  # Build by default
  option (BUILD_EXAMPLES_${example} "Build ${example}." ON)
  mark_as_advanced (BUILD_EXAMPLES_${example})
endforeach (example)

# Create targets for each example
foreach (example ${CISST_EXAMPLES})
  if (BUILD_EXAMPLES_${example})
    add_subdirectory (${example})
  endif (BUILD_EXAMPLES_${example})
endforeach (example)
