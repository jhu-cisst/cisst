#
# $Id$
#
# CMakeLists for cisstCommon
#
# (C) Copyright 2003-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstCommonXML)

set (DEPENDENCIES cisstCommon)
if (CISST_HAS_LIBXML2_XML)
  include_directories (${LIBXML2_INCLUDE_DIR})
else (CISST_HAS_LIBXML2_XML)
  set (DEPENDENCIES ${DEPENDENCIES} cisstCommonQt)
endif (CISST_HAS_LIBXML2_XML)

cisst_add_library (
  LIBRARY cisstCommonXML
  LIBRARY_DIR cisstCommon
  DEPENDENCIES ${DEPENDENCIES}
  SOURCE_FILES cmnXMLPath.cpp
  HEADER_FILES cmnExportXML.h
               cmnXMLPath.h
)
