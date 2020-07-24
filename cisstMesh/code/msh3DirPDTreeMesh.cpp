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

#include <cisstMesh/msh3DirPDTreeMesh.h>

msh3DirPDTreeMesh::msh3DirPDTreeMesh(msh3Mesh mesh, int countThresh, double diagThresh )
{
    this->Mesh = mesh;

    NData = mesh.NumTriangles();
    DataIndices = new int[NData];
    for (int i=0;i<NData;i++) {
        DataIndices[i]=i;
        }
    Top = new msh3DirPDTreeNode(DataIndices,NData,this,NULL);
    NNodes = 0; NNodes++;
    treeDepth = Top->ConstructSubtree(countThresh,diagThresh);

#ifdef DebugDirPDTree
    fprintf(debugFile, "Directional Mesh Cov Tree built: NNodes=%d  NData=%d  TreeDepth=%d\n", NumNodes(), NumData(), TreeDepth());
#endif
}

msh3DirPDTreeMesh::~msh3DirPDTreeMesh()
{
    if (Top) delete Top;
    if (DataIndices) delete DataIndices;
}

vct3 msh3DirPDTreeMesh::DatumSortPoint(int datum)
{
    //// use vertex 0 as the sort point
    //return mesh.FaceCoord(datum, 0);

    // use triangle center as the sort point
    return (Mesh.FaceCoord(datum, 0) +
            Mesh.FaceCoord(datum, 1) +
            Mesh.FaceCoord(datum, 2)) / 3.0;
}

vct3 msh3DirPDTreeMesh::DatumNorm(int datum)
{
    return Mesh.faceNormals(datum);
}

void msh3DirPDTreeMesh::EnlargeBounds(const vctFrm3& F, int datum, msh3BoundingBox& BB) const
{
    vct3 v1, v2, v3;
    Mesh.FaceCoords(datum, v1, v2, v3);
    BB.Include(F*v1);
    BB.Include(F*v2);
    BB.Include(F*v3);
}

void msh3DirPDTreeMesh::EnlargeBounds(const vctFrm3& F, msh3DirPDTreeNode *pNode) const
{
    if (!pNode->IsTerminalNode()) {
        EnlargeBounds(F, pNode->pLEq);
        msh3BoundingBox LparentBounds = pNode->pLEq->pParent->Bounds;
        msh3BoundingBox LchildBounds = pNode->pLEq->Bounds;
        LparentBounds.Include(LchildBounds);

        EnlargeBounds(F, pNode->pMore);
        msh3BoundingBox RparentBounds = pNode->pMore->pParent->Bounds;
        msh3BoundingBox RchildBounds = pNode->pMore->Bounds;
        RparentBounds.Include(RchildBounds);
    }
    else if (pNode->IsTerminalNode()) {
        for (int i = 0; i < pNode->NumData(); i++)
            EnlargeBounds(F, pNode->Datum(i), pNode->Bounds);
    }
}

void msh3DirPDTreeMesh::EnlargeBounds(const vctFrm3& F) const
{
    msh3DirPDTreeNode *pNode;
    pNode = Top;
    EnlargeBounds(F, pNode);
}
