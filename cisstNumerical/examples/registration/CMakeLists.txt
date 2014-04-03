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

set (REQUIRED_CISST_LIBRARIES cisstCommon cisstVector cisstNumerical)
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)
  include (${CISST_USE_FILE})

  if (CISST_HAS_CISSTNETLIB)
    add_executable (nmrExRegistrationAccuracy Accuracy.cpp DataSet.cpp DataSet.h)
    set_property (TARGET nmrExRegistrationAccuracy PROPERTY FOLDER "cisstNumerical/examples")
    cisst_target_link_libraries (nmrExRegistrationAccuracy ${REQUIRED_CISST_LIBRARIES})

    add_executable (nmrExRegistrationDistances Distances.cpp DataSet.cpp DataSet.h)
    set_property (TARGET nmrExRegistrationDistances PROPERTY FOLDER "cisstNumerical/examples")
    cisst_target_link_libraries (nmrExRegistrationDistances ${REQUIRED_CISST_LIBRARIES})

  else (CISST_HAS_CISSTNETLIB)
    message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires CISST_HAS_CISSTNETLIB")
  endif (CISST_HAS_CISSTNETLIB)

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
