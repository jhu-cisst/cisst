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
#ifndef _DirPDTreeBase_h
#define _DirPDTreeBase_h

#include <limits>

#include <stdio.h>
#include <assert.h>

#include <cisstMesh/BoundingBox.h>
#include <cisstMesh/DirPDTreeNode.h>
#include <cisstMesh/algDirPDTree.h>

//#define DebugDirPDTree


class DirPDTreeBase
{
  //
  // This is the base class for a directional PD tree
  //  (i.e. for oriented data).
  // This class defines the entry point for performing
  //  a search. The type of datum (i.e. triangle, point, etc.)
  //  is unknown to this class and must be defined within a
  //  derived class, making this class abstract.
  // This class stores a pointer to an pAlgorithm object
  //  which implements the key search routines.
  //

  friend class DirPDTreeNode;


  //--- Variables ---//

public:

  // reference to pAlgorithm must exist here so that all nodes 
  //  may access it
  algDirPDTree *pAlgorithm;
  
protected:

#ifdef DebugDirPDTree
  FILE *debugFile;
  FILE *debugFile2;
#endif

  int NData, NNodes, treeDepth;
  int* DataIndices;
  DirPDTreeNode *Top;


  //--- Methods ---//

public:

  // constructors
  DirPDTreeBase():
      pAlgorithm(NULL),
      NData(0), NNodes(0), treeDepth(0), 
      DataIndices(NULL), Top(NULL)
  { 
#ifdef DebugDirPDTree
    debugFile = fopen("../ICP_TestData/LastRun/debugDirPDTree.txt","w");
    debugFile2 = fopen("../ICP_TestData/LastRun/debugDirPDTree2.txt","w");
#endif
  };

  // destructor
  virtual ~DirPDTreeBase() {}

  int FastInitializeProximalDatum(
    const vct3 &v, const vct3 &n, vct3 &proxPoint, vct3 &proxNorm);

  void SetSearchAlgorithm(algDirPDTree *pAlg)
  {
    pAlgorithm = pAlg;
  }

  // Return the index for the datum in the tree that has lowest match error for
  //  the given point and set the closest point values
  int FindClosestDatum(
    const vct3 &v, const vct3 &n,
    vct3 &closestPoint, vct3 &closestPointNorm,
    int prevDatum,
    double &matchError,
    unsigned int &numNodesSearched,
    double currentMatchError = std::numeric_limits<double>::max());

  // Compute the match error for a given datum
  double ComputeDatumMatchError( const vct3 &v, const vct3 &n, int datum);

  int NumData() const { return NData; };
  int NumNodes() const { return NNodes; };
  int TreeDepth() const { return treeDepth; };

  // debug routines
  int   ValidateClosestDatum( const vct3 &v, const vct3 &n,
                              vct3 &closestPoint, vct3 &closestPointNorm);
  int   FindTerminalNode(int datum, DirPDTreeNode **termNode);
  void  PrintTerminalNodes(std::ofstream &fs);


  //--- Virtual Methods ---//
  //
  // These methods require a known datum type
  //
  virtual vct3  DatumSortPoint(int datum) = 0; // reference point for assigning datum to a node
  virtual vct3  DatumNorm(int datum) = 0;      // datum orientation (normal vector)
  virtual void  EnlargeBounds(const vctFrm3& F, int datum, BoundingBox& BB) const = 0;

};

#endif // _DirPDTreeBase_h
