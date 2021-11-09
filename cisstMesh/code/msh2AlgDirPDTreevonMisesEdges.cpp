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

#include <cisstMesh/msh2AlgDirPDTreevonMisesEdges.h>

#include <limits>

double msh2AlgDirPDTreevonMisesEdges::FindClosestPointOnDatum(
                                                              const vct2 &v, const vct2 &n,
                                                              vct2 &closest, vct2 &closestNorm,
                                                              int datum)
{
    //  cost:  k*(1-N'*Nclosest) + ||v - closest||^2 / (2*sigma2)

    // Project point onto the edge
    //  (storing lambda value in temp buffer)
    closest = pDirTree->GetEdge(datum).ProjectOnEdge(v); //, &searchLambdas.Element(datum));
    closestNorm = pDirTree->GetEdge(datum).Norm;

    // Is this a permitted match?
    double dTheta = acos(vctDotProduct(n, closestNorm));
    if (dTheta > dThetaMax) {
        return std::numeric_limits<double>::max();
    }
    else {
        bPermittedMatchFound = true;
        return k*(1.0 - n*closestNorm) + (v - closest).NormSquare() / (2.0*sigma2);
    }

    // This doesn't work, i.e. we can't keep returning the best match among the non-permitted
    //  matches because then when a permitted match finally comes along, then it may have
    //  higher error, which would prevent it from being chosen. The only way to accomplish the
    //  above is to modify the core PD tree search routine, which I don't want to do.
    //  => only return match errors for permitted matches.
    //// is this a permitted match?
    //double matchError = k*(1.0 - n*closestNorm) + (v - closest).NormSquare() / (2.0*sigma2);
    //double dTheta = acos(vctDotProduct(n, closestNorm));
    //if (dTheta > dThetaMax)
    //{
    //  if (bPermittedMatchFound)
    //  { // skip this match as long as some other permitted match has been already been found
    //    // do this by returning an astronomical match error
    //    matchError = std::numeric_limits<double>::max();
    //  }
    //}
    //else
    //{
    //  bPermittedMatchFound = true;
    //}
    //return matchError;
}

int msh2AlgDirPDTreevonMisesEdges::DatumMightBeCloser(const vct2 & CMN_UNUSED(v), const vct2 & CMN_UNUSED(n),
                                                      int CMN_UNUSED(datum),
                                                      double CMN_UNUSED(ErrorBound))
{
    // doing a decent proximity check is complicated enough that it is
    //  better to just compute the full error directly
    return 1;
}
