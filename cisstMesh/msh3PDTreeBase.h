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

#ifndef _msh3PDTreeBase_h
#define _msh3PDTreeBase_h

#include <stdio.h>
#include <assert.h>

#include <cisstMesh/mshForwardDeclarations.h>

#include <cisstMesh/msh3BoundingBox.h>
#include <cisstMesh/msh3PDTreeNode.h>
#include <cisstMesh/msh3AlgPDTree.h>

//#define DEBUG_PD_TREE

// Always include last!
#include <cisstMesh/mshExport.h>

class CISST_EXPORT msh3PDTreeBase
{
    //
    // This is the base class for a PD tree.
    //  This class defines the entry point for performing
    //  a search. The type of datum (i.e. triangle, point, etc.)
    //  is unknown to this class and must be defined within a
    //  derived class, making this class abstract.
    // This class stores a pointer to an algorithm object
    //  which implements the key search routines.
    //

    friend class msh3PDTreeNode;

    //--- Variables ---//

 public:

    // reference to algorithm must exist here so that all nodes
    //  may access it
    msh3AlgPDTree *pAlgorithm;

 protected:

#ifdef DEBUG_PD_TREE
    FILE *debugFile;
    FILE *debugFile2;
#endif

    int NData, NNodes, treeDepth;
    int* DataIndices;
    msh3PDTreeNode *Top;

    //--- Methods ---//

 public:

    // constructors
    msh3PDTreeBase() :
        pAlgorithm(NULL),
        NData(0), NNodes(0), treeDepth(0),
        DataIndices(NULL), Top(NULL)
        {
#ifdef DEBUG_PD_TREE
            debugFile = fopen("debugPDTree.txt","w");
            debugFile2 = fopen("debugPDTree2.txt","w");
#endif
        };

    // destructor
    virtual ~msh3PDTreeBase() {}

    int FastInitializeProximalDatum(const vct3 &v, vct3 &proxPoint);

    inline void SetSearchAlgorithm(msh3AlgPDTree *pAlg) {
        pAlgorithm = pAlg;
    }

    // Returns the index for the datum in the tree that has lowest match error for
    //  the given point and set the closest point values
    int FindClosestDatum(const vct3 &v,
                         vct3 &closestPoint,
                         int prevDatum,
                         double &matchError,
                         unsigned int &numNodesSearched);

    //  // Finds the intersected points of a PDTree given a bounding distance
    //  virtual int FindIntersectedPoints(
    //            const vct3 &v, const double boundingDistance, std::vector<vct3> &intersectedPoints,
    //             std::vector<int> &datumPoints,
    //             std::vector<bool> &edgePointFlag);

    inline int NumData() const { return NData; };
    inline int NumNodes() const { return NNodes; };
    inline int TreeDepth() const { return treeDepth; };

    // debug routines
    int ValidateClosestDatum(const vct3 &v, vct3 &closestPoint);
    int ValidateClosestDatum_ByEuclideanDist(const vct3 &v, vct3 &closestPoint);
    int FindTerminalNode(int datum, msh3PDTreeNode **termNode);
    void PrintTerminalNodes(std::ofstream &fs);

    //--- Virtual Methods ---//
    //
    // These methods require a known datum type
    //
    virtual vct3 DatumSortPoint(int datum) const = 0;
    virtual void EnlargeBounds(const vctFrm3& F, int datum, msh3BoundingBox& BB) const = 0;

#ifdef ENABLE_PDTREE_NOISE_MODEL

    //--- Noise Model Methods ---//

    virtual vct3x3& DatumCov(int datum) = 0;    // return measurement noise model for this datum
    virtual vct3x3* DatumCovPtr(int datum) = 0; // return measurement noise model for this datum
    virtual vct3& DatumCovEig(int datum) = 0;
    virtual vct3* DatumCovEigPtr(int datum) = 0;

    // may have to be manually called by user after defining the noise
    //  model of the datums
    //  (depending on the PD tree type and constructor used)
    void ComputeNodeNoiseModels();
    void ComputeSubNodeNoiseModel(msh3PDTreeNode *node, bool useLocalVarsOverride);

#endif // ENABLE_PDTREE_NOISE_MODEL

};

#endif
