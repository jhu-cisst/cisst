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

#ifndef _svlSampleCUDAImage_h
#define _svlSampleCUDAImage_h

#include <cisstStereoVision/svlSampleImage.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlSampleCUDAImage : public svlSampleImage
{
public:
    svlSampleCUDAImage();
    svlSampleCUDAImage(const svlSampleCUDAImage & other);
    virtual ~svlSampleCUDAImage();

    virtual svlSample* GetNewInstance() const = 0;
    virtual svlStreamType GetType() const = 0;
    virtual int SetSize(const svlSample* sample) = 0;
    virtual int SetSize(const svlSample& sample) = 0;
    virtual int CopyOf(const svlSample* sample) = 0;
    virtual int CopyOf(const svlSample& sample) = 0;
    virtual bool IsInitialized() const = 0;
    virtual unsigned char* GetUCharPointer();
    virtual const unsigned char* GetUCharPointer() const;
    virtual unsigned int GetDataSize() const = 0;
    virtual void SerializeRaw(std::ostream & outputStream) const;
    virtual void DeSerializeRaw(std::istream & inputStream);

    virtual IplImage* IplImageRef(const unsigned int videochannel = 0) const;
    virtual unsigned char* GetUCharPointer(const unsigned int videochannel);
    virtual const unsigned char* GetUCharPointer(const unsigned int videochannel) const;
    virtual unsigned char* GetUCharPointer(const unsigned int videochannel, const unsigned int x, const unsigned int y);
    virtual const unsigned char* GetUCharPointer(const unsigned int videochannel, const unsigned int x, const unsigned int y) const;
    virtual void SetSize(const unsigned int width, const unsigned int height) = 0;
    virtual void SetSize(const unsigned int videochannel, const unsigned int width, const unsigned int height) = 0;
    virtual int SetSize(const IplImage* ipl_image, const unsigned int videochannel = 0) = 0;
    virtual int CopyOf(const IplImage* ipl_image, const unsigned int videochannel = 0);
    virtual unsigned int GetVideoChannels() const = 0;
    virtual unsigned int GetDataChannels() const = 0;
    virtual unsigned int GetBPP() const = 0;
    virtual unsigned int GetWidth(const unsigned int videochannel = 0) const = 0;
    virtual unsigned int GetHeight(const unsigned int videochannel = 0) const = 0;
    virtual unsigned int GetRowStride(const unsigned int videochannel = 0) const = 0;
    virtual unsigned int GetDataSize(const unsigned int videochannel) const = 0;

    virtual svlSampleImage* GetSubImage(const unsigned int top, const unsigned int height, const unsigned int videochannel = 0);
    virtual svlSampleImage* GetSubImage(svlProcInfo* procInfo, const unsigned int videochannel = 0);

    virtual int CopyTo(svlSample* sample) const = 0;
    virtual int CopyTo(svlSample& sample) const = 0;
    virtual void* GetCUDAPointer(const unsigned int videochannel = 0) = 0;
    virtual const void* GetCUDAPointer(const unsigned int videochannel = 0) const = 0;
};

#endif // _svlSampleCUDAImage_h

