/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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
                  int mindisparity, int maxdisparity, int smoothness, int blocksize, bool subpixel_precision, bool xcheck)
{
    cerr << "Please wait while initializing... ";

    svlInitialize();

    unsigned int srcwidth, srcheight;
    char filestr[1024];
    svlCameraGeometry geometry;
    svlRect roi;


//////////////////////////////////////////////////////////////
// instantiating SVL streams and filters

    // 3D reconstruction
    svlStreamManager stereo_stream(2);
    svlFilterSourceImageFile stereo_source(2);
    svlFilterComputationalStereo stereo_stereo;
    svlFilterDisparityMapToSurface stereo_3d;
    svlFilterStreamTypeConverter stereo_converter(svlTypeImageMonoFloat, svlTypeImageRGB);
    svlFilterImageWindow stereo_window;


//////////////////////////////////////////////////////////////
// creating stereo reconstruction pipeline

    // setup source
    if (stereo_source.SetFilePath(filepath1, "bmp", SVL_LEFT) != SVL_OK) goto labError;
    if (stereo_source.SetFilePath(filepath2, "bmp", SVL_RIGHT) != SVL_OK) goto labError;
    stereo_source.SetTargetFrequency(10.0);

    sprintf(filestr, "%s.bmp", filepath1);
    svlImageIO::ReadDimensions(filestr, srcwidth, srcheight);

    // setup stereo
    geometry.SetIntrinsics((double)srcwidth, (double)srcwidth,
                           (double)srcwidth / 2, (double)srcheight / 2,
                           0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                           SVL_LEFT);
    geometry.SetIntrinsics((double)srcwidth, (double)srcwidth,
                           (double)srcwidth / 2, (double)srcheight / 2,
                           0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                           SVL_RIGHT);
    geometry.SetExtrinsics(0.0, 0.0, 0.0,
                           0.0, 0.0, 0.0,
                           SVL_LEFT);
    geometry.SetExtrinsics(0.0, 0.0, 0.0,
                           10.0, 0.0, 0.0, // baseline = 10.0 mm
                           SVL_RIGHT);
    stereo_stereo.SetCameraGeometry(geometry);

    roi.Assign(5, 5, srcwidth - maxdisparity, srcheight - 5);
    stereo_stereo.SetROI(roi);

    stereo_stereo.SetCrossCheck(xcheck);
    stereo_stereo.SetSubpixelPrecision(subpixel_precision);
    stereo_stereo.SetDisparityRange(mindisparity, maxdisparity);
    stereo_stereo.SetScalingFactor(0);
    stereo_stereo.SetBlockSize(blocksize);
    stereo_stereo.SetQuickSearchRadius(maxdisparity);
    stereo_stereo.SetSmoothnessFactor(smoothness);
    stereo_stereo.SetTemporalFiltering(0);
    stereo_stereo.SetSpatialFiltering(0);

    // setup converter
    stereo_converter.SetScaling(255.0f / maxdisparity);

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

    //ComputeStereo("venus_l",  "venus_r",  0, 20, 80, 1, false, true);
    ComputeStereo("tsukuba3", "tsukuba4", 0, 16, 40, 1, false, true);

    cerr << "Quit" << endl;
    return 1;
}

