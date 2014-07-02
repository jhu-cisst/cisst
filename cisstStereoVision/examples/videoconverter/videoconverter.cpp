/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Balazs Vagvolgyi
  Created on: 2009

  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlInitializer.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlStreamManager.h>

#include <cisstStereoVision/svlFilterSourceVideoFile.h>
#include <cisstStereoVision/svlFilterVideoFileWriter.h>
#include <cisstStereoVision/svlFilterImageResizer.h>


using namespace std;


///////////////////////
//  Video Converter  //
///////////////////////

int VideoConverter(std::string & src_path, std::string &dst_path, bool loadcodec)
{
    const bool resize   = false;
    const bool cropping = false;
    const double crop_start = 0.0;
    const double crop_end   = 1304623501.0 - 1.0;

    svlInitialize();

    svlStreamManager stream(8);
    svlFilterSourceVideoFile source(1);
    svlFilterVideoFileWriter writer;

    // setup video file source
    if (!src_path.empty()) {
        source.SetFilePath(src_path);
    }
    else {
        if (source.DialogFilePath() != SVL_OK) {
            cerr << " -!- No source file has been selected." << endl;
            return -1;
        }
        source.GetFilePath(src_path);
    }
    source.SetTargetFrequency(1000.0); // as fast as possible
    source.SetLoop(false);


    // setup video file writer
    if (dst_path.empty()) {
        if (writer.DialogOpenFile() != SVL_OK) {
            cerr << " -!- No destination file has been selected." << endl;
            return -1;
        }
        writer.GetFilePath(dst_path);
    }
    else {
        if (!loadcodec || writer.LoadCodec("codec.dat") != SVL_OK) {
            writer.DialogFramerate();
            writer.DialogCodec(dst_path);
        }
        writer.SetFilePath(dst_path);
        writer.OpenFile();
    }
    if (loadcodec) {
        writer.SaveCodec("codec.dat");
    }
    std::string encoder;
    writer.GetCodecName(encoder);


    // setup image resizer
    svlFilterImageResizer resizer;
    resizer.SetOutputRatio(0.5, 0.5);
    resizer.SetInterpolation(true);


    // chain filters to pipeline
    svlFilterOutput* output = 0;

    stream.SetSourceFilter(&source);
        output = source.GetOutput();

    if (resize) {
        output->Connect(resizer.GetInput());
            output = resizer.GetOutput();
    }

    output->Connect(writer.GetInput());
        output = writer.GetOutput();

    cerr << "Converting: '" << src_path << "' to '" << dst_path <<"' using codec: '" << encoder << "'" << endl;

    // initialize stream
    if (stream.Initialize() != SVL_OK) goto labError;

    if (cropping) {
        vctInt2 range;
        range.Assign(source.GetPositionAtTime(crop_start),
                     source.GetPositionAtTime(crop_end));
        if (range[0] < 0) range[0] = 0;
        if (range[1] >= source.GetLength()) range[1] = source.GetLength() - 1;
        cerr << "Cropping video frames from #" << range[0] << " to #" << range[1] << endl;
        source.SetRange(range);
    }

    // start stream
    if (stream.Play() != SVL_OK) goto labError;

    do {
        cerr << " > Frames processed: " << source.GetFrameCounter() << "     \r";
    } while (stream.IsRunning() && stream.WaitForStop(0.5) == SVL_WAIT_TIMEOUT);
    cerr << " > Frames processed: " << source.GetFrameCounter() << "           " << endl;

    if (stream.GetStreamStatus() < 0) {
        // Some error
        cerr << " -!- Error occured during conversion." << endl;
    }
    else {
        // Success
        cerr << " > Conversion done." << endl;
    }

    // release pipeline
    stream.Release();
    stream.DisconnectAll();

labError:
    return 0;
}


//////////////////////////////////
//             main             //
//////////////////////////////////

int main(int argc, char** argv)
{
    string source, destination;
    if (argc >= 3) destination = argv[2];
    if (argc >= 2) source = argv[1];
    else {
        cerr << endl << "svlExVideoConverter - cisstStereoVision example by Balazs Vagvolgyi" << endl;
        cerr << "See http://www.cisst.org/cisst for details." << endl;
        cerr << "Command line format:" << endl;
        cerr << "     svlExVideoConverter [source_pathname [destination_pathname]]" << endl;
        cerr << "Examples:" << endl;
        cerr << "     svlExVideoConverter" << endl;
        cerr << "     svlExVideoConverter src.cvi" << endl;
        cerr << "     svlExVideoConverter src.avi dest.cvi" << endl << endl;
    }

    VideoConverter(source, destination, false);

    return 1;
}

