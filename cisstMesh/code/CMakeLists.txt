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

# cisstNetlib is required
if (NOT CISST_HAS_CISSTNETLIB)
  message (SEND_ERROR "cisstMesh requires cisstNumerical compiled with cisstNetlib")
endif ()

# do we need external project for RPLY -- this is not implemented
option (CISST_MSH_HAS_RPLY "Build RPLY as external project for cisstMesh" OFF)
mark_as_advanced (CISST_MSH_HAS_RPLY)

# all source files
set (SOURCE_FILES
  msh2Utilities.cpp
  msh2BoundingBox.cpp
  msh2Edges.cpp
  msh2PointCloud.cpp
  PointProjectionRoutines.cpp

  msh2DirPDTreeBase.cpp
  msh2DirPDTreeEdges.cpp
  msh2DirPDTreeNode.cpp
  msh2DirPDTreePointCloud.cpp
  msh2AlgDirPDTreeCP.cpp
  msh2AlgDirPDTreeCPEdges.cpp
  msh2AlgDirPDTree.cpp
  msh2AlgDirPDTreeCPPointCloud.cpp
  msh2AlgDirPDTreevonMises.cpp
  msh2AlgDirPDTreevonMisesEdges.cpp
  msh2AlgDirPDTreevonMisesPointCloud.cpp

  msh3Mesh.cpp
  msh3PointCloud.cpp
  msh3Utilities.cpp
  TriangleClosestPointSolver.cpp
  EllipsoidOBBIntersectionSolver.cpp

  msh3PDTreeNode.cpp
  msh3PDTreeBase.cpp
  msh3PDTreeMesh.cpp
  msh3PDTreePointCloud.cpp
  msh3AlgPDTree.cpp
  msh3AlgPDTreeCP.cpp
  msh3AlgPDTreeCPMesh.cpp
  msh3AlgPDTreeCPPointCloud.cpp
  msh3AlgPDTreeMLP.cpp
  msh3AlgPDTreeMLPMesh.cpp
  msh3AlgPDTreeMLPPointCloud.cpp

  msh3DirPDTreeNode.cpp
  msh3DirPDTreeBase.cpp
  msh3DirPDTreeMesh.cpp
  msh3DirPDTreePointCloud.cpp
  msh3AlgDirPDTreeBA.cpp
  msh3AlgDirPDTree.cpp
  msh3AlgDirPDTreeBAMesh.cpp
  msh3AlgDirPDTreevonMisesProj.cpp
  msh3AlgDirPDTreeBAPointCloud.cpp
  msh3AlgDirPDTreevonMisesProjMesh.cpp
  )

# all header files
set (HEADER_FILES
  msh2Utilities.h
  msh2BoundingBox.h
  msh2Edge.h
  msh2Edges.h
  msh2PointCloud.h
  PointProjectionRoutines.h

  msh2DirPDTreeBase.h
  msh2DirPDTreeEdges.h
  msh2DirPDTreeNode.h
  msh2DirPDTreePointCloud.h

  msh2AlgDirPDTreeCPEdges.h
  msh2AlgDirPDTreeCP.h
  msh2AlgDirPDTreeCPPointCloud.h
  msh2AlgDirPDTree.h
  msh2AlgDirPDTreevonMisesEdges.h
  msh2AlgDirPDTreevonMises.h
  msh2AlgDirPDTreevonMisesPointCloud.h

  msh3BoundingBox.h
  msh3Mesh.h
  msh3PointCloud.h
  msh3Utilities.h
  TriangleClosestPointSolver.h
  EllipsoidOBBIntersectionSolver.h

  msh3PDTreeNode.h
  msh3PDTreeBase.h
  msh3PDTreeMesh.h
  msh3PDTreePointCloud.h
  msh3AlgPDTree.h
  msh3AlgPDTreeCP.h
  msh3AlgPDTreeCPMesh.h
  msh3AlgPDTreeCPPointCloud.h
  msh3AlgPDTreeMLP.h
  msh3AlgPDTreeMLPMesh.h
  msh3AlgPDTreeMLPPointCloud.h

  msh3DirPDTreeNode.h
  msh3DirPDTreeBase.h
  msh3DirPDTreeMesh.h
  msh3DirPDTreePointCloud.h
  msh3AlgDirPDTreeBA.h
  msh3AlgDirPDTree.h
  msh3AlgDirPDTreeBAMesh.h
  msh3AlgDirPDTreevonMisesProj.h
  msh3AlgDirPDTreeBAPointCloud.h
  msh3AlgDirPDTreevonMisesProjMesh.h

  mshExport.h
  mshForwardDeclarations.h
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
