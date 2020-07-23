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
#ifndef _DirPDTree2DBase_h
#define _DirPDTree2DBase_h

#include <stdio.h>
#include <assert.h>

#include <cisstMesh/BoundingBox2D.h>
#include <cisstMesh/DirPDTree2DNode.h>
#include <cisstMesh/alg2D_DirPDTree.h>


class DirPDTree2DBase
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

  friend class DirPDTree2DNode;


  //--- Variables ---//

public:

  // reference to algorithm must exist here so that all nodes 
  //  may access it
  alg2D_DirPDTree    *algorithm;

//protected:

#ifdef DebugDirPDTree2D
  FILE *debugFile;
  FILE *debugFile2;
#endif

  DirPDTree2DNode *Top;
  int* DataIndices;
  int NData;
  int NNodes;
  int treeDepth;
  

  //--- Methods ---//

public:

  // constructors
  DirPDTree2DBase() :
    algorithm(NULL),
    Top(NULL),
    DataIndices(NULL),
    NData(0), NNodes(0), treeDepth(0)
  {
#ifdef DebugDirPDTree2D
    //debugFile = fopen("D:/Code/Repos_Git/SinusProject/MATLAB/debugDirPDTree.txt", "w");
    //debugFile2 = fopen("D:/Code/Repos_Git/SinusProject/MATLAB/debugDirPDTree2.txt", "w");
    debugFile = fopen("./debugDirPDTree.txt", "w");
    debugFile2 = fopen("./debugDirPDTree2.txt", "w");
    //debugFile = fopen("../ICP_TestData/LastRun/debugDirPDTree.txt","w");
    //debugFile2 = fopen("../ICP_TestData/LastRun/debugDirPDTree2.txt","w");

    //int printReturn;
    //printReturn = fprintf(debugFile, "Debug File Open\n");
    //printReturn = fprintf(debugFile2, "Debug File2 Open\n");
    //fflush(debugFile);
    //fflush(debugFile2);
#endif
  };

  virtual ~DirPDTree2DBase() {}

  void SetSearchAlgorithm(alg2D_DirPDTree *alg)
  {
    algorithm = alg;
  }

  // Return the index for the datum in the tree that has lowest match error for
  //  the given point and set the closest point values
  int FindClosestDatum(
    const vct2 &v, const vct2 &n,
    vct2 &closestPoint, vct2 &closestPointNorm,
    int prevDatum,
    double &matchError,
    unsigned int &numNodesSearched);

  // quickly computes a nearby (not best matching) datum point
  //  used for initializing the closest datums prior to
  //  performing a true tree search
  int FastInitializeProximalDatum(
    const vct2 &v, const vct2 &n,
    vct2 &proxPoint, vct2 &proxNorm);

  int NumData() const { return NData; };
  int NumNodes() const { return NNodes; };
  int TreeDepth() const { return treeDepth; };

  // debug routines
  int   ValidateClosestDatum(const vct2 &v, const vct2 &n, vct2 &closestPoint, vct2 &closestPointNorm);
  int   FindTerminalNode(int datum, DirPDTree2DNode **termNode);
  void  PrintTerminalNodes(std::ofstream &fs);


  //--- Virtual Methods ---//
  //
  // These methods require a known datum type
  //

  virtual vct2  DatumSortPoint(int datum) const = 0; // reference point for assigning datum to a node
  virtual vct2  DatumNorm(int datum) const = 0;      // datum orientation (normal vector)
  virtual void  EnlargeBounds(const vctFrm2& F, int datum, BoundingBox2D& BB) const = 0;
  virtual void  EnlargeBounds(int datum, BoundingBox2D& BB) const = 0;

  //virtual void  Print(FILE* chan, int indent);
  //virtual void  PrintDatum(FILE* chan, int indent, int datum);

};

#endif
