#
# (C) Copyright 2020 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# All cisstMesh libraries
project (cisstMeshLibs)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory (code)

cisst_offer_python (cisstMesh)
cisst_offer_examples (cisstMesh)
cisst_offer_tests (cisstMesh)
