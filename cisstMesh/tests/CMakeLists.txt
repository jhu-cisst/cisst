#
# CMakeLists for cisstMesh tests
#
# (C) Copyright 2006-2013 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# paths for headers/libraries
cisst_set_directories (cisstCommon cisstVector cisstNumerical cisstMesh cisstTestsDriver)

# all source files
set (SOURCE_FILES
  mshAlgPDTreeCPMeshTest.cpp
  )

# all header files
set (HEADER_FILES
  mshAlgPDTreeCPMeshTest.h
  )

add_executable (cisstMeshTests ${SOURCE_FILES} ${HEADER_FILES})
set_property (TARGET cisstMeshTests PROPERTY FOLDER "cisstMesh/tests")
target_link_libraries (cisstMeshTests cisstTestsDriver )
cisst_target_link_libraries (cisstMeshTests cisstCommon cisstMesh cisstTestsDriver)

# to generate a CTest list of tests
cisst_add_test (cisstMeshTests INSTANCES 2 ITERATIONS 2)

# To find generated executables
set_target_properties (cisstMeshTests
                       PROPERTIES COMPILE_DEFINITIONS "CISST_BINARY_DIR=\"${cisst_BINARY_DIR}/\"")
