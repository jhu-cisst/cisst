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

#include <cisstMesh/msh3PDTreeNode.h>
#include <cisstMesh/msh3PDTreeBase.h>
#include <cisstMesh/msh3AlgPDTree.h>
#include <cisstMesh/msh3Utilities.h>

msh3PDTreeNode::msh3PDTreeNode(int* pDataIndexArray,
                               int numIndexes,
                               msh3PDTreeBase* pTree,
                               msh3PDTreeNode* pParent)
    : pMyTree(pTree),
      pParent(pParent),
      pLEq(NULL),
      pMore(NULL),
      pDataIndices(pDataIndexArray),
      NData(numIndexes)
#ifdef ENABLE_PDTREE_NOISE_MODEL
    ,pEigMax(NULL),
      pEigRankMin(NULL)
#endif // ENABLE_PDTREE_NOISE_MODEL
{

#ifdef ENABLE_PDTREE_NOISE_MODEL
    // By default, assume zero measurement noise of the model
    EigMax = 0.0;
    EigRankMin.SetAll(0.0);
    if (pParent && pParent->pParent) {
        // this is a subnode at least 2 levels below root
        bUseParentEigMaxBound = true;
        bUseParentEigRankMinBounds = true;
    }
    else {
        // this is the root node or a direct child of the root
        //  => must use local noise model
        // Note: the direct children of the root must reference
        //       their own noise models because the search may start at the children
        //       of the root (rather than the root itself) since all datums must
        //       lie w/in the root
        bUseParentEigMaxBound = false;
        bUseParentEigRankMinBounds = false;
    }
#endif // ENABLE_PDTREE_NOISE_MODEL

    // Compute local coordinate frame for this node
    //  compute global -> local transform
    F = ComputeCovFrame(0, NData);

    // Set node boundaries s.t. all datums in this node are completely contained
    for (int i = 0; i < NData; i++) {
        // We must call the enlarge bounds function from the tree where
        //  the datum type is known.
        pMyTree->EnlargeBounds(F, Datum(i), Bounds);
    }
}

msh3PDTreeNode::~msh3PDTreeNode()
{
    if (pLEq != NULL) delete pLEq;
    if (pMore != NULL) delete pMore;
}

// computes a local reference frame for this node based on the
//  covariances of the datum sort positions; returns a
//  transformation that converts points from world -> node coordinates
// NOTE:  the origin of node coordinates is placed at the data centroid
//        with the x-axis oriented in the direction of largest data spread
vctFrm3 msh3PDTreeNode::ComputeCovFrame(int i0, int i1)
{
    vct3 p(0, 0, 0);
    vctRot3 R;
    vctDouble3x3 C(0.0);    // covariances
    vctDouble3x3 Q;		      // Eigen vectors
    vct3 e;				          // Eigen values

    int i;
    if (i1 <= i0) return vctFrm3();
    int N = i1 - i0;
    if (N < 5) {
        // since we can't create a fully determined covariance matrix
        //  from only a few points, use parent frame or identity frame
        return (pParent != NULL) ? pParent->F : vctFrm3();
    }
    //compute centroid of sort positions
    for (i = i0; i < i1; i++)
        {
            AccumulateCentroid(Datum(i), p);
        }
    p *= (1.0 / (i1 - i0));
    // compute covariance of sort positions
    for (i = i0; i < i1; i++) {
        AccumulateVariances(Datum(i), p, C);
    }

    // compute eigen decomposition of covariances
    //  NOTE:  can speed this up using the WildMagic5 closed-form
    //         covariance decomposition
    ComputeCovEigenDecomposition_NonIter(C, e, Q);

    // find largest eigenvalue
    int j = 0;
    for (i = 1; i < 3; i++) {
        if (fabs(e(i)) > fabs(e(j))) j = i;
    }
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

    // [R,p] is the node -> world transform
    //   what we want is the inverse of this
    return vctFrm3(R, p).Inverse();
}

void msh3PDTreeNode::AccumulateCentroid(int datum, vct3 &sum) const
{
    sum += pMyTree->DatumSortPoint(datum);
}

// NOTE: providing the M argument is not important for the calling function,
//       it merely helps with speed-up, as memory for the matrix doesn't
//       have to be re-allocated N times
void msh3PDTreeNode::AccumulateVariances(int datum, const vct3 &mean, vctDouble3x3 &C) const
{
    static vctDouble3x3 M;
    vct3 d = pMyTree->DatumSortPoint(datum) - mean;
    M.OuterProductOf(d, d);
    C += M;
}

// returns a value "top", for which datums should be on the More side if t>=top
int msh3PDTreeNode::SortNodeForSplit()
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
        if (kx > 0) { // this one needs to go to the end of the line
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

msh3PDTreeNode* msh3PDTreeNode::GetChildSplitNode(const vct3 &datumPos)
{
    // node split occurs along the local x-axis
    double x_node = F.Rotation().Row(0)*datumPos + F.Translation()[0];
    if (x_node > 0) {
        return pMore;
    }
    else {
        return pLEq;
    }
}

// returns tree depth
int msh3PDTreeNode::ConstructSubtree(int CountThresh, double DiagThresh) {

    if (NumData() < CountThresh || Bounds.DiagonalLength() < DiagThresh) {
        // leaf node
#ifdef DEBUG_PD_TREE
        fprintf(MyTree->debugFile, "Leaf Node: Ndata=%d\tDiagLen=%f\n", NumData(), Bounds.DiagonalLength());
#endif
        myDepth = 0;
        return myDepth;
    }

    int topLEq = SortNodeForSplit();

    if (topLEq == NumData() || topLEq == 0) {
        // need this in case count threshold = 1
        // TODO: could avoid this case by NumData()<=CountThresh above
#ifdef DEBUG_PD_TREE
        // NOTE: it sometimes occurs that all data sorts to one node even when multiple
        //       datums are present in the node; this happens because a vertex is chosen
        //       as the datum sort point. Therefore, muliple datums sharing the same
        //       vertex value may all be sorted wrt the same point. A way to prevent this
        //       (if desired) would be to sort each datum by it's centroid position.
        fprintf(MyTree->debugFile, "WARNING! all data sorts to one node; topLEq=%d\tNdata=%d\tDiagLen=%f\n",
                topLEq, NumData(), Bounds.DiagonalLength());
#endif

        myDepth = 0;  // stop here and do not split any further
        return 0;
    }

#ifdef DEBUG_PD_TREE
    fprintf(MyTree->debugFile2, "NNodeL=%d\tNNodeR=%d\n", topLEq, NumData() - topLEq);
#endif

    assert(topLEq > 0 && topLEq < NumData());

    int depthL, depthR;
    pLEq = new msh3PDTreeNode(pDataIndices, topLEq, pMyTree, this);
    pMyTree->NNodes++;
    depthL = pLEq->ConstructSubtree(CountThresh, DiagThresh);

    pMore = new msh3PDTreeNode(&pDataIndices[topLEq], NumData() - topLEq, pMyTree, this);
    pMyTree->NNodes++;
    depthR = pMore->ConstructSubtree(CountThresh, DiagThresh);

    this->myDepth = (depthL > depthR ? depthL : depthR) + 1;
    return myDepth;
}

// Check if a datum in this node has a lower match error than the error bound
//  If a lower match error is found, set the new closest point, update error
//  bound, and return the global datum index of the closest datum.
//  Otherwise, return -1.
int msh3PDTreeNode::FindClosestDatum(const vct3 &v,
                                     vct3 &closestPoint,
                                     double &ErrorBound,
                                     unsigned int &numNodesVisited,
                                     unsigned int &numNodesSearched)
{
    numNodesVisited++;

    // fast check if this node may contain a datum with better match error
    if (pMyTree->pAlgorithm->NodeMightBeCloser(v, this, ErrorBound) == 0) {
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
            if (pMyTree->pAlgorithm->DatumMightBeCloser(v, datum, ErrorBound)) {
                // a candidate
                vct3 candidate;
                // close check if this datum has a lower match error than error bound
                double err = pMyTree->pAlgorithm->FindClosestPointOnDatum(v, candidate, datum);
                if (err < ErrorBound) {
                    closestPoint = candidate;
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

    // 1st call to LEq updates both distance bound and closest point
    //  before 2nd call to More. If More returns (-1), then More had
    //  nothing better than LEq and the resulting datum should be
    //  the return value of LEq (whether that is -1 or a closer datum index)
    ClosestLEq = pLEq->FindClosestDatum(v, closestPoint, ErrorBound, numNodesVisited, numNodesSearched);
    ClosestMore = pMore->FindClosestDatum(v, closestPoint, ErrorBound, numNodesVisited, numNodesSearched);
    ClosestDatum = (ClosestMore < 0) ? ClosestLEq : ClosestMore;
    return ClosestDatum;
}

void msh3PDTreeNode::FindIntersectedPoints(const vct3 &v, const double boundingDistance,
                                           msh3Mesh& mesh, std::vector<int> &faceIdx)
{
    // v is defined in world frame, transform it to local coordinate
    // quickly determines if we need to search by using bounding box
    vct3 vMinCorner = F*v - boundingDistance;
    vct3 vMaxcorner = F*v + boundingDistance;
    msh3BoundingBox vBound(vMinCorner,vMaxcorner);

    // check if given bounding box intersects with node
    if (vBound.Intersect(Bounds)) {
        // if we have child nodes, we keep searching
        if (myDepth > 0) {
            pLEq->FindIntersectedPoints(v,boundingDistance,mesh,faceIdx);
            pMore->FindIntersectedPoints(v,boundingDistance,mesh,faceIdx);
        }
        // else we brute force search all triangles are append the closest points
        else {
            // compute the closest point and find the distance
            vct3 closestPoint; int location; double err;
            for (int i = 0; i < NData; i ++) {
                err = pMyTree->pAlgorithm->FindClosestPointOnDatumWithEdgeDetection(v,closestPoint,location,Datum(i));
                // if distance is smaller than the bound, append
                if (err <= boundingDistance) {
                    mesh.closestPoint[Datum(i)] = closestPoint;
                    mesh.cpLocation[Datum(i)] = location;
                    mesh.distance[Datum(i)] = err;
                    faceIdx.push_back(Datum(i));
                }
            }
        }
    }

    return;
}

// find terminal node holding the specified datum
int msh3PDTreeNode::FindTerminalNode(int datum, msh3PDTreeNode **termNode)
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

void msh3PDTreeNode::PrintTerminalNodes(std::ofstream &fs)
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
