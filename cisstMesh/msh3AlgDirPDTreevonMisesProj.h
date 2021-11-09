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

#ifndef _msh3AlgDirPDTreevonMisesProj_h
#define _msh3AlgDirPDTreevonMisesProj_h

#include <cisstMesh/msh3AlgDirPDTree.h>
#include <cisstMesh/EllipsoidOBBIntersectionSolver.h>

// Always include last!
#include <cisstMesh/mshExport.h>

class CISST_EXPORT msh3AlgDirPDTreevonMisesProj : public msh3AlgDirPDTree
{
    //
    // This algorithm implements most-likely point search using a projected
    //   von-Mises distribution (for 2D orientation) on an arbitrary 2D plane
    //   along with a generalized Gaussian distribution (for 3D positions)
    //

    //--- Algorithm Parameters ---//

 protected:

    EllipsoidOBBIntersectionSolver IntersectionSolver;

    vct2 Xpln;   // non-transformed in-plane orientation of sample
    //  NOTE: 3d orientation in local sample coords = [Xpln; 0]

    // sample noise model
    double sample_k;   // orientational concentration
    //vct3x3 sample_M;   // positional covariance
    //vct3   sample_M_Eig;  // eigenvalues of sample covariance in order of decreasing magnitude

    //  Current rotation of registration
    //vctRot3 Rreg;   // position matches Yp close to Rreg * Xp + t = Xp_xfm

    // Yn_2d_xfm = Rpln_y * Yn_3d
    //  NOTE: orientation matches Yn_3d are chosen such that
    //        Prj_xy(Rreg^-1 * Yn_3d) = Yn_2d_xfm is close to Xpln
    vctFixedSizeMatrix<double, 2, 3> Rpln_y;

    // effective noise model for node intersection test
    //  (i.e. for transformed samples)
    //vct3x3 M;     // effective measurement error covariance for a node & sample pair
    vct3x3 Minv;    // M = effective measurement error covariance for a node & sample pair
    vct3x3 N, Ninv; // decomposition of inv(M) = N'N
    double Dmin;    // inverse sqrt of largest eigenvalue of M
    //  (or the sqrt of the smallest eigenvalue of inv(M))

    // the log term is constant when the measurement error of the target is
    //  is assumed to be zero
    //double MinLogM; // lower bound on the log component of error for this node

    //--- Algorithm Methods ---//

 public:

    // constructor
    msh3AlgDirPDTreevonMisesProj(msh3DirPDTreeBase *pDirTree)
        : msh3AlgDirPDTree(pDirTree)
        {}

    // destructor
    virtual ~msh3AlgDirPDTreevonMisesProj() {}

    // Set sample to be matched in the search (slower routine)
    //  Xpln       ~ non-transformed in-plane (2d) sample orientation
    //  Rx_pln     ~ transformation from plane to y coordinates:  Xn = Rx_pln * [Xpln; 0]
    //  sample_k   ~ orientation concentration of sample
    //  sample_M   ~ position covariance of non-transformed sample
    //  Rreg       ~ rotation from current registration:  Yp = Rreg*Xp + t
    void InitializeSample(vct2 Xpln,
                          vctRot3 Rx_pln,
                          double sample_k,
                          vct3x3 sample_M, //vct3 sample_M_Eig,
                          vctRot3 Rreg);

    // Set sample to be matched in the search
    //  (fast routine for pre-computed noise-model decompositions)
    //  Xpln         ~ non-transformed in-plane (2d) sample orientation
    //  Ry_pln       ~ Ry_pln = Rreg * Rx_pln
    //  sample_k     ~ orientation concentration of sample
    //  Minv         ~ M is position covariance of transformed sample
    //  N, Ninv      ~ inv(M) = N'N
    //  Dmin         ~ sqrt of smallest eigenvalue of inv(M)
    void InitializeSample(vct2 Xpln,
                          vctRot3 Ry_pln,
                          double sample_k,
                          vct3x3 Minv,  // vct3x3 M,
                          vct3x3 N, vct3x3 Ninv,
                          double Dmin);

    static void ComputeCovDecomposition(const vct3x3 &M, vct3x3 &Minv,
                                        vct3x3 &N, vct3x3 &Ninv,
                                        double &Dmin);

    //--- PD Tree Interface Methods ---//

    int NodeMightBeCloser(const vct3 &Xp, const vct3 &Xn,
                          msh3DirPDTreeNode const *node,
                          double ErrorBound);

    virtual double FindClosestPointOnDatum(const vct3 &Xp, const vct3 &Xn,
                                           vct3 &closest, vct3 &closestNorm,
                                           int datum) = 0;

    virtual int DatumMightBeCloser(const vct3 &Xp, const vct3 &Xn,
                                   int datum,
                                   double ErrorBound) = 0;
};

#endif
