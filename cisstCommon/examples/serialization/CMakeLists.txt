#
# (C) Copyright 2007-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

find_package (cisst COMPONENTS cisstCommon)

if (cisst_FOUND)

  add_library (cmnExSerializationClasses STATIC classes.h classes.cpp)
  set_property (TARGET cmnExSerializationClasses PROPERTY FOLDER "cisstCommon/examples")
  target_link_libraries (cmnExSerializationClasses PRIVATE cisst::cisstCommon)

  add_executable (cmnExSerializationWrite write.cpp)
  set_property (TARGET cmnExSerializationWrite PROPERTY FOLDER "cisstCommon/examples")
  target_link_libraries (cmnExSerializationWrite cisst::cisstCommon cmnExSerializationClasses)

  add_executable (cmnExSerializationRead read.cpp)
  set_property (TARGET cmnExSerializationRead PROPERTY FOLDER "cisstCommon/examples")
  target_link_libraries (cmnExSerializationRead cisst::cisstCommon cmnExSerializationClasses)

  add_executable (cmnExSerializationStringStream writeStringStream.cpp)
  set_property (TARGET cmnExSerializationStringStream PROPERTY FOLDER "cisstCommon/examples")
  target_link_libraries (cmnExSerializationStringStream cisst::cisstCommon cmnExSerializationClasses)

else (cisst_FOUND)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires cisstCommon")
endif (cisst_FOUND)
