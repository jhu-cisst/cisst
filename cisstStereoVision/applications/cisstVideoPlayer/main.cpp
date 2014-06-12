/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
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
#include <cisstStereoVision/svlFilterImageResizer.h>
#include <cisstStereoVision/svlFilterStereoImageSplitter.h>
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
    unsigned int width = 0;
    unsigned int height = 0;

    int numberOfThreads = 4;
    options.AddOptionOneValue("t", "threads",
                              "Number of threads, default is 4",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &numberOfThreads);

    options.AddOptionOneValue("c", "channels",
                              "Number of channels, 1 for mono (default), 2 for stereo",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &numberOfChannels);

    options.AddOptionOneValue("i", "ip",
                              "IP for network based codec",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &ip);

    options.AddOptionOneValue("p", "port",
                              "IP port for network based codec",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &portNumber);

    options.AddOptionOneValue("w", "width",
                              "Resize width (if specified, requires height)",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &width);

    options.AddOptionOneValue("h", "height",
                              "Resize height (if specified, requires width)",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &height);
  
    options.AddOptionNoValue("d", "dual-port",
                             "Create two ports to receive left/right separately (default is single port)",
                             cmnCommandLineOptions::OPTIONAL_OPTION);

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

    if ((width != 0) || (height != 0)) {
        if ((width == 0) || (height == 0)) {
            std::cerr << "Error: you need to specify both width and height, both need to be greater than 0." << std::endl;
            return -1;
        }
    }

    svlInitialize();

    svlStreamManager stream(numberOfThreads);

    int sourceNumberOfChannels = 1;
    if ((numberOfChannels == 2) && options.IsSet("dual-port")) {
        sourceNumberOfChannels = 2;
    }
    svlFilterSourceVideoFile source(sourceNumberOfChannels);
    source.SetName("Source");

    // connect the source
    svlFilterOutput * output;
    stream.SetSourceFilter(&source);
    output = source.GetOutput();

    // connect to source, split stereo if needed
    svlFilterStereoImageSplitter stereoSplitter;
    std::stringstream filePath;
    filePath << ip << "@" << portNumber << codecName;
    std::cout << "Opening network using " << filePath.str() << std::endl;
    // stereo
    if (numberOfChannels == 2) {
        // two channels
        if (options.IsSet("dual-port")) {
            source.SetFilePath(filePath.str(), SVL_LEFT);
            filePath.str(std::string());
            filePath << ip << "@" << portNumber + 1 << codecName;
            std::cout << "Opening network using " << filePath.str() << std::endl;
            source.SetFilePath(filePath.str(), SVL_RIGHT);
        } else {
            // join the two channels using the stereo image joiner
            output->Connect(stereoSplitter.GetInput());
            output = stereoSplitter.GetOutput();
            source.SetFilePath(filePath.str());
        }
    } else {
        // mono
        source.SetFilePath(filePath.str());
    }

    // resize if needed
    svlFilterImageResizer resize;
    if (width != 0) {
        resize.SetName("Resize");
        resize.SetInterpolation(true);
        resize.SetOutputSize(width, height, SVL_LEFT);
        resize.SetOutputSize(width, height, SVL_RIGHT);
        output->Connect(resize.GetInput());
        output = resize.GetOutput();
    }

    // preview
    svlFilterImageWindow previewWindow;
    previewWindow.SetName("Video");
    previewWindow.SetTitle("cisstVideoPlayer");
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
