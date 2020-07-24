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
#ifndef _PDTree_Mesh_h
#define _PDTree_Mesh_h

#include <cisstMesh/msh3PDTreeBase.h>
#include <cisstMesh/msh3Mesh.h>
#include <limits>

class msh3PDTreeMesh : public msh3PDTreeBase
{ 
  //
  // This class implements a PD tree for a mesh shape
  //


  //--- Variables ---//

public:

    cisstMesh *Mesh;
    msh3BoundingBox Bounds;


  //--- Methods ---//

public:

  // constructor
  //  mesh       - target shape from which to construct the tree
  //               (each triangle of the mesh becomes a datum in the tree)
  //  nThresh    - min number of datums to subdivide a node
  //  diagThresh - min physical size to subdivide a node
    msh3PDTreeMesh(cisstMesh &Mesh, int nThresh, double diagThresh);

  // destructor
  virtual ~msh3PDTreeMesh();


  //--- Base Class Virtual Methods ---//

  virtual vct3 DatumSortPoint(int datum) const;

    virtual void EnlargeBounds(const vctFrm3& F) const;
    virtual void EnlargeBounds(const vctFrm3& F, msh3PDTreeNode *pNode) const;
    virtual void EnlargeBounds(const vctFrm3& F, int datum, msh3BoundingBox& BB) const;


  //--- Noise Model Methods ---//

  vct3x3& DatumCov(int datum)       // return measurement noise model for this datum
  {
    return Mesh->TriangleCov[datum];
  }
  vct3x3* DatumCovPtr(int datum)    // return measurement noise model for this datum
  {
    return &(Mesh->TriangleCov[datum]);
  }

  vct3& DatumCovEig(int datum)         // return measurement noise model for this datum
  {
    return Mesh->TriangleCovEig[datum];
  }
  vct3* DatumCovEigPtr(int datum)      // return measurement noise model for this datum
  {
    return &(Mesh->TriangleCovEig[datum]);
  }

  // Finds the intersected points of a PDTree given a bounding distance
  int FindIntersectedPoints(
          const vct3 &v,
          const double boundingDistance,
          std::vector<int> &faceIdx);

};

#endif
