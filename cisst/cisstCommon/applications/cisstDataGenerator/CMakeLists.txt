# -*- Mode: CMAKE; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set filetype=cmake softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:
#
# $Id$
#
# (C) Copyright 2010-2012 Johns Hopkins University (JHU), All Rights
# Reserved.
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


# name the main executable and specifies with source files to use
add_executable (cisstDataGenerator
                cisstDataGenerator.cpp
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
                cdgTypedef.h
                cdgTypedef.cpp
                cdgInline.h
                cdgInline.cpp
                cdgFile.h
                cdgFile.cpp
                )

set_property (TARGET cisstDataGenerator PROPERTY FOLDER "cisstCommon/applications")
cisst_target_link_libraries (cisstDataGenerator ${REQUIRED_CISST_LIBRARIES})

install (TARGETS cisstDataGenerator
         COMPONENT cisstCommon
         RUNTIME DESTINATION bin)
