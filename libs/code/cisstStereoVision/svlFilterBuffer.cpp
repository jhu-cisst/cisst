/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlFliterBuffer.cpp 1218 2010-02-22 16:08:09Z adeguet1 $

  Author(s):	Daniel Mirota
  Created on:	2008-06-10

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstStereoVision/svlFilterBuffer.h>


svlFilterBuffer::svlFilterBuffer() :
    svlFilterBase(),
        myBufferImage(0),
        ProcessOneFrame(false),
        IsFrameSet(false),
        OutputData(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageRGBStereo);
    AddInputType("input", svlTypeImageMono8Stereo);

    //AddOutput("output", true);
    //SetAutomaticOutputType(true);

}

svlFilterBuffer::~svlFilterBuffer()
{
    delete myBufferImage;
    myBufferImage = 0;
}

int svlFilterBuffer::Initialize(svlSample* inputdata, svlSample* &syncOutput)
{
    OutputData = inputdata;
	
    myBufferImage = new svlBufferImage(GetWidth(),GetHeight(),GetDataChannels());

    syncOutput = OutputData;

    return SVL_OK;
}

int svlFilterBuffer::Process(svlProcInfo* procInfo, svlSample* inputdata, svlSample* &syncOutput)
{
    //The callback call should be single threaded just in case
    svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(inputdata);
	
    syncOutput = OutputData;




    _OnSingleThread(procInfo) {
        myBufferImage->Push(img->GetMatrixRef().Pointer(),img->GetMatrixRef().size(),false);
    }

    if(!IsFrameSet){
        IsFrameSetEvent.Raise();
        IsFrameSet = true;
    }


    if(ProcessOneFrame)
        return SVL_STOP_REQUEST;

    return SVL_OK;
}

unsigned int svlFilterBuffer::GetWidth(unsigned int videoch)
{
    if (!OutputData) return 0;
    return dynamic_cast<svlSampleImage*>(OutputData)->GetWidth(videoch);
}

unsigned int svlFilterBuffer::GetHeight(unsigned int videoch)
{
    if (!OutputData) return 0;
    return dynamic_cast<svlSampleImage*>(OutputData)->GetHeight(videoch);
}


unsigned int svlFilterBuffer::GetDataChannels()
{
    if (!OutputData) return 0;
    return dynamic_cast<svlSampleImage*>(OutputData)->GetDataChannels();
}


vctDynamicMatrixRef<unsigned char> svlFilterBuffer::GetCurrentFrame(bool wait_for_new)
{
    if(!IsFrameSet){
        IsFrameSetEvent.Wait();
        if(ProcessOneFrame)
            IsFrameSet = false;
    }

    svlImageRGB* current_image = myBufferImage->Pull(wait_for_new);
    if(current_image){
        return vctDynamicMatrixRef<unsigned char>(*current_image);
    }else{
        current_image = myBufferImage->Pull(false);
        return vctDynamicMatrixRef<unsigned char>(*current_image);
    }
}


int svlFilterBuffer::GetCurrentFrameNArray(NumpyNArrayType matrix_in,bool wait_for_new)
{
    if(!IsFrameSet){
        IsFrameSetEvent.Wait();
        if(ProcessOneFrame)
            IsFrameSet = false;
    }

    svlImageRGB* current_image = myBufferImage->Pull(wait_for_new);
     vctDynamicMatrixRef<unsigned char> currentImage;
    if(current_image){
        currentImage.SetRef(*current_image);
    }else{
        current_image = myBufferImage->Pull(false);
        currentImage.SetRef(*current_image);
    }

    SizeType svlBufferNArrayRefSize(GetHeight(), GetWidth(), GetDataChannels());
    StrideType svlBufferNArrayRefStride(currentImage.row_stride(), GetDataChannels(),currentImage.col_stride());
    svlBufferNArrayRef.SetRef(currentImage.Pointer(),svlBufferNArrayRefSize,svlBufferNArrayRefStride);

    //Set RGA submatrix to the current image
    //VideoBuffer[VideoBindex]
    SizeType startPosition(0,0,0);
    SizeType length(GetHeight(),GetWidth(),GetDataChannels());
    numpyNArrayRef.SubarrayOf(matrix_in,startPosition,length);

    numpyNArrayRef.Assign(svlBufferNArrayRef);

    return SVL_OK;
}
