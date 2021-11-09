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

#ifndef _msh3AlgPDTreeMLP_h
#define _msh3AlgPDTreeMLP_h

#include <cisstMesh/msh3AlgPDTree.h>
#include <cisstMesh/msh3PDTreeBase.h>
#include <cisstMesh/EllipsoidOBBIntersectionSolver.h>

// Always include last!
#include <cisstMesh/mshExport.h>

class CISST_EXPORT msh3AlgPDTreeMLP : public msh3AlgPDTree
{
    //
    // Implements the most-likely point algorithm for PD tree search under anisotropic noise
    //

    //--- Algorithm Parameters ---//

 protected:

    EllipsoidOBBIntersectionSolver IntersectionSolver;

    // must set these before doing the search using the
    //  method for initializing a sample search
    vct3x3 sampleXfm_M;   // covariance of transformed sample point
    vct3   sample_M_Eig;  // eigenvalues of sample covariance in order of decreasing magnitude

    // these are computed automatically
    vct3x3 M;       // effective measurement error covariance for a node & sample pair
    vct3x3 N;       // decomposition of inv(M) = N'N
    double Dmin;    // inverse sqrt of largest eigenvalue of M
    //  (or the sqrt of the smallest eigenvalue of inv(M))
    double MinLogM; // lower bound on the log component of error for this node

    //--- Algorithm Methods ---//

 public:

    // constructor
    msh3AlgPDTreeMLP(msh3PDTreeBase *pTree)
        : msh3AlgPDTree(pTree)
        {}

    // destructor
    virtual ~msh3AlgPDTreeMLP() {}

    // must call this prior to beginning search for each sample
    //   sampleXfm_M   ~ noise covariance of the transformed sample point
    //   sample_M_Eig  ~ eigenvalues of the sample covariance (in order of decreasing magnitude)
    void InitializeSampleSearch(vct3x3 sampleXfm_M, vct3 sample_M_Eig);

 protected:

    void ComputeNodeMatchCov(msh3PDTreeNode *node);
    void ComputeCovDecomposition_NonIter(const vct3x3 &M, vct3x3 &Minv, vct3x3 &N, vct3x3 &Ninv, double &det_M);
    void ComputeCovDecomposition_NonIter(const vct3x3 &M, vct3x3 &Minv, double &det_M);

    //--- PD Tree Interface Methods ---//

    int NodeMightBeCloser(const vct3 &v,
                          msh3PDTreeNode *node,
                          double ErrorBound);

    virtual double FindClosestPointOnDatum(const vct3 &v,
                                           vct3 &closest,
                                           int datum) = 0;

    virtual int DatumMightBeCloser(const vct3 &v,
                                   int datum,
                                   double ErrorBound) = 0;
};

#endif
