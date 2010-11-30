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


#include <cisstStereoVision.h>
#include <cisstCommon/cmnGetChar.h>

using namespace std;


///////////////////////////////////
//     Window callback class     //
///////////////////////////////////

class CWindowCallback : public svlImageWindowCallbackBase
{
public:
    CWindowCallback() :
        svlImageWindowCallbackBase(),
        ExposureCorrectionFilter(0),
        Gamma(true)
    {
    }

    void OnUserEvent(unsigned int CMN_UNUSED(winid), bool ascii, unsigned int eventid)
    {
        int x, y;
        GetMousePos(x, y);

        // handling user inputs
        if (ascii) {
            if (eventid == 'g') {
                if (Gamma) Gamma = false;
                else Gamma = true;
            }
        }
        else {
            switch (eventid) {
                case winInput_MOUSEMOVE:
                    if (ExposureCorrectionFilter) {
                        if (Gamma) {
                            ExposureCorrectionFilter->SetGamma(Scale[0] * (x - Center.x));
                        }
                        else {
                            ExposureCorrectionFilter->SetBrightness(Scale[0] * (x - Center.x));
                            ExposureCorrectionFilter->SetContrast(Scale[1] * (y - Center.y));
                        }
                    }

                default:
                    return;
            }
        }
    }

    svlFilterImageExposureCorrection* ExposureCorrectionFilter;
    svlPoint2D Center;
    vct2 Scale;
    bool Gamma;
};


///////////////////////////
//  Exposure Correction  //
///////////////////////////

int ExposureCorrection(std::string &src_path)
{
    svlInitialize();

    svlStreamManager stream(1);
    svlFilterSourceVideoFile source(1);
    svlFilterImageExposureCorrection exposurecorrection;
    svlFilterImageWindow window;
    CWindowCallback window_cb;

    if (src_path.empty()) {
        if (source.DialogFilePath() != SVL_OK) {
            cout << " -!- No source file has been selected." << endl;
            return -1;
        }
        source.GetFilePath(src_path);
    }
    else {
        source.SetFilePath(src_path);
    }

    window.SetEventHandler(&window_cb);
    window_cb.ExposureCorrectionFilter = &exposurecorrection;

    // chain filters to pipeline
    stream.SetSourceFilter(&source);
    source.GetOutput()->Connect(exposurecorrection.GetInput());
    exposurecorrection.GetOutput()->Connect(window.GetInput());
    
    stream.Initialize();

    const unsigned int cx = source.GetWidth() / 2;
    const unsigned int cy = source.GetHeight() / 2;
    window_cb.Center = svlPoint2D(cx, cy);
    window_cb.Scale = vct2(100.0 / cx, 100.0 / cy);

    // initialize and start stream
    if (stream.Play() != SVL_OK) goto labError;

    int ch;
    do {
        ch = cmnGetChar();
    } while (ch != 'q');

    // release pipeline
    stream.Release();

labError:
    return 0;
}


//////////////////////////////////
//             main             //
//////////////////////////////////

int main(int argc, char** argv)
{
    string source;
    if (argc >= 2) source = argv[1];
    else {
        cout << endl << "svlExExposureCorrection - cisstStereoVision example by Balazs Vagvolgyi" << endl;
        cout << "See http://www.cisst.org/cisst for details." << endl;
        cout << "Command line format:" << endl;
        cout << "     svlExExposureCorrection source_pathname" << endl;
        cout << "Examples:" << endl;
        cout << "     svlExExposureCorrection" << endl;
        cout << "     svlExExposureCorrection src.cvi" << endl;
    }

    ExposureCorrection(source);

    return 1;
}

