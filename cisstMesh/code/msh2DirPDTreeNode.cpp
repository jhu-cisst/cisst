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

#include <cisstMesh/msh2DirPDTreeNode.h>
#include <cisstMesh/msh2DirPDTreeBase.h>
#include <cisstMesh/msh2Utilities.h>

msh2DirPDTreeNode::msh2DirPDTreeNode(int* pDataIndexArray,
                                     size_t numIndexes,
                                     msh2DirPDTreeBase* pTree,
                                     msh2DirPDTreeNode* pParent,
                                     bool bUseOBB, unsigned int splitDimension
                                     ) :
    MyTree(pTree),
    Parent(pParent),
    DataIndices(pDataIndexArray),
    NData(numIndexes),
    LEq(NULL),
    More(NULL),
    F(vctFrm2::Identity()),
    Bounds(),
    bUsingOBB(bUseOBB),
    splitDim(splitDimension),
    posAvg(0.0), posCov(0.0),
    Navg(0.0), dThetaMax(0.0),
    posSum(0.0), covSum(0.0),
    Nsum(0.0)
{
    // compute bounding box of node
    if (bUsingOBB) {
        vct2    p;
        vct2x2  M;
        for (size_t i = 0; i < NData; i++) {
            p = MyTree->DatumSortPoint(Datum(i));
            // accumulate positions
            posSum += p;
            // accumulate covariances
            M.OuterProductOf(p, p);
            covSum += M;
        }

        // compute covariance and average of node positions
        posAvg = posSum.Divide(NData);
        M.OuterProductOf(posAvg, posAvg);
        posCov = covSum.Divide(NData) + M;

        // Compute local coordinate frame for this node
        //  (transformation from world -> node coords)
        F = ComputeCovFrame(posCov, posAvg);

        // Construct node bounding box
        for (size_t i = 0; i < NData; i++) {
            // since we don't know what type of datum we're dealing with
            //  (and since we want the bounds to completely hold all of this datum)
            //  we must place the enlarge bounds function at the tree level where
            //  the datum type is known.
            MyTree->EnlargeBounds(F, Datum(i), Bounds);
        }
    }
    else {
        // Add all datums in the node to the bounding box
        for (size_t i = 0; i < NData; i++) {
            // accumulate positions
            posSum += MyTree->DatumSortPoint(Datum(i));;

            // add datum to node
            MyTree->EnlargeBounds(Datum(i), Bounds);
        }

        // compute the average of node positions
        posAvg = posSum.Divide(NData);

        // Define split direction to be the bounding box axis of greatest extent
        vct2 dimSize = Bounds.MaxCorner - Bounds.MinCorner;
        dimSize[0] > dimSize[1] ? splitDim = 0 : splitDim = 1;
    }
}

msh2DirPDTreeNode::~msh2DirPDTreeNode()
{
    if (LEq != NULL) delete LEq;
    if (More != NULL) delete More;
}


// returns tree depth
int msh2DirPDTreeNode::ConstructTree(const size_t CountThresh, double DiagThresh)
{
    // Check leaf node condition
    if (NumData() <= CountThresh || Bounds.DiagonalLength() < DiagThresh) {
        // leaf node
        ConstructLeaf();
        myDepth = 0;
        return myDepth;
    }

    // Not a leaf => sort node for splitting
    size_t topLEq = SortNodeForSplit();

    // Since this PD tree depends on averaging,
    //  ensure that at least 2 datums exist in each node
    //   NOTE: topLEq == NumData() means all data allocated to left node
    //         topLEq == 0 means all data allocated to right node
    if (topLEq < 2 || topLEq > NumData()-2) {
        ConstructLeaf();
        myDepth = 0;  // we decide to stop here and not split any further
        return myDepth;
    }
    //  // Double check that data split into two nodes
    //  if (topLEq == NumData() || topLEq == 0)
    //  { // all data allocated to only one node
    //    // need this in case count threshold = 1
    //    // TODO: avoid this case by NumData()<=CountThresh above
    //    // BUG: this sometimes occurs when multiple data exists in the node
    //#ifdef DebugDirPDTree2D
    //    fprintf(MyTree->debugFile, "ERROR! all data splits to one node; topLEq=%d\tNdata=%d\tDiagLen=%f\n",
    //      topLEq, NumData(), Bounds.DiagonalLength());
    //#endif
    //    ConstructLeaf();
    //    myDepth = 0;  // we decide to stop here and not split any further
    //    return myDepth;
    //  }

#ifdef DebugDirPDTree2D
    fprintf(MyTree->debugFile2, "NNodeL=%d\tNNodeR=%d\n", topLEq, NumData() - topLEq);
#endif
#ifdef DEBUG_DirPDTree2dNode
    assert(topLEq > 0 && topLEq<NumData());
#endif

    // create child nodes
    int depthL, depthR;

    LEq = new msh2DirPDTreeNode(DataIndices, topLEq, MyTree, this, bUsingOBB, (splitDim + 1) % 2);
    MyTree->NNodes++;
    depthL = LEq->ConstructTree(CountThresh, DiagThresh);

    More = new msh2DirPDTreeNode(&DataIndices[topLEq], NumData() - topLEq, MyTree, this, bUsingOBB, (splitDim + 1) % 2);
    MyTree->NNodes++;
    depthR = More->ConstructTree(CountThresh, DiagThresh);

    // finish construction of this node
    myDepth = (depthL > depthR ? depthL : depthR) + 1;

    // compute orientation statistics
    Nsum = LEq->Nsum + More->Nsum;
    Navg = ComputeOrientationAverage(Nsum);
    dThetaMax = ComputeOrientationThetaMax(Navg);

    return myDepth;
}


void msh2DirPDTreeNode::ConstructLeaf()
{
#ifdef DebugDirPDTree2D
    fprintf(MyTree->debugFile, "Leaf Node: Ndata=%d\tDiagLen=%f\n", NumData(), Bounds.DiagonalLength());
    fflush(MyTree->debugFile);
#endif

    // compute orientation statistics for this node
    Nsum = ComputeOrientationSum();
    Navg = ComputeOrientationAverage(Nsum);
    dThetaMax = ComputeOrientationThetaMax(Navg);
}


// returns a value "top", for which a datum should be on the More side if t>=top
int msh2DirPDTreeNode::SortNodeForSplit()
{
    if (bUsingOBB) {
        int top = NData;
        vct2 Ck; vct2 Ct;
        vct2 r = F.Rotation().Row(0);
        double px = F.Translation()[0];
        for (int k = 0; k < top; k++) {
            Ck = MyTree->DatumSortPoint(Datum(k)); // 3D coordinate in global coord system
            double kx = r*Ck + px;  // compute the x coordinate in local coord system
            if (kx > 0) {
                // this one needs to go to the end of the line
                while ((--top) > k) {
                    Ct = MyTree->DatumSortPoint(Datum(top));
                    double tx = r*Ct + px;
                    if (tx <= 0) {
                        int Temp = Datum(k);
                        Datum(k) = Datum(top);
                        Datum(top) = Temp;
                        break; // from the "top" loop
                    };
                };	// end of the "t" loop
            };	// end of the kx>0 case; at this point F*datum(i).x-coord <= 0 for i=0,...,k
        };	// end of k loop
        return top;
    }
    else {
        // split node at centroid of positional data
        int top = NData;
        double Ck, Ct;
        double splitPoint = posAvg.Element(splitDim);
        for (int k = 0; k < top; k++) {
            Ck = MyTree->DatumSortPoint(Datum(k)).Element(splitDim);
            if (Ck > splitPoint) {
                // this one needs to go to the end of the line
                while ((--top) > k) {
                    Ct = MyTree->DatumSortPoint(Datum(top)).Element(splitDim);
                    if (Ct <= splitPoint) {
                        int Temp = Datum(k);
                        Datum(k) = Datum(top);
                        Datum(top) = Temp;
                        break; // from the "top" loop
                    }
                }
            }
        }
        return top;
    }
}

msh2DirPDTreeNode* msh2DirPDTreeNode::GetChildSplitNode(const vct2 &datumPos)
{
    if (bUsingOBB) {
        std::cout << "ERROR: msh2DirPDTreeNode::GetChildSplitNode() feature not"
                  << " implemented for OBB search" << std::endl;
        assert(0);
        return NULL;
    }
    else {
        // split node at centroid of positional data
        double splitPoint = posAvg.Element(splitDim);
        double Ck = datumPos.Element(splitDim);
        if (Ck > splitPoint) {
            return More;
        }
        else {
            return LEq;
        }
    }
}


//// computes average orientation and max deviation for this node
////  returns the running sum of vector orientations
//vct2 msh2DirPDTreeNode::ComputePositionSum()
//{
//  vct2 posSum(0.0);
//  for (int i = 0; i < NumData(); i++)
//  {
//    posSum += MyTree->DatumSortPoint(Datum(i));
//  }
//  return posSum;
//}
//
//// computes average orientation and max deviation for this node
////  returns the running sum of vector orientations
//vct2x2 msh2DirPDTreeNode::ComputeCovarianceSum()
//{
//  vct2x2 covSum(0.0);
//  vct2 p;
//  vct2x2 M;
//  for (int i = 0; i < NumData(); i++)
//  {
//    p = MyTree->DatumSortPoint(Datum(i));
//    M.OuterProductOf(p, p);
//    covSum += M;
//  }
//  return covSum;
//}

vct2 msh2DirPDTreeNode::ComputeOrientationAverage(vct2 &Nsum)
{
    vct2 Navg;
    if (Nsum.Norm() < 1.0e-11) { // norm of sum is very small => choose any orientation as average
        Navg = vct2(1.0, 0.0);
    }
    else {
        Navg = Nsum.Normalized();
    }
    return Navg;
}


// computes average orientation and max deviation for this node
//  returns the running sum of vector orientations
vct2 msh2DirPDTreeNode::ComputeOrientationSum()
{
    vct2 Nsum(0.0);
    for (size_t i = 0; i < NumData(); i++) {
        Nsum += MyTree->DatumNorm(Datum(i));
        //int datum = Datum(i);
        //vct2 datumNorm = MyTree->DatumNorm(datum);
        //Nsum += datumNorm;
    }
    return Nsum;
}

double msh2DirPDTreeNode::ComputeOrientationThetaMax(vct2 Navg)
{
    // compute max deviation from the avg orientation
    //  cos(theta) = n'*Navg   (note n & Navg are unit vectors)
    double dThetaMax = 0.0;
    double Theta;
    for (size_t i = 0; i < NumData(); i++) {
        Theta = acos(MyTree->DatumNorm(Datum(i)).DotProduct(Navg));
        if (Theta > dThetaMax) { dThetaMax = Theta; }
    }
#ifdef DebugDirPDTree2D
    fprintf(MyTree->debugFile, " dThetaMax = %f\n", dThetaMax);
#endif
    return dThetaMax;
}

// computes a local reference frame for this node based on the
//  covariances of the datum sort positions; returns a frame
//  transformation that converts points from local -> global coordinates
// returns a frame whose origin is at centroid and whose x-axis
//  points in the direction of largest spread in positions
vctFrm2 msh2DirPDTreeNode::ComputeCovFrame(vct2x2 posCov, vct2 posMean)
{
    vctRot2 R;
    vctDouble2x2 Q;		      // Eigen vectors
    vct2 e;				          // Eigen values

    // compute eigen decomposition of covariance matrix
    ComputeCovEigenDecomposition_SVD(posCov, e, Q);
    //int rc = nmrJacobi(posCov, e, Q); // e=eigen values, Q=eigenVectors

    // align x-axis with the eigenvector of greatest eigenvalue magnitude
    if (e(0) > e(1)) {
        // E[0] is biggest eigenvalue
        R = Q;
    }
    else {
        // E[1] is biggest eigenvalue
        // by right hand rule, map x->y, y->-x
        R.Column(0) = Q.Column(1);
        R.Column(1) = -Q.Column(0);
    }

    // [R,p] is the coordinate frame that transforms node -> world coords
    //   we want transform from world -> node coords => return [R',-R'*p]?
    return vctFrm2(R, posMean).Inverse();
}

// Check if any datum in this node has a lower match error than the error bound
//  If a lower match error is found, set the new closest point, update error
//  bound, and return the global datum index of the closest datum.
//  Otherwise, return -1.
int msh2DirPDTreeNode::FindClosestDatum(const vct2 &v, const vct2 &n,
                                        vct2 &closestPoint, vct2 &closestPointNorm,
                                        double &ErrorBound,
                                        unsigned int &numNodesVisited,
                                        unsigned int &numNodesSearched)
{
    numNodesVisited++;

    // fast check if this node may contain a datum with better match error
    if (MyTree->algorithm->NodeMightBeCloser(v, n, this, ErrorBound) == 0) {
        return -1;
    }

    numNodesSearched++;

    // Search points w/in this node
    int ClosestDatum = -1;
    if (IsTerminalNode()) {
        // look at each datum in the node
        for (size_t i = 0; i < NData; i++) {
            // for each datum in this node
            int datum = Datum(i);

            // fast check if this datum might have a lower match error than error bound
            if (MyTree->algorithm->DatumMightBeCloser(v, n, datum, ErrorBound)) {
                // a candidate
                vct2 candidate;
                vct2 candidateNorm;
                // close check if this datum has a lower match error than error bound
                double err = MyTree->algorithm->FindClosestPointOnDatum(v, n, candidate, candidateNorm, datum);
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

    // here if not a terminal node
    //  extend search to both child nodes

    int ClosestLEq = -1;
    int ClosestMore = -1;

    // 1st call to LEq updates both distance bound and closest point
    //  before 2nd call to More. If More returns (-1), then More had
    //  nothing better than LEq and the resulting datum should be
    //  the return value of LEq (whether that is -1 or a closer datum index)
    ClosestLEq = LEq->FindClosestDatum(v, n, closestPoint, closestPointNorm, ErrorBound, numNodesVisited, numNodesSearched);
    ClosestMore = More->FindClosestDatum(v, n, closestPoint, closestPointNorm, ErrorBound, numNodesVisited, numNodesSearched);
    ClosestDatum = (ClosestMore < 0) ? ClosestLEq : ClosestMore;
    return ClosestDatum;
}


// find terminal node holding the specified datum
int msh2DirPDTreeNode::FindTerminalNode(int datum, msh2DirPDTreeNode **termNode)
{
    if (!IsTerminalNode()) {
        if (LEq->FindTerminalNode(datum, termNode)) return 1;
        if (More->FindTerminalNode(datum, termNode)) return 1;
        return 0;
    }

    for (size_t i = 0; i < NData; i++) {
        if (Datum(i) == datum) {
            *termNode = this;
            return 1;
        }
    }
    return 0;
}

void msh2DirPDTreeNode::PrintTerminalNodes(std::ofstream &fs)
{
    if (IsTerminalNode()) {
        fs << "Terminal Node:" << std::endl
           << "  NData = " << NData << std::endl
           << F << std::endl
           << "  Bounds Min: " << Bounds.MinCorner << std::endl
           << "  Bounds Max: " << Bounds.MaxCorner << std::endl
           << "  Datum Indices: " << std::endl;
        for (size_t i = 0; i < NData; i++) {
            fs << "    " << Datum(i) << std::endl;
        }
    }
    else {
        LEq->PrintTerminalNodes(fs);
        More->PrintTerminalNodes(fs);
    }
}

//void msh2DirPDTreeNode::Print(FILE* chan, int indent)
//{
//  fprintfBlanks(chan, indent);
//  fprintf(chan, "NData = %d Bounds = [", NData); fprintfVct2(chan, Bounds.MinCorner);
//  fprintf(chan, "] ["); fprintfVct2(chan, Bounds.MaxCorner);
//  fprintf(chan, "]\n");
//  fprintfBlanks(chan, indent);
//  //fprintfRodFrame(chan,"F =", F); fprintf(chan,"\n");
//  if (IsTerminalNode())
//  {
//    for (int k = 0; k < NData; k++)
//    {
//      MyTree->PrintDatum(chan, indent + 2, Datum(k));
//    };
//    fprintf(chan, "\n");
//  }
//  else
//  {
//    LEq->Print(chan, indent + 2);
//    More->Print(chan, indent + 2);
//  };
//}
