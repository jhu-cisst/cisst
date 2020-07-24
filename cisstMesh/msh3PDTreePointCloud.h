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

#ifndef _msh3PDTreePointCloud_h
#define _msh3PDTreePointCloud_h

#include <limits>

#include <cisstMesh/msh3PDTreeBase.h>
#include <cisstMesh/msh3PointCloud.h>

// Always include last!
#include <cisstMesh/mshExport.h>

class CISST_EXPORT msh3PDTreePointCloud : public msh3PDTreeBase
{
    //
    // This class implements a PD tree for a point cloud shape
    //

    //--- Variables ---//

 public:

    msh3PointCloud pointCloud;

    //--- Methods ---//

    // constructors
    //  pointCloud - target shape from which to construct the tree
    //                (each point of the cloud becomes a datum in the tree)
    //  nThresh    - min number of datums to subdivide a node
    //  diagThresh - min physical size to subdivide a node
    msh3PDTreePointCloud(msh3PointCloud &pointCloud,
                         int nThresh,
                         double diagThresh);

    // destructor
    virtual ~msh3PDTreePointCloud();

    void SavePointCloud(std::string &file)
    {
        pointCloud.WritePointCloudToFile(file);
        pointCloud.SavePLY(file);
    }

    //--- Base Class Virtual Methods ---//
    virtual vct3 DatumSortPoint(int datum) const;  // return sort point of this datum
    virtual void EnlargeBounds(const vctFrm3& F, int datum, msh3BoundingBox& BB) const;


#ifdef ENABLE_PDTREE_NOISE_MODEL

    //--- Noise Model Methods ---//

    inline vct3x3& DatumCov(int datum)         // return measurement noise model for this datum
    {
        return pointCloud.pointCov.Element(datum);
    }
    inline vct3x3* DatumCovPtr(int datum)      // return measurement noise model for this datum
    {
        return &(pointCloud.pointCov.Element(datum));
    }

    inline vct3& DatumCovEig(int datum)         // return measurement noise model for this datum
    {
        return pointCloud.pointCovEig.Element(datum);
    }
    inline vct3* DatumCovEigPtr(int datum)      // return measurement noise model for this datum
    {
        return &(pointCloud.pointCovEig.Element(datum));
    }

#endif // ENABLE_PDTREE_NOISE_MODEL

};

#endif
