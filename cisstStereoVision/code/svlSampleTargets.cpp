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

#include <cisstStereoVision/svlTypes.h>


/******************************/
/*** svlSampleTargets class ***/
/******************************/

CMN_IMPLEMENT_SERVICES(svlSampleTargets)

svlSampleTargets::svlSampleTargets() :
    svlSample(),
    Channels(0),
    Dimensions(0)
{
}

svlSampleTargets::svlSampleTargets(const svlSampleTargets & other) :
    svlSample(other)
{
    CopyOf(other);
}

svlSampleTargets & svlSampleTargets::operator= (const svlSampleTargets & other)
{
    CopyOf(other);
    return *this;
}

svlSample* svlSampleTargets::GetNewInstance() const
{
    return new svlSampleTargets;
}

svlStreamType svlSampleTargets::GetType() const
{
    return svlTypeTargets;
}

int svlSampleTargets::SetSize(const svlSample* sample)
{
    const svlSampleTargets* targets = dynamic_cast<const svlSampleTargets*>(sample);
    if (targets == 0) return SVL_FAIL;

    Channels = targets->GetChannels();
    Dimensions = targets->GetDimensions();
    Matrix.SetSize(1 + Channels * (1 + Dimensions), targets->GetMaxTargets());

    return SVL_OK;
}

int svlSampleTargets::SetSize(const svlSample& sample)
{
    const svlSampleTargets* targets = dynamic_cast<const svlSampleTargets*>(&sample);
    if (targets == 0) return SVL_FAIL;

    Channels = targets->GetChannels();
    Dimensions = targets->GetDimensions();
    Matrix.SetSize(1 + Channels * (1 + Dimensions), targets->GetMaxTargets());

    return SVL_OK;
}

int svlSampleTargets::CopyOf(const svlSample* sample)
{
    if (SetSize(sample) != SVL_OK) return SVL_FAIL;
    memcpy(GetUCharPointer(), sample->GetUCharPointer(), GetDataSize());
    SetTimestamp(sample->GetTimestamp());
    return SVL_OK;
}

int svlSampleTargets::CopyOf(const svlSample& sample)
{
    if (SetSize(sample) != SVL_OK) return SVL_FAIL;
    memcpy(GetUCharPointer(), sample.GetUCharPointer(), GetDataSize());
    SetTimestamp(sample.GetTimestamp());
    return SVL_OK;
}

bool svlSampleTargets::IsInitialized() const
{
    return true;
}

unsigned char* svlSampleTargets::GetUCharPointer()
{
    return reinterpret_cast<unsigned char*>(Matrix.Pointer());
}

const unsigned char* svlSampleTargets::GetUCharPointer() const
{
    return reinterpret_cast<const unsigned char*>(Matrix.Pointer());
}

unsigned int svlSampleTargets::GetDataSize() const
{
    return static_cast<unsigned int>(Matrix.size() * sizeof(int));
}

void svlSampleTargets::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);

    cmnSerializeRaw(outputStream, GetType());
    cmnSerializeRaw(outputStream, GetTimestamp());
    cmnSerializeRaw(outputStream, Channels);
    cmnSerializeRaw(outputStream, Dimensions);
    cmnSerializeRaw(outputStream, Matrix);
}

void svlSampleTargets::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);

    int type = -1;
    double timestamp;
    cmnDeSerializeRaw(inputStream, type);
    if (type != GetType()) {
        CMN_LOG_CLASS_RUN_ERROR << "Deserialized sample type mismatch " << std::endl;
        return;
    }
    cmnDeSerializeRaw(inputStream, timestamp);
    SetTimestamp(timestamp);
    cmnDeSerializeRaw(inputStream, Channels);
    cmnDeSerializeRaw(inputStream, Dimensions);
    cmnDeSerializeRaw(inputStream, Matrix);
    if (Matrix.rows() != (1 + Channels * (1 + Dimensions))) {
        // Error
        Channels = 0;
        Dimensions = 0;
        Matrix.SetSize(0, 0);
    }
}

svlSampleTargets::svlSampleTargets(unsigned int dimensions, unsigned int maxtargets, unsigned int channels) :
    svlSample(),
    Channels(channels),
    Dimensions(dimensions)
{
    SetSize(Dimensions, maxtargets, channels);
    ResetTargets();
}

void svlSampleTargets::SetSize(unsigned int dimensions, unsigned int maxtargets, unsigned int channels)
{
    Dimensions = dimensions;
    Channels = channels;
    Matrix.SetSize(1 + channels * (1 + dimensions), maxtargets);
}

void svlSampleTargets::SetDimensions(unsigned int dimensions)
{
    Dimensions = dimensions;
    SetSize(dimensions, static_cast<unsigned int>(Matrix.cols()), Channels);
}

unsigned int svlSampleTargets::GetDimensions() const
{
    return Dimensions;
}

void svlSampleTargets::SetMaxTargets(unsigned int maxtargets)
{
    SetSize(Dimensions, maxtargets, Channels);
}

unsigned int svlSampleTargets::GetMaxTargets() const
{
    return static_cast<unsigned int>(Matrix.cols());
}

void svlSampleTargets::SetChannels(unsigned int channels)
{
    Channels = channels;
    SetSize(Dimensions, static_cast<unsigned int>(Matrix.cols()), channels);
}

unsigned int svlSampleTargets::GetChannels() const
{
    return Channels;
}

vctDynamicVectorRef<int> svlSampleTargets::GetFlagVectorRef()
{
    return Matrix.Row(0);
}

const vctDynamicConstVectorRef<int> svlSampleTargets::GetFlagVectorRef() const
{
    return Matrix.Row(0);
}

vctDynamicVectorRef<int> svlSampleTargets::GetConfidenceVectorRef(unsigned int channel)
{
    return Matrix.Row(1 + channel * (1 + Dimensions));
}

const vctDynamicConstVectorRef<int> svlSampleTargets::GetConfidenceVectorRef(unsigned int channel) const
{
    return Matrix.Row(1 + channel * (1 + Dimensions));
}

vctDynamicMatrixRef<int> svlSampleTargets::GetPositionMatrixRef(unsigned int channel)
{
    return vctDynamicMatrixRef<int>(Matrix, 1 + channel * (1 + Dimensions) + 1, 0, Dimensions, Matrix.cols());
}

const vctDynamicConstMatrixRef<int> svlSampleTargets::GetPositionMatrixRef(unsigned int channel) const
{
    return vctDynamicConstMatrixRef<int>(Matrix, 1 + channel * (1 + Dimensions) + 1, 0, Dimensions, Matrix.cols());
}

int* svlSampleTargets::GetFlagPointer()
{
    return GetFlagVectorRef().Pointer();
}

const int* svlSampleTargets::GetFlagPointer() const
{
    return GetFlagVectorRef().Pointer();
}

int* svlSampleTargets::GetConfidencePointer(unsigned int channel)
{
    return GetConfidenceVectorRef(channel).Pointer();
}

const int* svlSampleTargets::GetConfidencePointer(unsigned int channel) const
{
    return GetConfidenceVectorRef(channel).Pointer();
}

int* svlSampleTargets::GetPositionPointer(unsigned int channel)
{
    return GetPositionMatrixRef(channel).Pointer();
}

const int* svlSampleTargets::GetPositionPointer(unsigned int channel) const
{
    return GetPositionMatrixRef(channel).Pointer();
}

void svlSampleTargets::ResetTargets()
{
    memset(Matrix.Pointer(), 0, GetDataSize());
}

void svlSampleTargets::SetFlag(unsigned int targetid, int value)
{
    if (targetid < Matrix.cols() && Dimensions > 0) GetFlagVectorRef().Element(targetid) = value;
}

int svlSampleTargets::GetFlag(unsigned int targetid) const
{
    if (targetid >= Matrix.cols() || Dimensions < 1) return SVL_FAIL;
    return GetFlagVectorRef().Element(targetid);
}

void svlSampleTargets::SetConfidence(unsigned int targetid, int value, unsigned int channel)
{
    if (targetid >= Matrix.cols() || Dimensions < 1 || channel >= Channels) return;
    GetConfidenceVectorRef(channel).Element(targetid) = value;
}

int svlSampleTargets::GetConfidence(unsigned int targetid, unsigned int channel) const
{
    if (targetid >= Matrix.cols() || Dimensions < 1 || channel >= Channels) return SVL_FAIL;
    return GetConfidenceVectorRef(channel).Element(targetid);
}

void svlSampleTargets::SetPosition(unsigned int targetid, const vctInt2& value, unsigned int channel)
{
    if (targetid >= Matrix.cols() || Dimensions != 2 || channel >= Channels) return;
    
    int* ptr = Matrix.Pointer(2 + channel * 3, targetid);
    
    *ptr = value[0]; ptr += Matrix.cols();
    *ptr = value[1];
}

void svlSampleTargets::SetPosition(unsigned int targetid, const vctInt3& value, unsigned int channel)
{
    if (targetid >= Matrix.cols() || Dimensions != 3 || channel >= Channels) return;
    
    int* ptr = Matrix.Pointer(2 + channel * 4, targetid);
    const unsigned int stride = static_cast<unsigned int>(Matrix.cols());
    
    *ptr = value[0]; ptr += stride;
    *ptr = value[1]; ptr += stride;
    *ptr = value[2];
}

int svlSampleTargets::GetPosition(unsigned int targetid, vctInt2& value, unsigned int channel) const
{
    if (targetid >= Matrix.cols() || Dimensions != 2 || channel >= Channels) return SVL_FAIL;
    
    const int* ptr = Matrix.Pointer(2 + channel * 3, targetid);
    
    value[0] = ptr[0];
    value[1] = ptr[Matrix.cols()];
    
    return SVL_OK;
}

int svlSampleTargets::GetPosition(unsigned int targetid, vctInt3& value, unsigned int channel) const
{
    if (targetid >= Matrix.cols() || Dimensions != 3 || channel >= Channels) return SVL_FAIL;
    
    const int* ptr = Matrix.Pointer(2 + channel * 4, targetid);
    unsigned int stride = static_cast<unsigned int>(Matrix.cols());
    
    value[0] = ptr[0];
    value[1] = ptr[stride]; stride <<= 1;
    value[2] = ptr[stride];
    
    return SVL_OK;
}

