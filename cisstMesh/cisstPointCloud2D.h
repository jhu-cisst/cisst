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
#ifndef _cisstPointCloud2D_h_
#define _cisstPointCloud2D_h_

#include <stdio.h>

#include <cisstMesh/mshConfig.h>

#if CISST_MSH_HAS_RPLY
  #include "ply_io.h"
#endif

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctDynamicVectorTypes.h>

class cisstPointCloud2D
{

private:

#if CISST_MSH_HAS_RPLY
  ply_io ply_obj;
#endif

public:

  vctDynamicVector<vct2> points;
  vctDynamicVector<vct2> pointOrientations;

  cisstPointCloud2D( vctDynamicVector<vct2> &points) :
    points(points)
  {};

  cisstPointCloud2D(vctDynamicVector<vct2> &points, vctDynamicVector<vct2> &pointOrientations) :
    points(points),
    pointOrientations(pointOrientations)
  {};

  // Point Set I/O
  void LoadPLY(const std::string &input_file);
  void SavePLY(const std::string &output_file);
  static int WritePointCloudToFile(vctDynamicVector<vct2> &points, std::string &filePath);
  static int ReadPointCloudFromFile(vctDynamicVector<vct2> &points, std::string &filePath);
  static int AppendPointCloudFromFile(vctDynamicVector<vct2> &points, std::string &filePath);

  int WritePointCloudToFile(std::string &filePath)
  {
    return WritePointCloudToFile(points, filePath);
  }

  int ReadPointCloudFromFile(std::string &filePath)
  {
    return ReadPointCloudFromFile(points, filePath);
  }

  int AppendPointCloudFromFile(std::string &filePath)
  {
    return AppendPointCloudFromFile(points, filePath);
  }

};

#endif // _cisstPointCloud2D_h_
