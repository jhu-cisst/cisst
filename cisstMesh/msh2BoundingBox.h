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

#ifndef _msh2BoundingBox_h
#define _msh2BoundingBox_h

#include <cisstVector/vctFixedSizeVectorTypes.h>

// Always include last!
#include <cisstMesh/mshExport.h>

class CISST_EXPORT msh2BoundingBox
{
 public:
    vct2 MinCorner;
    vct2 MaxCorner;
    //vct2 HalfExtents;  // SDB
 public:

    //void ComputeHalfExtents();

    vct2 Diagonal() const {return MaxCorner-MinCorner;};
    double DiagonalLength() const {return Diagonal().Norm();};

    vct2 MidPoint() const { return (MinCorner+MaxCorner)*0.5;};

    msh2BoundingBox(const vct2& MinC, const vct2& MaxC )
        : MinCorner(MinC), MaxCorner(MaxC)
    {}; //{ ComputeHalfExtents(); };

    msh2BoundingBox()
        : MinCorner(HUGE_VAL,HUGE_VAL), MaxCorner(-HUGE_VAL,-HUGE_VAL)
        {}; //{ ComputeHalfExtents(); };

    msh2BoundingBox(const msh2BoundingBox& S)
        : MinCorner(S.MinCorner), MaxCorner(S.MaxCorner)
        {}; //{ ComputeHalfExtents(); };

    msh2BoundingBox& operator=(const msh2BoundingBox& S)
        { MinCorner=S.MinCorner; MaxCorner=S.MaxCorner;
            //HalfExtents=S.HalfExtents;
            return *this; };

    msh2BoundingBox& operator=(const vct2& V)
        { MinCorner=MaxCorner=V;
            //HalfExtents.Assign(0.0,0.0,0.0);
            return *this;};

    msh2BoundingBox& Include(const msh2BoundingBox& him);
    msh2BoundingBox& Include(const vct2& V);

    msh2BoundingBox& EnlargeBy(double dist)
    { MinCorner -= dist; MaxCorner+= dist;
        return *this;
    };
    //ComputeHalfExtents(); };

    int Includes(const vct2& p, double dist=0.0) const
    { if (p[0] +dist < MinCorner[0]) return 0;
        if (p[1] +dist < MinCorner[1]) return 0;
        //if (p[2] +dist < MinCorner[2]) return 0;
        if (p[0] -dist > MaxCorner[0]) return 0;
        if (p[1] -dist > MaxCorner[1]) return 0;
        //if (p[2] -dist > MaxCorner[2]) return 0;
        return 1; // point is within distance "dist" of this bounding box
    };

    int Includes(const msh2BoundingBox& B, double dist=0.0) const
    { return Includes(B.MinCorner,dist)&&Includes(B.MaxCorner,dist);
    };
};

#endif
