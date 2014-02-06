/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlFilterBase.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Anton Deguet
  Created on: 2014

  (C) Copyright 2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnGetChar.h>

#include <cisstStereoVision/svlInitializer.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlStreamManager.h>
#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlFilterSplitter.h>
#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlVideoIO.h>
#include <cisstStereoVision/svlFilterVideoFileWriter.h>

int main(int argc, char** argv) {

    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    
    svlInitialize();

    /////////////////////////////////////////////////////////////////
    
    // connect the source to the stream
    svlFilterSourceVideoCapture source(1);
    if (source.LoadSettings("camera-viewer.dat") != SVL_OK) {
        source.DialogSetup(SVL_LEFT);
        source.DialogSetup(SVL_RIGHT);
    }
    source.SaveSettings("camera-viewer.dat");
    
#if 0
    svlFilterImageResizer resize;
    resize.SetName("Resize");
    resize.SetInterpolation(true);
    resize.SetOutputSize(640, 480, SVL_LEFT);
    resize.SetOutputSize(640, 480, SVL_RIGHT);

    svlFilterStereoImageJoiner joiner;
    joiner.SetName("Joiner");
#endif

    svlFilterSplitter splitter;
    splitter.SetName("Splitter");
    splitter.AddOutput("Window",  1, 3);

#if 0    
    svlFilterAddLatency latency;
#endif

    svlFilterImageWindow windowlive;
    windowlive.SetName("LiveVideo");
    windowlive.SetTitle("Live Video");

    svlFilterImageWindow windowdelay;
    windowdelay.SetName("DelayVideo");
    windowdelay.SetTitle("Delay Video");

    svlVideoCodecBase *codec = svlVideoIO::GetCodec(".njpg");
    svlVideoIO::Compression *compr = codec->GetCompression();
    svlVideoIO::ReleaseCodec(codec);
    compr->data[0] = 75;

    svlFilterVideoFileWriter writer;
    writer.SetCodecParams(compr);
    svlVideoIO::ReleaseCompression(compr);

    int port = 10001;
    char portASCII[20];
    sprintf(portASCII, "@%d.njpg", port);
    writer.SetFilePath(portASCII);
    writer.OpenFile();

    ///////////////////////////////////////////////////////////////////////

    svlStreamManager stream(4);
    stream.SetSourceFilter(&source);

    // connect the source to the resize
    svlFilterOutput *output;

    output = source.GetOutput();
    // output->Connect(resize.GetInput());

    // connect the resize to the joiner
    // output = resize.GetOutput();
    // output->Connect(joiner.GetInput());

    // connect the joiner to the splitter
    // output = joiner.GetOutput();

    //    output->Connect(splitter.GetInput());

    // connect the splitter output to the latency
    // output = splitter.GetOutput();
    // output->Connect(latency.GetInput());

    // connect the latency to the writer
    // output = latency.GetOutput();
    output->Connect(writer.GetInput());

    if (1 < argc) {
	// connect the latency to the writer
	output = writer.GetOutput();
	output->Connect(windowdelay.GetInput());
      
	// connect the splitter output to the window
	output = splitter.GetOutput("Window");
	output->Connect(windowlive.GetInput());
    }

    if (stream.Play() != SVL_OK) {
	std::cerr << "Failed to start the stream." << std::endl;
	return 0;
    }
    std::cout << "SVL server started, press 'q' to stop." << std::endl;
	  
    char c = 'q';
    do {
	c = cmnGetChar();
      
	switch (c) {
	    /*
	      case 'l':
	      fleaLeftSource.DialogImageProperties();
	      break;
	  
	      case 'r': 
	      fleaRightSource.DialogImageProperties();
	      break;
	    */
	
	case '+':
	    // latency.UpLatency();
	    break;
	
	case '-':
	    // latency.DownLatency();
	    break;
	
	}
    } while (c != 'q');
    std::cout << "Stopping video stream" << std::endl;
    
    return 0;
}
