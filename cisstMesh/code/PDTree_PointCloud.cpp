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

#include <stdio.h>

#include <cisstMesh/PDTree_PointCloud.h>
#include <cisstMesh/PDTreeNode.h>
#include <cisstMesh/utilities.h>

// Build tree from point cloud input
//  this constructor does not define the noise model for the point cloud;
//  the user must do this manually by calling the noise model methods of this class
PDTree_PointCloud::PDTree_PointCloud(
  cisstPointCloud &pointCloud,
  int nThresh, 
  double diagThresh ) :
pointCloud(pointCloud)
{
  NData = pointCloud.points.size();
  DataIndices = new int[NData];
  for (int i = 0; i < NData; i++)
  {
    DataIndices[i] = i;
  }
  Top = new PDTreeNode(DataIndices, NData, this, NULL);
  NNodes = 0; NNodes++;
  treeDepth = Top->ConstructSubtree(nThresh, diagThresh);

#ifdef DEBUG_PD_TREE
  fprintf(debugFile, "Point Cloud Cov Tree built: NNodes=%d  NData=%d  TreeDepth=%d\n", NumNodes(), NumData(), TreeDepth());
#endif
}


PDTree_PointCloud::~PDTree_PointCloud()
{
  if (Top) delete Top;
  if (DataIndices) delete DataIndices;
}


vct3 PDTree_PointCloud::DatumSortPoint(int datum) const
{ // datum sort point is the point itself
#ifdef DEBUG_PD_TREE
  return pointCloud.points.at(datum);
#else
  return pointCloud.points.Element(datum);
#endif
}

void PDTree_PointCloud::EnlargeBounds(const vctFrm3& F, int datum, BoundingBox& BB) const
{
  BB.Include(F*pointCloud.points.Element(datum));
}
