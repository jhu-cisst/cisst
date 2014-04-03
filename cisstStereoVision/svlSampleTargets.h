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

#ifndef _svlSampleTargets_h
#define _svlSampleTargets_h

#include <cisstStereoVision/svlSample.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlSampleTargets : public svlSample
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlSampleTargets();
    svlSampleTargets(const svlSampleTargets & other);
    svlSampleTargets & operator= (const svlSampleTargets & other);

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

    svlSampleTargets(unsigned int dimensions, unsigned int maxtargets, unsigned int channels);
    void SetSize(unsigned int dimensions, unsigned int maxtargets, unsigned int channels);
    void SetDimensions(unsigned int dimensions);
    unsigned int GetDimensions() const;
    void SetMaxTargets(unsigned int maxtargets);
    unsigned int GetMaxTargets() const;
    void SetChannels(unsigned int channels);
    unsigned int GetChannels() const;

    vctDynamicVectorRef<int> GetFlagVectorRef();
    const vctDynamicConstVectorRef<int> GetFlagVectorRef() const;
    vctDynamicVectorRef<int> GetConfidenceVectorRef(unsigned int channel = 0);
    const vctDynamicConstVectorRef<int> GetConfidenceVectorRef(unsigned int channel = 0) const;
    vctDynamicMatrixRef<int> GetPositionMatrixRef(unsigned int channel = 0);
    const vctDynamicConstMatrixRef<int> GetPositionMatrixRef(unsigned int channel = 0) const;
    int* GetFlagPointer();
    const int* GetFlagPointer() const;
    int* GetConfidencePointer(unsigned int channel = 0);
    const int* GetConfidencePointer(unsigned int channel = 0) const;
    int* GetPositionPointer(unsigned int channel = 0);
    const int* GetPositionPointer(unsigned int channel = 0) const;
    void ResetTargets();

    void SetFlag(unsigned int targetid, int value);
    int GetFlag(unsigned int targetid) const;
    void SetConfidence(unsigned int targetid, int value, unsigned int channel = 0);
    int GetConfidence(unsigned int targetid, unsigned int channel = 0) const;
    void SetPosition(unsigned int targetid, const vctInt2& value, unsigned int channel = 0);
    void SetPosition(unsigned int targetid, const vctInt3& value, unsigned int channel = 0);
    int GetPosition(unsigned int targetid, vctInt2& value, unsigned int channel = 0) const;
    int GetPosition(unsigned int targetid, vctInt3& value, unsigned int channel = 0) const;

protected:
    unsigned int Channels;
    unsigned int Dimensions;
    vctDynamicMatrix<int> Matrix;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleTargets)

#endif // _svlSampleTargets_h

