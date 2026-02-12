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

#ifndef _EllipsoidOBBIntersectionSolver_h
#define _EllipsoidOBBIntersectionSolver_h

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctFixedSizeMatrixTypes.h>
#include <cisstVector/vctTransformationTypes.h>

#include <cisstMesh/msh3BoundingBox.h>

// Always include last!
#include <cisstMesh/mshExport.h>

class CISST_EXPORT EllipsoidOBBIntersectionSolver
{
 public:

    int TestEllipsoidOBBIntersection(const vct3 &v,
                                     const msh3BoundingBox &OBB, const vctFrm3 &Fobb,
                                     double NodeErrorBound,
                                     const  vct3x3 &N,
                                     double Dmin);

 private:

    bool IntersectionSphereFace(const vct3 &n,
                                const vct3 &v0, const vct3 &v1,
                                const vct3 &v2, const vct3 &v3,
                                double radius, double sqrRadius);

    int FindVisibleEdges(double q0, double q1,
                         double v00, double v01,
                         double v10, double v11,
                         double v20, double v21,
                         double v30, double v31,
                         int *vsblEdges,
                         bool ccwSequence);

    inline
        bool EdgeIsVisible(double q0, double q1,
                           double v0, double v1,
                           double n0, double n1);

    inline
        double SquareDistanceToEdge(const vct3 &p, const vct3 &r);

};

#endif
