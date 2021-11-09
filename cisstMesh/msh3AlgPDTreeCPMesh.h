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

#ifndef _msh3AlgPDTreeCPMesh_h
#define _msh3AlgPDTreeCPMesh_h

#include <cisstMesh/msh3AlgPDTreeCP.h>
#include <cisstMesh/msh3PDTreeMesh.h>
#include <cisstMesh/TriangleClosestPointSolver.h>

// Always include last!
#include <cisstMesh/mshExport.h>

class CISST_EXPORT msh3AlgPDTreeCPMesh : public msh3AlgPDTreeCP
{
    //
    // Implements closest point search algorithms for a mesh shape
    //  (i.e. triangle datum type)
    //

    //--- Algorithm Parameters ---//

 protected:

    msh3PDTreeMesh *pTree;
    TriangleClosestPointSolver TCPS;

    //--- Algorithm Methods ---//

 public:

    // constructor
    msh3AlgPDTreeCPMesh(msh3PDTreeMesh *pTree) :
        msh3AlgPDTreeCP(pTree),
        pTree(pTree),
        TCPS(*(pTree->Mesh))
            {}

    // destructor
    virtual ~msh3AlgPDTreeCPMesh() override {}


    //--- PD Tree Interface Methods ---//

    double FindClosestPointOnDatum(const vct3 &v,
                                   vct3 &closest,
                                   int datum) override;

    double FindClosestPointOnDatumWithEdgeDetection(const vct3 &v,
                                                    vct3 &closest,
                                                    int &cpLocation,
                                                    int datum) override;

    int DatumMightBeCloser(const vct3 &v,
                           int datum,
                           double ErrorBound) override;
};

#endif
