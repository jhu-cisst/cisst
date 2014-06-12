/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Marcin Balicki
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
#include <cisstCommon/cmnCommandLineOptions.h>

#include <cisstStereoVision/svlInitializer.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlStreamManager.h>
#include <cisstStereoVision/svlFilterSourceVideoFile.h>
#include <cisstStereoVision/svlFilterVideoFileWriter.h>
#include <cisstStereoVision/svlFilterImageOverlay.h>
#include <cisstStereoVision/svlFilterImageChannelSwapper.h>
#include <cisstStereoVision/svlFilterImageFlipRotate.h>
#include <cisstStereoVision/svlFilterImageResizer.h>
#include <cisstStereoVision/svlFilterFrameTimeSync.h>
#include <cisstStereoVision/svlFilterImageWindow.h>

int main(int argc, char **argv)
{
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);

    double resizeScale = 1.0;
    double frameRate = 20.0;
    double rotation = 0.0;
    int numberOfThreads = 2;

    std::string timesyncFile;
    std::string inputFile;
    std::string outputFile;

    cmnCommandLineOptions options;

    options.AddOptionOneValue("i", "input",
                              "input file name",
                              cmnCommandLineOptions::REQUIRED_OPTION, &inputFile);

    options.AddOptionOneValue("o", "output",
                              "output file",
                              cmnCommandLineOptions::REQUIRED_OPTION, &outputFile);

    options.AddOptionOneValue("s", "scale",
                              "scale to Resize video, default is 1.0",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &resizeScale);

    options.AddOptionNoValue("c", "colorSwap",
                             "Swaps r and b in rgb color",
                             cmnCommandLineOptions::OPTIONAL_OPTION);

    options.AddOptionOneValue("r", "rotate",
                              "Rotates clockwise by 90/180/270 degrees only",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &rotation);

    options.AddOptionNoValue("v", "vflip",
                             "flip along the horizontal axis",
                             cmnCommandLineOptions::OPTIONAL_OPTION);

    options.AddOptionNoValue("h", "hflip",
                             "flip along the vertical axis",
                             cmnCommandLineOptions::OPTIONAL_OPTION);

    options.AddOptionOneValue("f", "fps",
                              "Set frame rate of the input",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &frameRate);

    options.AddOptionOneValue("t", "timesync",
                              "corrects timestamps with an external timesync file",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &timesyncFile);

    options.AddOptionNoValue("y", "timeoverlay",
                             "shows a timestamp on the output video",
                             cmnCommandLineOptions::OPTIONAL_OPTION);

    options.AddOptionOneValue("j", "threads",
                              "number of threads to use",
                              cmnCommandLineOptions::OPTIONAL_OPTION, &numberOfThreads);

    options.AddOptionNoValue("d", "display",
                             "displays the output",
                             cmnCommandLineOptions::OPTIONAL_OPTION);


    std::string errorMessage;
    if (!options.Parse(argc, argv, errorMessage)) {
        std::cerr << "Error: " << errorMessage << std::endl;
        options.PrintUsage(std::cerr);
        return -1;
    }

    svlInitialize();

    svlStreamManager            stream(numberOfThreads);
    svlFilterSourceVideoFile    source(1);
    svlFilterVideoFileWriter    writer;

    svlFilterImageOverlay         overlay;
    svlFilterImageChannelSwapper  swapper;
    svlFilterImageFlipRotate      fliprotate;
    svlFilterImageResizer         resizer;
    svlFilterImageWindow          previewWindow;

    svlFilterFrameTimeSync       frameTimeSync;

    // Add timestamp overlay
    svlOverlayTimestamp ts_overlay(0, true, &source, svlRect(4, 4, 134, 21),
                                   15.0, svlRGB(255, 200, 200), svlRGB(32, 32, 32));

    overlay.AddOverlay(ts_overlay);

    if (inputFile.empty()) {
        if (source.DialogFilePath() != SVL_OK) {
            std::cerr << "Error: no source file has been selected." << std::endl;
            return -1;
        }
        source.GetFilePath(inputFile);
    } else {
        source.SetFilePath(inputFile);
    }

    source.SetTargetFrequency(1000.0); // as fast as possible
    source.SetLoop(false);

    // setup video file writer
    if (outputFile.empty()) {
        if (writer.DialogOpenFile() != SVL_OK) {
            std::cerr << "Error: no destination file has been selected." << std::endl;
            return -1;
        }
        writer.GetFilePath(outputFile);
    } else {
        if (writer.LoadCodec("codec.dat") != SVL_OK) {
            writer.DialogCodec(outputFile);
        }
        writer.SetFilePath(outputFile);
        writer.OpenFile();
    }

    writer.SaveCodec("codec.dat");

    std::string encoder;
    writer.GetCodecName(encoder);
    writer.SetFramerate(frameRate);

    // chain filters to pipeline
    svlFilterOutput * output = 0;
    stream.SetSourceFilter(&source);
    output = source.GetOutput();

    if (options.IsSet("scale")) {
        resizer.SetOutputRatio(resizeScale, resizeScale);
        output->Connect(resizer.GetInput());
        output = resizer.GetOutput();
    }

    if (options.IsSet("vflip")) {
        fliprotate.SetVerticalFlip(true);
    }

    if (options.IsSet("hflip")) {
        fliprotate.SetHorizontalFlip(true);
    }

    if (options.IsSet("rotate")) {
        if (rotation == 90.0 ) {
            fliprotate.SetRotation(1);
        } else if (rotation == 180.0) {
            fliprotate.SetRotation(2);
        } else if (rotation == 270.0) {
            fliprotate.SetRotation(3);
        } else {
            fliprotate.SetRotation(0);
            std::cerr << "Error: rotation " << rotation << " not possible (90, 180 or 270)" << std::endl;
        }
    }

    if (options.IsSet("vflip") || options.IsSet("hflip") || options.IsSet("rotate")) {
        output->Connect(fliprotate.GetInput());
        output = fliprotate.GetOutput();
    }

    if (options.IsSet("colorSwap")) {
        output->Connect(swapper.GetInput());
        output = swapper.GetOutput();
    }

    if (!timesyncFile.empty()) {
        frameTimeSync.OpenConversionFile(timesyncFile);
        output->Connect(frameTimeSync.GetInput()); 
        output = frameTimeSync.GetOutput();
    }

    if (options.IsSet("timeoverlay")) {
        output->Connect(overlay.GetInput());
        output = overlay.GetOutput();
    }

    output->Connect(writer.GetInput());
    output = writer.GetOutput();

    if (options.IsSet("display")) {
        previewWindow.SetName("Video");
        previewWindow.SetTitle(outputFile);
        output->Connect(previewWindow.GetInput());
        output = previewWindow.GetOutput();
    }

    std::cout << "Converting: '" << inputFile << "' to '" << outputFile << "' using codec: '" << encoder << "'" << std::endl;

    // initialize and start stream
    if (stream.Play() != SVL_OK) {
        std::cerr << "Error: failed to start the stream." << std::endl;
        return -1;
    }
    do {
        std::cout << " > Frames processed: " << source.GetFrameCounter() << "     \r";
    }
    while (stream.IsRunning() && stream.WaitForStop(0.5) == SVL_WAIT_TIMEOUT);

    std::cout << " > Frames processed: " << source.GetFrameCounter() << "           " << std::endl;

    if (stream.GetStreamStatus() < 0) {
        // Some error
        std::cerr << "Error: error occured during conversion." << std::endl;
    } else {
        // Success
        std::cout << " > Conversion done." << std::endl;
    }

    stream.Release();
    cmnLogger::Kill();

    return 1;
}

