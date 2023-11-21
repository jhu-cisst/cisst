/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// ****************************************************************************
//
//    Copyright (c) 2020, Seth Billings, Russell Taylor & Max Zhaoshuo Li, Johns Hopkins University
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

#include <cisstMesh/msh3Mesh.h>
#include <cisstMesh/msh3Utilities.h>

#include <cisstNumerical/nmrLSSolver.h>

#include <assert.h>
#include <string>
#undef NDEBUG       // enable assert in release mode

#include <fstream>

#define DEBUG_MESH false

// compares vertex for storing in std::Map
//  NOTE: this routine is used for loading a mesh from an STL file;
//        it is used to detect multiple copies of the same vertex
//        in order to prevent storing the same vertex coordinate to
//        multiple locations in the vertices array
struct VertexCompare {
    bool operator() (const vct3 &k1, const vct3 &k2) const
    { // Return true if k1 goes before k2 in the strict weak
        //  ordering for the map object;
        //  i.e. if k1 is not strictly less than k2, return false
        //       otherwise, return true
        //
        // Note: https://ece.uwaterloo.ca/~dwharder/aads/Relations/Weak_ordering/

        // order by x,y,z in that order
        if (k1.X() < k2.X())
            return true;
        else if (k1.X() == k2.X()) {
            if (k1.Y() < k2.Y())
                return true;
            else if (k1.Y() == k2.Y()) {
                if (k1.Z() < k2.Z())
                    return true;
            }
        }
        return false;
    }
};


msh3Mesh::msh3Mesh(double robotError, bool convertMMtoM):
    mRobotResolution(robotError), mConvertMMtoM(convertMMtoM)
{
    mMeshResolution = 1E-3;
    // if given unit mm and the desired unit is m, convert tolerance
    if (mConvertMMtoM) {
        mMeshResolution /= m_to_mm;
        mRobotResolution /= m_to_mm;
    }
}

msh3Mesh::msh3Mesh(bool convertMMtoM):
    mConvertMMtoM(convertMMtoM)
{
    mMeshResolution = 1E-3;
    mRobotResolution = 1E-3;
    // if given unit mm and the desired unit is m, convert tolerance
    if (mConvertMMtoM){
        mMeshResolution /= m_to_mm;
        mRobotResolution /= m_to_mm;
    }
}

msh3Mesh::msh3Mesh(double robotError):
    mRobotResolution(robotError)
{
    mConvertMMtoM = false;
    mMeshResolution = 1E-3;
}

msh3Mesh::msh3Mesh()
{
    mConvertMMtoM = false;
    mMeshResolution = 1E-3;
    mRobotResolution = 1E-3;
}

void msh3Mesh::ResetMesh()
{
    vertices.SetSize(0);
    faces.SetSize(0);
    faceNormals.SetSize(0);
    vertexNormals.SetSize(0);
    faceNeighbors.SetSize(0);
    TriangleCov.SetSize(0);
    TriangleCovEig.SetSize(0);
}

void msh3Mesh::InitializeNoiseModel()
{
    TriangleCov.SetSize(faces.size());
    TriangleCovEig.SetSize(faces.size());

    TriangleCov.SetAll(vct3x3(0.0));
    TriangleCovEig.SetAll(vct3(0.0));
}

void msh3Mesh::InitializeNoiseModel(double noiseInPlaneVar,
                                    double noisePerpPlaneVar)
{
    vct3x3 M, M0;
    vctRot3 R;
    vct3 z(0.0, 0.0, 1.0);
    vct3 norm;

    if (faceNormals.size() != faces.size()) {
        std::cout << "ERROR: must initialize face normals in order to compute mesh noise model" << std::endl;
        TriangleCov.SetSize(0);
        TriangleCovEig.SetSize(0);
        assert(0);
    }

    TriangleCov.SetSize(faces.size());
    TriangleCovEig.SetSize(faces.size());

    // set covariance eigenvalues (in order of decreasing magnitude)
    if (noiseInPlaneVar >= noisePerpPlaneVar) {
        TriangleCovEig.SetAll(vct3(noiseInPlaneVar, noiseInPlaneVar, noisePerpPlaneVar));
    }
    else {
        TriangleCovEig.SetAll(vct3(noisePerpPlaneVar, noiseInPlaneVar, noiseInPlaneVar));
    }

    // compute covariance matrices
    for (unsigned int i = 0; i < faces.size(); i++) {
        TriangleCov[i] = ComputePointCovariance(faceNormals[i], noisePerpPlaneVar, noiseInPlaneVar);
    }
}

void msh3Mesh::SaveTriangleCovariances(std::string &filePath)
{
    std::cout << "Saving mesh covariances to file: " << filePath << std::endl;
    std::ofstream fs(filePath.c_str());
    if (!fs.is_open()) {
        std::cout << "ERROR: failed to open file for saving cov: " << filePath << std::endl;
        assert(0);
    }
    size_t numCov = this->TriangleCov.size();
    //fs << "NUMCOV " << numCov << "\n";
    for (size_t i = 0; i < numCov; i++) {
        fs << this->TriangleCov.at(i).Row(0) << " "
           << this->TriangleCov.at(i).Row(1) << " "
           << this->TriangleCov.at(i).Row(2) << "\n";
    }
}

#if CISST_MSH_HAS_RPLY
void msh3Mesh::LoadPLY(const std::string & input_file) {
    ply_obj.read_ply_mesh(input_file,
                          &vertices, &faces,
                          &faceNormals, &faceNeighbors,
                          &vertexNormals);
}
#else
void msh3Mesh::LoadPLY(const std::string &) {
    std::cerr << "ERROR: cisstMesh has been compiled without RPLY support" << std::endl;
}
#endif

#if CISST_MSH_HAS_RPLY
void msh3Mesh::SavePLY(const std::string &output_file) {
    ply_obj.write_ply_mesh(output_file,
                           &vertices, &faces,
                           &faceNormals, &faceNeighbors,
                           &vertexNormals);
}
#else
void msh3Mesh::SavePLY(const std::string & CMN_UNUSED(output_file)) {
    std::cerr << "ERROR: cisstMesh has been compiled without RPLY support" << std::endl;
}
#endif

int msh3Mesh::LoadMesh(const vctDynamicVector<vct3> *vertices,
                       const vctDynamicVector<vctInt3> *faces,
                       const vctDynamicVector<vct3> *face_normals,
                       const vctDynamicVector<vctInt3> *face_neighbors,
                       const vctDynamicVector<vct3> *vertex_normals
                       )
{
    ResetMesh();

    if (!vertices || !faces) {
        std::cout << "ERROR: vertices and faces must not be null" << std::endl;
        assert(0);
    }
    if (vertices->size() < 1 || faces->size() < 1) {
        std::cout << "ERROR: vertices and faces must not be empty" << std::endl;
        assert(0);
    }
    this->vertices = *vertices;
    this->faces = *faces;

    if (face_normals) {
        if (face_normals->size() != this->faces.size()) {
            std::cout << "ERROR: number of face normals does not equal number of faces" << std::endl;
            assert(0);
        }
        this->faceNormals = *face_normals;
    }

    if (face_neighbors) {
        if (face_neighbors->size() != this->faces.size()) {
            std::cout << "ERROR: number of face neighbors does not equal number of faces" << std::endl;
            assert(0);
        }
        this->faceNeighbors = *face_neighbors;
    }

    if (vertex_normals) {
        if (vertex_normals->size() != this->vertices.size()) {
            std::cout << "ERROR: number of face neighbors does not equal number of faces" << std::endl;
            assert(0);
        }
        this->vertexNormals = *vertex_normals;
    }

    InitializeNoiseModel();

    return 0;
}

int msh3Mesh::LoadMesh(const vctDynamicVector<vct3> &V,
                       const vctDynamicVector<vctInt3> &T,
                       const vctDynamicVector<vct3> &N)
{
    ResetMesh();

    if (V.size() < 1 || T.size() < 1 || T.size() != N.size()) {
        std::cout << "ERROR: invalid input" << std::endl;
        assert(0);
    }

    vertices = V;
    faces = T;
    faceNormals = N;

    InitializeNoiseModel();

    //std::cout << " Mesh Build Complete (Points: " << vertices.size()
    //  << ", Triangles: " << faces.size() << ")" << std::endl;
    return 0;
}

int msh3Mesh::LoadMeshFile(const std::string &meshFilePath)
{
    int rv;

    ResetMesh();

    rv = AddMeshFile(meshFilePath);

    InitializeNoiseModel();

    return rv;
}

int msh3Mesh::LoadMeshFileMultiple(const std::vector<std::string> &meshFilePaths)
{
    int rv;

    ResetMesh();

    std::vector<std::string>::const_iterator iter;
    for (iter = meshFilePaths.begin(); iter != meshFilePaths.end(); iter++) {
        rv = AddMeshFile(*iter);
        if (rv == -1) return -1;
    }

    InitializeNoiseModel();

    return 0;
}

int msh3Mesh::AddMeshFile(const std::string &meshFilePath)
{
    // Load mesh from ASCII file having format:
    //
    //  POINTS numPoints
    //  px py pz
    //   ...
    //  px py pz
    //  TRIANGLES numTriangles
    //  vx1 vx2 vx3
    //    ...
    //  vx1 vx2 vx3
    //  NORMALS numTriangles
    //  nx ny nz
    //    ...
    //  nx ny nz
    //
    //  where vx's are indices into the points array

    //std::cout << "Loading mesh file: " << meshFilePath << std::endl;

    float f1, f2, f3;
    int d1, d2, d3;
    unsigned int itemsRead;
    std::string line;

    size_t vOffset = vertices.size();
    size_t tOffset = faces.size();

    // open file
    std::ifstream meshFile;
    meshFile.open(meshFilePath.c_str());
    if (!meshFile.is_open()) {
        std::cout << "ERROR: failed to open mesh file" << std::endl;
        return -1;
    }

    // read points
    //  (vertex coordinates)
    unsigned int numPoints;
    std::getline(meshFile, line);
    itemsRead = std::sscanf(line.c_str(), "POINTS %u", &numPoints);
    if (itemsRead != 1) {
        std::cout << "ERROR: expected POINTS header at line: " << line << std::endl;
        return -1;
    }
    vct3 v;
    vertices.resize(vOffset + numPoints);  // non destructive resize
    unsigned int pointCount = 0;
    while (meshFile.good() && pointCount < numPoints) {
        std::getline(meshFile, line);
        itemsRead = std::sscanf(line.c_str(), "%f %f %f", &f1, &f2, &f3);
        if (itemsRead != 3) {
            std::cout << "ERROR: expected a point value at line: " << line << std::endl;
            return -1;
        }
        v[0] = f1;
        v[1] = f2;
        v[2] = f3;
        vertices.at(vOffset + pointCount).Assign(v);
        pointCount++;
    }
    if (meshFile.bad() || meshFile.fail() || pointCount != numPoints) {
        std::cout << "ERROR: read points from mesh file failed; last line read: " << line << std::endl;
        return -1;
    }

    // read triangles
    //  (three values that index the points array, one for each vertex)
    unsigned int numTriangles;
    std::getline(meshFile, line);
    itemsRead = std::sscanf(line.c_str(), "TRIANGLES %u", &numTriangles);
    if (itemsRead != 1) {
        std::cout << "ERROR: expected TRIANGLES header at line: " << line << std::endl;
        return -1;
    }
    vctInt3 T;
    faces.resize(tOffset + numTriangles); // non destructive size change
    unsigned int triangleCount = 0;
    while (meshFile.good() && triangleCount < numTriangles) {
        std::getline(meshFile, line);
        itemsRead = std::sscanf(line.c_str(), "%d %d %d", &d1, &d2, &d3);
        if (itemsRead != 3) {
            std::cout << "ERROR: expeced three index values on line: " << line << std::endl;
            return -1;
        }
        T.Assign(d1 + vOffset, d2 + vOffset, d3 + vOffset);
        faces.at(tOffset + triangleCount) = T;
        triangleCount++;
    }
    if (meshFile.bad() || meshFile.fail() || triangleCount != numTriangles) {
        std::cout << "ERROR: while reading triangles from mesh file; last line read: " << line << std::endl;
        return -1;
    }

    // read triangle normals
    unsigned int numNormals;
    std::getline(meshFile, line);
    itemsRead = std::sscanf(line.c_str(), "NORMALS %u", &numNormals);
    if (numNormals != numTriangles) {
        std::cout << std::endl << "ERROR: number of triangles and normals in the mesh do not match" << std::endl << std::endl;
        return -1;
    }
    if (itemsRead != 1) {
        std::cout << std::endl << " ===> WARNING: normal vectors are missing in this legacy mesh file <=== " << std::endl << std::endl;
        return 0;
    }
    vct3 N;
    faceNormals.resize(tOffset + numNormals); // non destructive size change
    unsigned int normCount = 0;
    while (meshFile.good() && normCount < numNormals) {
        std::getline(meshFile, line);
        itemsRead = std::sscanf(line.c_str(), "%f %f %f", &f1, &f2, &f3);
        if (itemsRead != 3) {
            std::cout << "ERROR: expeced three decimal values on line: " << line << std::endl;
            return -1;
        }
        N.Assign(f1, f2, f3);
        faceNormals.at(tOffset + normCount) = N.Normalized();
        normCount++;
    }
    if (meshFile.bad() || meshFile.fail() || normCount != numNormals) {
        std::cout << "ERROR: while reading normals from mesh file; last line read: " << line << std::endl;
        return -1;
    }

    //std::cout << " Mesh Load Complete (Points: " << vertices.size()
    //  << ", Triangles: " << faces.size() << ")" << std::endl;
    return 0;
}

void msh3Mesh::FindFaceNeighbor(int faceIdx, std::unordered_multimap<std::string, int>& map)
{
    std::string edgeKey;
    // edge12
    edgeKey = VertexToKey(V1,V2,faces.at(faceIdx)); //std::to_string(std::min(faces.at(faceIdx)[V1-vertexOffset],faces.at(faceIdx)[V2-vertexOffset]))+"-"+std::to_string(std::max(faces.at(faceIdx)[V1-vertexOffset],faces.at(faceIdx)[V2-vertexOffset]));
    // see link below for how to use unordered multimap - https://en.cppreference.com/w/cpp/container/unordered_multimap/equal_range
    auto range = map.equal_range(edgeKey);
    for (auto it = range.first; it != range.second; it ++) {
        if (it->second != faceIdx) {
            faceNeighbors.at(faceIdx)[V1V2-edgeOffset] = it->second;
        }
    }
    // edge13
    edgeKey = VertexToKey(V1,V3,faces.at(faceIdx)); //std::to_string(std::min(faces.at(faceIdx)[V1-vertexOffset],faces.at(faceIdx)[V3-vertexOffset]))+"-"+std::to_string(std::max(faces.at(faceIdx)[V1-vertexOffset],faces.at(faceIdx)[V3-vertexOffset]));
    range = map.equal_range(edgeKey);
    for (auto it = range.first; it != range.second; it ++){
        if (it->second != faceIdx){
            faceNeighbors.at(faceIdx)[V1V3-edgeOffset] = it->second;
        }
    }
    // edge23
    edgeKey = VertexToKey(V2,V3,faces.at(faceIdx)); // std::to_string(std::min(faces.at(faceIdx)[V2-vertexOffset],faces.at(faceIdx)[V3-vertexOffset]))+"-"+std::to_string(std::max(faces.at(faceIdx)[V2-vertexOffset],faces.at(faceIdx)[V3-vertexOffset]));
    range = map.equal_range(edgeKey);
    for (auto it = range.first; it != range.second; it ++) {
        if (it->second != faceIdx) {
            faceNeighbors.at(faceIdx)[V2V3-edgeOffset] = it->second;
        }
    }
}

std::string msh3Mesh::VertexToKey(const int& vertex1, const int& vertex2, const vctInt3& triangle)
{
    return std::to_string(std::min(triangle[vertex1-vertexOffset],triangle[vertex2-vertexOffset]))+"-"+std::to_string(std::max(triangle[vertex1-vertexOffset],triangle[vertex2-vertexOffset]));
}

void msh3Mesh::MergeEdgePoint(std::vector<int> &faceIdx, vct3 &target)
{
    // add a mesh only when:
    // 1. closest point fall in-triangle and is on the positive side of normal
    //  => add normal
    // 2. closest point fall on edge, neighbor closest point is on the same edge and locally concave
    //  => add target-closestpoint
    // 3. closest point fall on edge, on positive side of current normal and locally convex
    //  => add normal

    for (auto i = faceIdx.begin(); i < faceIdx.end();) {
#if DEBUG_MESH
        std::cout << "Robot resolution " << mRobotResolution << std::endl;
        std::cout << "Mesh resolution " << mMeshResolution << std::endl;
        if (cpLocation.at(*i) > V3) {
            std::cout << "Triangle "<< *i << std::endl;
            std::cout << cpLocation.at(*i) << std::endl;
        }
#endif
        // within mesh
        if (cpLocation.at(*i) == IN) {
#if DEBUG_MESH
            if (*i == 0) {
                std::cout << "Triangle "<< *i << std::endl;
                std::cout << "CP in mesh " << std::endl;
                std::cout << "check normal result " << CheckNormal(target,*i,mRobotResolution) << std::endl;
                std::cout << "Normal " << faceNormals.at(*i) << std::endl;
                std::cout << "Vector " << (target-closestPoint.at(*i)) << std::endl;
                std::cout << "Dot product " << (target-closestPoint.at(*i)).DotProduct(faceNormals.at(*i)) << std::endl;
            }
#endif
            // positive side of normal
            if ( CheckNormal(target,*i,mRobotResolution)) {
                activeNormal.at(*i).Assign(faceNormals.at(*i));
                i ++; // keep, increment iterator by 1
                continue;
            }
        }
        else {
            // on vertex
            if (cpLocation.at(*i) <= V3) {
                // if target almost equal to closest point, average normals for numerical stability
                if (target.AlmostEqual(closestPoint.at(*i),mRobotResolution)) {
                    activeNormal.at(*i).Assign(faceNormals.at(*i));
                    // find all same vertex, average and remove
                    for (auto j = i+1; j < faceIdx.end(); ) {
                        if (closestPoint.at(*j).AlmostEqual(closestPoint.at(*i), mMeshResolution)
                            && CheckConcavity(*i, *j)==true) {
                            if (*i == 0 && *j == 3) {
                                std::cout << "Concavity? " << (CheckConcavity(*i, *j)) << std::endl;
                                std::cout << "Normal " << faceNormals.at(*i) << std::endl;
                                std::cout << "CP of neighbor " << cpLocation.at(*j) << std::endl;
                                std::cout << "Normal " << faceNormals.at(*j) << std::endl;
                            }
                            activeNormal.at(*i).Add(faceNormals.at(*j));
                            faceIdx.erase(j);
                        }
                        else {
                            j ++;
                        }
                    }
                    activeNormal.at(*i).NormalizedSelf();
                    i ++; // keep, increment iterator by 1
                    continue;
                }
                // proceed as normal
                else {
#if DEBUG_MESH
                    if (*i == 3) {
                        std::cout << "Triangle "<< *i << std::endl;
                        std::cout << "CP on vertex" << std::endl;
                        std::cout << "Target != CP " << std::endl;
                    }
#endif
                    int neighborIdx1 = 0, neighborIdx2 = 0;
                    switch (cpLocation.at(*i)) {
                    case V1:
                        neighborIdx1 = faceNeighbors.at(*i)[V1V2-edgeOffset];
                        neighborIdx2 = faceNeighbors.at(*i)[V1V3-edgeOffset];
                        break;
                    case V2:
                        neighborIdx1 = faceNeighbors.at(*i)[V1V2-edgeOffset];
                        neighborIdx2 = faceNeighbors.at(*i)[V2V3-edgeOffset];
                        break;
                    case V3:
                        neighborIdx1 = faceNeighbors.at(*i)[V1V3-edgeOffset];
                        neighborIdx2 = faceNeighbors.at(*i)[V2V3-edgeOffset];
                        break;
                    }
                    bool keep=false;
                    if (closestPoint.at(*i).AlmostEqual(closestPoint.at(neighborIdx1),mMeshResolution)) {
                        for (auto j = i+1; j < faceIdx.end(); ){
                            if (*j == neighborIdx1) {
                                // remove neighbor
                                faceIdx.erase(j);
                                cpLocation.at(neighborIdx1) = VOID;
                                keep=true;
#if DEBUG_MESH
                                std::cout << "Remove Neighbor " << neighborIdx1 << std::endl;
#endif
                                break;

                            }
                            else {
                                j ++;
                            }
                        }

                    }
                    if (closestPoint.at(*i).AlmostEqual(closestPoint.at(neighborIdx2),mMeshResolution)) {
                        for (auto j = i+1; j < faceIdx.end(); ) {
                            if (*j == neighborIdx2) {
                                // remove neighbor
                                faceIdx.erase(j);
                                cpLocation.at(neighborIdx2) = VOID;
                                keep=true;
#if DEBUG_MESH
                                std::cout << "Remove Neighbor " << neighborIdx2 << std::endl;
#endif
                                break;
                            }
                            else {
                                j ++;
                            }
                        }
                    }
                    if (keep) {
                        activeNormal.at(*i).Assign(target-closestPoint.at(*i));
                        i ++; // keep, increment iterator by 1
                        continue;
                    }
                }
            }
            // on edge
            else {
                int neighborIdx = faceNeighbors.at(*i)[cpLocation.at(*i)-edgeOffset];
                // if neightbor on the same edge, then closest point must have been identical. We proceed only when locally concave.
                if (CheckConcavity(*i, neighborIdx)) {
                    if (closestPoint.at(neighborIdx).AlmostEqual(closestPoint.at(*i),mMeshResolution)) {
                        // if target almost equal to closest point, average normals for numerical stability
                        if (target.AlmostEqual(closestPoint.at(*i),mRobotResolution)) {
                            activeNormal.at(*i).Assign(faceNormals.at(*i));
                            activeNormal.at(*i).Add(faceNormals.at(neighborIdx));
                            activeNormal.at(*i).NormalizedSelf();
#if DEBUG_MESH
                            if (*i == 5 || *i == 2) {
                                std::cout << "Traingle " << *i << std::endl;
                                std::cout << "Target == CP, Average normal " << std::endl;
                                std::cout << "Current normal " << faceNormals.at(*i) << std::endl;
                                std::cout << "Neighbor normal " << faceNormals.at(neighborIdx) << std::endl;
                            }
#endif
                            i ++; // keep, increment iterator by 1
                            continue;
                        }
                        else {
#if DEBUG_MESH
                            if (*i == 5 || *i == 2) {
                                std::cout << "Traingle " << *i << std::endl;
                                std::cout << "Target != CP " << std::endl;
                                std::cout << "use vector CP to target" << target-closestPoint.at(*i)<< std::endl;
                            }
#endif
                            cpLocation.at(neighborIdx) = VOID;
                            for (auto j = i+1; j < faceIdx.end(); ) {
                                if (*j == neighborIdx) {
                                    // remove neighbor
                                    faceIdx.erase(j);
                                    break;
                                }
                                else {
                                    j ++;
                                }
                            }
                            activeNormal.at(*i).Assign(target-closestPoint.at(*i));
                            i ++; // keep, increment iterator by 1
                            continue;
                        }
                    }
                }
                // if convex and on the positive side of normal
                else if (cpLocation.at(neighborIdx) != VOID // neighbor must be in the current list (in case removed by previous SAME EDGE case)
                         && CheckNormal(target,*i,mRobotResolution)
                         ) {
#if DEBUG_MESH
                    if (*i == 5 || *i == 2) {
                        std::cout << "Triangle "<< *i << std::endl;
                        std::cout << "last case" << std::endl;
                        std::cout << "neighbor index " << neighborIdx<< std::endl;
                        std::cout << "neighbor cp location " << cpLocation.at(neighborIdx)<< std::endl;
                        std::cout << "normal of neighbor " << faceNormals.at(neighborIdx)<< std::endl;
                        std::cout << "convexity " << CheckConvexity(*i, neighborIdx)<< std::endl;
                    }
#endif
                    activeNormal.at(*i).Assign(faceNormals.at(*i));
                    i ++; // keep, increment iterator by 1
                    continue;
                }
            }
        }

        // otherwise, delete current mesh
        cpLocation.at(*i) = VOID;
        faceIdx.erase(i);
    }
}

void msh3Mesh::ResetMeshConstraintValues()
{
    closestPoint.Zeros();
    cpLocation.Zeros();
    activeNormal.Zeros();
    distance.Zeros();
}

void msh3Mesh::TransformTriangle(const vctFrm4x4 &transformation)
{
    for (auto & vert: vertices) {
        vert = transformation * vert;
    }

    for (auto & normal: faceNormals) {
        normal = transformation.Rotation() * normal;
    }
}

// Save current mesh to a single mesh file
int msh3Mesh::SaveMeshFile(const std::string &filePath)
{
    // Save ASCII file in format:
    //
    //  POINTS numPoints
    //  px py pz
    //   ...
    //  px py pz
    //  TRIANGLES numTriangles
    //  vx1 vx2 vx3
    //    ...
    //  vx1 vx2 vx3
    //  NORMALS numTriangles
    //  nx ny nz
    //    ...
    //  nx ny nz
    //
    //  where vx's are indices into the points array

    //std::cout << "Saving mesh to file: " << filePath << std::endl;
    std::ofstream fs(filePath.c_str());
    if (!fs.is_open()) {
        std::cout << "ERROR: failed to open file for saving mesh: " << filePath << std::endl;
        return -1;
    }
    fs << "POINTS " << vertices.size() << "\n";
    for (unsigned int i = 0; i < vertices.size(); i++) {
        fs << vertices.at(i)[0] << " "
           << vertices.at(i)[1] << " "
           << vertices.at(i)[2] << "\n";
    }
    fs << "TRIANGLES " << faces.size() << "\n";
    for (unsigned int i = 0; i < faces.size(); i++) {
        fs << faces.at(i)[0] << " "
           << faces.at(i)[1] << " "
           << faces.at(i)[2] << "\n";
    }
    fs << "NORMALS " << faceNormals.size() << "\n";
    for (unsigned int i = 0; i < faceNormals.size(); i++) {
        fs << faceNormals.at(i)[0] << " "
           << faceNormals.at(i)[1] << " "
           << faceNormals.at(i)[2] << "\n";
    }
    fs.close();
    return 0;
}


// Load mesh from an STL File (.stl)
// Assumes STL File has the following format:
//
//  solid ascii
//   facet normal -0.206371 -0.756911 -0.620078
//    outer loop
//     vertex 171.49 93.7415 0.593199
//     vertex 170.849 93.4625 1.14687
//     vertex 169.704 93.7397 1.18966
//    endloop
//   endfacet
//    ...
//  endsolid
int msh3Mesh::LoadMeshFromSTLFile(const std::string &stlFilePath)
{
    std::cout << "Building mesh from STL file: " << stlFilePath << std::endl;

    // initialize vectors
    vertices.SetSize(0);
    faces.SetSize(0);
    faceNormals.SetSize(0);
    faceNeighbors.SetSize(0);
    closestPoint.SetSize(0);
    cpLocation.SetSize(0);
    activeNormal.SetSize(0);
    distance.SetSize(0);

    // open mesh file
    std::ifstream stlFile;
    stlFile.open(stlFilePath.c_str());

    if (!stlFile.is_open()) {
        std::cout << "ERROR: failed to open STL file" << std::endl;
        return -1;
    }

    //--- Process STL File ---//
    //  Initially, store all vertices in a Map in order to ensure uniqueness
    //  of vertices and to assign sequential indices to each
    vct3 v, n;
    vctInt3 T;
    float f1, f2, f3;
    unsigned int itemsRead;
    std::vector<vctInt3> triangles;         // temp storage
    std::vector<vct3> triangleNormals;      // temp storage
    unsigned int vertexCount = 0;
    unsigned int triangleCount = 0;

    typedef std::map<vct3, unsigned int, VertexCompare> VertexMapType;
    VertexMapType vertexMap;
    std::pair<vct3, unsigned int>             mapInput;
    std::pair<VertexMapType::iterator, bool>  mapResult;

    // an edge to triangle map for neighbor index lookup
    std::unordered_multimap<std::string, int> edgeToTriangleMap;

    // read mesh header
    std::string line;
    std::getline(stlFile, line);
    // Note: different STL formats exist, such as
    //       "solid ascii" or "solid vcg" => just check for "solid"
    if (line.find("solid") == std::string::npos) {
        std::cout << "ERROR: unrecognized STL format, missing \"solid ascii\" or \"solid vcg\", etc" << std::endl;
        return -1;
    }
    //  unsigned int update;
    while (stlFile.good()) {
        //    // print progress update
        //    update = triangleCount % 10000;
        //    if (update == 0)
        //    {
        //      std::cout << "Triangle #: " << triangleCount / 1000 << "k" << std::endl;
        //    }
        // check for end of file (i.e. line: "endsolid")
        std::getline(stlFile, line);
        if (line.find("endsolid") != std::string::npos)
            { // no more triangles in file
                break;
            }
        // check for empty line
        if (line.empty()) {
            continue;
        }

        // read data for next triangle
        // line: " facet normal -0.206371 -0.756911 -0.620078"
        itemsRead = std::sscanf(trim(line).c_str(), "facet normal %f %f %f", &f1, &f2, &f3);
        if (itemsRead != 3) {
            std::cout << "ERROR: STL file missing \"facet normal %f %f %f\"" << std::endl;
            return -1;
        }
        n.Assign(f1, f2, f3);
        n.NormalizedSelf();
        // line: "  outer loop"
        std::getline(stlFile, line);
        if (line.find("outer loop") == std::string::npos) {
            std::cout << "ERROR: STL file missing \"outer loop\"" << std::endl;
            return -1;
        }
        // 3 lines of: "   vertex 171.49 93.7415 0.593199"
        for (unsigned int i = 0; i < 3; i++) {
            std::getline(stlFile, line);
            itemsRead = std::sscanf(trim(line).c_str(), "vertex %f %f %f", &f1, &f2, &f3);
            if (itemsRead != 3) {
                std::cout << "ERROR: STL file missing \"vertex %f %f %f\"" << std::endl;
                return -1;
            }

            if (mConvertMMtoM) {
                //        v.Assign(round3(f1)/m_to_mm, round3(f2)/m_to_mm, round3(f3)/m_to_mm);
                v.Assign(f1/m_to_mm, f2/m_to_mm, f3/m_to_mm);
            }
            else {
                //        v.Assign(round3(f1), round3(f2), round3(f3)); // round to 3 decimal places to avoid gap in model,
                // if STL is in mm, leading to a precision of um
                v.Assign(f1, f2, f3);
            }

            // add vertex to map, along with this vertex's index position
            mapInput.first = v;
            mapInput.second = vertexCount;
            mapResult = vertexMap.insert(mapInput);
            if (mapResult.second == true) {
                // new vertex was added to map => increment the index position for the next vertex
                vertexCount++;
            }
            // store index position to triangle
            T[i] = mapResult.first->second;
            if (mapResult.first->second < 0) {
                std::cout << "ERROR: vertex maps to negative index" << std::endl;
                return -1;
            }
        }
        // line: "  end loop"
        std::getline(stlFile, line);
        if (line.find("endloop") == std::string::npos) {
            std::cout << "ERROR: STL file missing \"  endloop\"" << std::endl;
            return -1;
        }
        // line: " endfacet"
        std::getline(stlFile, line);
        if (line.find("endfacet") == std::string::npos) {
            std::cout << "ERROR: STL file missing \" endfacet\"" << std::endl;
            return -1;
        }
        // add mesh triangle
        triangles.push_back(T);
        triangleNormals.push_back(n);
        // add to edge triangle map
        edgeToTriangleMap.insert(std::make_pair(VertexToKey(V1,V2,T),triangleCount));
        edgeToTriangleMap.insert(std::make_pair(VertexToKey(V1,V3,T),triangleCount));
        edgeToTriangleMap.insert(std::make_pair(VertexToKey(V2,V3,T),triangleCount));
        // increment count
        triangleCount++;
    }

    if (!stlFile.good()) {
        // report error except end-of-line
        if (!stlFile.eof()) {
            std::cout << "ERROR: processing of STL encountered error" << std::endl;
            return -1;
        }
    }

    //--- Mesh Post-Processing ---//

    // move triangles to mesh from std::vector
    faces.SetSize(triangles.size());
    faceNormals.SetSize(triangles.size());
    faceNeighbors.SetSize(faces.size());
    closestPoint.SetSize(faces.size());
    cpLocation.SetSize(faces.size());
    activeNormal.SetSize(faces.size());
    distance.SetSize(faces.size());
    for (unsigned int i = 0; i < triangles.size(); i++) {
        faces.at(i) = triangles[i];
        faceNormals.at(i) = triangleNormals[i];
        // find face neighbors
        FindFaceNeighbor(i, edgeToTriangleMap);
    }

    // move vertices to mesh from std::map
    vertices.SetSize(vertexMap.size());
    VertexMapType::iterator mapIter;
    for (mapIter = vertexMap.begin(); mapIter != vertexMap.end(); mapIter++) { // move each vertex to its assigned index position in the mesh array
        vertices.at(mapIter->second).Assign(mapIter->first);
    }

    InitializeNoiseModel();

    std::cout << "Mesh Construction Complete (Points: " << vertices.size()
              << ", Triangles: " << faces.size() << ")" << std::endl;

    return 0;
}


//--- Legacy I/O ---//

void msh3Mesh::ReadMeshFile(const char *fn)
{
    char ext[10];
    int i;
    unsigned int j;

    ResetMesh();

    i = strlen(fn) - 1;
    while (i >= 0 && fn[i] != '.') i--;

    i++;
    for (j = i; j < strlen(fn); j++) {
        ext[j - i] = fn[j];
    }
    ext[j - i] = 0;

    if (!strcmp(ext, "sur") || !strcmp(ext, "SUR")) {
        ReadSURMeshFile(fn);
    }
    else if (!strcmp(ext, "sfc") || !strcmp(ext, "SFC")) {
        ReadSFCMeshFile(fn);
    }
    else {
        std::cout << "ERROR: Read mesh requires .sfc, . SFC, .sur, or .SUR files" << std::endl;
    };

    InitializeNoiseModel();
}

void msh3Mesh::WriteMeshFile(const char *fn)
{
    char ext[10];
    int i;
    unsigned int j;

    i = strlen(fn) - 1;
    while (i >= 0 && fn[i] != '.') i--;

    i++;
    for (j = i; j < strlen(fn); j++) {
        ext[j - i] = fn[j];
    }
    ext[j - i] = 0;

    if (!strcmp(ext, "sur") || !strcmp(ext, "SUR")) {
        WriteSURMeshFile(fn);
    }
    else {
        std::cout << "SUR Meshes require .sur or .SUR files" << std::endl;
    };
}

void msh3Mesh::ReadSURMeshFile(const char *fn)
{
    FILE *fp;

    if ((fp = fopen(fn, "r")) == NULL) {
        std::cerr << "SUR mesh file not found" << std::endl;
        return;
    }

    int vert_n, face_n;
    float f1, f2, f3;
    int nb_found;
    nb_found = fscanf(fp, "%d\n", &vert_n);	// off file
    if (nb_found != 1) {
        std::cerr << "ReadSURMeshFile for " << fn
                  << " failed to parse number of vertices" << std::endl;
        return;
    }
    vertices.SetSize(vert_n);

    int i;
    for (i = 0; i < vert_n; i++) {
        nb_found = fscanf(fp, "%f %f %f\n", &f1, &f2, &f3);
        if (nb_found != 3) {
            std::cerr << "ReadSURMeshFile for " << fn
                      << " failed to parse vertex " << i << std::endl;
            return;
        }
        vertices[i] = vct3(f1, f2, f3);
    }

    nb_found = fscanf(fp, "%d\n", &face_n);	// sur file
    if (nb_found != 1) {
        std::cerr << "ReadSURMeshFile for " << fn
                  << " failed to parse number of faces" << std::endl;
        return;
    }
    faces.SetSize(face_n);
    faceNeighbors.SetSize(face_n);

    char buff[1024];
    for (i = 0; i < face_n; i++) {
        int a, b, c; int d = 1, e = 1, f = 1;
        //JA have to fix this, probably should be using streams...
        fgets(buff, 1024, fp);
        //sscanf(buff,"%d %d %d",&a,&b,&c);
        sscanf(buff, "%d %d %d %d %d %d\n", &a, &b, &c, &d, &e, &f);
        //fscanf(fp,"%d %d %d %f %f %f\n",&a,&b,&c,&d,&e,&f);
        //cout<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<f<<endl;
        faces[i].Assign(a, b, c);
        faceNeighbors[i].Assign(d, e, f);
    };
    fclose(fp);
}

void msh3Mesh::ReadSFCMeshFile(const char *fn)
{
    FILE *fp;

    if ((fp = fopen(fn, "r")) == NULL) {
        std::cout << "ERROR: SFC mesh file not found" << std::endl;
        return;
    }

    int vert_n, face_n;
    float f1, f2, f3;
    int nb_found;

    nb_found = fscanf(fp, "%d\n", &vert_n);	// off file
    if (nb_found != 1) {
        std::cerr << "ReadSFCMeshFile for " << fn
                  << " failed to parse number of vertices" << std::endl;
        return;
    }
    vertices.SetSize(vert_n);

    int i;
    for (i = 0; i < vert_n; i++) {
        nb_found = fscanf(fp, "%f %f %f\n", &f1, &f2, &f3);
        if (nb_found != 3) {
            std::cerr << "ReadSFCMeshFile for " << fn
                      << " failed to parse vertex " << i << std::endl;
            return;
        }
        vertices[i] = vct3(f1, f2, f3);
    }

    nb_found = fscanf(fp, "%d\n", &face_n);	// sur file
    if (nb_found != 1) {
        std::cerr << "ReadSFCMeshFile for " << fn
                  << " failed to parse number of faces" << std::endl;
        return;
    }
    faces.SetSize(face_n);
    faceNormals.SetSize(face_n);


    for (i = 0; i < face_n; i++) {
        int a, b, c; int d = 1, e = 1, f = 1;
        //JA have to fix this, probably should be using streams...

        nb_found = fscanf(fp, "%d %d %d\n", &a, &b, &c); d = e = f = -1;  // rht hack to get around not having neighbors
        if (nb_found != 3) {
            std::cerr << "ReadSFCMeshFile for " << fn
                      << " failed to parse face " << i << std::endl;
            return;
        }
        //fscanf(fp,"%d %d %d %f %f %f\n",&a,&b,&c,&d,&e,&f);
        //cout<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<f<<endl;
        faces[i].Assign(a, b, c);
        faceNeighbors[i].Assign(d, e, f);
#if 0
        printf("%d: ",i); FT.Print(stdout); printf("\n");
#endif
    }
    fclose(fp);
}

void msh3Mesh::WriteSURMeshFile(const char *fn)
{
    FILE *fp;

    if ((fp = fopen(fn, "w")) == NULL) {
        return;
    }

    int vert_n, face_n;

    vert_n = vertices.size();
    face_n = faces.size();
    fprintf(fp, "%d\n", vert_n); // sur file

    int i;

    for (i = 0; i < vert_n; i++) {
        vct3 V = vertices[i];
        // fprintf(fp,"%f %f %f   ; Vertex %d\n",V.x, V.y, V.z, i);;
        fprintf(fp, "%f %f %f\n", V[0], V[1], V[2]);;
    };

    fprintf(fp, "%d\n", face_n); // sur file

    for (i = 0; i < face_n; i++) {
        vctInt3 face = faces[i];
        vctInt3 neighbor(-1, -1, -1);
        if (faceNeighbors.size() > 0) neighbor = faceNeighbors[i];
        fprintf(fp, "%d %d %d %d %d %d\n",
                face[0], face[1], face[2],
                neighbor[0], neighbor[1], neighbor[2]);
    };

    fclose(fp);
}
