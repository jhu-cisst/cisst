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

#include <cisstMesh/msh3AlgDirPDTreeBA.h>
#include <cisstMesh/msh3DirPDTreeNode.h>

// PD Tree Methods

int msh3AlgDirPDTreeBA::NodeMightBeCloser(const vct3 &Xp, const vct3 &Xn,
                                          msh3DirPDTreeNode const *node,
                                          double ErrorBound)
{
    //
    // Match Error:
    //
    //  cost:  ||Xp - Yp||^2  such that angle between Xn & Yn < maxMatchAngle
    //

    // --- Compute Lower Bound on Orientation Error --- //

    double cos_dThetaAvg = Xn.DotProduct(node->Navg);
    if (cos_dThetaAvg < cos(node->dThetaMax + maxMatchAngle)) {
        // all orientations in this node exceed the maximum angular match error
        return 0;
    }

    // --- Positional Node Distance Test --- //

    // transform point into local coordinate system of node
    vct3 Xp_node = node->F*Xp;

    // check if node lies within the closest match distance found so far
    return node->Bounds.Includes(Xp_node, ErrorBound);
}
