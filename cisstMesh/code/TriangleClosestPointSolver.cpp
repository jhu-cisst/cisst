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

#include <cisstMesh/TriangleClosestPointSolver.h>

TriangleClosestPointSolver::TriangleClosestPointSolver(const msh3Mesh &mesh) :
    P1(0.0,0.0),
    E12(-1.0, 0.0)
{
    init(mesh.vertices, mesh.faces);
}


TriangleClosestPointSolver::TriangleClosestPointSolver(const vctDynamicVector<vct3> &vertices_,
                                                       const vctDynamicVector<vctInt3> &triangles_) :
    P1(0.0, 0.0),
    E12(-1.0, 0.0)
{
    init(vertices_, triangles_);
}

void TriangleClosestPointSolver::init(const vctDynamicVector<vct3> &vertices_,
                                      const vctDynamicVector<vctInt3> &triangles_)
{
    vertices = vertices_;
    triangles = triangles_;

    // precompute properties for each triangle
    size_t numTriangles = triangles.size();
    triXfm.SetSize(numTriangles);
    triXfmInv.SetSize(numTriangles);
    P2.SetSize(numTriangles);
    P3.SetSize(numTriangles);
    E13.SetSize(numTriangles);
    E23.SetSize(numTriangles);
    P1P3.SetSize(numTriangles);
    P2P3.SetSize(numTriangles);
    for (int triIdx = 0; triIdx < (int)triangles.size(); triIdx++) {
        triXfm[triIdx] = computeTriangleXfm(triIdx);
        triXfmInv[triIdx] = triXfm[triIdx].Inverse();

        // compute the y coordinate of P2 which lies on the y-axis
        vct3 tmp = triXfm[triIdx] * vertices[triangles[triIdx][1]];
        P2[triIdx].Assign(tmp[0],tmp[1]);
        if (tmp[0] > 1e-10 || tmp[2] > 1e-10) {
            std::cout << "WARNING: P2.x or P2.z for xfmd triangle index " << triIdx <<
                " is greater than zero with 3D coord: " << tmp << std::endl;
        }

        // compute the xy coordinates of P3 which lies on the xy-axis
        tmp = triXfm[triIdx] * vertices[triangles[triIdx][2]];
        P3[triIdx].Assign(tmp[0],tmp[1]);
        if (tmp[2] > 1e-10) {
            std::cout << "WARNING: P3.z for xfmd triangle index " << triIdx <<
                " is greater than zero with 3D coord: " << tmp << std::endl;
        }

        P1P3[triIdx].Assign(P3[triIdx]-P1);
        P2P3[triIdx].Assign(P3[triIdx]-P2[triIdx]);
        P1P3[triIdx].NormalizedSelf();
        P2P3[triIdx].NormalizedSelf();

        // compute the in-plane xfmd edge normal directions pointing
        //  outward from the triangle
        //  do this by rotating each directed edge outwards by 90 degrees
        // Note: E12 is always (-1,0)
        E13[triIdx].Assign(P3[triIdx][1],-P3[triIdx][0]);
        E23[triIdx].Assign(-P2P3[triIdx][1],P2P3[triIdx][0]);
    }
}


vctFrm3 TriangleClosestPointSolver::computeTriangleXfm(int triangleIndex)
{
    vct3 v1 = vertices[triangles[triangleIndex][0]];
    vct3 v2 = vertices[triangles[triangleIndex][1]];
    vct3 v3 = vertices[triangles[triangleIndex][2]];

    return computeTriangleXfm(v1, v2, v3);
}


// TODO: make this routine safe for degenerate triangles
vctFrm3 TriangleClosestPointSolver::computeTriangleXfm(const vct3 &v1,
                                                       const vct3 &v2,
                                                       const vct3 &v3)
{
    // compute transformation such that
    //
    //   P1   at origin
    //   P2   on y-axis
    //   P3   on xy-plane (1st or 4th quadrant)
    //
    //   where P1, P2, P3 are the xfmd triangle vertices
    //
    vct3 yaxis = (v2 - v1).Normalized();
    vct3 zaxis = vctCrossProduct((v3-v1).Normalized(),yaxis).Normalized();
    vct3 xaxis = vctCrossProduct(yaxis,zaxis).Normalized();

    vctRot3 R;
    R.Row(0) = xaxis;
    R.Row(1) = yaxis;
    R.Row(2) = zaxis;

    vctFrm3 xfm;
    xfm.Rotation() = R;
    xfm.Translation() = -R * v1;

    return xfm;
}

void TriangleClosestPointSolver::FindClosestPointOnTriangle(const vct3 &point,
                                                            int triangleIndex,
                                                            vct3 &closestPoint)
{
    FindClosestPointOnTriangle(
                               point,
                               closestPoint,
                               triXfm[triangleIndex],
                               triXfmInv[triangleIndex],
                               P2[triangleIndex],
                               P3[triangleIndex],
                               P1P3[triangleIndex],
                               P2P3[triangleIndex],
                               E13[triangleIndex],
                               E23[triangleIndex]);
};

void TriangleClosestPointSolver::FindClosestPointOnTriangle(const vct3 &point,
                                                            const vct3 &v1,
                                                            const vct3 &v2,
                                                            const vct3 &v3,
                                                            vct3 &closestPoint)
{
    //--- precomputations ---//

    vctFrm3 triXfm = computeTriangleXfm(v1,v2,v3);

    // compute the y coordinate of P2 which lies on the y-axis
    vct3 tmp = triXfm * v2;
    vct2 P2(tmp[0], tmp[1]);
    //if (tmp[0] > 1e-10 || tmp[2] > 1e-10)
    //{
    //  std::cout << "WARNING: P2.x or P2.z is greater than zero with 3D coord: " << tmp << std::endl;
    //}

    // compute the xy coordinates of P3 which lies on the xy-axis
    tmp = triXfm * v3;
    vct2 P3(tmp[0], tmp[1]);
    //if (tmp[2] > 1e-10)
    //{
    //  std::cout << "WARNING: P3.z is greater than zero with 3D coord: " << tmp << std::endl;
    //}

    vct2 P1P3(P3 - P1);
    vct2 P2P3(P3 - P2);
    P1P3.NormalizedSelf();
    P2P3.NormalizedSelf();

    // compute the in-plane xfmd edge normal directions pointing
    //  outward from the triangle
    //  do this by rotating each directed edge outwards by 90 degrees
    // Note: E12 is always (-1,0)
    vct2 E13(P3[1], -P3[0]);
    vct2 E23(-P2P3[1], P2P3[0]);


    //--- compute closeset point on triangle ---//

    FindClosestPointOnTriangle(point, closestPoint,
                               triXfm, triXfm.Inverse(),
                               P2, P3,
                               P1P3, P2P3,
                               E13, E23);
}

int TriangleClosestPointSolver::FindClosestPointOnTriangleWithEdgeDetection(const vct3 &point,
                                                                            int triangleIndex,
                                                                            vct3 &closestPoint)
{
    return FindClosestPointOnTriangle(point,
                                      closestPoint,
                                      triXfm[triangleIndex],
                                      triXfmInv[triangleIndex],
                                      P2[triangleIndex],
                                      P3[triangleIndex],
                                      P1P3[triangleIndex],
                                      P2P3[triangleIndex],
                                      E13[triangleIndex],
                                      E23[triangleIndex]);
};

int TriangleClosestPointSolver::FindClosestPointOnTriangle(const vct3 &point,
                                                           vct3 &closestPoint,
                                                           const vctFrm3 &triXfm,
                                                           const vctFrm3 &triXfmInv,
                                                           const vct2 &P2,
                                                           const vct2 &P3,
                                                           const vct2 &P1P3,
                                                           const vct2 &P2P3,
                                                           const vct2 &E13,
                                                           const vct2 &E23 )
{
    // This closest point computation is based on the
    //  2D algorithm described in:
    //
    //    "3D Distance from a Point to a Triangle"
    //    Tehnical Report CSR-5-95
    //    Dept of Computer Science, Univ of Wales Swansea
    //    Mark W. Jones
    //

    vct3 closest_local;
    vct2 tmp;
    double x,y;

    // transform point to local triangle coordinates
    vct3 pt_local(triXfm * point);

    x = pt_local[0];
    y = pt_local[1];

    // extract the in-plane component
    vct2 pt_2d(x,y);

    // edge P1P2
    //  special case since P1 is origin and P2 is on y-axis
    if (x <= 0.0) { // outside edge P1P2
        if (y <= 0.0) {
            // left of edge normal P1P2 extending from P1
            if (E(pt_2d,P1,E13) >= 0.0) {
                // right of edge normal P1P3 extending from P1
                //  => closest to vertex P1
                closest_local.Assign( P1[0], P1[1], 0.0 );
                closestPoint.Assign(triXfmInv * closest_local);
                return msh3Mesh::V1;
            }
        }
        else if (y >= P2[1]) {
            // right of edge normal P1P2 extending from P2
            if (E(pt_2d,P2,E23) <= 0.0) {
                // left of edge normal P2P3 extending from P2
                //  => closest to vertex P2
                closest_local.Assign( P2[0], P2[1], 0.0 );
                closestPoint.Assign(triXfmInv * closest_local);
                return msh3Mesh::V2;
            }
        }
        else {
            // closest to edge segment P1P2
            // since edge P1P2 is on the y-axis, the projection
            // onto this edge is simply the y coordinate of the
            // point in local coordinates
            closest_local.Assign(0.0, y, 0.0);
            closestPoint.Assign(triXfmInv * closest_local);
            return msh3Mesh::V1V2;
        }
    }

    // edge P1P3
    if (E(pt_2d,P1,P1P3) >= 0.0) {
        // outside edge P1P3
        if (E(pt_2d,P1,E13) >= 0.0) {
            // right of edge normal P1P3 extending from P1
            //  => closest to vertex P1
            //
            //  NOTE: since x > 0.0 we already know that the point
            //        is left of edge normal P1P2 extending from P1
            //
            closest_local.Assign( P1[0], P1[1], 0.0 );
            closestPoint.Assign(triXfmInv * closest_local);
            return msh3Mesh::V1;
        }
        else if (E(pt_2d,P3,E13) <= 0.0) {
            // left of edge normal P1P3 extending from P3
            if (E(pt_2d,P3,E23) >= 0.0) {
                // right of edge normal P2P3 extending from P3
                // => closest to vertex P3
                closest_local.Assign( P3[0], P3[1], 0.0 );
                closestPoint.Assign(triXfmInv * closest_local);
                return msh3Mesh::V3;
            }
        }
        else {
            // closest to edge segment P1P3
            // project 2d point to edge P1P3
            //
            //  prj_2d = ((pt_2d - P1) dot P1P3) * P1P3 + P1
            //
            //  NOTE: the projection equation above simplifies
            //        since P1 is the origin
            //
            tmp = vctDotProduct(pt_2d,P1P3) * P1P3;
            closest_local.Assign(tmp[0],tmp[1],0.0);
            closestPoint.Assign(triXfmInv * closest_local);
            return msh3Mesh::V1V3;
        }
    }

    // edge P2P3
    if (E(pt_2d,P2,P2P3) <= 0) {
        // outside edge P2P3
        if (E(pt_2d,P2,E23) <= 0) {
            // left of edge normal P2P3 extending from P2
            //  => closest to vertex P2
            //
            //  NOTE: since x > 0.0 we already know that the point
            //        is right of edge normal P1P2 extending from P2
            //
            closest_local.Assign( P2[0], P2[1], 0.0 );
            closestPoint.Assign(triXfmInv * closest_local);
            return msh3Mesh::V2;
        }
        else if (E(pt_2d,P3,E23) >= 0) {
            // right of edge normal P2P3 extending from P3
            //  => closest to vertex P3
            //
            //  NOTE: since the point is inside edge P1P3
            //        we already know that the point
            //        is left of edge normal P1P3 extending from P3
            //
            closest_local.Assign( P3[0], P3[1], 0.0 );
            closestPoint.Assign(triXfmInv * closest_local);
            return msh3Mesh::V3;
        }
        else {
            // closest to edge segment P2P3
            // project 2d point to edge P2P3
            //
            //  prj_2d = ((pt_2d - P2) dot P2P3) * P1P3 + P2
            //
            tmp = (vctDotProduct(pt_2d - P2,P2P3) * P2P3) + P2;
            closest_local.Assign(tmp[0],tmp[1],0.0);
            closestPoint.Assign(triXfmInv * closest_local);
            return msh3Mesh::V2V3;
        }
    }

    // all edges checked
    // point is within the triangle
    // the closest point on the triangle is the projection
    // of the point onto the triangle plane, which in local
    // coordinates is simply the projection to the xy-plane
    closest_local.Assign(pt_2d[0],pt_2d[1],0.0);

    // return the closest point in global coordinates
    closestPoint.Assign(triXfmInv * closest_local);

    return msh3Mesh::IN;
};


void TriangleClosestPointSolver::FindMostLikelyPointOnTriangle(const vct3 &point,
                                                               const vct3 &v0,
                                                               const vct3 &v1,
                                                               const vct3 &v2,
                                                               const vct3x3 &N, const vct3x3 &Ninv,
                                                               vct3 &closestPoint)
{
    // Find closest point on triangle in a Mahalanobis distance sense
    //
    //   Mahalanobis Distance:  sqrt((x-v)'*Minv*(x-v))
    //
    //  Method
    //   1) Translation + Affine transform to spherical space converting
    //      Mahalanobis ellipsoid -> sphere and triangle -> triangle2
    //   2) Find closest point (c') on triangle' to the origin
    //      (using standard Euclidean means)
    //   3) Affine transform c' back to normal coordinates
    static vct3 p0, p1, p2, c;

    // 1: transform triangle to spherical coords
    p0 = N*(v0 - point);
    p1 = N*(v1 - point);
    p2 = N*(v2 - point);

    // 2: find closest point on triangle to origin in spherical coords
    FindClosestPointOnTriangle(vct3(0.0), p0, p1, p2, c);

    // 3: transform closest point back to standard coords
    closestPoint = Ninv*c + point;
}


void TriangleClosestPointSolver::FindMostLikelyPointOnTriangle(const vct3 &point,
                                                               int triangleIndex,
                                                               const vct3x3 &N, const vct3x3 &Ninv,
                                                               vct3 &closestPoint)
{
    vct3 v1 = vertices[triangles[triangleIndex][0]];
    vct3 v2 = vertices[triangles[triangleIndex][1]];
    vct3 v3 = vertices[triangles[triangleIndex][2]];

    FindMostLikelyPointOnTriangle(point,
                                  v1, v2, v3,
                                  N, Ninv,
                                  closestPoint);
}
