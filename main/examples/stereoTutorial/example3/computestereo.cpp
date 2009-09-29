/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: computestereo.cpp 840 2009-09-11 18:33:56Z bvagvol1 $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2007

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

--- begin TSUKUBA LEGAL NOTES ---

The "tsukuba" and "venus" stereo image pairs are available at:

  Middlebury Stereo Vision Page:
     http://vision.middlebury.edu/stereo/

References:
[1] D. Scharstein and R. Szeliski. A taxonomy and evaluation of dense two-frame stereo correspondence algorithms.
International Journal of Computer Vision, 47(1/2/3):7-42, April-June 2002.
[2] D. Scharstein and R. Szeliski. High-accuracy stereo depth maps using structured light.
In IEEE Computer Society Conference on Computer Vision and Pattern Recognition (CVPR 2003), volume 1, pages 195-202, Madison, WI, June 2003.
[3] D. Scharstein and C. Pal. Learning conditional random fields for stereo.
In IEEE Computer Society Conference on Computer Vision and Pattern Recognition (CVPR 2007), Minneapolis, MN, June 2007.
[4] H. Hirschmüller and D. Scharstein. Evaluation of cost functions for stereo matching.
In IEEE Computer Society Conference on Computer Vision and Pattern Recognition (CVPR 2007), Minneapolis, MN, June 2007.

--- end TSUKUBA LEGAL NOTES ---
*/


#ifdef _WIN32
#include <conio.h>
#endif

#include <iostream>

#include <string>
#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstStereoVision.h>


using namespace std;


//////////////////////////////////
//     Computational Stereo     //
//////////////////////////////////

int ComputeStereo(const char* filepath1, const char* filepath2,
                  int mindisparity, int maxdisparity, int smoothness, int blocksize, bool interpolation, bool xcheck)
{
    cerr << "Please wait while initializing... ";

    int srcwidth, srcheight;
    double focallength, baseline, ppx_left, ppx_right, ppy;
    int left, right, top, bottom;
    char filestr[1024];

    svlImageFileTypeList filetype;
    svlImageFile* bmphandler = filetype.GetHandlerInstance("bmp");

//////////////////////////////////////////////////////////////
// instantiating SVL streams and filters

    // 3D reconstruction
    svlStreamManager stereo_stream(2);
    svlFilterSourceImageFile stereo_source(true);
    svlFilterComputationalStereo stereo_stereo;
    svlFilterStreamTypeConverter stereo_converter(svlTypeDepthMap, svlTypeImageRGB);
    svlFilterImageWindow stereo_window;


//////////////////////////////////////////////////////////////
// creating stereo reconstruction pipeline

    // setup source
    if (stereo_source.SetFilePath(filepath1, "bmp", SVL_LEFT) != SVL_OK) goto labError;
    if (stereo_source.SetFilePath(filepath2, "bmp", SVL_RIGHT) != SVL_OK) goto labError;
    stereo_source.SetTargetFrequency(10.0);

    sprintf(filestr, "%s.bmp", filepath1);
    bmphandler->ExtractDimensions((char*)filestr, srcwidth, srcheight);
    delete(bmphandler);

    // setup stereo
    focallength = (double)srcwidth;
    baseline = 10.0;
    ppx_left = (double)srcwidth / 2;
    ppx_right = (double)srcwidth / 2;
    ppy = (double)srcheight / 2;
    left = 5;
    right = srcwidth - maxdisparity;
    top = 5;
    bottom = srcheight - 5;

    stereo_stereo.SetCrossCheck(xcheck);
    stereo_stereo.DisparityOutput(true);
    stereo_stereo.DisparityInterpolation(interpolation);
    stereo_stereo.SetFocalLength(focallength);
    stereo_stereo.SetStereoBaseline(baseline);
    stereo_stereo.SetPrincipalPoints(ppx_left, ppx_right, ppy);
    stereo_stereo.SetValidRect(left, top, right, bottom);
    stereo_stereo.SetDisparityRange(mindisparity, maxdisparity);
    stereo_stereo.SetScalingFactor(0);
    stereo_stereo.SetBlockSize(blocksize);
    stereo_stereo.SetQuickSearchRadius(maxdisparity);
    stereo_stereo.SetSmoothnessFactor(smoothness);
    stereo_stereo.SetTemporalFiltering(0);
    stereo_stereo.SetSpatialFiltering(0);

    // setup converter
    stereo_converter.SetDistanceIntensityRatio((float)(255.0 / maxdisparity));

    // chain filters to pipeline
    if (stereo_stream.Trunk().Append(&stereo_source)    != SVL_OK ||
        stereo_stream.Trunk().Append(&stereo_stereo)    != SVL_OK ||
        stereo_stream.Trunk().Append(&stereo_converter) != SVL_OK ||
        stereo_stream.Trunk().Append(&stereo_window)    != SVL_OK) goto labError;


//////////////////////////////////////////////////////////////
// pipeline control

    cerr << "Done" << endl << "Starting stream... ";

    // initialize and start stream
    if (stereo_stream.Start() != SVL_OK) goto labError;

    cerr << "Done" << endl << endl << "Keyboard commands:" << endl << endl;
    cerr << "    'q' - Quit" << endl;

    int ch;

    do {
        ch = cmnGetChar();
        osaSleep(1.0 * cmn_ms);
    } while (ch != 'q');

    cerr << endl;

    // stop stream
    stereo_stream.Stop();

labError:
    // clean up
    stereo_stream.RemoveAll();
    return 0;
}


//////////////////////////////////
//             main             //
//////////////////////////////////

int main(int CMN_UNUSED(argc), char** CMN_UNUSED(argv))
{
    cerr << endl << "computestereo - cisstStereoVision example by Balazs Vagvolgyi" << endl;
    cerr << "See http://www.cisst.org/cisst for details." << endl << endl;

    ComputeStereo("venus_l",  "venus_r",  0, 20, 80, 1, false, true);
    //ComputeStereo("tsukuba3", "tsukuba4", 0, 16, 40, 1, false, true);

    cerr << "Quit" << endl;
    return 1;
}

