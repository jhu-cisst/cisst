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
// ****************************************************************************
//
//  $Id: DirPDTree2DNode.h, v0.1 Exp $
//
//  Description:
//	        
//
//  Usage:
//	        
//
//  See Also:
//	        
//
//  Author(s):	Seth Billings
//
//  Created on:	
//
//
//              Developed by the Engineering Research Center for
//          Computer-Integrated Surgical Systems & Technology (cisstST)
//
//               Copyright(c) 2001, The Johns Hopkins University
//                          All Rights Reserved.
//
//  Experimental Software - Not certified for clinical use.
//  For use only by cisstST sponsored research projects.  For use outside of
//  cisstST, please contact Dr. Russell Taylor, cisstST Director, at rht@cs.jhu.edu
//  
// ****************************************************************************

#ifndef _DirPDTree2DNode_h
#define _DirPDTree2DNode_h

#include <stdio.h>
#include <assert.h>

#include <cisstMesh/BoundingBox2D.h>
#include <cisstVector/vctTransformationTypes.h>

//#define DebugDirPDTree2D
//#define DEBUG_DirPDTree2dNode

class DirPDTree2DBase;    // forward declerations for mutual dependency
class alg2D_DirPDTree;    //  ''

class DirPDTree2DNode
{
  // This class represents a node in a directed PD tree (a PD tree
  //  for oriented points); there are no derived versions of this class.

  //friend class DirPDTree2DBase;


  //--- Variables ---//

public:

  DirPDTree2DBase*	MyTree;
  DirPDTree2DNode*  Parent;
  int* DataIndices;
  int NData;
  int myDepth;
  DirPDTree2DNode *LEq, *More;

  vctFrm2 F;                  // transforms world -> local node coords
  BoundingBox2D Bounds;  // bounding box for this node

  bool bUsingOBB;
  unsigned int splitDim;  // X: 0, y: 1 -- only applies when not using OBB

  vct2    posAvg;
  vct2x2  posCov;
  vct2    Navg;         // avg orientation of the datums in this node  
  double  dThetaMax;    // max deviation (in radians) from the avg orientation

  // efficiency variables to speed-up building the tree
  vct2    posSum;       // summation of positions in this node
  vct2x2  covSum;       // summation of outer products of positions
  vct2    Nsum;         // summation of the orientations in this node



  //--- Methods ---//

public:

  // constructor
  DirPDTree2DNode(
    int *pDataIndexArray, int numIndexes,
    DirPDTree2DBase* pTree, DirPDTree2DNode* pParent,
    bool bComputeOBB = true, unsigned int splitDimension = 0);

  // debug constructor
  DirPDTree2DNode(double) :
    MyTree(NULL),
    Parent(NULL),
    DataIndices(NULL),
    NData(0),
    LEq(NULL),
    More(NULL)
  {};

  // destructor
  ~DirPDTree2DNode();

  // Check if a datum in this node has a lower match error than the error bound
  //  If a lower match error is found, set the new closest point, update error
  //  bound, and return the global datum index of the closest datum.
  //  Otherwise, return -1.
  int FindClosestDatum(const vct2 &v, const vct2 &n,
    vct2 &closestPoint, vct2 &closestPointNorm,
    double &ErrorBound,
    unsigned int &numNodesVisited,
    unsigned int &numNodesSearched);

  inline int   NumData() const { return NData; };
  inline int   IsTerminalNode() const { return LEq == NULL; };

  int   ConstructTree(int CountThresh, double DiagThresh);

  DirPDTree2DNode* GetChildSplitNode(const vct2 &datumPos);

  //virtual void Print(FILE* chan, int indent);

  // Return the global datum index of the ith datum in this node
  inline int Datum(int i) const
  {
#ifdef DEBUG_DirPDTree2dNode
    assert(i >= 0 && i < NData);
#endif
    return DataIndices[i];
  }

  int& Datum(int i)
  {
#ifdef DEBUG_DirPDTree2dNode
    assert(i >= 0 && i < NData);
#endif
    return DataIndices[i];
  }

protected:

  int   SortNodeForSplit();
  void  ConstructLeaf();

  //vct2    ComputePositionSum();
  //vct2x2  ComputeCovarianceSum();
  vct2    ComputeOrientationSum();
  vct2    ComputeOrientationAverage(vct2 &Nsum);
  double  ComputeOrientationThetaMax(vct2 Navg);
  vctFrm2 ComputeCovFrame(vct2x2 posCov, vct2 posMean);

public:

  // debug routines
  int   FindTerminalNode(int datum, DirPDTree2DNode **termNode);
  void  PrintTerminalNodes(std::ofstream &fs);
  bool  NodeContainsDatum(int datum)
  {
    for (int i = 0; i < NData; i++)
    {
      if (datum == DataIndices[i]) return true;
    }
    return false;
  }

};

#endif

//// Check if a datum in this node has a lower match error than the error bound,
////  and an, in addition, subject to the Euclidean distance constraint from the sample.
////  If a lower match error is found, set the new closest point, update error
////  bound, and return the global datum index of the closest datum.
////  Otherwise, return -1.
////  Returns datum closer than "ErrorBound" to v subject to a geometric distance
////  constraint, else return -1
////  Sets the new closest point and updates error bound
//int FindClosestDatum( const vct3 &v, const vct3 &n,
//                       vct3 &closestPoint, vct3 &closestPointNorm,
//                       double posWeight, double normWeight,
//                       double &ErrorBound,
//                       unsigned int &numNodesVisited,
//                       unsigned int &numNodesSearched,
//                       double geomDistBound );
