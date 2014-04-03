/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlSampleBlobs_h
#define _svlSampleBlobs_h

#include <cisstStereoVision/svlSample.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlSampleBlobs : public svlSample
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlSampleBlobs();
    svlSampleBlobs(const svlSampleBlobs & other);
    svlSampleBlobs & operator= (const svlSampleBlobs & other);

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

    unsigned char* GetUCharPointer(const unsigned int videochannel);
    const unsigned char* GetUCharPointer(const unsigned int videochannel) const;
    unsigned int GetDataSize(const unsigned int videochannel) const;

    void SetChannelCount(const unsigned int channelcount);
    unsigned int GetChannelCount() const;
    int SetBufferSize(const unsigned int size, const unsigned int videochannel = 0);
    unsigned int GetBufferSize(const unsigned int videochannel = 0) const;
    int SetBufferUsed(const unsigned int used, const unsigned int videochannel = 0);
    unsigned int GetBufferUsed(const unsigned int videochannel = 0) const;
    int SetBlob(const unsigned int blob_id, const svlBlob& blob, const unsigned int videochannel = 0);
    int GetBlob(const unsigned int blob_id, svlBlob& blob, const unsigned int videochannel = 0) const;
    svlBlob* GetBlobsPointer(const unsigned int videochannel = 0);
    const svlBlob* GetBlobsPointer(const unsigned int videochannel = 0) const;

protected:
    vctDynamicVector< vctDynamicVector<svlBlob> > Blobs;
    vctDynamicVector< unsigned int >              BufferUsed;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleBlobs)

#endif // _svlSampleBlobs_h

