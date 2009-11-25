/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlStreamDefs.h>


/******************************/
/*** svlSample class **********/
/******************************/

svlSample::svlSample() : ModifiedFlag(true)
{
}

svlSample::~svlSample()
{
}

bool svlSample::IsImage()
{
    return false;
}

bool svlSample::IsInitialized()
{
    return false;
}

void svlSample::SetTimestamp(double ts)
{
    Timestamp = ts;
}

double svlSample::GetTimestamp()
{
    return Timestamp;
}

void svlSample::SetModified(bool modified)
{
    ModifiedFlag = modified;
}

bool svlSample::IsModified()
{
    return ModifiedFlag;
}

svlSample* svlSample::GetNewFromType(svlStreamType type)
{
    switch (type) {
        case svlTypeInvalid:           return 0;                              break;
        case svlTypeStreamSource:      return 0;                              break;
        case svlTypeStreamSink:        return 0;                              break;
        case svlTypeImageCustom:       return 0;                              break;
        case svlTypeImageRGB:          return new svlSampleImageRGB;          break;
        case svlTypeImageRGBA:         return new svlSampleImageRGBA;         break;
        case svlTypeImageRGBStereo:    return new svlSampleImageRGBStereo;    break;
        case svlTypeImageRGBAStereo:   return new svlSampleImageRGBAStereo;   break;
        case svlTypeImageMono8:        return new svlSampleImageMono8;        break;
        case svlTypeImageMono8Stereo:  return new svlSampleImageMono8Stereo;  break;
        case svlTypeImageMono16:       return new svlSampleImageMono16;       break;
        case svlTypeImageMono16Stereo: return new svlSampleImageMono16Stereo; break;
        case svlTypeImageMonoFloat:    return new svlSampleImageMonoFloat;    break;
        case svlTypeImage3DMap:        return new svlSampleImage3DMap;        break;
        case svlTypeRigidXform:        return new svlSampleRigidXform;        break;
        case svlTypePointCloud:        return new svlSamplePointCloud;        break;
    }
    return 0;
}


/*********************************/
/*** svlSampleImageBase class ****/
/*********************************/

svlSampleImageBase::svlSampleImageBase() : svlSample()
{
}

svlSampleImageBase::~svlSampleImageBase()
{
}

bool svlSampleImageBase::IsImage()
{
    return true;
}


/*********************************/
/*** svlSampleRigidXform class ***/
/*********************************/

svlSampleRigidXform::svlSampleRigidXform() : svlSample()
{
}

svlSample* svlSampleRigidXform::GetNewInstance()
{
    return new svlSampleRigidXform;
}

svlStreamType svlSampleRigidXform::GetType() const
{
    return svlTypeRigidXform;
}

bool svlSampleRigidXform::IsInitialized()
{
    return true;
}

int svlSampleRigidXform::SetSize(const svlSample & sample)
{
    if (dynamic_cast<const svlSampleRigidXform*>(&sample) == 0) return SVL_FAIL;
    return SVL_OK;
}

unsigned char* svlSampleRigidXform::GetUCharPointer()
{
    return reinterpret_cast<unsigned char*>(frame4x4.Pointer());
}

double* svlSampleRigidXform::GetPointer()
{
    return frame4x4.Pointer();
}

unsigned int svlSampleRigidXform::GetDataSize() const
{
    return (frame4x4.size() * sizeof(double));
}


/*********************************/
/*** svlSamplePointCloud class ***/
/*********************************/

svlSamplePointCloud::svlSamplePointCloud() : svlSample()
{
}

svlSample* svlSamplePointCloud::GetNewInstance()
{
    return new svlSamplePointCloud;
}

svlStreamType svlSamplePointCloud::GetType() const
{
    return svlTypePointCloud;
}

bool svlSamplePointCloud::IsInitialized()
{
    if (points.cols() > 0) { return true; } return false;
}

int svlSamplePointCloud::SetSize(const svlSample & sample)
{
    const svlSamplePointCloud* samplepc = dynamic_cast<const svlSamplePointCloud*>(&sample);
    if (samplepc == 0) return SVL_FAIL;
    SetSize(samplepc->GetDimensions(), samplepc->GetSize());
    return SVL_OK;
}

void svlSamplePointCloud::SetSize(unsigned int dimensions, unsigned int size)
{
    points.SetSize(dimensions, size);
}

unsigned int svlSamplePointCloud::GetDimensions() const
{
    return points.rows();
}

unsigned int svlSamplePointCloud::GetSize() const
{
    return points.cols();
}

unsigned char* svlSamplePointCloud::GetUCharPointer()
{
    return reinterpret_cast<unsigned char*>(points.Pointer());
}

double* svlSamplePointCloud::GetPointer()
{
    return points.Pointer();
}

unsigned int svlSamplePointCloud::GetDataSize() const
{
    return (points.size() * sizeof(double));
}


/*********************/
/*** svlRect class ***/
/*********************/

svlRect::svlRect() :
    left(0),
    top(0),
    right(0),
    bottom(0)
{
}

svlRect::svlRect(int left, int top, int right, int bottom) :
    left(left),
    top(top),
    right(right),
    bottom(bottom)
{
}

void svlRect::Assign(const svlRect & rect)
{
    left = rect.left;
    top = rect.top;
    right = rect.right;
    bottom = rect.bottom;
}

void svlRect::Assign(int left, int top, int right, int bottom)
{
    this->left = left;
    this->top = top;
    this->right = right;
    this->bottom = bottom;
}

void svlRect::Normalize()
{
    int temp;
    if (left > right) {
        temp = right;
        right = left;
        left = temp;
    }
    if (top > bottom) {
        temp = bottom;
        bottom = top;
        top = temp;
    }
}

void svlRect::Trim(int minx, int maxx, int miny, int maxy)
{
    if (left < minx) left = minx;
    if (left > maxx) left = maxx;
    if (right < minx) right = minx;
    if (right > maxx) right = maxx;
    if (top < miny) top = miny;
    if (top > maxy) top = maxy;
    if (bottom < miny) bottom = miny;
    if (bottom > maxy) bottom = maxy;
}

