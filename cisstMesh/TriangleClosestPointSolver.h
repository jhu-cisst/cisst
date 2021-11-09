/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// ****************************************************************************
//
//    Copyright (c) 2015, Seth Billings
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

#ifndef H_TriangleClosestPointSolver
#define H_TriangleClosestPointSolver

#include <cisstMesh/msh3Utilities.h>
#include <cisstMesh/msh3Mesh.h>

// Always include last!
#include <cisstMesh/mshExport.h>

class CISST_EXPORT TriangleClosestPointSolver
{
 protected:

    vctDynamicVector<vct3>      vertices;     // 3D coordinates for all vertices
    vctDynamicVector<vctInt3>   triangles;    // global indexes for each triangle vertex

    // xfms to local triangle coords
    vctDynamicVector<vctFrm3>   triXfm;
    vctDynamicVector<vctFrm3>   triXfmInv;

    // NOTE: P1 is always the origin
    const vct2 P1;                    // in-plane coords of the xfmd first triangle vertices is always (0,0)
    vctDynamicVector<vct2>      P2;   // in-plane coords of the xfmd second triangle vertices
    vctDynamicVector<vct2>      P3;   // in-plane coords of the xfmd third triangle vertices
    vctDynamicVector<vct2>      P1P3; // directed unit vector from P1 to P3
    vctDynamicVector<vct2>      P2P3; // directed unit vector from P2 to P3

    const vct2 E12;                   // edge normal direction for edge P1P3 is always (-1,0)
    vctDynamicVector<vct2>      E13;  // edge normal direction for edge P1P3
    vctDynamicVector<vct2>      E23;  // edge normal direction for edge P2P3

 public:

    // constructor without precomputations
    TriangleClosestPointSolver() {};

    // constructor with mesh-based precomputations
    TriangleClosestPointSolver(const msh3Mesh &mesh);

    // constructor with mesh-based precomputations
    TriangleClosestPointSolver(const vctDynamicVector<vct3> &vertices,
                               const vctDynamicVector<vctInt3> &triangles);

    // initializes the triangle object with a mesh and precomputes
    // triangle properties across the mesh, enabling more efficient
    // repeated closest point searches for triangles belonging to the mesh
    void init(const vctDynamicVector<vct3> &vertices,
              const vctDynamicVector<vctInt3> &triangles);

    // most efficient routine, which uses triangle properties pre-computed from a mesh
    void FindClosestPointOnTriangle(const vct3 &point,
                                    int triangleIndex,
                                    vct3 &closestPoint);

    int FindClosestPointOnTriangleWithEdgeDetection(const vct3 &point,
                                                    int triangleIndex,
                                                    vct3 &closestPoint);

    // this routine is for one-off triangles that do not belong to the
    // a pre-initilized mesh; it does not use any pre-computed triangle properties
    void FindClosestPointOnTriangle(const vct3 &point,
                                    const vct3 &v1,
                                    const vct3 &v2,
                                    const vct3 &v3,
                                    vct3 &closestPoint);

    // where M is the noise covariance of Xp
    //  and inv(M) = N'N
    void FindMostLikelyPointOnTriangle(const vct3 &point,
                                       const vct3 &v0,
                                       const vct3 &v1,
                                       const vct3 &v2,
                                       const vct3x3 &N, const vct3x3 &Ninv,
                                       vct3 &closestPoint);

    // where M is the noise covariance of Xp
    //  and inv(M) = N'N
    void FindMostLikelyPointOnTriangle(const vct3 &point,
                                       int triangleIndex,
                                       const vct3x3 &N, const vct3x3 &Ninv,
                                       vct3 &closestPoint);

 protected:

    // return true if on edge or corner, return false if within the triangle
    int FindClosestPointOnTriangle(const vct3 &point,
                                   vct3 &closestPoint,
                                   const vctFrm3 &triXfm,
                                   const vctFrm3 &triXfmInv,
                                   const vct2 &P2,
                                   const vct2 &P3,
                                   const vct2 &P1P3,
                                   const vct2 &P2P3,
                                   const vct2 &E13,
                                   const vct2 &E23);

    // computes xfm to local triangle coordinates such that
    //   P1   at origin
    //   P2   on z-axis
    //   P3   on xy-plane (1st or 4th quadrant)
    //
    //   where P1, P2, P3 are the xfmd triangle vertices
    //
    vctFrm3 computeTriangleXfm(const vct3 &v1, const vct3 &v2, const vct3 &v3);
    vctFrm3 computeTriangleXfm(int triangleIndex);

    // edge equation determines whether point xy lies to the left or right
    //  of the edge that passes through XY with slope dxdy
    //
    // NOTE: this assumes a directed (vectorized) edge, with edge direction
    //       pointing along dxdy; the left / right status is relative to
    //       this defined direction
    //
    // returns:
    //   0     xy is on edge
    //   < 0   xy lies left of edge
    //   > 0   xy lies right of edge
    //
    inline double E(const vct2 &xy, const vct2 &XY, const vct2 &dxdy)
    {
        return round6((xy[0]-XY[0])*dxdy[1] - (xy[1]-XY[1])*dxdy[0]);
    }

};

#endif
