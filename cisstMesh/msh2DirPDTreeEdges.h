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

#ifndef _msh2DirPDTreeEdges_h
#define _msh2DirPDTreeEdges_h

#include <cisstMesh/msh2DirPDTreeBase.h>
#include <cisstMesh/msh2Edges.h>

// Always include last!
#include <cisstMesh/mshExport.h>

class CISST_EXPORT msh2DirPDTreeEdges : public msh2DirPDTreeBase
{
    // NOTE: for function overrides, be sure "const" type is same as the base class otherwise,
    //       the base class function will be treated as a different function and not actually
    //       be overridden!

 public:

    msh2Edges EdgeList;

    //-- Methods --//

    // constructor
    //  nThresh   - min number of datums in subdivided node
    //  diagThresh - min physical size of subdivided node
    msh2DirPDTreeEdges(const vctDynamicVector<vct2> &edgesV1,
                       const vctDynamicVector<vct2> &edgesV2,
                       const vctDynamicVector<vct2> &edgesNorm,
                       int nThresh, double diagThresh, bool bUseOBB = true);

    // destructor
    virtual ~msh2DirPDTreeEdges();

    //-- Base Class Method Overrides --//
    virtual vct2 DatumSortPoint(int datum) const;  // return sort point of this datum
    virtual vct2 DatumNorm(int datum) const;       // return normal orientation of this datum

    virtual void EnlargeBounds(const vctFrm2& F, int datum, msh2BoundingBox& BB) const;
    virtual void EnlargeBounds(int datum, msh2BoundingBox& BB) const;

    //-- Edge Methods --//

    // TODO: &
    // return the triangle object corresponding to this datum index
    inline msh2Edge GetEdge(int datum) const
    {
        return EdgeList.Edges[(unsigned int)datum];
    }

};

#endif
