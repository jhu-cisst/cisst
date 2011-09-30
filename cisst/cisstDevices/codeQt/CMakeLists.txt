#
# $Id$
#
# CMakeLists for cisstDevice
#
# (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstDevicesQt)


# cisstDevicesQt relies on cisstQt settings (for future use)
cisst_library_use_settings (cisstDevicesQt cisstQt)

# set directories for this project
cisst_set_directories (cisstQt)


MACRO (CISST_QT4_WRAP_UI outfiles )
  QT4_EXTRACT_OPTIONS (ui_files ui_options ${ARGN})

  FOREACH (it ${ui_files})
    GET_FILENAME_COMPONENT (outfile ${it} NAME_WE)
    GET_FILENAME_COMPONENT (infile ${it} ABSOLUTE)
# the only thing we modified ...
    SET (outfile ${cisstLibs_BINARY_DIR}/include/cisstDevices/${outfile}.h)
    ADD_CUSTOM_COMMAND (OUTPUT ${outfile}
                        COMMAND ${QT_UIC_EXECUTABLE}
                        ARGS ${ui_options} -o ${outfile} ${infile}
                        MAIN_DEPENDENCY ${infile})
    SET (${outfiles} ${${outfiles}} ${outfile})
  ENDFOREACH (it)
ENDMACRO (CISST_QT4_WRAP_UI)

include_directories (${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp (CISST_DEV_NDISERIAL_QDEVICES_GENERATED
              ../devNDISerialControllerQDevice.h
              ../devNDISerialToolQDevice.h
              )
set (CISST_DEV_NDISERIAL_QWIDGETS_UI
     devNDISerialControllerQWidget.ui
     devNDISerialToolQWidget.ui)
cisst_qt4_wrap_ui (CISST_DEV_NDISERIAL_QWIDGETS_GENERATED
                  ${CISST_DEV_NDISERIAL_QWIDGETS_UI}
                  )

cisst_add_library (LIBRARY cisstDevicesQt
                   LIBRARY_DIR cisstDevices
                   DEPENDENCIES cisstCommon
                                cisstOSAbstraction
                                cisstVector
                                cisstMultiTask
                                cisstParameterTypes
                                cisstRobot
                                cisstDevices
                   SOURCE_FILES devNDISerialControllerQDevice.cpp
                                devNDISerialToolQDevice.cpp
                   HEADER_FILES devNDISerialControllerQDevice.h
                                devNDISerialToolQDevice.h
                                devExportQt.h
                   ADDITIONAL_SOURCE_FILES ${CISST_DEV_NDISERIAL_QDEVICES_GENERATED}
                                           ${CISST_DEV_NDISERIAL_QWIDGETS_GENERATED})

target_link_libraries (cisstDevicesQt ${QT_LIBRARIES})
