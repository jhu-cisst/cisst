/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Max Zhaoshuo Li, Anton Deguet
  Created on: 2020-07-20

  (C) Copyright 2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mshAlgPDTreeCPMeshTest.h"

#include <cisstCommon/cmnPath.h>
#include <cisstMesh/msh3AlgPDTreeCPMesh.h>

void mshAlgPDTreeCPMeshTest::setUp(void)
{
    cmnPath path;
    path.AddRelativeToCisstShare("/models/meshes");
    std::string testFile = "Cube.STL";
    std::string fullPath = path.Find(testFile);
    if (cmnPath::Exists(fullPath)) {
        workingDir = fullPath;
        workingDir.erase(workingDir.end() - testFile.size(),
                         workingDir.end());
    } else {
        std::cerr << "mshAlgPDTreeCPMeshTest::setUp: can't find meshes used for testing.  Make sure CISST_ROOT is set" << std::endl;
    }
    faceIdx.clear();
}

void mshAlgPDTreeCPMeshTest::CubeCorner(void)
{
    msh3Mesh cube;
    msh3PDTreeMesh* pTreeCube;
    cube.LoadMeshFromSTLFile(workingDir + "Cube.STL");
    // construct PD-Tree
    pTreeCube = new msh3PDTreeMesh(cube, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgCube = new msh3AlgPDTreeCPMesh(pTreeCube);
    pTreeCube->SetSearchAlgorithm(pAlgCube);

    // corner
    closestPoint.Assign(0.0, 0.0, 0.0);
    normal.Assign(-0.5, -0.5, -0.5);
    tip = normal+closestPoint;
    numIntersected = pTreeCube->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreeCube->Mesh->MergeEdgePoint(faceIdx, tip);
    for (auto it : faceIdx) {
        CPPUNIT_ASSERT(pTreeCube->Mesh->closestPoint[it].AlmostEqual(closestPoint));
        CPPUNIT_ASSERT(pTreeCube->Mesh->activeNormal[it].NormalizedSelf()
                       .AlmostEqual(normal.NormalizedSelf(), 1E-2));
    }
}

void mshAlgPDTreeCPMeshTest::CubeSide(void)
{
    msh3Mesh cube;
    msh3PDTreeMesh* pTreeCube;
    cube.LoadMeshFromSTLFile(workingDir + "Cube.STL");
    // construct PD-Tree
    pTreeCube = new msh3PDTreeMesh(cube, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgCube = new msh3AlgPDTreeCPMesh(pTreeCube);
    pTreeCube->SetSearchAlgorithm(pAlgCube);

    // side
    closestPoint.Assign(5.0, 0.0, 9.0);
    normal.Assign(0.0, -1.0, 0.0);
    tip = normal+closestPoint;
    numIntersected = pTreeCube->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreeCube->Mesh->MergeEdgePoint(faceIdx, tip);
    int matchedCount = 0;
    for (auto it : faceIdx) {
        if (pTreeCube->Mesh->closestPoint[it].AlmostEqual(closestPoint)) {
            matchedCount ++;
        }
        if (pTreeCube->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal.NormalizedSelf(), 1E-2)) {
            matchedCount ++;
        }
    }
    CPPUNIT_ASSERT_EQUAL(matchedCount, 2);
}

void mshAlgPDTreeCPMeshTest::CubeEdge(void)
{
    msh3Mesh cube;
    msh3PDTreeMesh* pTreeCube;
    cube.LoadMeshFromSTLFile(workingDir + "Cube.STL");
    // construct PD-Tree
    pTreeCube = new msh3PDTreeMesh(cube, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgCube = new msh3AlgPDTreeCPMesh(pTreeCube);
    pTreeCube->SetSearchAlgorithm(pAlgCube);

    // edge
    closestPoint.Assign(0.0, 0.0, 7.0);
    normal.Assign(-1.7, -1.0, 0.0);
    tip = normal+closestPoint;
    numIntersected = pTreeCube->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreeCube->Mesh->MergeEdgePoint(faceIdx, tip);
    for (auto it : faceIdx) {
        CPPUNIT_ASSERT(pTreeCube->Mesh->closestPoint[it].AlmostEqual(closestPoint));
        CPPUNIT_ASSERT(pTreeCube->Mesh->activeNormal[it].NormalizedSelf()
                       .AlmostEqual(normal.NormalizedSelf(), 1E-2));
    }
}

void mshAlgPDTreeCPMeshTest::CubeOnEdge(void)
{
    msh3Mesh cube;
    msh3PDTreeMesh* pTreeCube;
    cube.LoadMeshFromSTLFile(workingDir + "Cube.STL");
    // construct PD-Tree
    pTreeCube = new msh3PDTreeMesh(cube, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgCube = new msh3AlgPDTreeCPMesh(pTreeCube);
    pTreeCube->SetSearchAlgorithm(pAlgCube);

    // edge
    closestPoint.Assign(0.0, 0.0, 0.0);
    normal.Assign(-1.0, 0.0, 0.0);
    vct3 normal2(0.0, -1.0, 0.0);
    vct3 normal3(0.0, 0.0, -1.0);
    tip = closestPoint;
    numIntersected = pTreeCube->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreeCube->Mesh->MergeEdgePoint(faceIdx, tip);

    for (auto it : faceIdx) {
        CPPUNIT_ASSERT(pTreeCube->Mesh->closestPoint[it].AlmostEqual(closestPoint));
    }
}

void mshAlgPDTreeCPMeshTest::CubeOnSide(void)
{
    msh3Mesh cube;
    msh3PDTreeMesh* pTreeCube;
    cube.LoadMeshFromSTLFile(workingDir + "Cube.STL");
    // construct PD-Tree
    pTreeCube = new msh3PDTreeMesh(cube, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgCube = new msh3AlgPDTreeCPMesh(pTreeCube);
    pTreeCube->SetSearchAlgorithm(pAlgCube);

    // side
    closestPoint.Assign(10.0, 6.0, 8.0);
    normal.Assign(1.0, 0.0, 0.0);
    tip = closestPoint;
    numIntersected = pTreeCube->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreeCube->Mesh->MergeEdgePoint(faceIdx, tip);
    int matchedCount = 0;
    for (auto it : faceIdx) {
        if (pTreeCube->Mesh->closestPoint[it].AlmostEqual(closestPoint)) {
            matchedCount ++;
        }
        if (pTreeCube->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal.NormalizedSelf(), 1E-2)) {
            matchedCount ++;
        }
        std::cout << "Closest point " << pTreeCube->Mesh->closestPoint[it] << std::endl;
        std::cout << "Normal " << pTreeCube->Mesh->activeNormal[it] << std::endl;
    }
    CPPUNIT_ASSERT_GREATEREQUAL(2,matchedCount);
}


// concave - cylinder
void mshAlgPDTreeCPMeshTest::CylinderTop(void)
{
    msh3Mesh cylinder;
    msh3PDTreeMesh* pTreeCylinder;
    cylinder.LoadMeshFromSTLFile(workingDir + "Cylinder.STL");
    // construct PD-Tree
    pTreeCylinder = new msh3PDTreeMesh(cylinder, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgCylinder= new msh3AlgPDTreeCPMesh(pTreeCylinder);
    pTreeCylinder->SetSearchAlgorithm(pAlgCylinder);

    // top
    vct3 normal2;
    closestPoint.Assign(4.0, 10.0, 4.0);
    normal.Assign(0.0, 1.0, 0.0);
    tip = normal+closestPoint;
    normal2.Assign(tip-vct3(5.0, 10.0, 5.0));
    numIntersected = pTreeCylinder->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreeCylinder->Mesh->MergeEdgePoint(faceIdx, tip);
    int matchedCount = 0;
    for (auto it : faceIdx) {
        if (pTreeCylinder->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal.NormalizedSelf(), 1E-2)) {
            matchedCount ++;
        }
        if (pTreeCylinder->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal2.NormalizedSelf(), 1E-2)) {
            matchedCount ++;
        }
        std::cout << "normal " << pTreeCylinder->Mesh->activeNormal[it] << std::endl;
    }
    // expecting 2 normals, one is pointing up, one is pointing from center to tip
    CPPUNIT_ASSERT_GREATEREQUAL(2, matchedCount);
}

void mshAlgPDTreeCPMeshTest::CylinderSide(void)
{
    msh3Mesh cylinder;
    msh3PDTreeMesh* pTreeCylinder;
    cylinder.LoadMeshFromSTLFile(workingDir + "Cylinder.STL");
    // construct PD-Tree
    pTreeCylinder = new msh3PDTreeMesh(cylinder, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgCylinder= new msh3AlgPDTreeCPMesh(pTreeCylinder);
    pTreeCylinder->SetSearchAlgorithm(pAlgCylinder);

    // side
    tip.Assign(9.0, 5.0, 1.5);
    normal = tip - vct3(5.0, 5.0, 5.0);
    closestPoint = vct3(5.0, 5.0, 5.0) + normal.NormalizedSelf().Multiply(5.0);
    numIntersected = pTreeCylinder->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreeCylinder->Mesh->MergeEdgePoint(faceIdx, tip);
    int matchedCount = 0;
    for (auto it : faceIdx) {
        if (pTreeCylinder->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal.NormalizedSelf(), 1E-2)) {
            matchedCount ++;
        }
        std::cout << "normal " << pTreeCylinder->Mesh->activeNormal[it] << std::endl;
    }
    CPPUNIT_ASSERT_EQUAL(1,matchedCount);
}

// concave - sphere
// TODO: this needs to be validated again due to large number of normals
void mshAlgPDTreeCPMeshTest::Sphere(void)
{
    msh3Mesh sphere;
    msh3PDTreeMesh* pTreeSphere;
    sphere.LoadMeshFromSTLFile(workingDir + "Sphere.STL");
    // construct PD-Tree
    pTreeSphere = new msh3PDTreeMesh(sphere, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgSphere= new msh3AlgPDTreeCPMesh(pTreeSphere);
    pTreeSphere->SetSearchAlgorithm(pAlgSphere);


    tip.Assign(9.0, 6.0, 1.5);
    normal = tip - vct3(5.0, 5.0, 5.0);
    closestPoint = vct3(5.0, 5.0, 5.0) + normal.NormalizedSelf().Multiply(5.0);
    numIntersected = pTreeSphere->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreeSphere->Mesh->MergeEdgePoint(faceIdx, tip);
    int matchedCount = 0;
    for (auto it : faceIdx) {
        if (pTreeSphere->Mesh->activeNormal[it].NormalizedSelf()
            .AlmostEqual(normal.NormalizedSelf(), 1E-1)) {
            matchedCount ++;
        }
    }
    CPPUNIT_ASSERT_GREATEREQUAL(1,matchedCount);
}

// concave - pyramid
void mshAlgPDTreeCPMeshTest::PyramidTop(void)
{
    msh3Mesh pyramid;
    msh3PDTreeMesh* pTreePyramid;
    pyramid.LoadMeshFromSTLFile(workingDir + "Pyramid.STL");
    // construct PD-Tree
    pTreePyramid= new msh3PDTreeMesh(pyramid, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgPyramid= new msh3AlgPDTreeCPMesh(pTreePyramid);
    pTreePyramid->SetSearchAlgorithm(pAlgPyramid);


    // side
    tip.Assign(5.0, 11.0, 5.0);
    closestPoint = vct3(5.0, 10.0, 5.0);
    normal = tip - closestPoint;
    numIntersected = pTreePyramid->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreePyramid->Mesh->MergeEdgePoint(faceIdx, tip);
    int matchedCount = 0;
    for (auto it : faceIdx) {
        if (pTreePyramid->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal.NormalizedSelf(), 1E-1)) {
            matchedCount ++;
        }
        std::cout << "face id " << it << " with CP location " << pTreePyramid->Mesh->cpLocation[it] << std::endl;
        std::cout << "normal " << pTreePyramid->Mesh->activeNormal[it] << std::endl;
    }
    CPPUNIT_ASSERT_EQUAL(1, matchedCount);
}

void mshAlgPDTreeCPMeshTest::PyramidSide(void)
{
    msh3Mesh pyramid;
    msh3PDTreeMesh* pTreePyramid;
    pyramid.LoadMeshFromSTLFile(workingDir + "Pyramid.STL");
    // construct PD-Tree
    pTreePyramid= new msh3PDTreeMesh(pyramid, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgPyramid= new msh3AlgPDTreeCPMesh(pTreePyramid);
    pTreePyramid->SetSearchAlgorithm(pAlgPyramid);


    // side
    tip.Assign(5.0, 5.0, 2.0);
    normal.Assign(0.000000e+00, 4.472136e-01, -8.944272e-01);
    numIntersected = pTreePyramid->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreePyramid->Mesh->MergeEdgePoint(faceIdx, tip);
    int matchedCount = 0;
    for (auto it : faceIdx) {
        if (pTreePyramid->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal.NormalizedSelf(), 1E-1)) {
            matchedCount ++;
        }
        std::cout << "face id " << it << " with CP location " << pTreePyramid->Mesh->cpLocation[it] << std::endl;
        std::cout << "normal " << pTreePyramid->Mesh->activeNormal[it] << std::endl;
    }
    CPPUNIT_ASSERT_EQUAL(matchedCount, 1);
}

void mshAlgPDTreeCPMeshTest::PyramidBottom(void)
{
    msh3Mesh pyramid;
    msh3PDTreeMesh* pTreePyramid;
    pyramid.LoadMeshFromSTLFile(workingDir + "Pyramid.STL");
    // construct PD-Tree
    pTreePyramid= new msh3PDTreeMesh(pyramid, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgPyramid= new msh3AlgPDTreeCPMesh(pTreePyramid);
    pTreePyramid->SetSearchAlgorithm(pAlgPyramid);

    // bottom
    tip.Assign(3.0, -1.0, 6.0);
    normal.Assign(0.0, -1.0, 0.0);
    numIntersected = pTreePyramid->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreePyramid->Mesh->MergeEdgePoint(faceIdx, tip);
    int matchedCount = 0;
    for (auto it : faceIdx) {
        if (pTreePyramid->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal.NormalizedSelf(), 1E-1)) {
            matchedCount ++;
        }
        std::cout << "face id " << it << " with CP location " << pTreePyramid->Mesh->cpLocation[it] << std::endl;
        std::cout << "normal " << pTreePyramid->Mesh->activeNormal[it] << std::endl;
    }
    CPPUNIT_ASSERT_EQUAL(matchedCount, 1);
}

// convex - Cylindricaltunnel
void mshAlgPDTreeCPMeshTest::CylindricalTunnel(void)
{
    msh3Mesh tunnel;
    msh3PDTreeMesh* pTreeTunnel;
    tunnel.LoadMeshFromSTLFile(workingDir + "CylindricalTunnel.STL");
    // construct PD-Tree
    pTreeTunnel = new msh3PDTreeMesh(tunnel, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgTunnel= new msh3AlgPDTreeCPMesh(pTreeTunnel);
    pTreeTunnel->SetSearchAlgorithm(pAlgTunnel);

    tip.Assign(3.0, 5.0, 3.0);
    normal = vct3(5, 5, 5) - tip;
    numIntersected = pTreeTunnel->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreeTunnel->Mesh->MergeEdgePoint(faceIdx, tip);
    int matchedCount = 0;
    for (auto it : faceIdx) {
        if (pTreeTunnel->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal.NormalizedSelf(), 1E-1)) {
            matchedCount ++;
        }
        std::cout << "face id " << it << " with CP location " << pTreeTunnel->Mesh->cpLocation[it] << std::endl;
        std::cout << "normal " << pTreeTunnel->Mesh->activeNormal[it] << std::endl;
    }
    CPPUNIT_ASSERT(matchedCount > 1);
}

// concave and convex - Crown
void mshAlgPDTreeCPMeshTest::CrownValley(void)
{
    msh3Mesh crown;
    msh3PDTreeMesh* pTreecrown;
    crown.LoadMeshFromSTLFile(workingDir + "Crown.STL");
    // construct PD-Tree
    pTreecrown = new msh3PDTreeMesh(crown, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgcrown= new msh3AlgPDTreeCPMesh(pTreecrown);
    pTreecrown->SetSearchAlgorithm(pAlgcrown);

    tip.Assign(2.5, 5.0, 0.0);
    normal.Assign(8.944272e-01, 0.000000e+00, -4.472136e-01);
    vct3 normal2(-8.944272e-01, 0.000000e+00, -4.472136e-01);
    boundingDistance = 3;
    numIntersected = pTreecrown->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreecrown->Mesh->MergeEdgePoint(faceIdx, tip);
    int matchedCount = 0;
    for (auto it : faceIdx) {
        if (pTreecrown->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal.NormalizedSelf(), 1E-1)) {
            matchedCount ++;
        }
        if (pTreecrown->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal2.NormalizedSelf(), 1E-1)) {
            matchedCount ++;
        }
        std::cout << "face id " << it << " with CP location " << pTreecrown->Mesh->cpLocation[it] << std::endl;
        std::cout << "normal " << pTreecrown->Mesh->activeNormal[it] << std::endl;
    }
    CPPUNIT_ASSERT_EQUAL(matchedCount, 2);

}

void mshAlgPDTreeCPMeshTest::CrownPeak(void)
{
    msh3Mesh crown;
    msh3PDTreeMesh* pTreecrown;
    crown.LoadMeshFromSTLFile(workingDir + "Crown.STL");
    // construct PD-Tree
    pTreecrown = new msh3PDTreeMesh(crown, nThresh, diagThresh);
    msh3AlgPDTreeCPMesh* pAlgcrown= new msh3AlgPDTreeCPMesh(pTreecrown);
    pTreecrown->SetSearchAlgorithm(pAlgcrown);

    tip.Assign(5.0, 5.0, -0.5);
    normal.Assign(8.944272e-01, 0.000000e+00, -4.472136e-01);
    vct3 normal2(-8.944272e-01, 0.000000e+00, -4.472136e-01);
    vct3 normal3(0.0, 0.0, -1.0);
    boundingDistance = 5;
    numIntersected = pTreecrown->FindIntersectedPoints(tip, boundingDistance, faceIdx);

    pTreecrown->Mesh->MergeEdgePoint(faceIdx, tip);
    int matchedCount = 0;
    for (auto it : faceIdx) {
        if (pTreecrown->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal.NormalizedSelf(), 1E-1)) {
            matchedCount ++;
        }
        if (pTreecrown->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal2.NormalizedSelf(), 1E-1)) {
            matchedCount ++;
        }
        if (pTreecrown->Mesh->activeNormal[it].NormalizedSelf().AlmostEqual(normal3.NormalizedSelf(), 1E-1)) {
            matchedCount ++;
        }
        std::cout << "face id " << it << " with CP location " << pTreecrown->Mesh->cpLocation[it] << std::endl;
        std::cout << "normal " << pTreecrown->Mesh->activeNormal[it] << std::endl;
    }
    CPPUNIT_ASSERT(matchedCount >= 3);
}


void mshAlgPDTreeCPMeshTest::ResetMeshConstraintValues(void)
{
    msh3Mesh mesh;
    std::string loadMeshPath = workingDir + "Cube.STL";
    mesh.LoadMeshFromSTLFile(loadMeshPath);

    mesh.ResetMeshConstraintValues();
    for (auto it: mesh.closestPoint) {
        CPPUNIT_ASSERT(it.AlmostEqual(vct3(0.0, 0.0, 0.0)));
    }
    for (auto it: mesh.cpLocation) {
        CPPUNIT_ASSERT_EQUAL(it, 0);
    }
    for (auto it: mesh.activeNormal) {
        CPPUNIT_ASSERT(it.AlmostEqual(vct3(0.0, 0.0, 0.0)));
    }
    for (auto it: mesh.distance) {
        CPPUNIT_ASSERT_EQUAL(it, 0.0);
    }
}

void mshAlgPDTreeCPMeshTest::ConvertUnit(void)
{
    msh3Mesh meshInM(true);
    std::string loadMeshPath = workingDir + "Cube.STL";
    meshInM.LoadMeshFromSTLFile(loadMeshPath);

    msh3Mesh meshInMM(false);
    meshInMM.LoadMeshFromSTLFile(loadMeshPath);

    for (size_t i = 0; i < meshInM.vertices.size(); i ++) {
        CPPUNIT_ASSERT_EQUAL(meshInM.vertices[i]*1000.0, meshInMM.vertices[i]);
    }
    for (size_t i = 0; i < meshInM.faceNormals.size(); i ++) {
        CPPUNIT_ASSERT_EQUAL(meshInM.faceNormals[i], meshInMM.faceNormals[i]);
    }
    for (size_t i = 0; i < meshInM.faces.size(); i ++) {
        CPPUNIT_ASSERT_EQUAL(meshInM.faces[i], meshInMM.faces[i]);
    }
    for (size_t i = 0; i < meshInM.faceNeighbors.size(); i ++) {
        CPPUNIT_ASSERT_EQUAL(meshInM.faceNeighbors[i], meshInMM.faceNeighbors[i]);
    }
}


void mshAlgPDTreeCPMeshTest::TransformModel(void)
{
    msh3Mesh mesh;
    std::string loadMeshPath = workingDir + "Cube.STL";
    mesh.LoadMeshFromSTLFile(loadMeshPath);

    msh3Mesh meshTransformed;
    meshTransformed.LoadMeshFromSTLFile(loadMeshPath);

    // identity
    vctFrm4x4 identity;
    meshTransformed.TransformTriangle(identity);

    for (size_t i = 0; i < mesh.vertices.size(); i ++) {
        CPPUNIT_ASSERT_EQUAL(mesh.vertices[i], meshTransformed.vertices[i]);
    }
    for (size_t i = 0; i < mesh.faceNormals.size(); i ++) {
        CPPUNIT_ASSERT_EQUAL(mesh.faceNormals[i], meshTransformed.faceNormals[i]);
    }
    for (size_t i = 0; i < mesh.faces.size(); i ++) {
        CPPUNIT_ASSERT_EQUAL(mesh.faces[i], meshTransformed.faces[i]);
    }
    for (size_t i = 0; i < mesh.faceNeighbors.size(); i ++) {
        CPPUNIT_ASSERT_EQUAL(mesh.faceNeighbors[i], meshTransformed.faceNeighbors[i]);
    }

    // translation + rotation
    vctFrm4x4 transformation;
    vct3 translation(-1.0, 2.0, 3.0);
    msh3Mesh meshTransformed2;
    meshTransformed2.LoadMeshFromSTLFile(loadMeshPath);
    transformation.Assign(identity);
    transformation.Rotation().From(vctAxAnRot3(vct3(1.0, 0.0, 0.0), cmnPI)); // rotate around x by 180, y -> -y, z -> -z
    transformation.Translation().Assign(translation);
    meshTransformed2.TransformTriangle(transformation);
    for (size_t i = 0; i < mesh.vertices.size(); i ++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(mesh.vertices[i][0] + translation[0],
                                     meshTransformed2.vertices[i][0],
                                     1E-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(mesh.vertices[i][1] * -1.0 + translation[1],
                                     meshTransformed2.vertices[i][1],
                                     1E-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(mesh.vertices[i][2] * -1.0 + translation[2],
                                     meshTransformed2.vertices[i][2],
                                     1E-6);
    }
    for (size_t i = 0; i < mesh.faceNormals.size(); i ++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(mesh.faceNormals[i][0],
                                     meshTransformed2.faceNormals[i][0],
                                     1E-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(mesh.faceNormals[i][1] * -1.0,
                                     meshTransformed2.faceNormals[i][1],
                                     1E-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(mesh.faceNormals[i][2] * -1.0,
                                     meshTransformed2.faceNormals[i][2],
                                     1E-6);
    }
}

void mshAlgPDTreeCPMeshTest::Round3(void)
{
    double f;

    // just at the 3rd decimal place
    f = 0.001;
    CPPUNIT_ASSERT_EQUAL(round3(f), 0.001);

    // more than 3 decimal places, round up
    f = 0.0009;
    CPPUNIT_ASSERT_EQUAL(round3(f), 0.001);

    // more than 3 decimal places, round down
    f = 0.0001;
    CPPUNIT_ASSERT_EQUAL(round3(f), 0.000);

    // way more than 3 decimal places
    f = 1E-6;
    CPPUNIT_ASSERT_EQUAL(round3(f), 0.000);

    // more than 3 decimal places and with many significant digits
    f = 12345.12345;
    CPPUNIT_ASSERT_EQUAL(round3(f), 12345.123);

    // less than 3 decimal places
    f = 0.02;
    CPPUNIT_ASSERT_EQUAL(round3(f), f);

    // way less than 3 decimal places
    f = 1E6;
    CPPUNIT_ASSERT_EQUAL(round3(f), f);
}
