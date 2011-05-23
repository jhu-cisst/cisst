#
# $Id$
#
# (C) Copyright 2005-2010 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

if (BUILD_LIBS_cisstStereoVision)

add_subdirectory (simple)
add_subdirectory (cameraviewer)
add_subdirectory (stereocameraviewer)
add_subdirectory (computestereo)
add_subdirectory (videoplayer)
add_subdirectory (stereovideoplayer)
add_subdirectory (videoconverter)
add_subdirectory (pointtracker)
add_subdirectory (gridtracker)
add_subdirectory (exposurecorrection)
add_subdirectory (svlmts1)
add_subdirectory (cameraCalibration)
#ADD_SUBDIRECTORY(webgui)

else (BUILD_LIBS_cisstStereoVision)
  message (WARNING " stereoTutorial examples will not be compiled since they require BUILD_LIBS_cisstStereoVision")
endif (BUILD_LIBS_cisstStereoVision)