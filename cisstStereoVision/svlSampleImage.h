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

#ifndef _svlSampleImage_h
#define _svlSampleImage_h

#include <cisstStereoVision/svlSample.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

// Forward declarations
class svlSampleMatrix;
struct svlProcInfo;


class CISST_EXPORT svlSampleImage : public svlSample
{
public:
    svlSampleImage();
    svlSampleImage(const svlSampleImage & other);
    virtual ~svlSampleImage();

    virtual svlPixelType GetPixelType() const;
    virtual int GetAlphaChannel() const;

    virtual svlSample* GetNewInstance() const = 0;
    virtual svlStreamType GetType() const = 0;
    virtual int SetSize(const svlSample* sample) = 0;
    virtual int SetSize(const svlSample& sample) = 0;
    virtual int CopyOf(const svlSample* sample) = 0;
    virtual int CopyOf(const svlSample& sample) = 0;
    virtual bool IsInitialized() const = 0;
    virtual unsigned char* GetUCharPointer() = 0;
    virtual const unsigned char* GetUCharPointer() const = 0;
    virtual unsigned int GetDataSize() const = 0;
    virtual void SerializeRaw(std::ostream & outputStream) const = 0;
    virtual void DeSerializeRaw(std::istream & inputStream) = 0;

    virtual IplImage* IplImageRef(const unsigned int videochannel = 0) const = 0;
    virtual cv::Mat CvMatRef(const unsigned int videochannel = 0) const = 0;
    virtual unsigned char* GetUCharPointer(const unsigned int videochannel) = 0;
    virtual const unsigned char* GetUCharPointer(const unsigned int videochannel) const = 0;
    virtual unsigned char* GetUCharPointer(const unsigned int videochannel, const unsigned int x, const unsigned int y) = 0;
    virtual const unsigned char* GetUCharPointer(const unsigned int videochannel, const unsigned int x, const unsigned int y) const = 0;
    virtual void SetSize(const unsigned int width, const unsigned int height) = 0;
    virtual void SetSize(const unsigned int videochannel, const unsigned int width, const unsigned int height) = 0;
    virtual int SetSize(const IplImage* ipl_image, const unsigned int videochannel = 0) = 0;
    virtual int SetSize(const cv::Mat& cv_mat, const unsigned int videochannel = 0) = 0;
    virtual int CopyOf(const IplImage* ipl_image, const unsigned int videochannel = 0) = 0;
    virtual int CopyOf(const cv::Mat& cv_mat, const unsigned int videochannel = 0) = 0;
    virtual unsigned int GetVideoChannels() const = 0;
    virtual unsigned int GetDataChannels() const = 0;
    virtual unsigned int GetBPP() const = 0;
    virtual unsigned int GetWidth(const unsigned int videochannel = 0) const = 0;
    virtual unsigned int GetHeight(const unsigned int videochannel = 0) const = 0;
    virtual unsigned int GetRowStride(const unsigned int videochannel = 0) const = 0;
    virtual unsigned int GetDataSize(const unsigned int videochannel) const = 0;

    // Used to split up the image for multi-threaded processing
    virtual svlSampleImage* GetSubImage(const unsigned int top, const unsigned int height, const unsigned int videochannel = 0) = 0;
    virtual svlSampleImage* GetSubImage(svlProcInfo* procInfo, const unsigned int videochannel = 0) = 0;

    int ImportData(unsigned char* input, const unsigned int size, const int param = 0, const unsigned int videoch = SVL_LEFT);
    int ImportData(unsigned short* input, const unsigned int size, const int param = 0, const unsigned int videoch = SVL_LEFT);
    int ImportData(unsigned int* input, const unsigned int size, const int param = 0, const unsigned int videoch = SVL_LEFT);
    int ImportData(float* input, const unsigned int size, const int param = 0, const unsigned int videoch = SVL_LEFT);
    int ImportImage(const svlSampleImage* image, const int param = 0);
    int ImportImage(const svlSampleImage& image, const int param = 0);
    int ImportImage(const svlSampleImage* image, const unsigned int src_channel, const unsigned int dest_channel, const int param = 0);
    int ImportImage(const svlSampleImage& image, const unsigned int src_channel, const unsigned int dest_channel, const int param = 0);
    int ImportMatrix(const svlSampleMatrix* matrix, const int param = 0, const unsigned int videoch = SVL_LEFT);
    int ImportSample(const svlSample* sample);
    int ImportSample(const svlSample* sample, const unsigned int videoch);
};


#endif // _svlSampleImage_h

