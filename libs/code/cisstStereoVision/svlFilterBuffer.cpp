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
	VideoBindex(0),
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
}

int svlFilterBuffer::Initialize(svlSample* inputdata, svlSample* &syncOutput)
{
    OutputData = inputdata;
	
	for (int i=0; i<VBsize; i++)
			VideoBuffer[i].SetSize(GetHeight(),  GetWidth() * GetDataChannels());

    syncOutput = OutputData;

    return SVL_OK;
}

int svlFilterBuffer::Process(svlProcInfo* procInfo, svlSample* inputdata, svlSample* &syncOutput)
{
	//The callback call should be single threaded just in case
	svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(inputdata);
	
    syncOutput = OutputData;

    _OnSingleThread(procInfo) {
		int tmpind = 0;
		if(!IsFrameSet){
				IsFrameSetEvent.Raise();
				IsFrameSet = true;
			}
					
			tmpind = VideoBindex + 1;
			tmpind %= VBsize;

			VideoTimeStamp[tmpind] = inputdata->GetTimestamp();

			// Get Image Data
                        VideoBuffer[tmpind].FastCopyOf(img->GetMatrixRef());

			VideoBindex = tmpind;
    }
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


vctDynamicMatrixRef<unsigned char> svlFilterBuffer::GetCurrentFrame()
{
			if(!IsFrameSet){
				IsFrameSetEvent.Wait();
			}	
			return VideoBuffer[VideoBindex];
		}


int svlFilterBuffer::GetCurrentFrameNArray(NumpyNArrayType matrix_in)
{
			if(!IsFrameSet){
				IsFrameSetEvent.Wait();
			}

		    vctDynamicMatrixRef<unsigned char> currentImage = VideoBuffer[VideoBindex];

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
