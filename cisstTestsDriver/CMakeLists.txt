#
#

# (C) Copyright 2003-2013 Johns Hopkins University (JHU), All Rights
# Reserved.

# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstTestsDriver)

include_directories (${CPPUNIT_INCLUDE_DIR})

# Force static link for this library
set (IS_SHARED STATIC)

cisst_add_library (LIBRARY cisstTestsDriver
                   SOURCE_FILES cisstTestMain.cpp
                                cisstTestParameters.cpp
                                cisstTestParameters.h)

set_property (TARGET cisstTestsDriver PROPERTY FOLDER "cisstCommon/tests")

target_link_libraries (cisstTestsDriver
                       optimized ${CPPUNIT_LIBRARIES}
                       debug ${CPPUNIT_DEBUG_LIBRARIES})

# add dependency to external project if needed
if (TARGET cisstCppUnit)
   add_dependencies (cisstTestsDriver cisstCppUnit)
endif (TARGET cisstCppUnit)
