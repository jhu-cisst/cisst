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

# set the library name
SET(LIBRARY cisstCommon)

# all source files
SET(SOURCE_FILES
    cmnClassRegister.cpp
    cmnDeSerializer.cpp
    cmnGenericObject.cpp
    cmnGenericObjectProxy.cpp
    cmnGetChar.cpp
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
SET(HEADER_FILES
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

# Conditional inclusion of cmnXMLPath in the cisstCommon project.
IF(CISST_HAS_XML)
  SET(SOURCE_FILES ${SOURCE_FILES} cmnXMLPath.cpp)
  SET(HEADER_FILES ${HEADER_FILES} cmnXMLPath.h)
ENDIF(CISST_HAS_XML)

# Qt widgets
if(CISST_HAS_QT)
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  set(SOURCE_FILES
      ${SOURCE_FILES}
      cmnLoggerQWidget.cpp)
  set(HEADER_FILES
      ${HEADER_FILES}
      cmnLoggerQWidget.h)
endif(CISST_HAS_QT)

CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

