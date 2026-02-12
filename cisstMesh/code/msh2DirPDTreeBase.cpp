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

#include <cisstMesh/msh2DirPDTreeBase.h>

#include <stdio.h>
#include <limits>
#include <fstream>

// quickly find an approximate initial match by dropping straight down the
//   tree to the node containing the sample point and picking a datum from there
int msh2DirPDTreeBase::FastInitializeProximalDatum(const vct2 &v, const vct2 & CMN_UNUSED(n),
                                                   vct2 &proxPoint, vct2 &proxNorm)
{
    // find proximal leaf node
    msh2DirPDTreeNode *pNode;
    pNode = Top;

    while (!pNode->IsTerminalNode()) {
        pNode = pNode->GetChildSplitNode(v);
    }

    int proxDatum = pNode->Datum(0);        // choose any datum from the leaf node
    proxPoint = DatumSortPoint(proxDatum);  // choose any point on the datum
    proxNorm = DatumNorm(proxDatum);

    return proxDatum;
}



// Return the index for the datum in the tree that is closest to the given point
//  in terms of the complete error (orientation + distance) and set the closest point
int msh2DirPDTreeBase::FindClosestDatum(const vct2 &v, const vct2 &n,
                                        vct2 &closestPoint, vct2 &closestPointNorm,
                                        int prevDatum,
                                        double &matchError,
                                        unsigned int &numNodesSearched)
{
    // SDB: by specifying a good starting datum (such as previous closest datum)
    //      the search for new closest datum is more efficient because
    //      the bounds value will be a good initial guess => fewer datums are
    //      closely searched.
    unsigned int numNodesVisited = 0;
    numNodesSearched = 0;
    matchError = algorithm->FindClosestPointOnDatum(v, n, closestPoint, closestPointNorm, prevDatum);
    // since all datums must lie within the root node, we don't need to do a node bounds
    // check on the root => it is more efficient to explicitly search each child node of the
    // root rather than searching the root node itself.
    //int datum = Top->FindClosestDatum( v, n, closestPoint, closestPointNorm, matchError, numNodesVisited, numNodesSearched );

    int datum;
    if (treeDepth > 0) {
        // As an optimization, we can directly search the children of the root, in order
        //  to save a node bounds check, since all datums must lie within the root node
        // 1st call to LEq updates both distance bound and closest point
        //  before 2nd call to More. If More returns (-1), then More had
        //  nothing better than LEq and the resulting datum should be
        //  the return value of LEq (whether that is -1 or a closer datum index)
        int ClosestLEq = -1;
        int ClosestMore = -1;
        ClosestLEq = Top->LEq->FindClosestDatum(v, n, closestPoint, closestPointNorm, matchError, numNodesVisited, numNodesSearched);
        ClosestMore = Top->More->FindClosestDatum(v, n, closestPoint, closestPointNorm, matchError, numNodesVisited, numNodesSearched);
        datum = (ClosestMore < 0) ? ClosestLEq : ClosestMore;
    }
    else {
        datum = Top->FindClosestDatum(v, n, closestPoint, closestPointNorm, matchError, numNodesVisited, numNodesSearched);
    }
    if (datum < 0) {
        datum = prevDatum;  // no datum found closer than previous
    }

    //if (numNodesSearched==0)
    //{
    //  //FILE *debugPrint = fopen("../ICP_TestData/LastRun/debugPDTreePrint.txt","w");
    //  //Print( debugPrint, 0 );
    //  //fclose( debugPrint );

    //  std::ofstream fs("../ICP_TestData/LastRun/debugPDTreePrint.txt");
    //  PrintTerminalNodes( fs );
    //  fs.close();

    //  std::cout << "No nodes searched!" << std::endl
    //    << " v: " << v << std::endl
    //    << " n: " << n << std::endl
    //    << " c: " << closestPoint << std::endl
    //    << " cn: " << closestPointNorm << std::endl
    //    << " prevDatum: " << prevDatum << std::endl;
    //  msh2DirPDTreeNode termNode( 0.0 );
    //  int rv = FindTerminalNode( prevDatum, termNode );
    //  if (rv == 0)
    //    std::cout << " --> prevDatum has no terminal node!" << std::endl;
    //  else
    //  {
    //    std::cout << "Terminal Node:" << std::endl;
    //    termNode.Print( stdout, 0 );

    //    cisstICPNormalsAlgorithm_CovEst *alg;
    //    alg = dynamic_cast<cisstICPNormalsAlgorithm_CovEst *>(algorithm);
    //    std::cout << "covGamma: " << std::endl << alg->covGamma << std::endl;
    //    std::cout << "invCovGamma: " << std::endl << alg->covGammaInv << std::endl;
    //    std::cout << "ErrorBound = " << errorBound << std::endl;
    //    std::cout << "ClosestPointError = "
    //      << algorithm->FindClosestPointOnDatum( v, n, closestPoint, closestPointNorm, prevDatum ) << std::endl;
    //    algorithm->NodeMightBeCloser( v, n, &termNode, errorBound );
    //  }
    //}

    //std::cout << "numNodesVisited: " << numNodesVisited << "\tnumNodesSearched: " << numNodesSearched << std::endl;
    return datum;
}

// Exhaustive linear search of all datums in the tree for validation of closest datum
int msh2DirPDTreeBase::ValidateClosestDatum(const vct2 &v, const vct2 &n,
                                            vct2 &closestPoint, vct2 &closestPointNorm)
{
    double bestError = std::numeric_limits<double>::max();
    int    bestDatum = -1;
    double error;
    vct2 datumPoint;
    vct2 datumNorm;
    for (size_t datum = 0; datum < NData; datum++) {
        error = algorithm->FindClosestPointOnDatum(v, n, datumPoint, datumNorm, datum);
        if (error < bestError) {
            bestError = error;
            bestDatum = datum;
            closestPoint = datumPoint;
            closestPointNorm = datumNorm;
        }
    }
    return bestDatum;
}

int msh2DirPDTreeBase::FindTerminalNode(int datum, msh2DirPDTreeNode **termNode)
{
    return Top->FindTerminalNode(datum, termNode);
}

void msh2DirPDTreeBase::PrintTerminalNodes(std::ofstream &fs)
{
    Top->PrintTerminalNodes(fs);
}

//void msh2DirPDTreeBase::PrintDatum(FILE* chan, int level, int datum)
//{
//    std::cout << "ERROR: PrintDatum() not supported for msh2DirPDTreeBase" << std::endl;
//    //fprintfBlanks(chan,level);
//    //fprintf(chan,"%5d:",datum);
//    // // SDB
//    ////{ fprintf(stdout," ["); fprintfVct3(stdout,DatumSortPoint(datum)); fprintf(stdout,"] ");};
//    ////// fprintf(stdout," [ ...]");
//    ////fprintf(stdout,"\n");
//    //{ fprintf(chan," ["); fprintfVct3(chan,DatumSortPoint(datum)); fprintf(chan,"] ");};
//    //fprintf(chan,"\n");
//}
//
//void msh2DirPDTreeBase::Print(FILE* chan, int indent)
//{
//    fprintf(chan, "PD Tree Statistics\n");
//    fprintf(chan, "\tNumData = %d\n\tNumNodes = %d\n\tTreeDepth = %d\n\n", NData, NNodes, treeDepth);
//    Top->Print(chan, indent);
//}
