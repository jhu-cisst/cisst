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

#ifndef _msh2AlgDirPDTreevonMisesPointCloud_h
#define _msh2AlgDirPDTreevonMisesPointCloud_h

#include <cisstMesh/msh2AlgDirPDTreevonMises.h>
#include <cisstMesh/msh2DirPDTreePointCloud.h>

// Always include last!
#include <cisstMesh/mshExport.h>

class CISST_EXPORT msh2AlgDirPDTreevonMisesPointCloud : public msh2AlgDirPDTreevonMises
{
    //
    // Implements von-Mises-based oriented point algorithm
    //  for a 2D point cloud
    //

    //--- Algorithm Parameters ---//

 protected:

    msh2DirPDTreePointCloud *pDirTree;


    //--- Algorithm Methods ---//

 public:

    // constructor
    msh2AlgDirPDTreevonMisesPointCloud(msh2DirPDTreePointCloud *pDirTree,
                                       double k = 1.0,
                                       double sigma2 = 1.0)
        : msh2AlgDirPDTreevonMises(pDirTree,k,sigma2),
        pDirTree(pDirTree)
        {}

    // destructor
    virtual ~msh2AlgDirPDTreevonMisesPointCloud() {}


    //--- PD Tree Interface Methods ---//

    // fast check if a datum might have smaller match error than error bound
    virtual int DatumMightBeCloser(const vct2 &sample, const vct2 &sampleNorm,
                                   int datum,
                                   double ErrorBound);

    // finds the point on this datum with lowest match error
    //  and returns the match error and closest point
    virtual double FindClosestPointOnDatum(const vct2 &sample, const vct2 &sampleNorm,
                                           vct2 &closest, vct2 &closestNorm,
                                           int datum);
};

#endif
