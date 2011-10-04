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

add_subdirectory (cisstComponentManager)
add_subdirectory (cisstDataPlayer)

add_subdirectory (multiTaskGlobalComponentManager)
add_subdirectory (multiTaskGlobalComponentManagerFLTK)

add_subdirectory (cisstIREComponent)
