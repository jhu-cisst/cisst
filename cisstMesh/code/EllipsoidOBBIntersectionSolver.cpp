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

#include <cisstMesh/EllipsoidOBBIntersectionSolver.h>

#include <assert.h>
#undef NDEBUG       // enable assert in release mode


// chooses between quick escape technique used for node proximity test
//  comment to use ellipsoid projection (tighter bound, more computation)
//  uncomment to use boudning sphere projection (looser bound, less computation)
//  (Sphere method was more efficient for all tests performed)
#define QUICK_ESCAPE_SPHERE


// Test intersection between an ellipsoid and oriented bounding box
//  Given an Ellipsoid defined by:   x'*Minv*x = NodeErrorBound
//  N is the decomposition of Minv:  Minv = N'*N
//  Dmin is the inverse sqrt of the largest eigenvalue of M (used for quick escape test)
//    Dmin = 1/sqrt(D[0])  where D = diag(D[0], D[1], D[2]) is the diagonal matrix of
//    eigenvalues of M arranged largest to smallest
//    In other words, Dmin is the sqrt of the smallest eigenvalue of Minv
//  Note:  Minv = R*D^2*R' = N'*N     M = R*Dinv^2*R' => R' = V', Dinv = sqrt(S)
//         N = D*R'
//         Ninv = R*inv(D)
int EllipsoidOBBIntersectionSolver::TestEllipsoidOBBIntersection(const vct3 &v,
                                                                 const msh3BoundingBox &OBB, const vctFrm3 &Fobb,
                                                                 double NodeErrorBound,
                                                                 const  vct3x3 &N,
                                                                 double Dmin)
{
    // Algorithm is a modification of:
    //  Thomas Larsson, "An Efficient Ellipsoid-OBB Intersection Test"
    //
    // Method:
    //  1) determine faces of oriented bounding box (OBB) visible from ellipse center
    //  2) affine transform to convert: ellipsoid -> sphere and OBB -> parallelpiped
    //  3) Sphere-Parallelpiped intersection test
    //
    // Define Bounding Box face indexes as:
    //  F1: X+  F2: X-
    //  F3: Y+  F4: Y-
    //  F5: Z+  F6: Z-
    //
    // Define Bounding Box vertex positions as:
    //   p[0] = m + Vx + Vy + Vz;       where  m = geometric center
    //   p[1] = m + Vx + Vy - Vz;             Vx = extent along local x axis (Vy & Vz similar)
    //   p[2] = m + Vx - Vy + Vz;
    //   p[3] = m + Vx - Vy - Vz;
    //   p[4] = m - Vx + Vy + Vz;
    //   p[5] = m - Vx + Vy - Vz;
    //   p[6] = m - Vx - Vy + Vz;
    //   p[7] = m - Vx - Vy - Vz;
    //
    // Define edge numbering for each face as:
    //   Note:  edge numbers progress in counterclockwise order around
    //          each face, meaning adjacent edges on a face have adjacent numbers
    //          in the edge numbering assigned below:
    //
    //         Edge0   Edge1   Edge2   Edge3
    //   F1:  [p0, p2] [p2, p3] [p3, p1] [p1, p0]
    //   F2:  [p6, p4] [p4, p5] [p5, p7] [p7, p6]
    //   F3:  [p4, p0] [p0, p1] [p1, p5] [p5, p4]
    //   F4:  [p2, p6] [p6, p7] [p7, p3] [p3, p2]
    //   F5:  [p4, p6] [p6, p2] [p2, p0] [p0, p4]
    //   F6:  [p1, p3] [p3, p7] [p7, p5] [p5, p1]
    //
    //  Note:  to understand this code, it helps to draw a picture of a bounding box
    //         using the face, vertex, and edge numbering as defined above
    //  Note:  face, vertex, and edge numbers for parallelpiped elements are derived
    //         directly from the affine mapped bounding box elements
    //
    vct3 Fv;
    vctFrm3 Finv;
    vct3x3 Ns;
    vct3 m;
    vct3 nx, ny, nz;
    vct3 Vx_, Vy_, Vz_;
    vct3 Vxpos, Vypos, Vzpos;
    vct3 Vxneg, Vyneg, Vzneg;
    vct3 m_Vxp, m_Vxn;
    vct3 Vyp_Vzp, Vyp_Vzn, Vyn_Vzn, Vyn_Vzp;
    vct3 Nx, Ny, Nz;
    vct3 p0, p1, p2, p3, p4, p5, p6, p7;
    // vct3x3 Anode_sphere;
    // vct3 Tnode_sphere;
    // vct3 Edge0Norm, Edge1Norm;
    double S0, S1, S2;
    int vsblFaces[3], numFacesProcessed, Fi;
    bool rv;

    double sqrtNodeErrorBound = sqrt(NodeErrorBound);


    // === Determine Visible Faces of OBB === //
    //  (and quick escape test)

    // Determine visible faces of OBB by projecting ellipsoid center
    //  onto each axis of the bounding box
    int numVsblFaces = 0;
    Fv = Fobb*v;    // center of ellipsoid in OBB coords

#ifdef QUICK_ESCAPE_SPHERE
    // Quick Escape: Sphere projection
    //  project ellipsoid bounding sphere onto the node axis
    S0 = S1 = S2 = (sqrtNodeErrorBound/Dmin);
#else
    std::cout << "ERROR: quick escape by ellipsoid projection no longer supported" << std::endl;
    assert(0);
    //// Quick Escape: Ellipsoid projection
    ////  project ellipsoid itself onto the node axis
    //Ns = sqrtNodeErrorBound * NinvT * node->F.Rotation().Transpose();
    //// since we are working in the node coords, the projections
    ////  values are easy to compute as multiplications of
    ////  Ns and the standard axis unit vectors
    ////S0 = (Ns*vct3(1, 0, 0)).Norm();
    //S0 = Ns.Column(0).Norm();
    //S1 = Ns.Column(1).Norm();
    //S2 = Ns.Column(2).Norm();
#endif

    if (Fv[0] >= OBB.MaxCorner()[0]) {
        // Face Fx+ is visible from sample point
        if (Fv[0] > OBB.MaxCorner()[0] + S0) {
            // visible face is out-of-range
            return 0;
        }
        vsblFaces[numVsblFaces++] = 1;
    }
    else if (Fv[0] <= OBB.MinCorner()[0]) {
        // Face Fx- is visible
        if (Fv[0] < OBB.MinCorner()[0] - S0) {
            // visible face out-of-range
            return 0;
        }
        vsblFaces[numVsblFaces++] = 2;
    }
    if (Fv[1] >= OBB.MaxCorner()[1]) {
        // Face Fy+ is visible
        if (Fv[1] > OBB.MaxCorner()[1] + S1) {
            // visible face out-of-range
            return 0;
        }
        vsblFaces[numVsblFaces++] = 3;
    }
    else if (Fv[1] <= OBB.MinCorner()[1]) {
        // Face Fy- is visible
        if (Fv[1] < OBB.MinCorner()[1] - S1) {
            // visible face out-of-range
            return 0;
        }
        vsblFaces[numVsblFaces++] = 4;
    }
    if (Fv[2] >= OBB.MaxCorner()[2]) {
        // Face Fz+ is visible
        if (Fv[2] > OBB.MaxCorner()[2] + S2) {
            // visible face out-of-range
            return 0;
        }
        vsblFaces[numVsblFaces++] = 5;
    }
    else if (Fv[2] <= OBB.MinCorner()[2]) {
        // Face Fz- is visible
        if (Fv[2] < OBB.MinCorner()[2] - S2) {
            // visible face out-of-range
            return 0;
        }
        vsblFaces[numVsblFaces++] = 6;
    }
    if (numVsblFaces == 0) {
        // sample point lies w/in node => ellipsoid intersects OBB
        return 1;
    }


    //=== Affine Xfm Ellipsoid -> Sphere ===//

    // Apply affine transform to convert ellipsoid to a sphere
    //  and convert OBB to a parallelpiped;
    //  then apply translation to move center of sphere to the origin
    Finv = Fobb.Inverse();
    nx = Finv.Rotation().Column(0);   // OBB axis in world coords
    ny = Finv.Rotation().Column(1);   //  ''
    nz = Finv.Rotation().Column(2);   //  ''
    m = Finv.Translation();           // OBB origin in world coords
    m = N*(m - v);                // OBB origin in sphere coords

    //// Transforms points from sphere coords to node coords [A, t]
    //Anode_sphere = node->F.Rotation()*Ninv;                       // affine
    //Tnode_sphere = node->F.Rotation()*v + node->F.Translation();  // offset

    // parallelpiped axis
    //  Note: axis unit vectors are no longer oriented to the face normals
    //        due to the skewed nature of the parallelpiped (i.e. due to affine skew)
    Vx_ = N*nx;     // affine xfmd OBB axis (do not make these unit vectors!)
    Vy_ = N*ny;
    Vz_ = N*nz;

    //  Note: we cannot assume that the OBB have equal extents along the +/- dir
    //        of each coordinate axis, because the origin for the node is based
    //        on statistics of a single vertex from each triangle.
    //        We may, however, assume that the extents are positive along each
    //        axis direction, i.e. that the origin lies within the node.
    Vxpos = OBB.MaxCorner()[0]*Vx_;  // parallelpiped extents along each axis
    Vypos = OBB.MaxCorner()[1]*Vy_;  //  ''
    Vzpos = OBB.MaxCorner()[2]*Vz_;  //  ''
    Vxneg = OBB.MinCorner()[0]*Vx_;  //  ''
    Vyneg = OBB.MinCorner()[1]*Vy_;  //  ''
    Vzneg = OBB.MinCorner()[2]*Vz_;  //  ''
    m_Vxp = m + Vxpos;              // precompute these for efficiency
    m_Vxn = m + Vxneg;              //  ''
    Vyp_Vzp = Vypos + Vzpos;        //  ''
    Vyp_Vzn = Vypos + Vzneg;        //  ''
    Vyn_Vzp = Vyneg + Vzpos;        //  ''
    Vyn_Vzn = Vyneg + Vzneg;        //  ''
    p0 = m_Vxp + Vyp_Vzp;           // parallelpiped vertices
    p1 = m_Vxp + Vyp_Vzn;           //  ''
    p2 = m_Vxp + Vyn_Vzp;           //  ''
    p3 = m_Vxp + Vyn_Vzn;           //  ''
    p4 = m_Vxn + Vyp_Vzp;           //  ''
    p5 = m_Vxn + Vyp_Vzn;           //  ''
    p6 = m_Vxn + Vyn_Vzp;           //  ''
    p7 = m_Vxn + Vyn_Vzn;           //  ''

    // face normals of parallelpiped
    Nx.Assign(vctCrossProduct(Vy_, Vz_).Normalized());
    Ny.Assign(vctCrossProduct(Vz_, Vx_).Normalized());
    Nz.Assign(vctCrossProduct(Vx_, Vy_).Normalized());


    //=== Check for Sphere/Parallelpiped Intersection ===//

    // Check for Sphere-Parallelpiped Overlap
    //  Note: the code below assumes that visible faces are added to
    //        the queue in order from lowest to highest face index
    numFacesProcessed = 0;
    Fi = vsblFaces[numFacesProcessed++];

    //if (Fi == 1 || Fi == 2)
    //{
    // compute in-plane edge normals for this face pair
    //  Note: these need not be unit vectors
    //Edge0Norm =  Nz - vctDotProduct(Nz, Nx)*Nx;  // in-plane component of Nz
    //Edge1Norm = -Ny + vctDotProduct(Ny, Nx)*Nx;  // in-plane component of -Ny
    if (Fi == 1) {
        // check sphere-face intersection
        rv = IntersectionSphereFace(Nx, p0, p2, p3, p1,
                                    sqrtNodeErrorBound, NodeErrorBound);
        //Edge0Norm, Edge1Norm, -Edge0Norm, -Edge1Norm,
        if (rv) {
            return 1;
        }
        // no intersection => get next visible face
        if (numFacesProcessed == numVsblFaces) {
            return 0;
        }
        Fi = vsblFaces[numFacesProcessed++];
    }
    if (Fi == 2) {
        // check sphere-face intersection
        rv = IntersectionSphereFace(-Nx, p6, p4, p5, p7,
                                    sqrtNodeErrorBound, NodeErrorBound);
        //Edge0Norm, -Edge1Norm, -Edge0Norm, Edge1Norm,
        if (rv) {
            return 1;
        }
        // no intersection => get next visible face
        if (numFacesProcessed == numVsblFaces) {
            return 0;
        }
        Fi = vsblFaces[numFacesProcessed++];
    }
    //}
    //if (Fi == 3 || Fi == 4)
    //{
    // compute in-plane edge normals for this face pair
    //  Note: these need not be unit vectors
    //Edge0Norm = Nz - vctDotProduct(Nz, Ny)*Ny; // in-plane component of Nz
    //Edge1Norm = Nx - vctDotProduct(Nx, Ny)*Ny; // in-plane component of Nx
    if (Fi == 3) {
        // check sphere-face intersection
        rv = IntersectionSphereFace(Ny, p4, p0, p1, p5,
                                    sqrtNodeErrorBound, NodeErrorBound);
        //Edge0Norm, Edge1Norm, -Edge0Norm, -Edge1Norm,
        if (rv) {
            //std::cout << "--> Intersection" << std::endl;
            return 1;
        }
        // no intersection => get next visible face
        if (numFacesProcessed == numVsblFaces) return 0;
        Fi = vsblFaces[numFacesProcessed++];
    }
    if (Fi == 4) {
        // check sphere-face intersection
        rv = IntersectionSphereFace(-Ny, p2, p6, p7, p3,
                                    sqrtNodeErrorBound, NodeErrorBound);
        //Edge0Norm, -Edge1Norm, -Edge0Norm, Edge1Norm,
        if (rv) {
            return 1;
        }
        // no intersection => get next visible face
        if (numFacesProcessed == numVsblFaces) return 0;
        Fi = vsblFaces[numFacesProcessed++];
    }
    //}
    //if (Fi == 5 || Fi == 6)
    //{
    // compute in-plane edge normals for this face pair
    //  Note: these need not be unit vectors
    //Edge0Norm = -Nx + vctDotProduct(Nx, Nz)*Nz;  // in-plane component of -Nx
    //Edge1Norm = -Ny + vctDotProduct(Ny, Nz)*Nz;  // in-plane component of -Ny
    if (Fi == 5) {
        // check sphere-face intersection
        rv = IntersectionSphereFace(Nz, p4, p6, p2, p0,
                                    sqrtNodeErrorBound, NodeErrorBound);
        //Edge0Norm, Edge1Norm, -Edge0Norm, -Edge1Norm,
        if (rv) {
            return 1;
        }
        // no intersection => get next visible face
        if (numFacesProcessed == numVsblFaces) {
            return 0;
        }
        Fi = vsblFaces[numFacesProcessed++];
    }
    if (Fi == 6) {
        // check sphere-face intersection
        rv = IntersectionSphereFace(-Nz, p1, p3, p7, p5,
                                    sqrtNodeErrorBound, NodeErrorBound);
        //-Edge0Norm, Edge1Norm, Edge0Norm, -Edge1Norm,
        if (rv) {
            return 1;
        }
        // no intersection => get next visible face
        if (numFacesProcessed == numVsblFaces) {
            return 0;
        }
    }
    //}

    // should never arrive here
    std::cout << "ERROR: execution should never arrive here" << std::endl;
    assert(0);
    return 0;
}


// Determine if the given parallelpiped face intersects a sphere
//  of given radius centered at the origin
//    n         - face normal (not a unit normal)
//    v0..v3    - face vertices
//    sqrRadius - square radius of sphere
//
//    n0..n3    - in-plane edge normals  (removed)
//
//  Note: vertex ordering is arranged s.t. face edges are numbered as
//        below and egdes follow a CCW sequencing about the face normal.
//    e0  - [v0, v1]
//    e1  - [v1, v2]
//    e2  - [v2, v3]
//    e3  - [v3, v0]
//
bool EllipsoidOBBIntersectionSolver::IntersectionSphereFace(const vct3 &n,
                                                            const vct3 &v0, const vct3 &v1,
                                                            const vct3 &v2, const vct3 &v3,
                                                            double radius, double sqrRadius)
{
    int vsblEdges[2];
    int numVsblEdges, numProcessedEdges;
    double q_signed_mag;
    vct3 q;
    unsigned int maxEl;
    // double radius;
    double n_0, n_1, n_2, nmax;

    // Quick escape
    //  check distance from origin to face plane
    //radius = sqrt(sqrRadius);             // TODO: more efficient to send this as an argument
    q_signed_mag = vctDotProduct(v0, n);   // signed distance to plane
    if (fabs(q_signed_mag) > radius) {
        // sphere does not intersect face
        return false;
    }

    q = q_signed_mag*n;  // projection of origin onto face plane

    //std::cout << "q = [" << q << "]" << std::endl;
    //std::cout << "v(1, :) = [" << v0 << "]" << std::endl;
    //std::cout << "v(2, :) = [" << v1 << "]" << std::endl;
    //std::cout << "v(3, :) = [" << v2 << "]" << std::endl;
    //std::cout << "v(4, :) = [" << v3 << "]" << std::endl;


#if 1
    // Determine edges visible from q (at most 2)
    //  by projecting face to the closest axis-aligned plane
    //  (determined by largest element of n)
    n_0 = fabs(n[0]);
    n_1 = fabs(n[1]);
    n_2 = fabs(n[2]);
    if (n_0 > n_1) {
        nmax = n_0;
        maxEl = 0;
    }
    else {
        nmax = n_1;
        maxEl = 1;
    }
    if (n_2 > nmax) {
        maxEl = 2;
    }
    switch (maxEl) {
    case 0:
        { // project to y-z plane
            //   q0 <-- q[1]
            //   q1 <-- q[2]
            numVsblEdges = FindVisibleEdges(q[1], q[2],
                                            v0[1], v0[2], v1[1], v1[2], v2[1], v2[2], v3[1], v3[2],
                                            //n0[1], n0[2], n1[1], n1[2], n2[1], n2[2], n3[1], n3[2],
                                            vsblEdges, (n[0]>0));
            break;
        }
    case 1:
        { // project to z-x plane
            //   q0 <-- q[2]
            //   q1 <-- q[0]
            numVsblEdges = FindVisibleEdges(q[2], q[0],
                                            v0[2], v0[0], v1[2], v1[0], v2[2], v2[0], v3[2], v3[0],
                                            vsblEdges, (n[1]>0));
            break;
        }
    case 2:
        { // project to x-y plane
            //   q0 <-- q[0]
            //   q1 <-- q[1]
            numVsblEdges = FindVisibleEdges(q[0], q[1],
                                            v0[0], v0[1], v1[0], v1[1], v2[0], v2[1], v3[0], v3[1],
                                            vsblEdges, (n[2]>0));
            break;
        }
    default:
        std::cout << "Code Error!" << std::endl;
        assert(0);
    }

    if (numVsblEdges == 0) {
        // no edges visible from origin projection => projection lies within
        //  face => positive intersection
        // Note: distance to plane is already known to be < sphere radius
        return true;
    }

    // Compute distance from origin to each visible edge
    //  Note: the code below assumes visible edges have been tested
    //        and added to visible edge array in order e0, e2, e1, e3
    numProcessedEdges = 0;
    if (vsblEdges[numProcessedEdges] == 0) {
        // edge 0
        // compute distance to edge 0
        if (SquareDistanceToEdge(v0, v1) < sqrRadius) {
            // positive intersection
            return true;
        }
        // go to next visible edge
        numProcessedEdges++;
        if (numProcessedEdges == numVsblEdges) {
            // no intersection
            return false;
        }
    }
    if (vsblEdges[numProcessedEdges] == 2) {
        // edge 2
        // compute distance to edge 2
        if (SquareDistanceToEdge(v2, v3) < sqrRadius) {
            // positive intersection
            return true;
        }
        // go to next visible edge
        numProcessedEdges++;
        if (numProcessedEdges == numVsblEdges) {
            // no intersection
            return false;
        }
    }
    if (vsblEdges[numProcessedEdges] == 1) {
        // edge 1
        // compute distance to edge 1
        if (SquareDistanceToEdge(v1, v2) < sqrRadius) {
            // positive intersection
            return true;
        }
        // go to next visible edge
        numProcessedEdges++;
        if (numProcessedEdges == numVsblEdges) {
            // no intersection
            return false;
        }
    }
    if (vsblEdges[numProcessedEdges] == 3) {
        // edge 3
        // compute distance to edge 3
        if (SquareDistanceToEdge(v3, v0) < sqrRadius) {
            // positive intersection
            return true;
        }
        // go to next visible edge
        numProcessedEdges++;
        if (numProcessedEdges == numVsblEdges) {
            // no intersection
            return false;
        }
    }

    // should never reach here
    std::cout << "Code Error!" << std::endl;
    assert(0);
    return false;
#else
    // This code is a modification and simplification of the visible edge finding
    //  method; quite unexpectedly, it runs consistently slower than the standard
    //  methodd, thus it is not used.
    //  (Increases a typical registration runtime from 5.7 to 6.7 seconds!)
    // Note:  the undefined variables in this section were presented to this
    //        function as argument variables when this variation is used.

    // Determine edge visibility by transforming q partially back to node coordinates.
    //  Visibility check in this space is trivial since edges are perpendicular and
    //  axis aligned; this method is more efficient because it does not
    //  require computing a new projection for the four corners of the
    //  parallelegram and it also does not require computing the edge normals
    //  following the projection, since the edges are known to be perpendicular
    //  in the node coordinate space.
    // Note: it appears this multiplication takes longer than all the extra
    //       manipulations performed in the standard method.
    vct3 qnode;
    qnode = Anode_sphere*q + Tnode_sphere;
    numVsblEdges = 0;

    //std::cout << "MinCorner = [" << OBB.MinCorner << "]" << std::endl;
    //std::cout << "MaxCorner = [" << OBB.MaxCorner << "]" << std::endl;
    //std::cout << "vnode(1, :) = [" << Anode_sphere*v0 + Tnode_sphere << "]" << std::endl;
    //std::cout << "vnode(2, :) = [" << Anode_sphere*v1 + Tnode_sphere << "]" << std::endl;
    //std::cout << "vnode(3, :) = [" << Anode_sphere*v2 + Tnode_sphere << "]" << std::endl;
    //std::cout << "vnode(4, :) = [" << Anode_sphere*v3 + Tnode_sphere << "]" << std::endl;
    //std::cout << "qnode = [" << qnode << "]" << std::endl;

    switch(FaceNumber) {
    case 1:   // X+ face
        {
            if (qnode[2] > OBB.MaxCorner()[2])  // upper edge
                { // edge 0 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v0, v1) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            else if (qnode[2] < OBB.MinCorner()[2])  // lower edge
                { // edge 2 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v2, v3) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            if (qnode[1] < OBB.MinCorner()[1])  // left edge
                { // edge 1 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v1, v2) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            else if (qnode[1] > OBB.MaxCorner()[1])  // right edge
                { // edge 3 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v3, v0) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            break;
        }
    case 2:   // X- face
        {
            if (qnode[2] > OBB.MaxCorner()[2])  // upper edge
                { // edge 0 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v0, v1) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            else if (qnode[2] < OBB.MinCorner()[2])  // lower edge
                { // edge 2 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v2, v3) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            if (qnode[1] > OBB.MaxCorner()[1])  // left edge
                { // edge 1 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v1, v2) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            else if (qnode[1] < OBB.MinCorner()[1])  // right edge
                { // edge 3 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v3, v0) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            break;
        }
    case 3:   // Y+ face
        {
            if (qnode[2] > OBB.MaxCorner()[2])  // upper edge
                { // edge 0 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v0, v1) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            else if (qnode[2] < OBB.MinCorner()[2])  // lower edge
                { // edge 2 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v2, v3) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            if (qnode[0] > OBB.MaxCorner()[0])  // left edge
                { // edge 1 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v1, v2) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            else if (qnode[0] < OBB.MinCorner()[0])  // right edge
                { // edge 3 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v3, v0) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            break;
        }
    case 4:   // Y- face
        {
            if (qnode[2] > OBB.MaxCorner()[2])  // upper edge
                { // edge 0 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v0, v1) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            else if (qnode[2] < OBB.MinCorner()[2])  // lower edge
                { // edge 2 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v2, v3) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            if (qnode[0] < OBB.MinCorner()[0])  // left edge
                { // edge 1 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v1, v2) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            else if (qnode[0] > OBB.MaxCorner()[0])  // right edge
                { // edge 3 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v3, v0) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            break;
        }
    case 5:   // Z+ face
        {
            if (qnode[0] < OBB.MinCorner()[0])  // upper edge
                { // edge 0 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v0, v1) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            else if (qnode[0] > OBB.MaxCorner()[0])  // lower edge
                { // edge 2 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v2, v3) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            if (qnode[1] < OBB.MinCorner()[1])  // left edge
                { // edge 1 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v1, v2) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            else if (qnode[1] > OBB.MaxCorner()[1])  // right edge
                { // edge 3 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v3, v0) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            break;
        }
    case 6:   // Z- face
        {
            if (qnode[0] > OBB.MaxCorner()[0])  // upper edge
                { // edge 0 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v0, v1) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            else if (qnode[0] < OBB.MinCorner()[0])  // lower edge
                { // edge 2 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v2, v3) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            if (qnode[1] < OBB.MinCorner()[1])  // left edge
                { // edge 1 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v1, v2) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            else if (qnode[1] > OBB.MaxCorner()[1])  // right edge
                { // edge 3 visible
                    // compute distance to edge
                    if (SquareDistanceToEdge(v3, v0) < sqrRadius)
                        { // positive intersection
                            return true;
                        }
                    numVsblEdges++;
                }
            break;
        }
    }

    if (numVsblEdges == 0)
        { // trivial intersection since no visible edges
            return true;
        }
    else
        {
            // no intersection
            return false;
        }
#endif
}

// Finds which edges of a 2D parallelogram are visible from
//  a given 2D point.  Returns the number of visible edges found.
//
//   [q0, q1]      - given 2D point
//   [vi0, vi1]    - 2D vertex for edge i
//   vsblEdges    - array for returning visible edge index values
//   ccwSequence  - indicates if edge sequence runs CW or CCW
//
//   [ni0, ni1]    - 2D normal for edge i  (removed)
//
//  Note: vertex ordering is arranged s.t. face edges are numbered as
//        below. Whether e0..e3 progress in a CW or CCW order around
//        the parallelegram is defined by an argument value.
//    e0  - [v0, v1]
//    e1  - [v1, v2]
//    e2  - [v2, v3]
//    e3  - [v3, v0]
//
int EllipsoidOBBIntersectionSolver::FindVisibleEdges(double q0, double q1,
                                                     double v00, double v01,
                                                     double v10, double v11,
                                                     double v20, double v21,
                                                     double v30, double v31,
                                                     int *vsblEdges,
                                                     bool ccwSequence)
{
    int numVsblEdges;
    double e00, e01;  // edge vectors
    double e10, e11;
    double e20, e21;
    double e30, e31;
    double n00, n01;  // edge normals
    double n10, n11;
    double n20, n21;
    double n30, n31;

    numVsblEdges = 0;

    // compute edge vectors
    e00 = v10 - v00; e01 = v11 - v01;
    e10 = v20 - v10; e11 = v21 - v11;
    e20 = v30 - v20; e21 = v31 - v21;
    e30 = v00 - v30; e31 = v01 - v31;

    // compute edge normals
    if (ccwSequence) {
        n00 = e01; n01 = -e00;
        n10 = e11; n11 = -e10;
        n20 = e21; n21 = -e20;
        n30 = e31; n31 = -e30;
    }
    else {
        n00 = -e01; n01 = e00;
        n10 = -e11; n11 = e10;
        n20 = -e21; n21 = e20;
        n30 = -e31; n31 = e30;
    }

    //// compute edge normals
    //// edge 0
    //// compute a normal to edge
    //n00 = -e01; n01 = e00;
    //// make sure normal points in right direction
    ////  (should be in direction of previous edge in sequence)
    //if ((n00*e30 + n01*e31) < 0)
    //{ n00 = -n00; n01 = -n01; }
    //// edge 1
    //n10 = -e11; n11 = e10;
    //if ((n10*e00 + n11*e01) < 0)
    //{ n10 = -n10; n11 = -n11; }
    //// edge 2
    //n20 = -e21; n21 = e20;
    //if ((n20*e10 + n21*e11) < 0)
    //{ n20 = -n20; n21 = -n21; }
    //// edge 3
    //n30 = -e31; n31 = e30;
    //if ((n30*e20 + n31*e21) < 0)
    //{ n30 = -n30; n31 = -n31; }

    //std::cout << "q = [" << q0 << " " << q1 << "]" << std::endl;
    //std::cout << "v(1, :) = [" << v00 << " " << v01 << "]" << std::endl;
    //std::cout << "v(2, :) = [" << v10 << " " << v11 << "]" << std::endl;
    //std::cout << "v(3, :) = [" << v20 << " " << v21 << "]" << std::endl;
    //std::cout << "v(4, :) = [" << v30 << " " << v31 << "]" << std::endl;
    //std::cout << "n(1, :) = [" << n00 << " " << n01 << "]" << std::endl;
    //std::cout << "n(2, :) = [" << n10 << " " << n11 << "]" << std::endl;
    //std::cout << "n(3, :) = [" << n20 << " " << n21 << "]" << std::endl;
    //std::cout << "n(4, :) = [" << n30 << " " << n31 << "]" << std::endl;

    // Use "else if" on opposite egdes, since only one of a pair can
    //  be visible (then don't have to check for numVsblEdges >= 2)
    // edge 0
    if (EdgeIsVisible(q0, q1, v00, v01, n00, n01)) {
        vsblEdges[numVsblEdges++] = 0;
    }
    // edge 2
    else if (EdgeIsVisible(q0, q1, v20, v21, n20, n21)) {
        vsblEdges[numVsblEdges++] = 2;
    }

    // edge 1
    if (EdgeIsVisible(q0, q1, v10, v11, n10, n11)) {
        vsblEdges[numVsblEdges++] = 1;
    }
    // edge 3
    else if (EdgeIsVisible(q0, q1, v30, v31, n30, n31)) {
        vsblEdges[numVsblEdges++] = 3;
    }

    return numVsblEdges;
}

// Determine if a 2D edge is visible from a given 2D point
//    q0, q1 - given 2D point
//    v0, v1 - any 2D point on the edge
//    n0, n1 - 2D edge normal
bool EllipsoidOBBIntersectionSolver::EdgeIsVisible(double q0, double q1,
                                                   double v0, double v1,
                                                   double n0, double n1)
{
    // 2D dot product between edge norm and offset from edge point
    double dprod = (q0-v0)*n0 + (q1-v1)*n1;
    if (dprod >= 0.0) {
        // edge is visible
        return true;
    }
    else {
        // edge not visible
        return false;
    }
}

// Compute the distance from the origin to a 3D edge segment
//  [p, r] - edge segment
double EllipsoidOBBIntersectionSolver::SquareDistanceToEdge(const vct3 &p, const vct3 &r)
{
    // project origin to the edge segment
	vct3 pr=r-p;
	double lam = (-p*pr)/(pr*pr);
	if (lam<=0.0) {
        return p.NormSquare();
    }
	if (lam>1.0) {
        return r.NormSquare();
    }
	return (p+pr*lam).NormSquare();
}
