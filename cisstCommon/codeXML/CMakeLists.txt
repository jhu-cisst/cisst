#
# (C) Copyright 2003-2020 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstCommonXML)

# try to determine which lib to use for XML support
# we favor libxml2, it has write support and doesn't require a QApplication
if (CISST_FOUND_LIBXML2_XML)
  set (CISST_XML_LIB "LibXml2" CACHE STRING "Library used to support XML Path, can be either \"LibXml2\" or \"QtXML\"")
else (CISST_FOUND_LIBXML2_XML)
  if (CISST_FOUND_QT_XML)
    set (CISST_XML_LIB "QtXML" CACHE STRING "Library used to support XML Path, can be either \"LibXml2\" or \"QtXML\"")
  else (CISST_FOUND_QT_XML)
    set (CISST_XML_LIB "Either LibXml2 or QtXML" CACHE STRING "Library used to support XML Path, can be either \"LibXml2\" or \"QtXML\"")
  endif (CISST_FOUND_QT_XML)
endif (CISST_FOUND_LIBXML2_XML)

# now, based on user preference create the library
if (${CISST_XML_LIB} STREQUAL "LibXml2")
  find_package (LibXml2 REQUIRED)
  cisst_set_package_settings (cisstCommonXML LibXml2 LIBRARIES LIBXML2_LIBRARIES)
  include_directories (${LIBXML2_INCLUDE_DIR})
  cisst_library_remove_settings (cisstCommonXML cisstQt)
  set (CISST_HAS_QT_XML 0) # for cmnConfigXML.h
else (${CISST_XML_LIB} STREQUAL "LibXml2")
  cisst_unset_all_package_settings (cisstCommonXML LibXml2)
  if (${CISST_XML_LIB} STREQUAL "QtXML")
    # cisstCommonXML relies on cisstQt settings (for future use)
    cisst_library_use_settings (cisstCommonXML cisstQt)
    cisst_set_directories (cisstQt)
    set (CISST_HAS_QT_XML 1) # for cmnConfigXML.h
  else (${CISST_XML_LIB} STREQUAL "QtXML")
    message (SEND_ERROR "CISST_XML_LIB must be set to either \"LibXml2\" or \"QtXML\" to compile cisstCommonXML.  You can also turn off CISST_CommonXML")
    set (CISST_HAS_QT_XML 0) # for cmnConfigXML.h
  endif (${CISST_XML_LIB} STREQUAL "QtXML")
endif (${CISST_XML_LIB} STREQUAL "LibXml2")

# Create the config file
set (CISST_CMN_XML_CONFIG_FILE ${cisst_BINARY_DIR}/include/cisstCommon/cmnConfigXML.h)
configure_file (${cisstCommonLibs_SOURCE_DIR}/cmnConfigXML.h.in
                ${CISST_CMN_XML_CONFIG_FILE}
                @ONLY)
install (FILES ${CISST_CMN_XML_CONFIG_FILE}
         DESTINATION include/cisstCommon
         COMPONENT cisstCommonXML)
set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CISST_CMN_XML_CONFIG_FILE}")

# Add the config file to the project
set_source_files_properties ("${CISST_CMN_XML_CONFIG_FILE}"
                             PROPERTIES GENERATED TRUE)

# link against Qt libraries if needed
if (${CISST_XML_LIB} STREQUAL "QtXML")
  set (cisstCommonXML_EXTRA_DEP cisstCommonQt)
endif ()

cisst_add_library (LIBRARY cisstCommonXML
                   LIBRARY_DIR cisstCommon
                   FOLDER cisstCommon
                   DEPENDENCIES cisstCommon ${cisstCommonXML_EXTRA_DEP}
                   SOURCE_FILES cmnXMLPath.cpp
                   HEADER_FILES cmnExportXML.h cmnXMLPath.h
                   ADDITIONAL_HEADER_FILES ${CISST_CMN_XML_CONFIG_FILE})

# Add libxml2 runtime dependencies (so far, only tested on WIN32).
# There must be a better way to do this.
# Also, zlib1.dll is also needed by cisstStereoVision (gnuwin32 package) --
# which one should be used?
if (WIN32 AND ${CISST_XML_LIB} STREQUAL "LibXml2")
  # Assume that xmllint.exe was found and DLLs are in that directory
  get_filename_component (LIBXML2_BIN ${LIBXML2_XMLLINT_EXECUTABLE} PATH)
  install (FILES ${LIBXML2_BIN}/libxml2.dll ${LIBXML2_BIN}/iconv.dll ${LIBXML2_BIN}/zlib1.dll
           DESTINATION bin
           COMPONENT cisstCommonXML)
endif (WIN32 AND ${CISST_XML_LIB} STREQUAL "LibXml2")
