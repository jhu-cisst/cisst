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
#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlFilterImageResizer.h>
#include <cisstStereoVision/svlFilterImageFlipRotate.h>
#include <cisstStereoVision/svlFilterAddLatency.h>
#include <cisstStereoVision/svlFilterStereoImageJoiner.h>
#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlVideoIO.h>
#include <cisstStereoVision/svlFilterVideoFileWriter.h>

int main(int argc, char** argv)
{
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);

    cmnCommandLineOptions options;
    const std::string configFilePrefix = "camera-viewer";

    int portNumber = 0;
    std::string codecName = ".njpg";

    int numberOfChannels = 1;
    int latencyInFrames = 0;
    unsigned int width = 0;
    unsigned int height = 0;

    int numberOfThreads = 4;
    options.AddOptionOneValue("t", "threads",
                              "Number of threads, default is 4",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &numberOfThreads);

    options.AddOptionOneValue("c", "channels",
                              "Number of channels, 1 for mono (default), 2 for stereo",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &numberOfChannels);
    
    options.AddOptionOneValue("p", "port",
                              "IP port for network based codec",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &portNumber);

    options.AddOptionNoValue("d", "dual-port",
                             "Create two ports to send left/right separately (default is single port)",
                             cmnCommandLineOptions::OPTIONAL_OPTION);

    options.AddOptionOneValue("w", "width",
                              "Resize width (if specified, requires height)",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &width);

    options.AddOptionOneValue("h", "height",
                              "Resize height (if specified, requires width)",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &height);

    options.AddOptionNoValue("f", "flip-horizontal",
                             "Flip image left to right",
                             cmnCommandLineOptions::OPTIONAL_OPTION);

    options.AddOptionOneValue("l", "latency",
                              "Add latency (in number of frames)",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &latencyInFrames);

    options.AddOptionNoValue("s", "save-configuration",
                             std::string("Save camera configuration in ") + configFilePrefix + std::string("-{mono,stereo}.dat"),
                             cmnCommandLineOptions::OPTIONAL_OPTION);

    options.AddOptionNoValue("n", "no-window",
                             "don't display preview",
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

    std::string configFile = configFilePrefix;
    if (numberOfChannels == 1) {
        configFile.append("-mono.dat");
    } else {
        configFile.append("-stereo.dat");
    }
        
    if ((width != 0) || (height != 0)) {
        if ((width == 0) || (height == 0)) {
            std::cerr << "Error: you need to specify both width and height, both need to be greater than 0." << std::endl;
            return -1;
        }
    }

    svlInitialize();
    
    // connect the source to the stream
    svlFilterSourceVideoCapture source(numberOfChannels);
    if (source.LoadSettings(configFile.c_str()) != SVL_OK) {
        source.DialogSetup(SVL_LEFT);
        source.DialogSetup(SVL_RIGHT);
    }
    if (options.IsSet("save-configuration")) {
        source.SaveSettings(configFile.c_str());
    }
    
    svlFilterImageWindow previewWindow;
    previewWindow.SetName("Video");
    previewWindow.SetTitle("cisstCameraViewer");

    svlStreamManager stream(numberOfThreads);
    stream.SetSourceFilter(&source);

    // connect the source to next filter
    svlFilterOutput * output;
    output = source.GetOutput();

    // first resize if needed
    svlFilterImageResizer resize;
    if (width != 0) {
        resize.SetName("Resize");
        resize.SetInterpolation(true);
        resize.SetOutputSize(width, height, SVL_LEFT);
        resize.SetOutputSize(width, height, SVL_RIGHT);
        output->Connect(resize.GetInput());
        output = resize.GetOutput();
    }

    // flip if needed
    svlFilterImageFlipRotate flip;
    if (options.IsSet("flip-horizontal")) {
        flip.SetHorizontalFlip(true);
        output->Connect(flip.GetInput());
        output = flip.GetOutput();
    }

    // latency if needed
    svlFilterAddLatency latency;
    if (latencyInFrames > 0) {
        latency.SetFrameDelayed(latencyInFrames);
        output->Connect(latency.GetInput());
        output = latency.GetOutput();
    }

    // writer on network
    svlFilterVideoFileWriter writer;
    svlFilterStereoImageJoiner stereoJoiner;

    if (options.IsSet("port")) {
        // detect if codec is available
        svlVideoCodecBase * codec = svlVideoIO::GetCodec(codecName);
        if (codec == 0) {
            std::string formatlist;
            svlVideoIO::GetFormatList(formatlist);
            std::cerr << "Error: can't find codec " << codecName << std::endl
                      << "Supported formats:" << std::endl
                      << formatlist << std::endl;
            return -1;
        }
        svlVideoIO::Compression * compr = codec->GetCompression();
        svlVideoIO::ReleaseCodec(codec);
        compr->data[0] = 75;

        writer.SetCodecParams(compr);
        svlVideoIO::ReleaseCompression(compr);

        std::stringstream filePath;
        filePath << "@" << portNumber << codecName;
        std::cout << "Opening network using " << filePath.str() << std::endl;

        // stereo
        if (numberOfChannels == 2) {
            // two channels
            if (options.IsSet("dual-port")) {
                writer.SetFilePath(filePath.str(), SVL_LEFT);
                filePath.str(std::string());
                filePath << "@" << portNumber + 1 << codecName;
                std::cout << "Opening network using " << filePath.str() << std::endl;
                writer.SetFilePath(filePath.str(), SVL_RIGHT);
            } else {
                // join the two channels using the stereo image joiner
                output->Connect(stereoJoiner.GetInput());
                output = stereoJoiner.GetOutput();
                writer.SetFilePath(filePath.str());
            }
        } else {
            writer.SetFilePath(filePath.str());
        }
        writer.OpenFile();

        output->Connect(writer.GetInput());
        output = writer.GetOutput();
    }

    // connect the last filter to preview if desired
    if (!options.IsSet("no-window")) {
        output->Connect(previewWindow.GetInput());
    }
    
    if (stream.Play() != SVL_OK) {
        std::cerr << "Failed to start the stream." << std::endl;
        return 0;
    }
    std::cout << argv[0] << " started, press 'q' to stop." << std::endl;
	  
    char c;
    do {
        c = cmnGetChar();
        switch (c) {
        case '+':
            latency.UpLatency();
            break;
        case '-':
            latency.DownLatency();
            break;
        }
    } while (c != 'q');

    std::cout << "Stopping video stream." << std::endl;
    stream.Release();

    cmnLogger::Kill();
    return 0;
}
