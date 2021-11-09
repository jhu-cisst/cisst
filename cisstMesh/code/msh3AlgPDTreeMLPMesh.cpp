/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// ****************************************************************************
//
//    Copyright (c) 2014, Seth Billings, Russell Taylor, Johns Hopkins University
//    All rights reserved.
//
//    Redistribution and use in source and binary forms, with or without
//    modification, are NOT permitted without specific prior written permission
//    of the copyright holders.
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

#include <cisstMesh/msh3AlgPDTreeMLPMesh.h>
#include <cisstMesh/msh3Utilities.h>

// finds the point on this datum with lowest match error
double msh3AlgPDTreeMLPMesh::FindClosestPointOnDatum(const vct3 &point,
                                                     vct3 &closest,
                                                     int datum)
{
    static vct3 d;
    static vct3x3 M, Minv, N, Ninv;
    double det_M;

    // compute noise model for this datum
    M = sampleXfm_M + pTree->Mesh->TriangleCov[datum];
    ComputeCovDecomposition_NonIter(M, Minv, N, Ninv, det_M);

    // Find the closest point on this triangle in a Mahalanobis distance sense
    TCPS.FindMostLikelyPointOnTriangle(point, datum, N, Ninv, closest);

    d = (point - closest);
    return log(det_M) + vctDotProduct(d, Minv*d);
}


int msh3AlgPDTreeMLPMesh::DatumMightBeCloser(const vct3 & CMN_UNUSED(point),
                                             int CMN_UNUSED(datum),
                                             double CMN_UNUSED(ErrorBound))
{
    return true;
}
