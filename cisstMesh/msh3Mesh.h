/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// ****************************************************************************
//
//    Copyright (c) 2015, Seth Billings, Russell Taylor, Johns Hopkins University
//    All rights reserved.
//
//    Redistribution and use in source and binary forms, with or without
//    modification, are permitted provided that the following conditions are
//    met:
//
//    1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//    3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
//    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ****************************************************************************

#ifndef _msh3Mesh_h_
#define _msh3Mesh_h_

#include <cisstMesh/mshConfig.h>

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctFixedSizeMatrixTypes.h>
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>

#if CISST_MSH_HAS_RPLY
  #include <ply_io.h>
#endif

#include <unordered_map>

// Always include last!
#include <cisstMesh/mshExport.h>

class CISST_EXPORT msh3Mesh
{

public:

    //--- Variables ---//
    vctDynamicVector<vct3>      vertices;       // the coordinates for each vertex in the mesh
    vctDynamicVector<vctInt3>   faces;          // the vertex indices for each triangle in the mesh
    vctDynamicVector<vct3>      faceNormals;    // the face normal for each triangle in the mesh

    // optional mesh properties
    vctDynamicVector<vct3>      vertexNormals;  // a normal orientation associated with each vertex
    vctDynamicVector<vctInt3>   faceNeighbors;  // the face indices for the neighbors of each triangle

    // for mesh constraint
    vctDynamicVector<vct3>      closestPoint;  // the Eculidean location of the closest point
    vctDynamicVector<int>       cpLocation;  // the location of the closest point defined by enum (edge, vertex or inside)
    vctDynamicVector<vct3>      activeNormal;  // the active normal of the mesh after merging
    vctDynamicVector<double>    distance;

    // mesh noise model
    //  NOTE: if used, this must be set manually by the user AFTER loading the mesh file
    //        (defaults to all zeroes, i.e. zero measurement noise on the mesh)
    vctDynamicVector<vct3x3>  TriangleCov;        // triangle covariances
    vctDynamicVector<vct3>    TriangleCovEig;     // triangle covariance eigenvalues (in decreasing size)


    //--- Methods ---//

    // constructor
    msh3Mesh(double robotResolution, bool convertMMtoM); //robot resolution in mm
    msh3Mesh(bool convertMMtoM);
    msh3Mesh(double robotResolution);
    msh3Mesh();

    // destructor
    ~msh3Mesh() {}

    // initializes all mesh properties to empty (default initializer);
    //  this is a useful routine to use while building a mesh,
    //  since some mesh properties are optional and may not be
    //  initialized by the data used to build the mesh; calling this
    //  ensures that unused properties are emptied rather than left with
    //  possibly invalid values
    void ResetMesh();

    inline size_t NumVertices(void) const { return vertices.size(); }
    inline size_t NumTriangles(void) const { return faces.size(); }

    // initializes triangle noise models to zero (default initializer)
    void InitializeNoiseModel();

    // computes noise model covariances for each triangle in the mesh
    //  such that the in-plane and perpendicular-plane directions have
    //  the specified variance
    void InitializeNoiseModel(double noiseInPlaneVar, double noisePerpPlaneVar);

    void SaveTriangleCovariances(std::string &filePath);

    // get coordinates of all three vertices for a given face index
    inline void FaceCoords(int ti, vct3 &v0, vct3 &v1, vct3 &v2) const
    {
        v0 = vertices[faces[ti][0]];
        v1 = vertices[faces[ti][1]];
        v2 = vertices[faces[ti][2]];
    }
    // get vertex coordinate for a given face/vertex index
    inline vct3& FaceCoord(int ti, int vi)
    {
        return vertices[faces[ti][vi]];
    }

    // Mesh I/O
public:

    // Build mesh from data arrays
    int LoadMesh(const vctDynamicVector<vct3> *vertices,
      const vctDynamicVector<vctInt3> *faces,
      const vctDynamicVector<vct3> *face_normals = NULL,
      const vctDynamicVector<vctInt3> *face_neighbors = NULL,
      const vctDynamicVector<vct3> *vertex_normals = NULL
      );

    // Load mesh from PLY file
    void LoadPLY(const std::string &input_file);

    // Save mesh to PLY fle
    void SavePLY(const std::string &output_file);

    // Build mesh from an array of vertices, faces, and face normals
    int LoadMesh(const vctDynamicVector<vct3> &V,
            const vctDynamicVector<vctInt3> &T,
            const vctDynamicVector<vct3> &N);

    // Build new mesh from a single .mesh file
    int LoadMeshFile(const std::string &meshFilePath);

    // Build new mesh from multiple .mesh files
    int LoadMeshFileMultiple(const std::vector<std::string> &meshFilePaths);

    // Save mesh to .mesh file
    int SaveMeshFile(const std::string &filePath);

    // Build new mesh from .stl file
    // Assumes model have unit of mm
    int LoadMeshFromSTLFile(const std::string &stlFilePath);

    enum VertexIndex { V1 = 1, V2 = 2, V3 = 3, V1V2 = 4, V1V3 = 5, V2V3 = 6, IN = -1, VOID=0}; // avoid 0
    int edgeOffset = 4;
    int vertexOffset = 1;
    // merge edge points to approximate local geometry
    void MergeEdgePoint(std::vector<int>& faceIdx,
                        vct3& target // the target location that used to test against mesh model
                        );
    // reset mesh constraint related values
    void ResetMeshConstraintValues();
    void TransformTriangle(const vctFrm4x4 & transformation);
private:

    // Load .mesh file, adding it to the current mesh while preserving all
    //  data currently existing in the mesh
    int AddMeshFile(const std::string &meshFilePath);

    // helper function for finding neighboring triangle when loading STL file
    void FindFaceNeighbor(int faceIdx, std::unordered_multimap<std::string, int>& map);
    // helper function for vertex index to string conversion, e.g. triangle of vertex [12,3,4],
    // input (0,1,triangle) will return edge01 (vertex 12 and 3) of key "3-12"
    // vertex1 and 2 are enums V1,V2,V3 from VertexIndex
    std::string VertexToKey(const int& vertex1, const int& vertex2, const vctInt3& triangle);

    // check for convexity
    // if the dot product of
    // 1. vector pointing from the common edge to the other vertex
    // 2. normal of neighbor plane
    // is positive, then local shape is convex
    inline bool CheckConvexity(int idx, int idxNeighbor) {
        vct3 vec1 = {0.0, 0.0, 0.0}, vec2 = {0.0, 0.0, 0.0};
        switch (cpLocation.at(idx)){
            case V1V2:
                vec1 = vertices.at(faces.at(idx)[V3-vertexOffset])-vertices.at(faces.at(idx)[V1-vertexOffset]);
                vec2 = vertices.at(faces.at(idx)[V3-vertexOffset])-vertices.at(faces.at(idx)[V2-vertexOffset]);
                break;
            case V1V3:
                vec1 = vertices.at(faces.at(idx)[V2-vertexOffset])-vertices.at(faces.at(idx)[V1-vertexOffset]);
                vec2 = vertices.at(faces.at(idx)[V2-vertexOffset])-vertices.at(faces.at(idx)[V3-vertexOffset]);
                break;
            case V2V3:
                vec1 = vertices.at(faces.at(idx)[V1-vertexOffset])-vertices.at(faces.at(idx)[V2-vertexOffset]);
                vec2 = vertices.at(faces.at(idx)[V1-vertexOffset])-vertices.at(faces.at(idx)[V3-vertexOffset]);
                break;
            case V1:
                vec1 = vertices.at(faces.at(idx)[V2-vertexOffset])-vertices.at(faces.at(idx)[V1-vertexOffset]);
                vec2 = vertices.at(faces.at(idx)[V3-vertexOffset])-vertices.at(faces.at(idx)[V1-vertexOffset]);
                break;
            case V2:
                vec1 = vertices.at(faces.at(idx)[V1-vertexOffset])-vertices.at(faces.at(idx)[V2-vertexOffset]);
                vec2 = vertices.at(faces.at(idx)[V3-vertexOffset])-vertices.at(faces.at(idx)[V2-vertexOffset]);
                break;
            case V3:
                vec1 = vertices.at(faces.at(idx)[V1-vertexOffset])-vertices.at(faces.at(idx)[V3-vertexOffset]);
                vec2 = vertices.at(faces.at(idx)[V2-vertexOffset])-vertices.at(faces.at(idx)[V3-vertexOffset]);
                break;
        }
        if (vec1.DotProduct(faceNormals.at(idxNeighbor)) > 0.0 || vec2.DotProduct(faceNormals.at(idxNeighbor)) > 0.0){ // in case of perpendicularity
            return true;
        }
        if (vec1.DotProduct(faceNormals.at(idxNeighbor)) < 0.0 || vec2.DotProduct(faceNormals.at(idxNeighbor)) < 0.0){
            return false;
        }
        return false;
    }

    inline bool CheckConcavity(int idx, int idxNeighbor) {
        return !CheckConvexity(idx, idxNeighbor);
    }
    // check side of normal
    inline bool CheckNormal(const vct3 & target, const int idx, double tolerance) {
        if ((target-closestPoint.at(idx)).DotProduct(faceNormals.at(idx)) >= -tolerance){
            return true;
        }
        else {
            return false;
        }
    }

#if CISST_MSH_HAS_RPLY
    ply_io ply_obj;
#endif
    double mMeshResolution;
    double mRobotResolution;
    bool mConvertMMtoM;

public:

    // Legacy Mesh I/O
    void ReadMeshFile(const char *fn);
    void ReadMeshMeshFile( const std::string &meshFilePath );
    void ReadSURMeshFile(const char *fn);
    void ReadSFCMeshFile(const char *fn);
    void WriteMeshFile(const char *fn);
    void WriteSURMeshFile(const char *fn);
};

#endif // _cisstMesh_h_
