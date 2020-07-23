#
# CMakeLists for cisstMesh
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

project (cisstMesh)

# do we need external project for RPLY
option (CISST_MSH_HAS_RPLY "Build RPLY as external project for cisstMesh" OFF)

# all source files
set (SOURCE_FILES
  BoundingBox.cpp
  mshMesh.cpp
  mshPointCloud.cpp
  utilities.cpp
  TriangleClosestPointSolver.cpp
  Ellipsoid_OBB_Intersection_Solver.cpp

  PDTreeNode.cpp
  PDTreeBase.cpp
  PDTree_Mesh.cpp
  PDTree_PointCloud.cpp
  algPDTree.cpp
  algPDTree_CP.cpp
  algPDTree_CP_Mesh.cpp
  algPDTree_CP_PointCloud.cpp
  algPDTree_MLP.cpp
  algPDTree_MLP_Mesh.cpp
  algPDTree_MLP_PointCloud.cpp

  DirPDTreeNode.cpp
  DirPDTreeBase.cpp
  DirPDTree_Mesh.cpp
  DirPDTree_PointCloud.cpp
  algDirPDTree_BoundedAngle.cpp
  algDirPDTree.cpp
  algDirPDTree_BoundedAngle_Mesh.cpp
  algDirPDTree_vonMisesPrj.cpp
  algDirPDTree_BoundedAngle_PointCloud.cpp
  algDirPDTree_vonMisesPrj_Mesh.cpp
  )

# all header files
set (HEADER_FILES
  BoundingBox.h
  mshMesh.h
  mshPointCloud.h
  utilities.h
  TriangleClosestPointSolver.h
  Ellipsoid_OBB_Intersection_Solver.h

  PDTreeNode.h
  PDTreeBase.h
  PDTree_Mesh.h
  PDTree_PointCloud.h
  algPDTree.h
  algPDTree_CP.h
  algPDTree_CP_Mesh.h
  algPDTree_CP_PointCloud.h
  algPDTree_MLP.h
  algPDTree_MLP_Mesh.h
  algPDTree_MLP_PointCloud.h

  DirPDTreeNode.h
  DirPDTreeBase.h
  DirPDTree_Mesh.h
  DirPDTree_PointCloud.h
  algDirPDTree_BoundedAngle.h
  algDirPDTree.h
  algDirPDTree_BoundedAngle_Mesh.h
  algDirPDTree_vonMisesPrj.h
  algDirPDTree_BoundedAngle_PointCloud.h
  algDirPDTree_vonMisesPrj_Mesh.h
  )

# Create the config file
set (CISST_MSH_CONFIG_FILE ${cisst_BINARY_DIR}/include/cisstMesh/mshConfig.h)
configure_file (${cisstMeshLibs_SOURCE_DIR}/mshConfig.h.in
                ${CISST_MSH_CONFIG_FILE}
                @ONLY)
install (FILES ${CISST_MSH_CONFIG_FILE}
         DESTINATION include/cisstMesh
         COMPONENT cisstMesh)
set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CISST_MSH_CONFIG_FILE}")

# Add the config file to the project
set_source_files_properties ("${CISST_MSH_CONFIG_FILE}"
                             PROPERTIES GENERATED TRUE)
set (ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES} ${CISST_MSH_CONFIG_FILE})

# Finally, create main library
cisst_add_library (
  LIBRARY cisstMesh
  FOLDER cisstMesh
  DEPENDENCIES cisstCommon cisstVector cisstNumerical
  SOURCE_FILES ${SOURCE_FILES}
  HEADER_FILES ${HEADER_FILES}
  ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES})
