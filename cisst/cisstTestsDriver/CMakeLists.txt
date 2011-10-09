#
# $Id$
#

# (C) Copyright 2003-2011 Johns Hopkins University (JHU), All Rights
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

add_library (cisstTestsDriver STATIC
             cisstTestMain.cpp
             cisstTestParameters.cpp
             cisstTestParameters.h)

target_link_libraries (cisstTestsDriver
                       optimized ${CPPUNIT_LIBRARIES}
                       debug ${CPPUNIT_DEBUG_LIBRARIES})
