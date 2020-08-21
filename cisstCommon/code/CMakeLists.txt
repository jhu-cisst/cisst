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

project (cisstCommon)

# all source files
set (SOURCE_FILES
     cmnClassRegister.cpp
     cmnClassServicesBase.cpp
     cmnClassServices.cpp
     cmnCommandLineOptions.cpp
     cmnDataFunctions.cpp
     cmnDataFunctionsString.cpp
     cmnDataFormat.cpp
     cmnDeSerializer.cpp
     cmnGenericObject.cpp
     cmnGetChar.cpp
     cmnKbHit.cpp
     cmnLogLoD.cpp
     cmnLogger.cpp
     cmnObjectRegister.cpp
     cmnOutputMultiplexer.cpp
     cmnPortability.cpp
     cmnRandomSequence.cpp
     cmnSerializer.cpp
     cmnStreamRawParser.cpp
     cmnStrings.cpp
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
     cmnCommandLineOptions.h
     cmnConstants.h
     cmnDataFunctions.h
     cmnDataFunctionsEnumMacros.h
     cmnDataFunctionsMacros.h
     cmnDataFunctionsString.h
     cmnDataFunctionsVector.h
     cmnDataFunctionsArray.h
     cmnDataFunctionsList.h
     cmnDataFunctionsVectorHelpers.h
     cmnDataFunctionsMatrixHelpers.h
     cmnDataFormat.h
     cmnDeSerializer.h
     cmnExport.h
     cmnExportMacros.h
     cmnForwardDeclarations.h
     cmnGenericObject.h
     cmnGenericObjectProxy.h
     cmnGetChar.h
     cmnKbHit.h
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

if (CISST_HAS_JSON)
  set (SOURCE_FILES
       ${SOURCE_FILES}
       cmnDataFunctionsJSON.cpp)
  set (HEADER_FILES
       ${HEADER_FILES}
       cmnDataFunctionsJSON.h)
endif (CISST_HAS_JSON)

cisst_add_library (
  LIBRARY cisstCommon
  FOLDER cisstCommon
  SOURCE_FILES ${SOURCE_FILES}
  HEADER_FILES ${HEADER_FILES})

add_dependencies (cisstCommon cisstRevision cisstBuildType)

if (CISST_HAS_JSON)
  if (NOT JSON_USE_SYSTEM_VERSION)
    add_dependencies (cisstCommon cisstJSONExternal)
  endif ()
endif ()
