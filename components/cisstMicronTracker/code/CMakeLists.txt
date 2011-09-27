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

# MicronTracker library
find_path (MicronTracker_INCLUDE_DIR
           NAMES MTC.h
           PATHS "C:/Program Files/Claron Technology/MicronTracker/Dist"
                 "C:/Program Files (x86)/Claron Technology/MicronTracker/Dist")
find_library (MicronTracker_MTC_LIBRARY
              NAMES MTC
              HINTS ${MicronTracker_INCLUDE_DIR})
set (MicronTracker_LIBRARIES
     ${MicronTracker_MTC_LIBRARY})
if (UNIX)
  find_library (MicronTracker_dc1394_LIBRARY
                NAMES dc1394
                HINTS ${MicronTracker_INCLUDE_DIR})
  find_library (raw1394_LIBRARY
                NAMES raw1394)
  find_library (vnl_LIBRARY
                NAMES vnl)
  find_library (vnl_algo_LIBRARY
                NAMES vnl_algo)
  find_library (vcl_LIBRARY
                NAMES vcl)
  find_library (v3p_netlib_LIBRARY
                NAMES v3p_netlib)
  list (APPEND MicronTracker_LIBRARIES
        ${MicronTracker_dc1394_LIBRARY}
        ${raw1394_LIBRARY}
        ${vnl_LIBRARY}
        ${vnl_algo_LIBRARY}
        ${vcl_LIBRARY}
        ${v3p_netlib_LIBRARY})
endif (UNIX)

# Core library
set (CISST_LIBRARIES_NEEDED cisstCommon
                            cisstCommonXML
                            cisstVector
                            cisstNumerical
                            cisstOSAbstraction
                            cisstMultiTask
                            cisstParameterTypes
                            cisstRobot
                            cisstStereoVision)
cisst_set_directories (${CISST_LIBRARIES_NEEDED})

include_directories (${MicronTracker_INCLUDE_DIR})

add_library (cisstMicronTracker
             ../cisstMicronTrackerExport.h
             mtsMicronTracker.cpp
             ../mtsMicronTracker.h)

target_link_libraries (cisstMicronTracker ${MicronTracker_LIBRARIES})
cisst_target_link_libraries (cisstMicronTracker ${CISST_LIBRARIES_NEEDED})

# Qt widgets
if (CISST_HAS_QT)

# this needs to be moved to cisstMacros.cmake
MACRO (CISST_QT4_WRAP_UI outfiles)
  QT4_EXTRACT_OPTIONS (ui_files ui_options ${ARGN})

  FOREACH (it ${ui_files})
    GET_FILENAME_COMPONENT (outfile ${it} NAME_WE)
    GET_FILENAME_COMPONENT (infile ${it} ABSOLUTE)
# the only thing we modified ...
    SET (outfile ${cisst_BINARY_DIR}/components/cisstMicronTracker/${outfile}.h)
    ADD_CUSTOM_COMMAND (OUTPUT ${outfile}
                        COMMAND ${QT_UIC_EXECUTABLE}
                        ARGS ${ui_options} -o ${outfile} ${infile}
                        MAIN_DEPENDENCY ${infile})
    SET (${outfiles} ${${outfiles}} ${outfile})
  ENDFOREACH (it)
ENDMACRO (CISST_QT4_WRAP_UI)


  include_directories (${CMAKE_CURRENT_BINARY_DIR})
  qt4_wrap_cpp (CISST_NDISERIAL_QT_COMPONENTS_GENERATED
                ../mtsMicronTrackerControllerQtComponent.h
                ../mtsMicronTrackerToolQtComponent.h
                )
  cisst_qt4_wrap_ui (CISST_NDISERIAL_QT_WIDGETS_GENERATED
                     mtsMicronTrackerControllerQtWidget.ui
                     mtsMicronTrackerToolQtWidget.ui)

  add_library (cisstMicronTrackerQt
               ${CISST_NDISERIAL_QT_COMPONENTS_GENERATED}
               ${CISST_NDISERIAL_QT_WIDGETS_GENERATED}
               ../cisstMicronTrackerExportQt.h
               mtsMicronTrackerControllerQtComponent.cpp
               ../mtsMicronTrackerControllerQtComponent.h
               mtsMicronTrackerToolQtComponent.cpp
               ../mtsMicronTrackerToolQtComponent.h)

  target_link_libraries (cisstMicronTrackerQt ${QT_LIBRARIES})
  cisst_target_link_libraries (cisstMicronTrackerQt ${CISST_LIBRARIES_NEEDED})

endif (CISST_HAS_QT)

# create/configure file for find_package (cisstMicronTracker)
set (cisstMicronTracker_INCLUDE_DIR "${cisstComponents_SOURCE_DIR}" "${cisst_BINARY_DIR}/components" "${MicronTracker_INCLUDE_DIR}")
set (cisstMicronTracker_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}")
set (cisstMicronTracker_LIBRARIES cisstMicronTracker "${MicronTracker_LIBRARIES}")
set (cisstMicronTrackerQt_LIBRARIES cisstMicronTrackerQt)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/cisstMicronTrackerConfig.cmake.in"
                "${cisstComponents_BINARY_DIR}/cisstMicronTrackerConfig.cmake"
                 @ONLY@)
