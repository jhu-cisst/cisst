#
# CMakeLists for cisstVector tests
#
# (C) Copyright 2003-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

if (CISST_HAS_OPENGL OR CISST_HAS_VTK)
  set (HEADER_FILES
       vctPlot2DBaseTest.h)
  set (SOURCE_FILES
       vctPlot2DBaseTest.cpp)
endif (CISST_HAS_OPENGL OR CISST_HAS_VTK)

# Add executable for C++ tests
add_executable (cisstVectorQtTests ${SOURCE_FILES} ${HEADER_FILES})
set_property (TARGET cisstVectorQtTests PROPERTY FOLDER "cisstVector/tests")
target_link_libraries (cisstVectorQtTests
  cisstTestsDriver
  cisstCommon
  cisstVector)

# To generate a CTest list of tests
cisst_add_test (cisstVectorQtTests ITERATIONS 10 INSTANCES 2)
