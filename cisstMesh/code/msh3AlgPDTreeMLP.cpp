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

#include <cisstMesh/msh3AlgPDTreeMLP.h>
#include <cisstMesh/msh3PDTreeNode.h>
#include <cisstMesh/msh3Utilities.h>

void msh3AlgPDTreeMLP::InitializeSampleSearch(vct3x3 sampleXfm_M, vct3 sample_M_Eig)
{
    this->sampleXfm_M = sampleXfm_M;
    this->sample_M_Eig = sample_M_Eig;
}


int msh3AlgPDTreeMLP::NodeMightBeCloser(const vct3 &v,
                                        msh3PDTreeNode *node,
                                        double ErrorBound)
{
    // uncomment the desired node bounds check method
    //  NODE_SIMPLE_ELLIPSOID_BOUNDS seems much faster
    //#define NODE_SPHERE_BOUNDS
#define NODE_SIMPLE_ELLIPSOID_BOUNDS

#ifdef NODE_SIMPLE_ELLIPSOID_BOUNDS

    // if node only contains few datums then check the datum directly
    //  since the time to check each datum is similar to the time to
    //  compute node intersection
    if (node->NData <= 3) {
        return 1;
    }

    // TODO: add ability to override re-computing node match covariance
    //       (such as for isotropic noise model during first iteration
    //        of ICP-based method)
    // update Mahalanobis bound
    //  do this if node does not share a common bound with its parent
    if (!node->bUseParentEigMaxBound) {
        ComputeNodeMatchCov(node);
    }
    // update log bound
    //  do this if node does not share a common log bound with its parent
    if (!node->bUseParentEigRankMinBounds) {
        // Compute min bound on log component of match error
        //  a min bound on the determinant |RMxR'+My| is found by multiplying the
        //  eigenvalues of RMxR' with the min node eigenvalues of each magnitude
        //  rank in rank order
        double r0, r1, r2;
        r0 = node->EigRankMin[0] + sample_M_Eig[0];
        r1 = node->EigRankMin[1] + sample_M_Eig[1];
        r2 = node->EigRankMin[2] + sample_M_Eig[2];
        MinLogM = log(r0*r1*r2);
    }

    // get effective Mahalanobis bound
    //  subtract min bound of the log term from the current best match error
    //  to get the effective Mahalanobis bound for forming the boundary ellipse
    double NodeErrorBound = ErrorBound - MinLogM;

    // test for intersection between the ellipsoid and node
    //  (i.e. the oriented bounding box of the node)
    return IntersectionSolver.TestEllipsoidOBBIntersection(v, node->Bounds, node->F,
                                                           NodeErrorBound, N, Dmin);

#endif // NODE_SIMPLE_ELLIPSOID_BOUNDS

#ifdef NODE_SPHERE_BOUNDS

    // TODO: this has not been tested for this class...

    //// use local variables to enable different set of values on
    ////  first match
    double MEigMax;

    if (bFirstIter_Matches) {
        // isotropic noise model for first iteration
        MinLogM = 2.0794; // log(|I+I|) = log(2*2*2) = 2.0794
        MEigMax = 2;      // max eigenvalue of M = Mx + My = 2*I
    }
    else { // noise model defined by node for later iterations

        // update log bound for this node if it does not share a
        //  common log bound with its parent
        if (!node->bUseParentEigRankMinBounds) {
            // Compute min bound on log term for error
            //  a min bound on the determinant |RMxR'+My| is found by multiplying the
            //  sum of each eigenvalue rank pair for RMxR' and the min eigenvalues of
            //  that rank within the node
            double r0, r1, r2;
            r0 = node->EigRankMin[0] + sample_RMxRt_sigma2_Eig[0];
            r1 = node->EigRankMin[1] + sample_RMxRt_sigma2_Eig[1];
            r2 = node->EigRankMin[2] + sample_RMxRt_sigma2_Eig[2];
            MinLogM = log(r0*r1*r2);
        }
        MEigMax = node->EigMax + sample_RMxRt_sigma2_Eig[0];
    }

    // subtract min bound on the log term component of the match error
    //  from the match error bound to get an upper bound on the Mahalanobis
    //  part of the match error for finding a better match.
    double NodeErrorBound = ErrorBound - MinLogM;

    // Get the radius of a minimal bounding sphere around the source point
    //  that fully contains the Mahalanobis bound
    double maxSqrSearchDistance = MEigMax * NodeErrorBound;
    double maxSearchDistance = sqrt(maxSqrSearchDistance);

    // transform sample point into local coordinate system of node
    vct3 Fv = node->F*v;

    // Check if point lies w/in search range of the bounding box for this node
    //  Rather than comparing only the x-axis value, check all coordinate directions
    //   of the node bounding box to further refine whether this node may be within
    //   the search range of this point. Using the node coordinate frame is still
    //   useful in this context, because it ensures a node bounding box of minimum size.
    //  Conceptually, this check places another bounding box of size search distance
    //   around the point and then checks if this bounding box intersects the bounding
    //   box of this node.
    return node->Bounds.Includes(Fv, maxSearchDistance);
#endif

}


// Note: this function depends on the InitializeSampleSearch() function
//       to set the noise model of the current transformed sample
//       point before this function is called
void msh3AlgPDTreeMLP::ComputeNodeMatchCov(msh3PDTreeNode *node)
{
    // Compute the effective noise model for this node, assuming the noise
    //  model of the transformed sample point has already been computed

    // noise model of transformed sample
    M = sampleXfm_M;
    // add the effective My for this node
    M.Element(0, 0) += node->EigMax;
    M.Element(1, 1) += node->EigMax;
    M.Element(2, 2) += node->EigMax;

    // compute eigen decomposition of M
    //   M = V*S*V'
    //  NOTE: eigenvalues are returned in descending order
    vct3    eigenValues;
    vct3x3  eigenVectors;
    ComputeCovEigenDecomposition_NonIter(M, eigenValues, eigenVectors);

    // calculate decomposition of Minv = N'*N
    //   Minv = R*D^2*R' = N'*N     M = R*Dinv^2*R' => R' = V', Dinv = sqrt(S)
    //   N = D*R'      Ninv = R*inv(D)
    vct3 Dinv(sqrt(eigenValues[0]),
              sqrt(eigenValues[1]),
              sqrt(eigenValues[2]));
    N.Row(0) = eigenVectors.Column(0) / Dinv[0];
    N.Row(1) = eigenVectors.Column(1) / Dinv[1];
    N.Row(2) = eigenVectors.Column(2) / Dinv[2];
    Dmin = 1.0 / Dinv[0];
}


void msh3AlgPDTreeMLP::ComputeCovDecomposition_NonIter(const vct3x3 &M, vct3x3 &Minv, double &det_M)
{
    // Compute eigen decomposition of M
    vct3    eigenValues;
    vct3x3  eigenVectors;
    ComputeCovEigenDecomposition_NonIter(M, eigenValues, eigenVectors);

    // Compute Minv
    static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> V_Sinv;
    static vct3 Sinv;
    Sinv[0] = 1.0 / eigenValues[0];
    Sinv[1] = 1.0 / eigenValues[1];
    Sinv[2] = 1.0 / eigenValues[2];
    V_Sinv.Column(0) = eigenVectors.Column(0)*Sinv[0];
    V_Sinv.Column(1) = eigenVectors.Column(1)*Sinv[1];
    V_Sinv.Column(2) = eigenVectors.Column(2)*Sinv[2];
    Minv.Assign(V_Sinv * eigenVectors.TransposeRef());

    // compute determinant of M
    det_M = eigenValues.ProductOfElements();
}


void msh3AlgPDTreeMLP::ComputeCovDecomposition_NonIter(const vct3x3 &M, vct3x3 &Minv, vct3x3 &N, vct3x3 &Ninv, double &det_M)
{
    // Compute eigen decomposition of M
    vct3    eigenValues;
    vct3x3  eigenVectors;
    ComputeCovEigenDecomposition_NonIter(M, eigenValues, eigenVectors);

    // Compute Minv
    static vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> V_Sinv;
    static vct3 Sinv;
    Sinv[0] = 1.0 / eigenValues[0];
    Sinv[1] = 1.0 / eigenValues[1];
    Sinv[2] = 1.0 / eigenValues[2];
    V_Sinv.Column(0) = eigenVectors.Column(0)*Sinv[0];
    V_Sinv.Column(1) = eigenVectors.Column(1)*Sinv[1];
    V_Sinv.Column(2) = eigenVectors.Column(2)*Sinv[2];
    Minv.Assign(V_Sinv * eigenVectors.TransposeRef());

    // Compute Decomposition of Minv
    vct3 Dinv(sqrt(eigenValues[0]),
              sqrt(eigenValues[1]),
              sqrt(eigenValues[2]));
    N.Row(0) = eigenVectors.Column(0) / Dinv[0];
    N.Row(1) = eigenVectors.Column(1) / Dinv[1];
    N.Row(2) = eigenVectors.Column(2) / Dinv[2];
    Ninv.Column(0) = eigenVectors.Column(0)*Dinv[0];
    Ninv.Column(1) = eigenVectors.Column(1)*Dinv[1];
    Ninv.Column(2) = eigenVectors.Column(2)*Dinv[2];

    // Compute determinant of M
    det_M = eigenValues.ProductOfElements();
}
