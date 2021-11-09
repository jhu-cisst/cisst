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

#ifndef _msh3PDTreeNode_h
#define _msh3PDTreeNode_h

#include <stdio.h>
#include <assert.h>

#include <cisstMesh/mshForwardDeclarations.h>

#include <cisstMesh/msh3BoundingBox.h>
#include <cisstMesh/msh3Mesh.h>

// if the PDTree noise model is not needed then PDTree construction
// time may be reduced by disabling it
#define ENABLE_PDTREE_NOISE_MODEL

// Always include last!
#include <cisstMesh/mshExport.h>

class CISST_EXPORT msh3PDTreeNode
{
    //
    // This class implements a node of a PD tree;
    //  there are no derived versions of this class.
    //

    //--- Variables ---//

 public:

    msh3PDTreeBase  *pMyTree;
    msh3PDTreeNode  *pParent;
    msh3PDTreeNode  *pLEq, *pMore; // child nodes

    int *pDataIndices;
    int NData;
    int myDepth;

    vctFrm3 F;                // transforms world -> local node coords
    msh3BoundingBox Bounds;  // bounding box for this node

#ifdef ENABLE_PDTREE_NOISE_MODEL

    // Noise Model for this Node
    //  Note:  this group of variables depend only on the node and are
    //         computed only once when the tree is built
    double EigMax;      // largest eigenvector of covariances in the node
    vct3   EigRankMin;  // min eigenvalues by rank w/in node
    // Switch to use parent noise model instead of local noise model bounds
    bool bUseParentEigMaxBound;
    // this is for the spherical node bounds where the effective bound
    //  is comprised of only the log term and eigenvalues of the node and
    //  sample noise models
    bool bUseParentEigRankMinBounds;

    // these point to local values if using local noise model for this node
    //  or to parent's "values in use" if using parent's noise model for this node
    double *pEigMax;
    vct3 *pEigRankMin;

#endif

    //--- Methods ---//

 public:

    // constructor
    msh3PDTreeNode(int *pDataIndexArray, int numIndexes,
                   msh3PDTreeBase *pTree, msh3PDTreeNode *pParent);

    // debug constructor
    msh3PDTreeNode(double)
        : pMyTree(NULL),
        pParent(NULL),
        pLEq(NULL),
        pMore(NULL),
        pDataIndices(NULL),
        NData(0)
#ifdef ENABLE_PDTREE_NOISE_MODEL
        ,pEigMax(NULL),
        pEigRankMin(NULL)
#endif
            {}

    // destructor
    ~msh3PDTreeNode();

    // Check if a datum in this node has a lower match error than the error bound
    //  If a lower match error is found, set the new closest point, update error
    //  bound, and return the global datum index of the closest datum.
    //  Otherwise, return -1.
    int FindClosestDatum(const vct3 &v,
                         vct3 &closestPoint,
                         double &ErrorBound,
                         unsigned int &numNodesVisited,
                         unsigned int &numNodesSearched);

    // Find intersected points in node given a point and bounding distance
    void FindIntersectedPoints(const vct3 &v,
                               const double boundingDistance,
                               msh3Mesh& mesh,
                               std::vector<int> &faceIdx);

    inline int NumData() const { return NData; };
    inline int IsTerminalNode() const { return pLEq == NULL; };

    int SortNodeForSplit();
    vctFrm3 ComputeCovFrame(int i0, int i1);
    int ConstructSubtree(int CountThresh, double DiagThresh);

    void AccumulateCentroid(int datum, vct3 &sum) const;
    void AccumulateVariances(int datum, const vct3 &mean, vctDouble3x3 &C) const;

    // Return the global datum index of the ith datum in this node
    inline int Datum(int i) const
    {
        assert(i >= 0 && i < NData);
        return pDataIndices[i];
    };
    inline int& Datum(int i)
    {
        assert(i >= 0 && i < NData);
        return pDataIndices[i];
    };

    msh3PDTreeNode* GetChildSplitNode(const vct3 &datumPos);

    // debug routines
    int FindTerminalNode(int datum, msh3PDTreeNode **termNode);
    void PrintTerminalNodes(std::ofstream &fs);
    inline bool NodeContainsDatum(int datum)
    {
        for (int i = 0; i < NData; i++) {
            if (datum == pDataIndices[i]) return true;
        }
        return false;
    }

};

#endif
