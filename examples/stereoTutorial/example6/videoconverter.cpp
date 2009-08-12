/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
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

int VideoConverter(const std::string source, const std::string destination)
{
    // instantiating SVL stream and filters
    svlStreamManager converter_stream(1);
    svlFilterSourceVideoFile converter_source(false);
    svlFilterVideoFileWriterAVI converter_writerAVI;
#if (CISST_SVL_HAS_ZLIB == ON)
    svlFilterVideoFileWriter converter_writerCVI;
#endif // CISST_SVL_HAS_ZLIB
    svlFilterBase* converter_writer = 0;

    // setup source
    if (source.empty()) {
        if (converter_source.DialogFilePath() != SVL_OK) {
            cerr << "No source file has been selected." << endl;
            return -1;
        }
    }
    else {
        converter_source.SetFilePath(source);
    }

    converter_source.SetTargetFrequency(1000.0); // as fast as possible
    converter_source.SetLoop(false);

    // setup destination
    if (destination.empty()) {
#if (CISST_SVL_HAS_ZLIB == ON)
        cerr << "Select video format - AVI or CVI [press 'a' or 'c']";
        int ch = cmnGetChar();
        cerr << endl;
        switch (ch) {
            case 'a':
                if (converter_writerAVI.DialogFilePath() != SVL_OK) {
                    cerr << "No destination file has been selected." << endl;
                    return -1;
                }
                converter_writerAVI.DialogCodec();
                converter_writer = &converter_writerAVI;
            break;

            case 'c':
                if (converter_writerCVI.DialogFilePath() != SVL_OK) {
                    cerr << "No destination file has been selected." << endl;
                    return -1;
                }
                converter_writer = &converter_writerCVI;
            break;

            default:
            break;
        }
#else // CISST_SVL_HAS_ZLIB
        if (converter_writerAVI.DialogFilePath() != SVL_OK) {
            cerr << "No destination file has been selected." << endl;
            return -1;
        }
        converter_writerAVI.DialogCodec();
        converter_writer = &converter_writerAVI;
#endif // CISST_SVL_HAS_ZLIB
    }
    else {
#if (CISST_SVL_HAS_ZLIB == ON)
        if (destination.rfind(".avi") == destination.length() - 4) {
            converter_writerAVI.SetFilePath(destination);
        }
        else if (destination.rfind(".cvi") == destination.length() - 4) {
            converter_writerCVI.SetFilePath(destination);
        }
        else {
            cerr << "Cannot recognize file extension. Available options: '.AVI' or '.CVI'" << endl;
            return -1;
        }
#else // CISST_SVL_HAS_ZLIB
        converter_writerAVI.SetFilePath(destination);
        if (destination.rfind(".avi") == destination.length() - 4) {
            converter_writerAVI.SetFilePath(destination);
        }
        else {
            cerr << "Cannot recognize file extension.  Available options: '.AVI'" << endl;
            return -1;
        }
#endif // CISST_SVL_HAS_ZLIB
    }

    // chain filters to pipeline
    converter_stream.Trunk().Append(&converter_source);
    converter_stream.Trunk().Append(converter_writer);

    cerr << endl << "Conversion in progress. Please wait..." << endl;

    // initialize and start stream
    if (converter_stream.Start() != SVL_OK) goto labError;

    converter_stream.WaitForStop();
    if (converter_stream.GetStreamStatus() < 0) {
        // Some error
        cerr << endl << "Error while conversion." << endl;
    }
    else {
        // Success
        cerr << endl << "Conversion done." << endl;
    }

    // destroy pipeline
    converter_stream.EmptyFilterList();

labError:
    return 0;
}


//////////////////////////////////
//             main             //
//////////////////////////////////

int main(int argc, char** argv)
{
    cerr << endl << "stereoTutorialVideoConverter - cisstStereoVision example by Balazs Vagvolgyi" << endl;
    cerr << "See http://www.cisst.org/cisst for details." << endl;
    cerr << "Command line format:" << endl;
    cerr << "     stereoTutorialVideoConverter [source_pathname [destination_pathname]]" << endl;
    cerr << "Examples:" << endl;
    cerr << "     stereoTutorialVideoConverter src.cvi" << endl;
    cerr << "     stereoTutorialVideoConverter src.avi dest.cvi" << endl << endl;

    string source, destination;
    if (argc >= 2) {
        source = argv[1];
        if (argc >= 3) source = argv[2];
    }

    VideoConverter(source, destination);

    cerr << "Quit" << endl << endl;
    return 1;
}

