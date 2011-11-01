# -*- Mode: CMAKE; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set filetype=cmake softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:
#
# $Id$
#
# (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

cmake_minimum_required (VERSION 2.8)

# create a list of libraries needed for this project
set (REQUIRED_CISST_LIBRARIES cisstCommon)

# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} QUIET)

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

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
                  cdgMember.h
                  cdgMember.cpp
                  cdgTypedef.h
                  cdgTypedef.cpp
                  cdgCode.h
                  cdgCode.cpp
                  cdgFile.h
                  cdgFile.cpp
                  )

  cisst_target_link_libraries (cisstDataGenerator ${REQUIRED_CISST_LIBRARIES})

  install (TARGETS cisstDataGenerator
           COMPONENT cisstCommon
           RUNTIME DESTINATION bin)

else (cisst_FOUND_AS_REQUIRED)
  cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
endif (cisst_FOUND_AS_REQUIRED)
