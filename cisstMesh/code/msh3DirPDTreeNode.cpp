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

#include <stdio.h>
#include <iostream>

#include <cisstMesh/msh3DirPDTreeNode.h>
#include <cisstMesh/msh3DirPDTreeBase.h>
#include <cisstMesh/msh3Utilities.h>

msh3DirPDTreeNode::msh3DirPDTreeNode(
                                     int* pDataIndexArray,
                                     int numIndexes,
                                     msh3DirPDTreeBase* pTree,
                                     msh3DirPDTreeNode* pParent) :
    pMyTree(pTree),
    pParent(pParent),
    pLEq(NULL),
    pMore(NULL),
    pDataIndices(pDataIndexArray),
    NData(numIndexes),
    Bounds()
{
    // Compute local coordinate frame for this node
    //  computes local -> global
    //  take inverse to get global -> local
    F = ComputeCovFrame(0, NData).Inverse();
    for (int i = 0; i < NData; i++) {
        // since we don't know what type of datum we're dealing with
        //  (and since we want the bounds to completely hold all of this datum)
        //  we must place the enlarge bounds function at the tree level where
        //  the datum type is known.
        pMyTree->EnlargeBounds(F, Datum(i), Bounds);
    }
    //std::stringstream ss;
    //ss << F.Rotation().Row(0) << " " << F.Rotation().Row(1) << " "
    //  << " " << F.Rotation().Row(2) << " " << F.Translation() << std::endl;
    ////ss << "F:" << F << std::endl;
    //fprintf(pMyTree->debugFile, "%s", ss.str().c_str());
}

msh3DirPDTreeNode::~msh3DirPDTreeNode()
{
    if (pLEq != NULL) delete pLEq;
    if (pMore != NULL) delete pMore;
}

// computes a local reference frame for this node based on the
//  covariances of the datum sort positions; returns a frame
//  transformation that converts points from local -> global coordinates
vctFrm3 msh3DirPDTreeNode::ComputeCovFrame(int i0, int i1)
{ // returns a frame whose origin is at centroid and whose x-axis
  //  points in the direction of largest point spread
    vct3 p(0, 0, 0);
    vctRot3 R;
    vctDouble3x3 C(0.0);    // covariances
    vctDouble3x3 Q;		      // Eigen vectors
    vct3 e;				          // Eigen values

    int i;
    if (i1 <= i0) return vctFrm3();
    int N = i1 - i0;
    if (N < 5)
        return (pParent != NULL) ? pParent->F : vctFrm3();
    for (i = i0; i < i1; i++)
        AccumulateCentroid(Datum(i), p);
    p *= (1.0 / (i1 - i0));
    for (i = i0; i < i1; i++) {
        AccumulateVariances(Datum(i), p, C);
    }

    // compute eigen decomposition of covariance matrix
    ComputeCovEigenDecomposition_NonIter(C, e, Q);

    int j = 0;
    for (i = 1; i < 3; i++) {
        if (fabs(e(i)) > fabs(e(j))) j = i;
    };
    switch (j) {
    case 0: // E[0] is biggest eigen value
        R = Q;
        break;
    case 1:	// E[1] is biggest eigen value
        // by right hand rule, map x->y, y->-x, z->z
        //  (assuming Q is a valid rotation matrix)
        R.Column(0) = Q.Column(1);
        R.Column(1) = -Q.Column(0);
        R.Column(2) = Q.Column(2);
        break;
    case 2:	// E[2] is biggest eigen value
        // by right hand rule: x->z, y->y, z->-x
        R.Column(0) = Q.Column(2);
        R.Column(1) = Q.Column(1);
        R.Column(2) = -Q.Column(0);
    }
    // SDB: should this be: [R',-R'*p]?
    //  no, because the function that calls this takes the inverse
    return vctFrm3(R, p);
}

void msh3DirPDTreeNode::AccumulateCentroid(int datum, vct3 &sum) const
{
    sum += pMyTree->DatumSortPoint(datum);
}

// NOTE: providing the M argument is not important for the calling function,
//       it merely helps with speed-up, as memory for the matrix doesn't
//       have to be re-allocated N times
void msh3DirPDTreeNode::AccumulateVariances(int datum, const vct3 &mean, vctDouble3x3 &C) const
{
    static vctDouble3x3 M;
    vct3 d = pMyTree->DatumSortPoint(datum) - mean;
    M.OuterProductOf(d, d);
    C += M;
}

// returns a value "top", for which datums should be on the pMore side if t>=top
int msh3DirPDTreeNode::SortNodeForSplit()
{
    int top = NData;
    static int callNumber = 0;
    callNumber++;
    vct3 Ck; vct3 Ct;
    vct3 r = F.Rotation().Row(0);
    double px = F.Translation()[0];
    for (int k = 0; k < top; k++) {
        Ck = pMyTree->DatumSortPoint(Datum(k)); // 3D coordinate of datum in global coord system
        double kx = r*Ck + px;  // compute the x coordinate in local coord system
        if (kx > 0) {
            // this one needs to go to the end of the line
            while ((--top) > k) {
                Ct = pMyTree->DatumSortPoint(Datum(top));
                double tx = r*Ct + px;
                if (tx <= 0) {
                    int Temp = Datum(k);
                    Datum(k) = Datum(top);
                    Datum(top) = Temp;
                    break; // from the "top" loop
                };
            };	// end of the "t" loop
        };	// end of the kx>0 case; at this point F*datum.x-coord <= 0 for i=0,...,k
    };	// end of k loop
    return top;
}

msh3DirPDTreeNode* msh3DirPDTreeNode::GetChildSplitNode(const vct3 &datumPos)
{
    // node split occurs along the local x-axis
    double x_node = F.Rotation().Row(0)*datumPos + F.Translation()[0];
    if (x_node > 0)
        return pMore;
    else
        return pLEq;
}

// computes average orientation and max deviation for this node
//  returns the running sum of vector orientations
vct3 msh3DirPDTreeNode::ComputeOrientationParams()
{
    // avg orientation
    vct3 Nsum(0.0);
    for (int i = 0; i < NumData(); i++) {
        Nsum += pMyTree->DatumNorm(Datum(i));
    }
    if (Nsum.Norm() < 1e-10) {
        // prevent division by zero
        Navg.Assign(0.0, 0.0, 1.0);
    }
    else {
        Navg = Nsum.Normalized();
    }

    // max deviation from the avg orientation
    //  cos(theta) = n'*Navg   (note n & Navg are unit vectors)
    dThetaMax = 0.0;
    vct3 n;
    double Theta;
    for (int i = 0; i < NumData(); i++) {
        n = pMyTree->DatumNorm(Datum(i));
        Theta = acos(n.DotProduct(Navg));
        if (Theta > dThetaMax) {
            dThetaMax = Theta;
        }
    }
#ifdef DebugDirPDTree
    fprintf(pMyTree->debugFile, " dThetaMax = %f\n", dThetaMax);
#endif

    return Nsum;
}

// returns tree depth
int msh3DirPDTreeNode::ConstructSubtree(int CountThresh, double DiagThresh) {

    if (NumData() < CountThresh || Bounds.DiagonalLength() < DiagThresh) {
        // leaf node
#ifdef DebugDirPDTree
        fprintf(pMyTree->debugFile, "Leaf Node: Ndata=%d\tDiagLen=%f\n", NumData(), Bounds.DiagonalLength());
#endif
        ComputeOrientationParams();
        myDepth = 0;
        return myDepth;
    }

    int topLEq = SortNodeForSplit();

    if (topLEq == NumData() || topLEq == 0) {
        // need this in case count threshold = 1
        // TODO: avoid this case by NumData()<=CountThresh above
#ifdef DebugDirPDTree
        fprintf(pMyTree->debugFile, "ERROR! all data splits to one node; topLEq=%d\tNdata=%d\tDiagLen=%f\n",
                topLEq, NumData(), Bounds.DiagonalLength());
#endif
        ComputeOrientationParams();
        myDepth = 0;  // stop here and do not split any further
        return 0;
    }

#ifdef DebugDirPDTree
    fprintf(pMyTree->debugFile2, "NNodeL=%d\tNNodeR=%d\n", topLEq, NumData() - topLEq);
#endif

    assert (topLEq>0&&topLEq<NumData());

    int depthL, depthR;
    pLEq = new msh3DirPDTreeNode(pDataIndices, topLEq, pMyTree, this);
    pMyTree->NNodes++;
    depthL = pLEq->ConstructSubtree(CountThresh, DiagThresh);

    pMore = new msh3DirPDTreeNode(&pDataIndices[topLEq], NumData() - topLEq, pMyTree, this);
    pMyTree->NNodes++;
    depthR = pMore->ConstructSubtree(CountThresh, DiagThresh);

    myDepth = (depthL > depthR ? depthL : depthR) + 1;
    ComputeOrientationParams(); // TODO: speed up this one by using NSum from children
    return myDepth;
}

// Check if a datum in this node has a lower match error than the error bound
//  If a lower match error is found, set the new closest point, update error
//  bound, and return the global datum index of the closest datum.
//  Otherwise, return -1.
int msh3DirPDTreeNode::FindClosestDatum(const vct3 &v, const vct3 &n,
                                        vct3 &closestPoint, vct3 &closestPointNorm,
                                        double &ErrorBound,
                                        unsigned int &numNodesVisited,
                                        unsigned int &numNodesSearched)
{
    numNodesVisited++;

    // fast check if this node may contain a datum with better match error
    if (pMyTree->pAlgorithm->NodeMightBeCloser(v, n, this, ErrorBound) == 0) {
        return -1;
    }

    // Search points w/in this node
    int ClosestDatum = -1;
    numNodesSearched++;

    if (IsTerminalNode()) {
        // a leaf node => look at each datum in the node
        for (int i = 0; i < NData; i++) {
            int datum = Datum(i);

            // fast check if this datum might have a lower match error than error bound
            if (pMyTree->pAlgorithm->DatumMightBeCloser(v, n, datum, ErrorBound)) {
                // a candidate
                vct3 candidate;
                vct3 candidateNorm;
                // close check if this datum has a lower match error than error bound
                double err = pMyTree->pAlgorithm->FindClosestPointOnDatum(v, n, candidate, candidateNorm, datum);
                if (err < ErrorBound) {
                    closestPoint = candidate;
                    closestPointNorm = candidateNorm;
                    ErrorBound = err;
                    ClosestDatum = datum;
                }
            }
        }
        return ClosestDatum;
    }

    // here if not a terminal node =>
    //  extend search to both child nodes
    int ClosestLEq = -1;
    int ClosestMore = -1;

    // 1st call to pLEq updates both distance bound and closest point
    //  before 2nd call to pMore. If pMore returns (-1), then pMore had
    //  nothing better than pLEq and the resulting datum should be
    //  the return value of pLEq (whether that is -1 or a closer datum index)
    ClosestLEq = pLEq->FindClosestDatum(v, n, closestPoint, closestPointNorm, ErrorBound, numNodesVisited, numNodesSearched);
    ClosestMore = pMore->FindClosestDatum(v, n, closestPoint, closestPointNorm, ErrorBound, numNodesVisited, numNodesSearched);
    ClosestDatum = (ClosestMore < 0) ? ClosestLEq : ClosestMore;
    return ClosestDatum;
}

// find terminal node holding the specified datum
int msh3DirPDTreeNode::FindTerminalNode(int datum, msh3DirPDTreeNode **termNode)
{
    if (!IsTerminalNode()) {
        if (pLEq->FindTerminalNode(datum, termNode)) return 1;
        if (pMore->FindTerminalNode(datum, termNode)) return 1;
        return 0;
    }

    for (int i = 0; i < NData; i++) {
        if (Datum(i) == datum) {
            *termNode = this;
            return 1;
        }
    }
    return 0;
}

void msh3DirPDTreeNode::PrintTerminalNodes(std::ofstream &fs)
{
    if (IsTerminalNode()) {
        fs << "Terminal Node:" << std::endl
           << "  NData = " << NData << std::endl
           << F << std::endl
           << "  Bounds Min: " << Bounds.MinCorner() << std::endl
           << "  Bounds Max: " << Bounds.MaxCorner() << std::endl
           << "  Datum Indices: " << std::endl;
        for (int i = 0; i < NData; i++) {
            fs << "    " << Datum(i) << std::endl;
        }
    }
    else {
        pLEq->PrintTerminalNodes(fs);
        pMore->PrintTerminalNodes(fs);
    }
}

//void msh3DirPDTreeNode::Print(FILE* chan, int indent)
//{
//  fprintfBlanks(chan, indent);
//  fprintf(chan, "NData = %d Bounds = [", NData); fprintfVct3(chan, Bounds.MinCorner);
//  fprintf(chan, "] ["); fprintfVct3(chan, Bounds.MaxCorner);
//  fprintf(chan, "]\n");
//  fprintfBlanks(chan, indent);
//  fprintfRodFrame(chan, "F =", F); fprintf(chan, "\n");
//  if (IsTerminalNode())
//  {
//    for (int k = 0; k < NData; k++)
//    {
//      pMyTree->PrintDatum(chan, indent + 2, Datum(k));
//    };
//    fprintf(chan, "\n");
//  }
//  else
//  {
//    pLEq->Print(chan, indent + 2);
//    pMore->Print(chan, indent + 2);
//  };
//}

//void msh3DirPDTreeNode::Print(int indent)
//{
//	printf("NData = %d Bounds = [", NData); fprintfVct3(chan,Bounds.MinCorner);
//	printf("] ["); fprintfVct3(chan, Bounds.MaxCorner);
//	printf("]\n");
//  std::cout << "F:" << std::endl << F << std::endl;
//	if (IsTerminalNode())
//	{ for (int k=0;k<NData;k++)
//	  { pMyTree->PrintDatum(chan,indent+2,Datum(k));
//	  };
//	}
//	else
//	{	pLEq->Print(chan,indent+2);
//		pMore->Print(chan,indent+2);
//	};
//}
