/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlTypes.h>


/****************************/
/*** svlSampleBlobs class ***/
/****************************/

CMN_IMPLEMENT_SERVICES(svlSampleBlobs)

svlSampleBlobs::svlSampleBlobs() :
    svlSample()
{
}

svlSampleBlobs::svlSampleBlobs(const svlSampleBlobs & other) :
    svlSample(other)
{
    CopyOf(other);
}

svlSampleBlobs & svlSampleBlobs::operator= (const svlSampleBlobs & other)
{
    CopyOf(other);
    return *this;
}

svlSample* svlSampleBlobs::GetNewInstance() const
{
    return new svlSampleBlobs;
}

svlStreamType svlSampleBlobs::GetType() const
{
    return svlTypeBlobs;
}

int svlSampleBlobs::SetSize(const svlSample* sample)
{
    const svlSampleBlobs* blobs = dynamic_cast<const svlSampleBlobs*>(sample);
    if (blobs == 0) return SVL_FAIL;

    const unsigned int channelcount = blobs->GetChannelCount();

    SetChannelCount(channelcount);

    for (unsigned int ch = 0; ch < channelcount; ch ++) {
        SetBufferSize(blobs->GetBufferSize(ch), ch);
    }

    return SVL_OK;
}

int svlSampleBlobs::SetSize(const svlSample& sample)
{
    const svlSampleBlobs* blobs = dynamic_cast<const svlSampleBlobs*>(&sample);
    if (blobs == 0) return SVL_FAIL;

    const unsigned int channelcount = blobs->GetChannelCount();

    SetChannelCount(channelcount);

    for (unsigned int ch = 0; ch < channelcount; ch ++) {
        SetBufferSize(blobs->GetBufferSize(ch), ch);
    }

    return SVL_OK;
}

int svlSampleBlobs::CopyOf(const svlSample* sample)
{
    const svlSampleBlobs* blobs = dynamic_cast<const svlSampleBlobs*>(sample);
    if (blobs == 0) return SVL_FAIL;

    SetSize(blobs);

    const unsigned int channelcount = blobs->GetChannelCount();

    for (unsigned int ch = 0; ch < channelcount; ch ++) {
        memcpy(GetUCharPointer(ch), blobs->GetUCharPointer(ch), GetDataSize(ch));
        BufferUsed[ch] = blobs->GetBufferUsed(ch);
    }

    return SVL_OK;
}

int svlSampleBlobs::CopyOf(const svlSample& sample)
{
    const svlSampleBlobs* blobs = dynamic_cast<const svlSampleBlobs*>(&sample);
    if (blobs == 0) return SVL_FAIL;

    SetSize(blobs);

    const unsigned int channelcount = blobs->GetChannelCount();

    for (unsigned int ch = 0; ch < channelcount; ch ++) {
        memcpy(GetUCharPointer(ch), blobs->GetUCharPointer(ch), GetDataSize(ch));
        BufferUsed[ch] = blobs->GetBufferUsed(ch);
    }

    return SVL_OK;
}

bool svlSampleBlobs::IsInitialized() const
{
    return true;
}

unsigned char* svlSampleBlobs::GetUCharPointer()
{
    return reinterpret_cast<unsigned char*>(GetBlobsPointer(0));
}

const unsigned char* svlSampleBlobs::GetUCharPointer() const
{
    return reinterpret_cast<const unsigned char*>(GetBlobsPointer(0));
}

unsigned int svlSampleBlobs::GetDataSize() const
{
    return (GetBufferSize(0) * sizeof(svlBlob));
}

void svlSampleBlobs::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);

    cmnSerializeRaw(outputStream, GetType());
    cmnSerializeRaw(outputStream, GetTimestamp());

    // TO DO
}

void svlSampleBlobs::DeSerializeRaw(std::istream & inputStream)
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

    // TO DO
}

unsigned char* svlSampleBlobs::GetUCharPointer(const unsigned int videochannel)
{
    return reinterpret_cast<unsigned char*>(GetBlobsPointer(videochannel));
}

const unsigned char* svlSampleBlobs::GetUCharPointer(const unsigned int videochannel) const
{
    return reinterpret_cast<const unsigned char*>(GetBlobsPointer(videochannel));
}

unsigned int svlSampleBlobs::GetDataSize(const unsigned int videochannel) const
{
    if (videochannel < GetChannelCount()) {
        return (GetBufferSize(videochannel) * sizeof(svlBlob));
    }
    return 0;
}

void svlSampleBlobs::SetChannelCount(const unsigned int channelcount)
{
    if (channelcount != Blobs.size()) {
        Blobs.SetSize(channelcount);
        BufferUsed.SetSize(channelcount);
        BufferUsed.SetAll(0);
    }
}

unsigned int svlSampleBlobs::GetChannelCount() const
{
    return static_cast<unsigned int>(Blobs.size());
}

int svlSampleBlobs::SetBufferSize(const unsigned int size, const unsigned int videochannel)
{
    if (videochannel < GetChannelCount()) {
        if (size != GetBufferSize(videochannel)) {
            Blobs[videochannel].SetSize(size);
            memset(Blobs[videochannel].Pointer(), 0, size * sizeof(svlBlob));
            BufferUsed.SetAll(0);
        }
        return SVL_OK;
    }
    return SVL_FAIL;
}

unsigned int svlSampleBlobs::GetBufferSize(const unsigned int videochannel) const
{
    if (videochannel < GetChannelCount()) {
        return static_cast<unsigned int>(Blobs[videochannel].size());
    }
    return 0;
}

int svlSampleBlobs::SetBufferUsed(const unsigned int used, const unsigned int videochannel)
{
    if (videochannel < GetChannelCount() && used <= Blobs[videochannel].size()) {
        BufferUsed[videochannel] = used;
        return SVL_OK;
    }
    return SVL_FAIL;
}

unsigned int svlSampleBlobs::GetBufferUsed(const unsigned int videochannel) const
{
    if (videochannel < GetChannelCount()) {
        return BufferUsed[videochannel];
    }
    return 0;
}

int svlSampleBlobs::SetBlob(const unsigned int blob_id, const svlBlob& blob, const unsigned int videochannel)
{
    if (videochannel < GetChannelCount() && blob_id < GetBufferSize(videochannel)) {
        memcpy(GetBlobsPointer(videochannel) + blob_id, &blob, sizeof(svlBlob));
        return SVL_OK;
    }
    return SVL_FAIL;
}

int svlSampleBlobs::GetBlob(const unsigned int blob_id, svlBlob& blob, const unsigned int videochannel) const
{
    if (videochannel < GetChannelCount() && blob_id < GetBufferSize(videochannel)) {
        memcpy(&blob, GetBlobsPointer(videochannel) + blob_id, sizeof(svlBlob));
        return SVL_OK;
    }
    return SVL_FAIL;
}

svlBlob* svlSampleBlobs::GetBlobsPointer(const unsigned int videochannel)
{
    if (videochannel < GetChannelCount()) {
        return Blobs[videochannel].Pointer();
    }
    return 0;
}

const svlBlob* svlSampleBlobs::GetBlobsPointer(const unsigned int videochannel) const
{
    if (videochannel < GetChannelCount()) {
        return Blobs[videochannel].Pointer();
    }
    return 0;
}

