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

#include <cisstMesh/msh2AlgDirPDTreeCP.h>
#include <cisstMesh/msh2DirPDTreeNode.h>

// fast check if a node might contain a datum having smaller match error
//  than the error bound
int msh2AlgDirPDTreeCP::NodeMightBeCloser(const vct2 &v, const vct2 & CMN_UNUSED(n),
                                          msh2DirPDTreeNode const *node,
                                          double ErrorBound)
{
    // Check if point lies w/in search range of the bounding box for this node

    if (node->bUsingOBB) {
        vct2 Fv = node->F*v;  // transform point into local coordinate system of node

        // Rather than comparing only the x-axis value, check all coordinate directions
        //  of the node bounding box to further refine whether this node may be within
        //  the search range of this point. Using the node coordinate frame is still
        //  useful in this context, because it ensures a node bounding box of minimum size.
        // Conceptually, this check places another bounding box of size search distance
        //  around the point and then checks if this bounding box intersects the bounding
        //  box of this node.
        return node->Bounds.Includes(Fv, ErrorBound);
    }
    else {
        return node->Bounds.Includes(v, ErrorBound);
    }

    // Don't need this since average node orientation is calculated
    //  wrt the world reference frame, not local reference frame
    //vct3 Xn_node = node->F.Rotation()*Xn;
}
