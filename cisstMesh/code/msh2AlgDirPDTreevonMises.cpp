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

#include <cisstMesh/msh2AlgDirPDTreevonMises.h>
#include <cisstMesh/msh2DirPDTreeNode.h>

// fast check if a node might contain a datum having smaller match error
//  than the error bound
int msh2AlgDirPDTreevonMises::NodeMightBeCloser(const vct2 &v, const vct2 &n,
                                                msh2DirPDTreeNode const *node,
                                                double ErrorBound)
{
    //
    //  cost:  k*(1-N'*Nclosest) + (1/(2*sigma2))*||v - closest||^2
    //
    //  Improved Bound:  (Assume Best Match Normal is aligned by Max Angular Deviation
    //                    from the Mean Orientation of the Node)
    //
    //    If we know the avg normal (Navg) and the maximum angular deviation (dThetaMax)
    //     from the average normal for all triangles in a node, then:
    //
    //     given: 0 < dThetaMax < 180 (deg)
    //     N'*Navg = cos(dThetaAvg)
    //      set dTheta = dThetaAvg - dThetaMax    (assume avg normal tilted towards n by max deviation)
    //      if dTheta < 0 then dTheta = 0
    //     set N'*Nc = cos(dTheta)    (this is the max possible norm product (lowest possible orienation error)
    //                                  since N'*Nc <= cos(dThetaC) by reason of max deviation)
    //
    //     =>  cost = k*(1-cos(dTheta)) + dist^2 / (2*sigma2)
    //         maxSearchDist = sqrt([ErrorBound - k*(1-cos(dTheta))] * 2*sigma2)
    //
    //     =>  search a node if the node boundary enlarged by
    //         maxSearchDist contains the point v
    //

    // compute an upper bound on the search distance
    double dThetaAvg = acos(n * node->Navg);
    double dTheta = dThetaAvg - node->dThetaMax;

    // check if orientation error is outside the permitted range
    if (dTheta > dThetaMax) {
        return 0;
    }

    dTheta = dTheta > 0.0 ? dTheta : 0.0;   // enforce dTheta >= 0
    double maxSearchDist2 = (ErrorBound - k*(1.0 - cos(dTheta))) * 2.0 * sigma2;
    if (maxSearchDist2 < 0.0) {
        return 0;
    }

    if (node->bUsingOBB) {
        vct2 Fv = node->F*v;  // transform point into local coordinate system of node

        return node->Bounds.Includes(Fv, sqrt(maxSearchDist2));
    }
    else {
        return node->Bounds.Includes(v, sqrt(maxSearchDist2));
    }
}

// TODO: come up with a fast initialization that is more
//       likely to return a permitted match
//// quickly find an approximate initial match by dropping straight down the
////   tree to the node containing the sample point and picking a datum from there
//int msh2AlgDirPDTreevonMises::FastInitializeProximalDatum(
//  const vct2 &v, const vct2 &n,
//  vct2 &proxPoint, vct2 &proxNorm)
//{
//  // find proximal leaf node
//  msh2DirPDTreeNode *pNode;
//  pNode = pDirTree->Top;
//
//  // ensure average node angle is within
//
//
//  while (!pNode->IsTerminalNode())
//  {
//    pNode = pNode->GetChildSplitNode(v);
//  }
//
//  int proxDatum = pNode->Datum(0);                  // choose any datum from the leaf node
//  proxPoint = pDirTree->DatumSortPoint(proxDatum);  // choose any point on the datum
//  proxNorm = pDirTree->DatumNorm(proxDatum);
//
//  return proxDatum;
//}
