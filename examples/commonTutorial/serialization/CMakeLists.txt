#
# $Id$
#
# (C) Copyright 2007-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

find_package (cisst COMPONENTS cisstCommon)

if (cisst_FOUND_AS_REQUIRED)
  include (${CISST_USE_FILE})

  add_library (serializationClasses STATIC classes.h classes.cpp)

  add_executable (commonTutorialSerializationWrite write.cpp)
  cisst_target_link_libraries (commonTutorialSerializationWrite cisstCommon)
  target_link_libraries (commonTutorialSerializationWrite serializationClasses)

  add_executable (commonTutorialSerializationRead read.cpp)
  cisst_target_link_libraries (commonTutorialSerializationRead cisstCommon)
  target_link_libraries (commonTutorialSerializationRead serializationClasses)
else (cisst_FOUND_AS_REQUIRED)
  message ("Example in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires cisstCommon")
endif (cisst_FOUND_AS_REQUIRED)
