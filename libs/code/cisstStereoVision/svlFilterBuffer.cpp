/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3VideoInterfaceFilter.cpp 1218 2010-02-22 16:08:09Z adeguet1 $

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

#include <cisstStereoVision/svlFilterCallback.h>


svlFilterBuffer::svlFilterBuffer() :
    svlFilterBase(),
	VideoBindex(0),
	IsFrameSet(false)
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
}

svlFilterBuffer::~svlFilterBuffer()
{
}

int svlFilterBuffer::Initialize(svlSample* inputdata)
{
    OutputData = inputdata;

	for (int i=0; i<VBsize; i++)
			VideoBuffer[i].SetSize(GetHeight(),  GetWidth() * GetDataChannels());

    return SVL_OK;
}

int svlFilterBuffer::ProcessFrame(svlProcInfo* procInfo, svlSample* inputdata)
{
	//The callback call should be single threaded just in case
	svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(inputdata);
	

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
			VideoBuffer[tmpind].Assign(img->MatrixRef());

			VideoBindex = tmpind;
    }
    return SVL_OK;
}

unsigned int svlFilterBuffer::GetWidth(unsigned int videoch)
{
    if (!OutputData || !OutputData->IsImage()) return 0;
    return dynamic_cast<svlSampleImageBase*>(OutputData)->GetWidth(videoch);
}

unsigned int svlFilterBuffer::GetHeight(unsigned int videoch)
{
    if (!OutputData || !OutputData->IsImage()) return 0;
    return dynamic_cast<svlSampleImageBase*>(OutputData)->GetHeight(videoch);
}


unsigned int svlFilterBuffer::GetDataChannels()
{
    if (!OutputData || !OutputData->IsImage()) return 0;
    return dynamic_cast<svlSampleImageBase*>(OutputData)->GetDataChannels();
}


vctDynamicMatrixRef<unsigned char> svlFilterBuffer::GetCurrentFrame()
{
			if(!IsFrameSet){
				IsFrameSetEvent.Wait();
			}	
			return VideoBuffer[VideoBindex];
		}