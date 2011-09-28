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

set (REQUIRED_CISST_LIBRARIES cisstCommon cisstOSAbstraction cisstVector cisstVectorQt cisstMultiTask cisstStereoVision cisstStereoVisionQt cisstQt)

# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  include_directories (${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

  qt4_wrap_cpp (QT_WRAPPED_CPP
                cdpPlayerBase.h
                cdpPlayerExample.h
                cdpPlayerManager.h
                cdpPlayerVideo.h
                cdpPlayerPlot2D.h
                )

  qt4_wrap_ui (QT_WRAPPED_UI
               cdpPlayerManagerWidget.ui
               cdpPlayerWidget.ui)

  add_library (cisstDataPlayerComponents
               ${QT_WRAPPED_CPP}
               ${QT_WRAPPED_UI}
               cdpExport.h
               cdpPlayerBase.h
               cdpPlayerBase.cpp
               cdpPlayerManager.h
               cdpPlayerManager.cpp
               cdpSaveParameters.h
               cdpSaveParameters.cpp
               cdpPlayerDataInfo.h
               cdpPlayerDataInfo.cpp
               cdpPlayerExample.h
               cdpPlayerExample.cpp
               cdpPlayerPlot2D.h
               cdpPlayerPlot2D.cpp
               cdpPlayerVideo.h
               cdpPlayerVideo.cpp
               cdpPlayerParseStateTableData.h
               cdpPlayerParseStateTableData.cpp
               )

  cisst_target_link_libraries (cisstDataPlayerComponents ${REQUIRED_CISST_LIBRARIES})

  add_executable (cisstDataPlayer
                  main.cpp)

  target_link_libraries (cisstDataPlayer cisstDataPlayerComponents)
  cisst_target_link_libraries (cisstDataPlayer ${REQUIRED_CISST_LIBRARIES})

  add_executable (cisstQtVideoPlayer
                  qtVideoPlayer.cpp)

  target_link_libraries (cisstQtVideoPlayer cisstDataPlayerComponents)
  cisst_target_link_libraries (cisstQtVideoPlayer ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Utility in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
