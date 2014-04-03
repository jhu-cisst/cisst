#
#
# (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights
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

  # create data type using the data generator
  cisst_data_generator (cmnExDataGenerator
                        ${CMAKE_CURRENT_BINARY_DIR}
                        ""
                        demoData.cdg)

  # to compile cisst generated code, need to find header file
  include_directories (${CMAKE_CURRENT_BINARY_DIR})

  add_executable (cmnExDataGenerator
                  ${cmnExDataGenerator_CISST_DG_SRCS}
                  main.cpp)
  set_property (TARGET cmnExDataGenerator PROPERTY FOLDER "cisstCommon/examples")
  cisst_target_link_libraries (cmnExDataGenerator cisstCommon)
else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires cisstCommon")
endif (cisst_FOUND_AS_REQUIRED)
