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

#include <cisstMesh/msh3AlgDirPDTreevonMisesProj.h>
#include <cisstMesh/msh3DirPDTreeNode.h>
#include <cisstMesh/msh3Utilities.h>

#define EPS  1e-12

// PD Tree Methods

int msh3AlgDirPDTreevonMisesProj::NodeMightBeCloser(const vct3 &Xp, const vct3 & CMN_UNUSED(Xn),
                                                    msh3DirPDTreeNode const *node,
                                                    double ErrorBound)
{
    //
    // Match Error:
    //
    //  cost:  k*(1-Xn'*Nclosest) + (1/2)*(Xp-closest)'*inv(M)*(Xp-closest)
    //
    //    NOTE: an extra k*1 was added to the cost above in order to make the
    //          cost function always positive; in addition, when assuming a
    //          perfect orientation match, this allows the current error to
    //          be used directly by the position-based bound rather than
    //          having to subtract "k" for a perfect orientation match.
    //

    // if node only contains few datums then skip the node test and check
    //  the datums directly since the time to check each datum is similar
    //  to the time to compute node test
    if (node->NData <= 3) {
        return 1;
    }

    // --- Compute Lower Bound on Orientation Error --- //

    // For simplicity, we assume perfect orientation error;
    //  thus, orientation component of cost function = 0 and
    //  can be simply disregarded.

    // --- Positional Node Distance Test --- //

    // Assume ErrorBound = MinOrientationError + (1/2)*(Xp-closest)'*inv(M)*(Xp-closest)
    //  => MaxSqrMahalanobisError = (ErrorBound - MinOrientationError)*2.0
    double MaxSqrMahalError = ErrorBound * 2.0;

    // Ellipsoid / OBB Intersection Test
    //  test if the ellipsoid centered at the sample point and defined by the
    //  level set of MaxSqrMahalError intersects the oriented bounding box of this node
    return IntersectionSolver.TestEllipsoidOBBIntersection(Xp, node->Bounds, node->F, MaxSqrMahalError, N, Dmin);
}

// Xpln         ~ non-transformed in-plane (2d) sample orientation
// Ry_pln       ~ Ry_pln = Rreg * Rx_pln
// sample_k     ~ orientation concentration of sample
// Minv         ~ M is position covariance of transformed sample
// N, Ninv      ~ inv(M) = N'N
// Dmin         ~ sqrt of smallest eigenvalue of inv(M)
// Rreg         ~ rotation from current registration:  Yp = Rreg*Xp + t
void msh3AlgDirPDTreevonMisesProj::InitializeSample(vct2 Xpln,
                                                    vctRot3 Ry_pln,
                                                    double sample_k,
                                                    vct3x3 Minv,  // vct3x3 M,
                                                    vct3x3 N, vct3x3 Ninv,
                                                    double Dmin)
{
    this->Xpln = Xpln;
    this->sample_k = sample_k;
    this->Minv = Minv;
    this->N = N;
    this->Ninv = Ninv;
    this->Dmin = Dmin;
    //this->M = M;

    this->Rpln_y.Row(0) = Ry_pln.Column(0);
    this->Rpln_y.Row(1) = Ry_pln.Column(1);
}

// Xpln       ~ non-transformed in-plane (2d) sample orientation
// Rx_pln     ~ transformation from plane to y coordinates:  Xn = Rx_pln * [Xpln; 0]
// sample_k   ~ orientation concentration of sample
// sample_M   ~ position covariance of non-transformed sample
// Rreg       ~ rotation from current registration:  Yp = Rreg*Xp + t
void msh3AlgDirPDTreevonMisesProj::InitializeSample(vct2 Xpln,
                                                    vctRot3 Rx_pln,
                                                    double sample_k,
                                                    vct3x3 sample_M, //vct3 sample_M_Eig,
                                                    vctRot3 Rreg )
{
    this->Xpln = Xpln;
    this->sample_k = sample_k;
    //this->sample_M = sample_M;
    //this->Rreg = Rreg;

    vctRot3 Ry_pln(Rreg * Rx_pln);
    this->Rpln_y.Row(0) = Ry_pln.Column(0);
    this->Rpln_y.Row(1) = Ry_pln.Column(1);

    // compute noise model of transformed sample point
    vct3x3 RMxRt = Rreg * sample_M * Rreg.TransposeRef();

    // compute noise model decomposition
    ComputeCovDecomposition(RMxRt, Minv, N, Ninv, Dmin);
}

void msh3AlgDirPDTreevonMisesProj::ComputeCovDecomposition(const vct3x3 &M, vct3x3 &Minv,
                                                           vct3x3 &N, vct3x3 &Ninv, double &Dmin)
{
    // compute eigen decomposition of M
    //   M = V*S*V'
    //  NOTE: eigenvalues are returned in descending order
    vct3    eigenValues;
    vct3x3  eigenVectors;
    ComputeCovEigenDecomposition_NonIter(M, eigenValues, eigenVectors);

    // Compute Minv
    //   Minv = V*diag(1/S)*V'
    static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> V_Sinv;
    static vct3 Sinv;
    Sinv[0] = 1.0 / eigenValues[0];
    Sinv[1] = 1.0 / eigenValues[1];
    Sinv[2] = 1.0 / eigenValues[2];
    V_Sinv.Column(0) = eigenVectors.Column(0)*Sinv[0];
    V_Sinv.Column(1) = eigenVectors.Column(1)*Sinv[1];
    V_Sinv.Column(2) = eigenVectors.Column(2)*Sinv[2];
    Minv.Assign(V_Sinv * eigenVectors.TransposeRef());

    // calculate decomposition of Minv = N'*N
    //   Minv = R*D^2*R' = N'*N     M = R*Dinv^2*R' => R' = V', Dinv = sqrt(S)
    //   N = D*R'      Ninv = R*inv(D)
    vct3 Dinv(sqrt(eigenValues[0]),
              sqrt(eigenValues[1]),
              sqrt(eigenValues[2]));
    N.Row(0) = eigenVectors.Column(0) / Dinv[0];
    N.Row(1) = eigenVectors.Column(1) / Dinv[1];
    N.Row(2) = eigenVectors.Column(2) / Dinv[2];
    Ninv.Column(0) = eigenVectors.Column(0)*Dinv[0];
    Ninv.Column(1) = eigenVectors.Column(1)*Dinv[1];
    Ninv.Column(2) = eigenVectors.Column(2)*Dinv[2];

    Dmin = 1.0 / Dinv[0];
}


//// update Mahalanobis bound of this node if it does not
////  share a common covariance bound with its parent
//if (!node->bUseParentEigMaxBound)
//{
//  ComputeNodeMatchCov(node);
//}
//// update log bound for this node if it does not share a
////  common log bound with its parent
//if (!node->bUseParentEigRankMinBounds)
//{
//  // Compute min bound on log component of match error
//  //  a min bound on the determinant |RMxR'+My| is found by multiplying the
//  //  eigenvalues of RMxR' with the min node eigenvalues of each magnitude
//  //  rank in rank order
//  double r0, r1, r2;
//  r0 = node->EigRankMin[0] + sample_RMxRt_sigma2_Eig[0];
//  r1 = node->EigRankMin[1] + sample_RMxRt_sigma2_Eig[1];
//  r2 = node->EigRankMin[2] + sample_RMxRt_sigma2_Eig[2];
//  MinLogM = log(r0*r1*r2);
//}
//
//// subtract min bound of the log term component of node error
////  from the current best match error to get effective Mahalanobis bound
////  for creating the boundary ellipse
//double NodeErrorBound = ErrorBound - MinLogM;
//
//// Test intersection between the ellipsoid and the oriented bounding
////  box of the node
//return IntersectionSolver.Test_Ellipsoid_OBB_Intersection(v, node->Bounds, node->F,
//  NodeErrorBound, N, Dmin);



//vct3 Fv = node->F*v;          // transform point into local coordinate system of node
////vct3 Fn = F.Rotation()*n;   // don't need this since normal statistics for a node are calculated
////  wrt the world reference frame, not local reference frame
//
//// Simple Bound
////double searchDist2 = ErrorBound/posWeight;
//

//
//  Improved Bound:  (Assume Best Match Normal is aligned by Max Angular Deviation
//                    from the Mean Orientation of the Node)
//
//    If we know the avg normal (Navg) and the maximum angular deviation (dThetaMax)
//     from the average normal for all triangles in a node, then:
//
//     given: 0 < dThetaMax < 180 (deg)
//     N'*Navg = cos(dThetaAvg)
//      set ThetaC = dThetaAvg - dThetaMax    (assume avg normal tilted towards n by max deviation)
//      if dThetaC < 0 then dThetaC = 0
//     set N'*Nc = cos(dThetaC)    (this is the max possible norm product (lowest possible orienation error)
//                                  since N'*Nc <= cos(dThetaC) by reason of max deviation)
//
//     =>  cost = k*(1-cos(dThetaC)) + B*dist^2
//         maxSearchDist = sqrt([ErrorBound - k*(1-cos(dThetaC))]/B)
//
//     =>  search a node if the node boundary enlarged by
//         maxSearchDist contains the point v
//
//// Improved Bound
//double dThetaAvg = acos(n.DotProduct(node->Navg));
//double dThetaC = dThetaAvg - node->dThetaMax;
//dThetaC = dThetaC > 0.0 ? dThetaC : 0.0;   // enforce ThetaC >= 0
//double searchDist2 = (ErrorBound - k*(1 - cos(dThetaC))) / B;
//if (searchDist2 < 0.0)
//{ // orientation error alone dismisses this node
//  return 0;
//}
//
//// Rather than comparing only the x-axis value, check all coordinate directions
////  of the node bounding box to further refine whether this node may be within
////  the search range of this point. Using the node coordinate frame is still
////  useful in this context, because it ensures a node bounding box of minimum size.
//// Conceptually, this check places another bounding box of size search distance
////  around the point and then checks if this bounding box intersects the bounding
////  box of this node.
//return node->Bounds.Includes(Fv, sqrt(searchDist2));
//
