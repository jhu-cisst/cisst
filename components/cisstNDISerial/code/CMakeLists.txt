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

# Core library
set (CISST_LIBRARIES_NEEDED cisstCommon
                            cisstCommonXML
                            cisstVector
                            cisstNumerical
                            cisstOSAbstraction
                            cisstMultiTask
                            cisstParameterTypes
                            cisstRobot)
cisst_set_directories (${CISST_LIBRARIES_NEEDED})

add_library (cisstNDISerial
             ../cisstNDISerialExport.h
             mtsNDISerial.cpp
             ../mtsNDISerial.h)

cisst_target_link_libraries (cisstNDISerial ${CISST_LIBRARIES_NEEDED})

# Qt widgets
if (CISST_HAS_QT)

# this needs to be moved to cisstMacros.cmake
MACRO (CISST_QT4_WRAP_UI outfiles)
  QT4_EXTRACT_OPTIONS (ui_files ui_options ${ARGN})

  FOREACH (it ${ui_files})
    GET_FILENAME_COMPONENT (outfile ${it} NAME_WE)
    GET_FILENAME_COMPONENT (infile ${it} ABSOLUTE)
# the only thing we modified ...
    SET (outfile ${cisst_BINARY_DIR}/components/cisstNDISerial/${outfile}.h)
    ADD_CUSTOM_COMMAND (OUTPUT ${outfile}
                        COMMAND ${QT_UIC_EXECUTABLE}
                        ARGS ${ui_options} -o ${outfile} ${infile}
                        MAIN_DEPENDENCY ${infile})
    SET (${outfiles} ${${outfiles}} ${outfile})
  ENDFOREACH (it)
ENDMACRO (CISST_QT4_WRAP_UI)


  include_directories (${CMAKE_CURRENT_BINARY_DIR})
  qt4_wrap_cpp (CISST_NDISERIAL_QT_COMPONENTS_GENERATED
                ../mtsNDISerialControllerQtComponent.h
                ../mtsNDISerialToolQtComponent.h
                )
  cisst_qt4_wrap_ui (CISST_NDISERIAL_QT_WIDGETS_GENERATED
                     mtsNDISerialControllerQtWidget.ui
                     mtsNDISerialToolQtWidget.ui)

  add_library (cisstNDISerialQt
               ${CISST_NDISERIAL_QT_COMPONENTS_GENERATED}
               ${CISST_NDISERIAL_QT_WIDGETS_GENERATED}
               ../cisstNDISerialExportQt.h
               mtsNDISerialControllerQtComponent.cpp
               ../mtsNDISerialControllerQtComponent.h
               mtsNDISerialToolQtComponent.cpp
               ../mtsNDISerialToolQtComponent.h)

  cisst_target_link_libraries (cisstNDISerialQt ${CISST_LIBRARIES_NEEDED})

endif (CISST_HAS_QT)



set (cisstNDISerial_INCLUDE_DIR "${cisstComponents_SOURCE_DIR}" "${cisst_BINARY_DIR}/components")
set (cisstNDISerial_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}")
set (cisstNDISerial_LIBRARIES cisstNDISerial)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/cisstNDISerialConfig.cmake.in"
                "${cisstComponents_BINARY_DIR}/cisstNDISerialConfig.cmake"
                 @ONLY@)
