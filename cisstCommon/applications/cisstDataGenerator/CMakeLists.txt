# -*- Mode: CMAKE; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set filetype=cmake softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:
#
# (C) Copyright 2010-2015 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# this application is needed for cisst and is always compiled along
# the cisst libraries.  It shouldn't use the usual find_package (cisst ...)
set (REQUIRED_CISST_LIBRARIES
     cisstCommon)

cisst_set_directories (${REQUIRED_CISST_LIBRARIES})
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# we intentionally don't link against cisstCommon to make sure we are
# not hunting for a dll or shared library during the build process.
# Instead we explicitely include the symbols we need.

# to avoid JSON dependency in cmnGenericObject
add_definitions (-DCISST_SKIP_JSON)

set (SOURCE_FILES_FROM_cisstCommon
     ${cisstCommonLibs_SOURCE_DIR}/code/cmnClassRegister.cpp
     ${cisstCommonLibs_SOURCE_DIR}/code/cmnClassServicesBase.cpp
     ${cisstCommonLibs_SOURCE_DIR}/code/cmnCommandLineOptions.cpp
     ${cisstCommonLibs_SOURCE_DIR}/code/cmnGenericObject.cpp
     ${cisstCommonLibs_SOURCE_DIR}/code/cmnLogger.cpp
     ${cisstCommonLibs_SOURCE_DIR}/code/cmnLogLoD.cpp
     ${cisstCommonLibs_SOURCE_DIR}/code/cmnPath.cpp
     ${cisstCommonLibs_SOURCE_DIR}/code/cmnTokenizer.cpp
     ${cisstCommonLibs_SOURCE_DIR}/code/cmnClassServices.cpp
     ${cisstCommonLibs_SOURCE_DIR}/code/cmnSerializer.cpp
     ${cisstCommonLibs_SOURCE_DIR}/code/cmnDeSerializer.cpp
     )

# make sure no cisst symbols is considered "imported" by declspec
if (WIN32)
  remove_definitions (-DCISST_DLL)
endif (WIN32)

# name the main executable and specifies with source files to use
add_executable (cisstDataGenerator
                cisstDataGenerator.cpp
                cdgField.h
                cdgField.cpp
                cdgScope.h
                cdgScope.cpp
                cdgGlobal.h
                cdgGlobal.cpp
                cdgClass.h
                cdgClass.cpp
                cdgBaseClass.h
                cdgBaseClass.cpp
                cdgMember.h
                cdgMember.cpp
                cdgEnum.h
                cdgEnum.cpp
                cdgEnumValue.h
                cdgEnumValue.cpp
                cdgTypedef.h
                cdgTypedef.cpp
                cdgInline.h
                cdgInline.cpp
                cdgFile.h
                cdgFile.cpp
                ${SOURCE_FILES_FROM_cisstCommon}
                )

add_dependencies (cisstDataGenerator cisstRevision cisstBuildType)

set_property (TARGET cisstDataGenerator PROPERTY FOLDER "cisstCommon/applications")
add_dependencies (cisstDataGenerator cisstRevision)

install (TARGETS cisstDataGenerator
         COMPONENT cisstCommon
         RUNTIME DESTINATION bin)
