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

#include <cisstMesh/msh3AlgDirPDTreeBAMesh.h>

// PD Tree Methods

double msh3AlgDirPDTreeBAMesh::FindClosestPointOnDatum(const vct3 &Xp, const vct3 & CMN_UNUSED(Xn),
                                                       vct3 &closest, vct3 &closestNorm,
                                                       int datum)
{
    // This routine ignores the 3d orientation Xn and computes the
    //  closest point by Euclidean distance

    // compute closest point on triangle
    TCPS.FindClosestPointOnTriangle(Xp, datum, closest);

    // norm has same value everywhere on the triangle
    closestNorm = pDirTree->Mesh.faceNormals(datum);

    // return the match distance
    //  NOTE: distance is more convenient than square distance
    //        since the distance is required for doing the
    //        bounding box proximity tests.
    return (Xp - closest).Norm();
}


int msh3AlgDirPDTreeBAMesh::DatumMightBeCloser(const vct3 &Xp, const vct3 &Xn,
                                               int datum,
                                               double ErrorBound)
{
    // check if orientation error is past threshold
    //  for efficiency, use the dot product directly for comparison
    //  rather than using the actual angle computed from the inverse
    //  cosine of the dot product
    double dotProd = Xn.DotProduct(pDirTree->Mesh.faceNormals(datum));
    if (dotProd < cos_maxMatchAngle) {
        return 0;
    }
    //double matchAngle = acos(Xn.DotProduct(pDirTree->TriangleNorm(datum)));
    //if (matchAngle > maxMatchAngle)
    //  return 0;

    // create bounding box around triangle
    msh3BoundingBox BB;
    for (int vx = 0; vx < 3; vx++) {
        BB.Include(pDirTree->Mesh.FaceCoord(datum, vx));
    }

    // check distance to datum bounding box to attempt quick escape
    return BB.Includes(Xp, ErrorBound);
}
