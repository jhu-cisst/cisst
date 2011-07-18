#
# $Id$
#
# CMakeLists for cisstCommon
#
# (C) Copyright 2003-2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


# all source files
set(SOURCE_FILES
    cmnClassRegister.cpp
    cmnClassServicesBase.cpp
    cmnClassServices.cpp
    cmnDeSerializer.cpp
    cmnGenericObject.cpp
    cmnGetChar.cpp
    cmnLogLoD.cpp
    cmnLogger.cpp
    cmnObjectRegister.cpp
    cmnOutputMultiplexer.cpp
    cmnPortability.cpp
    cmnRandomSequence.cpp
    cmnSerializer.cpp
    cmnStreamRawParser.cpp
    cmnPath.cpp
    cmnPrintf.cpp
    cmnTokenizer.cpp
    cmnTypeTraits.cpp
    )

# all header files
set(HEADER_FILES
    cmnAccessorMacros.h
    cmnAssert.h
    cmnCallbackStreambuf.h
    cmnClassRegister.h
    cmnClassRegisterMacros.h
    cmnClassServicesBase.h
    cmnClassServices.h
    cmnConstants.h
    cmnDeSerializer.h
    cmnExport.h
    cmnExportMacros.h
    cmnForwardDeclarations.h
    cmnGenericObject.h
    cmnGenericObjectProxy.h
    cmnGetChar.h
    cmnLogLoD.h
    cmnLogger.h
    cmnLODMultiplexerStreambuf.h
    cmnLODOutputMultiplexer.h
    cmnMultiplexerStreambuf.h
    cmnMultiplexerStreambufProxy.h
    cmnNamedMap.h
    cmnObjectRegister.h
    cmnOutputMultiplexer.h
    cmnPortability.h
    cmnPrintf.h
    cmnRandomSequence.h
    cmnRequiresDeepCopy.h
    cmnSerializer.h
    cmnStreamRawParser.h
    cmnStrings.h
    cmnPath.h
    cmnThrow.h
    cmnTokenizer.h
    cmnTypeTraits.h
    cmnUnits.h
    )

cisst_add_library (
  LIBRARY cisstCommon
  SOURCE_FILES ${SOURCE_FILES}
  HEADER_FILES ${HEADER_FILES})

# XML Library
if (CISST_HAS_XML)
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
endif (CISST_HAS_XML)

# Qt widgets
if (CISST_HAS_QT)
  cisst_add_library (
    LIBRARY cisstCommonQt
    LIBRARY_DIR cisstCommon
    DEPENDENCIES cisstCommon
    SOURCE_FILES cmnClassServicesQt.cpp
                 cmnLoggerQtWidget.cpp
    HEADER_FILES cmnExportQt.h
                 cmnLoggerQtWidget.h)
  target_link_libraries (cisstCommonQt ${QT_LIBRARIES})
endif (CISST_HAS_QT)

# Python wrappers
if (CISST_HAS_SWIG_PYTHON)
  cisst_add_swig_module(MODULE cisstCommon
                        INTERFACE_DIRECTORY ../../include/cisstCommon
			CISST_LIBRARIES cisstCommon)
endif (CISST_HAS_SWIG_PYTHON)
