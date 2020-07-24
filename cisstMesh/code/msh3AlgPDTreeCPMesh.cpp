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

#include <cisstMesh/msh3AlgPDTreeCPMesh.h>

// finds the point on this datum with lowest match error
//  and returns the match error and closest point
double msh3AlgPDTreeCPMesh::FindClosestPointOnDatum(const vct3 &v,
                                                    vct3 &closest,
                                                    int datum)
{
    // set closest point
    TCPS.FindClosestPointOnTriangle(v, datum, closest);

    // return distance as match error
    //  NOTE: distance is more convenient than square distance
    //        since the distance is required for doing the
    //        bounding box proximity tests.
    return (v - closest).Norm();
}

double msh3AlgPDTreeCPMesh::FindClosestPointOnDatumWithEdgeDetection(const vct3 &v, vct3 &closest, int &cpLocation, int datum)
{
    // set closest point
    cpLocation = TCPS.FindClosestPointOnTriangleWithEdgeDetection(v, datum, closest);

    // return distance as match error
    //  NOTE: distance is more convenient than square distance
    //        since the distance is required for doing the
    //        bounding box proximity tests.
    return (v - closest).Norm();
}

// fast check if a datum might have smaller match error than error bound
int msh3AlgPDTreeCPMesh::DatumMightBeCloser(
                                            const vct3 &v,
                                            int datum,
                                            double ErrorBound)
{
    // create bounding box around triangle
    msh3BoundingBox BB;
    for (int vx = 0; vx < 3; vx++) {
        BB.Include(pTree->Mesh->FaceCoord(datum, vx));
    }

    // We want to know if this point can produce a cost less than the
    //  error bound. Error bound is the best cost we have so far.
    return BB.Includes(v, ErrorBound);

    // Referencing a bounding box built around the triangle at time
    //  of triangle creation adds 10% to application runtime!
    //return pTree->Triangle(datum).BB.Includes(v,ErrorBound);
}
