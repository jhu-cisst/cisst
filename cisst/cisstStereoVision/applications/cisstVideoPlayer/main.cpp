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
#include <cisstCommon/cmnCommandLineOptions.h>

#include <cisstStereoVision/svlInitializer.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlStreamManager.h>
#include <cisstStereoVision/svlFilterSourceVideoFile.h>
#include <cisstStereoVision/svlFilterImageWindow.h>

int main(int argc, char** argv)
{
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);

    cmnCommandLineOptions options;

    int numberOfChannels = 1;
    std::string ip = "";
    int portNumber = 0;
    std::string codecName = ".njpg";

    options.AddOptionOneValue("c", "channels",
                              "Number of channels, 1 for mono (default), 2 for stereo",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &numberOfChannels);

    options.AddOptionOneValue("i", "ip",
                              "IP for network based codec",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &ip);

    options.AddOptionOneValue("p", "port",
                              "IP port for network based codec",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &portNumber);
  
    std::string errorMessage;
    if (!options.Parse(argc, argv, errorMessage)) {
        std::cerr << "Error: " << errorMessage << std::endl;
        options.PrintUsage(std::cerr);
        return -1;
    }

    if ((numberOfChannels != 1) && (numberOfChannels != 2)) {
        std::cerr << "Error: number of channels can be either 1 or 2." << std::endl;
        return -1;
    }

    svlInitialize();

    svlStreamManager stream(4);

    svlFilterSourceVideoFile source(numberOfChannels);
    source.SetName("Source");

    std::stringstream filePath;
    filePath << ip << "@" << portNumber << codecName;
    std::cout << "Opening network using " << filePath.str() << std::endl;
    source.SetFilePath(filePath.str(), SVL_LEFT);
    if (numberOfChannels == 2) {
        filePath.str(std::string());
        filePath << ip << "@" << portNumber + 1 << codecName;
        std::cout << "Opening network using " << filePath.str() << std::endl;
        source.SetFilePath(filePath.str(), SVL_RIGHT);
    }

    svlFilterImageWindow previewWindow;
    previewWindow.SetName("Video");
    previewWindow.SetTitle("cisstVideoPlayer");

    // connect the source
    svlFilterOutput * output;
    stream.SetSourceFilter(&source);

    output = source.GetOutput();
    output->Connect(previewWindow.GetInput());

    std::cout << "Starting stream." << std::endl;

    if (stream.Play() != SVL_OK) { 
        std::cout << "Failed to start the stream." << std::endl;
        return 0;
    }

    std::cout << "Stream started." << std::endl
              << "Press any key to quit." << std::endl;

    cmnGetChar();
    std::cout << "Stopping video stream." << std::endl;
    stream.Release();

    cmnLogger::Kill();
    return 0;
}
