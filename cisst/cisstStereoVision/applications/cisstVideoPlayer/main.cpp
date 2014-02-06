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

#include <cisstStereoVision.h>
#include <cisstCommon/cmnGetChar.h>
#include <cisstCommon/cmnPath.h>

int main(int argc, char** argv) {

    // check arguments
    if (argc != 2) {
	std::cerr << "Usage: " << argv[0] << " SVLIP" 
		  << std::endl;
	return -1;
    } 

    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
  
    std::cout << "Initializing SVL" << std::endl;

    ////////////////////////////////////////////////////////////
    // SVL
    ////////////////////////////////////////////////////////////

    std::string svlpath(std::string(argv[1]) + "@10001.njpg");

    svlInitialize();

    svlStreamManager stream(4);

    svlFilterSourceVideoFile source(1);
    source.SetName("Source");
    source.SetFilePath(svlpath);

#if 0
    svlFilterImageFlipRotate flip;
    flip.SetName("Flip");
    flip.SetVerticalFlip(true);

    svlFilterImageResizer resize;
    resize.SetName("Resize");
    resize.SetInterpolation(true);
    resize.SetOutputSize(1024*2, 768);
#endif

    svlFilterImageWindow windowdelay;
    windowdelay.SetName("DelayVideo");
    windowdelay.SetTitle("Delay Video");

    // connect the source
    svlFilterOutput *output;
    stream.SetSourceFilter(&source);

#if 0
    // connect the source to the flipper
    output = source.GetOutput();
    output->Connect(flip.GetInput());

    // connect the flipper to the resizer
    output = flip.GetOutput();
    output->Connect(resize.GetInput());
#endif

    output = source.GetOutput();
    output->Connect(windowdelay.GetInput());

    std::cout << "Starting stream" << std::endl;

    if (stream.Play() != SVL_OK) { 
	std::cout << "Failed to start the stream" << std::endl;
	return 0;
    }

    std::cout << "Stream started" << std::endl;
    
    std::cout << "ENTER to exit" << std::endl;

    cmnGetChar();

    return 0;
}
