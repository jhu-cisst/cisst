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

#ifndef _msh3DirPDTreeMesh_h
#define _msh3DirPDTreeMesh_h

#include <cisstMesh/msh3DirPDTreeBase.h>
#include <cisstMesh/msh3Mesh.h>

// Always include last!
#include <cisstMesh/mshExport.h>

class CISST_EXPORT msh3DirPDTreeMesh : public msh3DirPDTreeBase
{
    // Here we define the datum type of the directed covariance
    //  tree to be a triangle and we represent the underlying
    //  datum data using a mesh.

    // NOTE: for function overrides, be sure "const" type is same as the base class otherwise,
    //       the base class function will be treated as a different function and not actually
    //       be overridden!

 public:
    msh3Mesh Mesh;
    msh3BoundingBox Bounds;

    //-- Methods --//

    // constructor
    //  mesh      - mesh from which to construct the tree
    //              (each datum in the tree is a triangle of the mesh)
    //  nThresh   - min number of datums in subdivided node
    //  diagThreh - min physical size of subdivided node
    msh3DirPDTreeMesh(msh3Mesh mesh, int nThresh, double diagThresh);

    // destructor
    virtual ~ msh3DirPDTreeMesh();

    //-- Base Class Method Overrides --//

	virtual vct3 DatumSortPoint(int datum);  // return sort point of this datum

    virtual vct3 DatumNorm(int datum);       // return normal orientation of this datum

    virtual void EnlargeBounds(const vctFrm3& F) const;
    virtual void EnlargeBounds(const vctFrm3& F, msh3DirPDTreeNode *pNode) const;
    virtual void EnlargeBounds(const vctFrm3& F, int datum, msh3BoundingBox& BB) const;

    //--- Noise Model Methods ---//

    vct3x3& DatumCov(int datum)       // return measurement noise model for this datum
    {
        return Mesh.TriangleCov[datum];
    }
    vct3x3* DatumCovPtr(int datum)    // return measurement noise model for this datum
    {
        return &(Mesh.TriangleCov[datum]);
    }

    vct3& DatumCovEig(int datum)         // return measurement noise model for this datum
    {
        return Mesh.TriangleCovEig[datum];
    }
    vct3* DatumCovEigPtr(int datum)      // return measurement noise model for this datum
    {
        return &(Mesh.TriangleCovEig[datum]);
    }

};

#endif
