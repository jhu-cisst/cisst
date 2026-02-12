/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// ****************************************************************************
//
//    Copyright (c) 2014, Seth Billings, Russell Taylor, Johns Hopkins University
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

#include <cisstMesh/msh3PointCloud.h>
#include <cisstMesh/msh3Utilities.h>

msh3PointCloud::msh3PointCloud(vctDynamicVector<vct3> &points) :
    points(points)
{
    InitializeNoiseModel();
}

msh3PointCloud::msh3PointCloud(
                               vctDynamicVector<vct3> &points,
                               vctDynamicVector<vct3> &pointOrientations) :
    points(points),
    pointOrientations(pointOrientations)
{
    if (points.size() != pointOrientations.size()) {
        std::cout << "ERROR: number of points and point orientations are not the same;"
                  << " setting point cloud size to zero" << std::endl;
        points.SetSize(0);
        pointOrientations.SetSize(0);
        return;
    }

    InitializeNoiseModel();
}

msh3PointCloud::msh3PointCloud(msh3Mesh &mesh)
{
    vct3 v0, v1, v2;

    // build point cloud from triangle centers
    int NData = mesh.NumTriangles();
    int *DataIndices = new int[NData];
    for (int i = 0; i < NData; i++) {
        DataIndices[i] = i;
    }
    points.SetSize(NData);
    pointOrientations.SetSize(NData);
    pointCov.SetSize(NData);
    pointCovEig.SetSize(NData);
    for (int i = 0; i < NData; i++) {
        mesh.FaceCoords(i, v0, v1, v2);
        points[i] = (v0 + v1 + v2) / 3.0;
        pointOrientations[i] = mesh.faceNormals[i];
        pointCov[i] = mesh.TriangleCov[i];
        pointCovEig[i] = mesh.TriangleCovEig[i];
    }
}

msh3PointCloud::msh3PointCloud(msh3Mesh &mesh,
                               double noisePerpPlaneSD)
{
    double noiseInPlaneVar, noisePerpPlaneVar;
    double sqrDist;
    vct3 c, v0, v1, v2;

    noisePerpPlaneVar = noisePerpPlaneSD*noisePerpPlaneSD;

    // build point cloud from triangle centers
    int NData = mesh.NumTriangles();
    int *DataIndices = new int[NData];
    for (int i = 0; i < NData; i++) {
        DataIndices[i] = i;
    }
    points.SetSize(NData);
    pointOrientations.SetSize(NData);
    for (int i = 0; i < NData; i++) {
        mesh.FaceCoords(i, v0, v1, v2);
        points[i] = (v0 + v1 + v2) / 3.0;
        pointOrientations[i] = mesh.faceNormals[i];
    }

    // Define noise properties of point cloud
    //  use triangles to determine in-plane noise
    pointCov.SetSize(NData);
    pointCovEig.SetSize(NData);
    for (int i = 0; i < NData; i++) {
        mesh.FaceCoords(i, v0, v1, v2);
        c = (v0 + v1 + v2) / 3.0;

        // compute in-plane noise as the variance of the distance
        // between the triangle center and its 3 vertices
        sqrDist = (v0 - c).NormSquare();
        sqrDist += (v1 - c).NormSquare();
        sqrDist += (v2 - c).NormSquare();
        sqrDist /= 3.0;
        noiseInPlaneVar = sqrDist;

        // set noise model for this point
        pointCov[i] = ComputePointCovariance(mesh.faceNormals.at(i),
                                             noisePerpPlaneVar,
                                             noiseInPlaneVar);

        // list eigenvalues in descending order
        if (noiseInPlaneVar >= noisePerpPlaneVar) {
            pointCovEig[i].Element(0) = noiseInPlaneVar;
            pointCovEig[i].Element(1) = noiseInPlaneVar;
            pointCovEig[i].Element(2) = noisePerpPlaneVar;
        }
        else {
            pointCovEig[i].Element(0) = noisePerpPlaneVar;
            pointCovEig[i].Element(1) = noiseInPlaneVar;
            pointCovEig[i].Element(2) = noiseInPlaneVar;
        }
    }
}

void msh3PointCloud::ResetPointCloud()
{
    points.SetSize(0);
    pointOrientations.SetSize(0);
    pointCov.SetSize(0);
    pointCovEig.SetSize(0);
}

void msh3PointCloud::InitializeNoiseModel()
{
    pointCov.SetSize(points.size());
    pointCovEig.SetSize(points.size());

    pointCov.SetAll(vct3x3(0.0));
    pointCovEig.SetAll(vct3(0.0));
}

void msh3PointCloud::SavePointCloudCov(std::string &filePath)
{
    //std::cout << "Saving point cloud covariances to file: " << filePath << std::endl;
    std::ofstream fs(filePath.c_str());
    if (!fs.is_open()) {
        std::cout << "ERROR: failed to open file for saving cov: " << filePath << std::endl;
        assert(0);
    }
    unsigned int numCov = this->pointCov.size();
    //fs << "NUMCOV " << numCov << "\n";
    for (unsigned int i = 0; i < numCov; i++) {
        fs << this->pointCov.at(i).Row(0) << " "
           << this->pointCov.at(i).Row(1) << " "
           << this->pointCov.at(i).Row(2) << "\n";
    }
}

#if CISST_MSH_HAS_RPLY
void msh3PointCloud::LoadPLY(const std::string &input_file) {
    ply_obj.read_ply_pointcloud(input_file, &points, &pointOrientations);
}
#else
void msh3PointCloud::LoadPLY(const std::string &) {
    std::cerr << "ERROR: cisstMesh has been compiled without RPLY support" << std::endl;
}
#endif

#if CISST_MSH_HAS_RPLY
void msh3PointCloud::SavePLY(const std::string &output_file) {
    ply_obj.write_ply_pointcloud(output_file, &points, &pointOrientations);
}
#else
void msh3PointCloud::SavePLY(const std::string & CMN_UNUSED(output_file)) {
    std::cerr << "ERROR: cisstMesh has been compiled without RPLY support" << std::endl;
}
#endif

int msh3PointCloud::WritePointCloudToFile(std::string &filePath)
{
    return WritePointCloudToFile(filePath, points, pointOrientations);
}

int msh3PointCloud::ReadPointCloudFromFile(std::string &filePath)
{
    ResetPointCloud();

    int rv = ReadPointCloudFromFile(filePath, points, pointOrientations);

    InitializeNoiseModel();
    return rv;
}

int msh3PointCloud::AppendPointCloudFromFile(std::string &filePath)
{
    return AppendPointCloudFromFile(filePath, points, pointOrientations);
}

int msh3PointCloud::WritePointCloudToFile(std::string &filePath,
                                          vctDynamicVector<vct3> &points)
{
    vctDynamicVector<vct3> orientations;
    return WritePointCloudToFile(filePath, points, orientations);
}

int msh3PointCloud::ReadPointCloudFromFile(std::string &filePath,
                                           vctDynamicVector<vct3> &points)
{
    vctDynamicVector<vct3> orientations;
    return ReadPointCloudFromFile(filePath, points, orientations);
}

int msh3PointCloud::AppendPointCloudFromFile(std::string &filePath,
                                             vctDynamicVector<vct3> &points)
{
    vctDynamicVector<vct3> orientations;
    return AppendPointCloudFromFile(filePath, points, orientations);
}


int msh3PointCloud::WritePointCloudToFile(std::string &filePath,
                                          vctDynamicVector<vct3> &pts,
                                          vctDynamicVector<vct3> &orientations)
{
    // Text file format:
    //
    //  POINTS numPoints
    //  px py pz
    //   ...
    //  px py pz
    //  POINT_ORIENTATIONS numOrientations
    //  nx ny nz
    //   ...
    //  nx ny nz
    //

    //std::cout << "Saving pts & normals to file: " << filePath << std::endl;
    std::ofstream fs(filePath.c_str());
    if (!fs.is_open()) {
        std::cout << "ERROR: failed to open file: " << filePath << std::endl;
        return -1;
    }
    // write points
    fs << "POINTS " << pts.size() << "\n";
    for (unsigned int i = 0; i < pts.size(); i++) {
        fs << pts.at(i)[0] << " " << pts.at(i)[1] << " " << pts.at(i)[2] << "\n";
    }
    // write orientations [optional]
    if (orientations.size() == 0) {
        return 0;
    }
    fs << "POINT_ORIENTATIONS " << pts.size() << "\n";
    for (unsigned int i = 0; i < pts.size(); i++) {
        fs << orientations.at(i)[0] << " " << orientations.at(i)[1] << " " << orientations.at(i)[2] << "\n";
    }
    fs.close();
    return 0;
}

int msh3PointCloud::ReadPointCloudFromFile(std::string &filePath,
                                           vctDynamicVector<vct3> &pts,
                                           vctDynamicVector<vct3> &orientations)
{
    pts.SetSize(0);
    orientations.SetSize(0);
    return AppendPointCloudFromFile(filePath, pts, orientations);
}

int msh3PointCloud::AppendPointCloudFromFile(std::string &filePath,
                                             vctDynamicVector<vct3> &pts,
                                             vctDynamicVector<vct3> &orientations )
{
    // Text file format:
    //
    //  POINTS numPoints
    //  px py pz
    //   ...
    //  px py pz
    //  POINT_ORIENTATIONS numOrientations
    //  nx ny nz
    //   ...
    //  nx ny nz
    //

    unsigned int itemsRead;
    std::string line;
    float f1, f2, f3;
    float n1, n2, n3;

    unsigned int pOffset;
    pOffset = pts.size();

    //std::cout << "Reading pts & normals from file: " << filePath << std::endl;
    std::ifstream fs(filePath.c_str());
    if (!fs.is_open()) {
        std::cout << "ERROR: failed to open file: " << filePath << std::endl;
        return -1;
    }

    // read points
    unsigned int numPoints;
    std::getline(fs, line);
    itemsRead = std::sscanf(line.c_str(), "POINTS %u", &numPoints);
    if (itemsRead != 1) {
        std::cout << "ERROR: expected POINTS header at line: " << line << std::endl;
        return -1;
    }
    vct3 v;
    pts.resize(pOffset + numPoints);    // non-destructive
    unsigned int pointCount = 0;
    while (fs.good() && pointCount < numPoints) {
        std::getline(fs, line);
        itemsRead = std::sscanf(line.c_str(), "%f %f %f", &f1, &f2, &f3);
        if (itemsRead != 3) {
            std::cout << "ERROR: expected a point value at line: " << line << std::endl;
            return -1;
        }
        v[0] = f1;
        v[1] = f2;
        v[2] = f3;
        pts.at(pOffset + pointCount).Assign(v);
        pointCount++;
    }
    if (fs.bad() || fs.fail() || pointCount != numPoints) {
        std::cout << "ERROR: read points from file failed; last line read: " << line << std::endl;
        return -1;
    }

    // read orientations [optional]
    unsigned int numNormals;
    std::getline(fs, line);
    itemsRead = std::sscanf(line.c_str(), "POINT_ORIENTATIONS %u", &numNormals);
    if (itemsRead != 1) {
        //std::cout << "ERROR: expected POINT_ORIENTATIONS header at line: " << line << std::endl;
        return 1;
    }
    if (numNormals != numPoints) {
        std::cout << "ERROR: number of orientations does not match number of points" << std::endl;
        return -1;
    }
    vct3 n;
    orientations.resize(pOffset + numPoints);  // non-destructive
    unsigned int normCount = 0;
    while (fs.good() && normCount < numNormals) {
        std::getline(fs, line);
        itemsRead = std::sscanf(line.c_str(), "%f %f %f", &n1, &n2, &n3);
        if (itemsRead != 3) {
            std::cout << "ERROR: expected an orientation value at line: " << line << std::endl;
            return -1;
        }
        n[0] = n1; n[1] = n2; n[2] = n3;
        orientations.at(pOffset + normCount).Assign(n);
        normCount++;
    }
    if (fs.bad() || fs.fail() || normCount != numNormals) {
        std::cout << "ERROR: read orientations from file failed; last line read: " << line << std::endl;
        return -1;
    }

    fs.close();
    //std::cout << " ..." << pts.size() << " sample points & orientations" << std::endl;
    return 0;
}
