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

# add_subdirectory based on dependencies
if (CISST_BUILD_LIBS_cisstMultiTask)
  add_subdirectory (cisstComponentGenerator)
  add_subdirectory (cisstDataGenerator)
  add_subdirectory (cisstComponentManager)

  if (CISST_HAS_QT AND CISST_BUILD_LIBS_cisstStereoVision)
    add_subdirectory (cisstDataPlayer)
  else (CISST_HAS_QT  AND CISST_BUILD_LIBS_cisstStereoVision)
    message (WARNING " utility cisstDataPlayer will not be compiled since it requires CISST_HAS_QT and cisstStereoVision")
  endif (CISST_HAS_QT AND CISST_BUILD_LIBS_cisstStereoVision)

  if (CISST_MTS_HAS_ICE)

    add_subdirectory (multiTaskGlobalComponentManager)
    if (CISST_HAS_FLTK)
      add_subdirectory (multiTaskGlobalComponentManagerFLTK)
    else (CISST_HAS_FLTK)
      message (WARNING " utility multiTaskGlobalComponentManagerFLTK will not be compiled since it requires CISST_HAS_FLTK")
    endif (CISST_HAS_FLTK)

    if (CISST_BUILD_LIBS_cisstInteractive)
      add_subdirectory (cisstIREComponent)
    else (CISST_BUILD_LIBS_cisstInteractive)
      message (WARNING " utility cisstIREComponent will not be compiled since it requires CISST_BUILD_LIBS_cisstInteractive")
    endif (CISST_BUILD_LIBS_cisstInteractive)

  else (CISST_MTS_HAS_ICE)
    message (WARNING " utilities multiTaskGlobalComponentManager, multiTaskGlobalComponentManagerFLTK, and cisstIREComponent will not be compiled since they require CISST_MTS_HAS_ICE")
  endif (CISST_MTS_HAS_ICE)

else (CISST_BUILD_LIBS_cisstMultiTask)
  message (WARNING " utilities cisstComponentGenerator, cisstDataGenerator, cisstDataPlayer, multiTaskGlobalComponentManager, multiTaskGlobalComponentManagerFLTK, and cisstIREComponent will not be compiled since they require CISST_BUILD_LIBS_cisstMultiTask")
endif (CISST_BUILD_LIBS_cisstMultiTask)


if (CISST_BUILD_LIBS_cisstRobot AND
    CISST_BUILD_LIBS_cisstNumerical AND
    CISST_HAS_CISSTNETLIB)
  add_subdirectory( cisstRobot )
endif (CISST_BUILD_LIBS_cisstRobot AND
       CISST_BUILD_LIBS_cisstNumerical AND
       CISST_HAS_CISSTNETLIB)

