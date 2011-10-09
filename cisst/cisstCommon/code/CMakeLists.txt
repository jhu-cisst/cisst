#
# $Id$
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

project (cisstCommon)

# all source files
set (SOURCE_FILES
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
set (HEADER_FILES
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
