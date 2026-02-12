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

#include <cisstMesh/msh3AlgDirPDTreevonMisesProjMesh.h>

// PD Tree Methods

double msh3AlgDirPDTreevonMisesProjMesh::FindClosestPointOnDatum(const vct3 &Xp, const vct3 & CMN_UNUSED(Xn),
                                                                 vct3 &closest, vct3 &closestNorm,
                                                                 int datum)
{
    // This routine ignores the 3d orientation Xn and uses the 2d orientation
    //  Xpln as stored in the base class

    // Find closest point on triangle in a Mahalanobis distance sense
    //
    //   Mahalanobis Distance:  sqrt((x-v)'*Minv*(x-v))
    //
    TCPS.FindMostLikelyPointOnTriangle(Xp, datum, N, Ninv, closest);

    // norm has same value everywhere on this datum
    static vct2 Yprj, Ypln;
    closestNorm = pDirTree->Mesh.faceNormals(datum);
    Yprj = Rpln_y * closestNorm;
    double Ynorm = Yprj.Norm();
    if (Ynorm < 0.001) {
        Ypln.SetAll(0.0);   // effectively sets orientation error to 90 deg.
    }
    else {
        Ypln = Yprj / Ynorm;
    }


    // Note: orientation error was pre-computed in DatumMightBeCloser routine
    // add extra k to the negative log-likelihood value in order
    //  to make match error always >= 0
    return sample_k*(1 - Ypln * Xpln)
        + ((Xp - closest)*Minv*(Xp - closest)) / 2.0;

    //// set closest point
    //TCPS.FindClosestPointOnTriangle(
    //  Xp,
    //  pDirTree->TriangleVertexCoord(datum, 0),
    //  pDirTree->TriangleVertexCoord(datum, 1),
    //  pDirTree->TriangleVertexCoord(datum, 2),
    //  -1, closest);
}


int msh3AlgDirPDTreevonMisesProjMesh::DatumMightBeCloser(const vct3 & CMN_UNUSED(Xp), const vct3 & CMN_UNUSED(Xn),
                                                         int CMN_UNUSED(datum),
                                                         double CMN_UNUSED(ErrorBound))
{
    return true;

    //// just do an orientation check here and save result for re-use in
    ////  computing the actual match error

    //// add extra k*1 to the negative log-likelihood value in order
    ////  to make match error always >= 0
    //vct2 Ypln = Rpln_y * pDirTree->TriangleNorm(datum);
    //double orientationError = sample_k*(1 - Ypln * Xpln);

    //// if orientation error is more than error bound then we can
    ////  exclude the match based on orientation error alone
    //return (ErrorBound - orientationError) > 0;
}
