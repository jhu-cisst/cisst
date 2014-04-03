/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlSampleTransform3D_h
#define _svlSampleTransform3D_h

#include <cisstStereoVision/svlSample.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlSampleTransform3D : public svlSample
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlSampleTransform3D();
    svlSampleTransform3D(const svlSampleTransform3D & other);
    svlSampleTransform3D & operator= (const svlSampleTransform3D & other);

    svlSample* GetNewInstance() const;
    svlStreamType GetType() const;
    int SetSize(const svlSample* sample);
    int SetSize(const svlSample& sample);
    int CopyOf(const svlSample* sample);
    int CopyOf(const svlSample& sample);
    bool IsInitialized() const;
    unsigned char* GetUCharPointer();
    const unsigned char* GetUCharPointer() const;
    unsigned int GetDataSize() const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);

    svlSampleTransform3D(const vct4x4 & matrix);
    svlSampleTransform3D(const vctFrm4x4 & frame);
    svlSampleTransform3D(const vctRot3 & rotation, const vct3 & translation);
    vct4x4 & GetMatrixRef();
    const vct4x4 & GetMatrixRef() const;
    vctFrm4x4 GetFrame() const;
    vctRot3 GetRotation() const;
    vct3 GetTranslation() const;
    double* GetDoublePointer();
    const double* GetDoublePointer() const;
    void Identity();

protected:
    vct4x4 Matrix;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleTransform3D)

#endif // _svlSampleTransform3D_h

