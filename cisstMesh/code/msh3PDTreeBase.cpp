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

#include <cisstMesh/msh3PDTreeBase.h>

#include <stdio.h>
#include <limits>

#include <cisstNumerical/nmrLSSolver.h>

#include <cisstMesh/msh3AlgPDTree.h>

// needed for debug routines
#include <cisstMesh/msh3PDTreeMesh.h>
#include <cisstMesh/msh3PDTreePointCloud.h>
#include <cisstMesh/TriangleClosestPointSolver.h>


// quickly find an approximate initial match by dropping straight down the
//   tree to the node containing the sample point and picking a datum from there
int msh3PDTreeBase::FastInitializeProximalDatum(const vct3 &v, vct3 &proxPoint)
{
    // find proximal leaf node
    msh3PDTreeNode *pNode;
    pNode = Top;

    while (!pNode->IsTerminalNode()) {
        pNode = pNode->GetChildSplitNode(v);
    }

    int proxDatum = pNode->Datum(0);        // choose any datum from the leaf node
    proxPoint = DatumSortPoint(proxDatum);  // choose any point on the datum

    return proxDatum;
}


// Return the index for the datum in the tree that is closest to the given point
//  in terms of the complete error and set the closest point
int msh3PDTreeBase::FindClosestDatum(const vct3 &v,
                                     vct3 &closestPoint,
                                     int prevDatum,
                                     double &matchError,
                                     unsigned int &numNodesSearched)
{
    // NOTE: by specifying a good starting datum (such as previous closest datum)
    //       the search for new closest datum is more efficient because
    //       the bounds value will be a good initial guess => fewer datums are
    //       closely searched.

    unsigned int numNodesVisited = 0;
    numNodesSearched = 0;
    matchError = pAlgorithm->FindClosestPointOnDatum(v, closestPoint, prevDatum);

    int datum;
    if (treeDepth > 0) {
        // since all datums must lie within the root node, we don't need to do a node bounds
        // check on the root => it is more efficient to start the search from each child node
        // of the root rather than starting the search from the root node itself.
        //int datum = Top->FindClosestDatum( v, closestPoint, matchError, numNodesVisited, numNodesSearched );
        int ClosestLEq = -1;
        int ClosestMore = -1;
        ClosestLEq = Top->pLEq->FindClosestDatum(v, closestPoint, matchError, numNodesVisited, numNodesSearched);
        ClosestMore = Top->pMore->FindClosestDatum(v, closestPoint, matchError, numNodesVisited, numNodesSearched);
        datum = (ClosestMore < 0) ? ClosestLEq : ClosestMore;
    }
    else {
        datum = Top->FindClosestDatum(v, closestPoint, matchError, numNodesVisited, numNodesSearched);
    }
    if (datum < 0) {
        datum = prevDatum;  // no datum found closer than previous
    }
    //std::cout << "numNodesVisited: " << numNodesVisited << "\tnumNodesSearched: " << numNodesSearched << std::endl;
    return datum;

}

// must be manually called by user after defining the noise
//  model on the points (unless using the mesh constructor)
void msh3PDTreeBase::ComputeNodeNoiseModels()
{
    // root pointers should always point to its own variables
    Top->bUseParentEigMaxBound = false;
    Top->bUseParentEigRankMinBounds = false;
    Top->pEigMax = &Top->EigMax;
    Top->pEigRankMin = &Top->EigRankMin;
    // set the covariance max eigenvalue for this node
    //  (the max eigenvalue among all datums since this is the root)
    vct3 eig;
    double maxEig = 0.0;
    vct3 minEigByRank(std::numeric_limits<double>::max());
    for (int i = 0; i < NData; i++) {
        eig = DatumCovEig(i);
        maxEig = maxEig >= eig[0] ? maxEig : eig[0];
        minEigByRank[0] = minEigByRank[0] <= eig[0] ? minEigByRank[0] : eig[0];
        minEigByRank[1] = minEigByRank[1] <= eig[1] ? minEigByRank[1] : eig[1];
        minEigByRank[2] = minEigByRank[2] <= eig[2] ? minEigByRank[2] : eig[2];
    }
    Top->EigMax = maxEig;
    Top->EigRankMin = minEigByRank;

    // compute noise models of children
    //  direct children of root must also point to their own
    //  variables because the node search begins from here for
    //  increased efficiency by not having to perform a node check
    //  on the root node.
    if (Top->pLEq)
        ComputeSubNodeNoiseModel(Top->pLEq, 1);
    if (Top->pMore)
        ComputeSubNodeNoiseModel(Top->pMore, 1);
}

void msh3PDTreeBase::ComputeSubNodeNoiseModel(msh3PDTreeNode *node, bool useLocalVarsOverride)
{
    // find the max eigenvalue and min eigenvalues by rank among all
    //  datums in this node
    // doing the search in this way is a bit innefficient but effective
    vct3 eig;
    double maxEig = 0.0;
    vct3 minEigByRank(std::numeric_limits<double>::max());
    for (int i = 0; i < node->NData; i++) {
        eig = DatumCovEig(node->pDataIndices[i]);
        maxEig = maxEig >= eig[0] ? maxEig : eig[0];
        minEigByRank[0] = minEigByRank[0] <= eig[0] ? minEigByRank[0] : eig[0];
        minEigByRank[1] = minEigByRank[1] <= eig[1] ? minEigByRank[1] : eig[1];
        minEigByRank[2] = minEigByRank[2] <= eig[2] ? minEigByRank[2] : eig[2];
    }
    node->EigMax = maxEig;
    node->EigRankMin = minEigByRank;

    bool useParentEigMaxBound = false;
    bool useParentEigRankMinBounds = false;
    if (!useLocalVarsOverride) {
        double ratio1;
        // protect from division by zero
        if (*(node->pParent->pEigMax) <= 1e-8)
            ratio1 = 1.0;
        else
            ratio1 = node->EigMax / *(node->pParent->pEigMax);
        if (ratio1 >= 0.75)
            useParentEigMaxBound = true;

        // protect from division by zero
        double ratio2;
        double nodeRankMinDet = node->EigRankMin[0] * node->EigRankMin[1] * node->EigRankMin[2];
        double parentRankMinDet = node->pParent->pEigRankMin->Element(0) * node->pParent->pEigRankMin->Element(1) * node->pParent->pEigRankMin->Element(2);
        if (nodeRankMinDet <= 1e-8)
            ratio2 = 1.0;
        else
            ratio2 = parentRankMinDet / nodeRankMinDet;
        if (ratio2 >= 0.75)
            useParentEigRankMinBounds = true;
    }

    if (useParentEigRankMinBounds) {
        // use log bound of parent
        node->bUseParentEigRankMinBounds = true;
        node->pEigRankMin = node->pParent->pEigRankMin;
    }
    else {
        // create new log bound for this node
        node->bUseParentEigRankMinBounds = false;
        node->pEigRankMin = &node->EigRankMin;
    }

    if (useParentEigMaxBound) {
        // use Mahalanobis bound of parent
        node->bUseParentEigMaxBound = true;
        node->pEigMax = node->pParent->pEigMax;
    }
    else { // create new Mahalanobis bound for this node
        node->bUseParentEigMaxBound = false;
        node->pEigMax = &node->EigMax;
    }

    // compute noise models of children
    if (node->pLEq)
        ComputeSubNodeNoiseModel(node->pLEq, 0);
    if (node->pMore)
        ComputeSubNodeNoiseModel(node->pMore, 0);
}


// Debug methods

// Exhaustive linear search of all datums in the tree to find datum
//  point with best match (used to validate efficient search routines)
int msh3PDTreeBase::ValidateClosestDatum(const vct3 &v,
                                         vct3 &closestPoint)
{
    double bestError = std::numeric_limits<double>::max();
    int    bestDatum = -1;
    double error;
    vct3 datumPoint;
    for (int datum = 0; datum < NData; datum++) {
        error = pAlgorithm->FindClosestPointOnDatum(v, datumPoint, datum);
        if (error < bestError) {
            bestError = error;
            bestDatum = datum;
            closestPoint = datumPoint;
        }
    }
    return bestDatum;
}

// Exhaustive linear search of all datums in the tree to find datum
//  point that is closest by Euclidean distance
//  (used to validate efficient search routines)
int msh3PDTreeBase::ValidateClosestDatum_ByEuclideanDist(const vct3 &v,
                                                         vct3 &closestPoint)
{
    msh3PDTreeMesh *pTreeMesh;
    msh3PDTreePointCloud *pPDTreePointCloud;

    double bestSqrDist = std::numeric_limits<double>::max();
    int    bestDatum = -1;
    double sqrDist;
    vct3 datumPoint;

    pTreeMesh = dynamic_cast<msh3PDTreeMesh*>(this);

    TriangleClosestPointSolver TCPS(*(pTreeMesh->Mesh));

    if (pTreeMesh) {
        // each datum is a triangle in a mesh
        for (int datum = 0; datum < NData; datum++) {
            // find closest point on datum
            TCPS.FindClosestPointOnTriangle(v, datum, datumPoint);
            sqrDist = (v - datumPoint).Norm();
            if (sqrDist < bestSqrDist) {
                bestSqrDist = sqrDist;
                bestDatum = datum;
                closestPoint = datumPoint;
            }
        }
        return bestDatum;
    }

    pPDTreePointCloud = dynamic_cast<msh3PDTreePointCloud*>(this);
    if (pPDTreePointCloud) {
        // each datum is a point in a point cloud
        for (int datum = 0; datum < NData; datum++) {
            datumPoint = pPDTreePointCloud->pointCloud.points.Element(datum);
            sqrDist = (v - datumPoint).Norm();
            if (sqrDist < bestSqrDist) {
                bestSqrDist = sqrDist;
                bestDatum = datum;
                closestPoint = datumPoint;
            }
        }
        return bestDatum;
    }

    std::cout << "ERROR: dynamic casts failed for ValidateClosestDatum_ByEuclideanDist()" << std::endl;
    assert(0);
    return -1;
}

int msh3PDTreeBase::FindTerminalNode(int datum, msh3PDTreeNode **termNode)
{
    return Top->FindTerminalNode(datum, termNode);
}

void msh3PDTreeBase::PrintTerminalNodes(std::ofstream &fs)
{
    Top->PrintTerminalNodes(fs);
}

//void msh3PDTreeBase::PrintDatum(FILE* chan,int level,int datum)
//{
//	fprintfBlanks(chan,level);
//	fprintf(chan,"%5d:",datum);
//  // SDB
//	//{ fprintf(stdout," ["); fprintfVct3(stdout,DatumSortPoint(datum)); fprintf(stdout,"] ");};
//	//// fprintf(stdout," [ ...]");
//	//fprintf(stdout,"\n");
//	{ fprintf(chan," ["); fprintfVct3(chan,DatumSortPoint(datum)); fprintf(chan,"] ");};
//	fprintf(chan,"\n");
//}
//
//void msh3PDTreeBase::Print(FILE* chan,int indent)
//{ fprintf(chan, "PD Tree Statistics\n");
//  fprintf(chan, "\tNumData = %d\n\tNumNodes = %d\n\tTreeDepth = %d\n\n", NData, NNodes, treeDepth);
//  Top->Print(chan,indent);
//}
