/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009

  (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#ifdef _WIN32
#include <conio.h>
#endif // _WIN32

#include <iostream>
#include <string>
#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstStereoVision.h>


using namespace std;


///////////////////////
//  Video Converter  //
///////////////////////

int VideoConverter(std::string &source, std::string &destination)
{
    svlInitialize();

    svlStreamManager converter_stream(4);
    svlFilterSourceVideoFile converter_source(1);
    svlFilterVideoFileWriter converter_writer;

    if (source.empty()) {
        if (converter_source.DialogFilePath() != SVL_OK) {
            cerr << " -!- No source file has been selected." << endl;
            return -1;
        }
        converter_source.GetFilePath(source);
    }
    else {
        converter_source.SetFilePath(source);
    }

    converter_source.SetTargetFrequency(1000.0); // as fast as possible
    converter_source.SetLoop(false);

    if (destination.empty()) {
        if (converter_writer.DialogFilePath() != SVL_OK) {
            cerr << " -!- No destination file has been selected." << endl;
            return -1;
        }
        converter_writer.GetFilePath(destination);
    }
    else {
        converter_writer.SetFilePath(destination);
    }

    if (converter_writer.LoadCodec("codec.dat") != SVL_OK) {
        if (converter_writer.DialogCodec() != SVL_OK) {
            cerr << " -!- Unable to set up compression." << endl;
            return -1;
        }
        converter_writer.SaveCodec("codec.dat");
    }

    std::string encoder;
    converter_writer.GetCodecName(encoder);

    // chain filters to pipeline
    converter_stream.Trunk().Append(&converter_source);
    converter_stream.Trunk().Append(&converter_writer);

    cerr << "Converting: '" << source << "' to '" << destination <<"' using codec: '" << encoder << "'" << endl;

    // initialize and start stream
    if (converter_stream.Start() != SVL_OK) goto labError;

    do {
        cerr << " > Frames processed: " << converter_source.GetFrameCounter() << "     \r";
    } while (converter_stream.IsRunning() && converter_stream.WaitForStop(0.5) == SVL_WAIT_TIMEOUT);
    cerr << " > Frames processed: " << converter_source.GetFrameCounter() << "           " << endl;

    if (converter_stream.GetStreamStatus() < 0) {
        // Some error
        cerr << " -!- Error occured during conversion." << endl;
    }
    else {
        // Success
        cerr << " > Conversion done." << endl;
    }

    // destroy pipeline
    converter_stream.RemoveAll();

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
        cerr << endl << "stereoTutorialVideoConverter - cisstStereoVision example by Balazs Vagvolgyi" << endl;
        cerr << "See http://www.cisst.org/cisst for details." << endl;
        cerr << "Command line format:" << endl;
        cerr << "     stereoTutorialVideoConverter [source_pathname [destination_pathname]]" << endl;
        cerr << "Examples:" << endl;
        cerr << "     stereoTutorialVideoConverter" << endl;
        cerr << "     stereoTutorialVideoConverter src.cvi" << endl;
        cerr << "     stereoTutorialVideoConverter src.avi dest.cvi" << endl << endl;
    }

    VideoConverter(source, destination);

    return 1;
}

