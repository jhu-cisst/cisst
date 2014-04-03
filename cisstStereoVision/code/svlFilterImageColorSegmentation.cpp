/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageColorSegmentation.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlImageProcessing.h>


/***********************************************/
/*** svlFilterImageColorSegmentation class *****/
/***********************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageColorSegmentation, svlFilterBase)

svlFilterImageColorSegmentation::svlFilterImageColorSegmentation() :
    svlFilterBase(),
    OutputImage(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);

    // Calculate Square Root Look-up Table Normalized to 0-255
    const unsigned int len = 3 * 255 * 255;
    const unsigned int maxval = static_cast<unsigned int>(sqrt(static_cast<float>(len)));
    NormSqrtLUT.SetSize(len);
    for (unsigned int i = 0; i < len; i ++) {
        NormSqrtLUT[i] = 255 * static_cast<unsigned int>(sqrt(static_cast<float>(i))) / maxval;
    }
}

svlFilterImageColorSegmentation::~svlFilterImageColorSegmentation()
{
    delete OutputImage;
}

void svlFilterImageColorSegmentation::AddColor(int x, int y, int z, unsigned char threshold, unsigned char label)
{
    AddColor(svlColorSpaceRGB, x, y, z, threshold, label);
}

void svlFilterImageColorSegmentation::AddColor(svlColorSpace colorspace, int x, int y, int z, unsigned char threshold, unsigned char label)
{
    unsigned int size = static_cast<unsigned int>(Color.size());

    Color.resize(size + 1);
    ColorSpace.resize(size + 1);
    ColorThreshold.resize(size + 1);
    ColorLabel.resize(size + 1);

    if (x > 255) x = 255;
    else if (x < -1) x = -1;
    if (y > 255) y = 255;
    else if (y < -1) y = -1;
    if (z > 255) z = 255;
    else if (z < -1) z = -1;

    Color[size][0] = x;
    Color[size][1] = y;
    Color[size][2] = z;
    ColorSpace[size] = colorspace;
    ColorThreshold[size] = threshold;
    ColorLabel[size] = label;
}

int svlFilterImageColorSegmentation::OnConnectInput(svlFilterInput &input, svlStreamType type)
{
    // Check if type is on the supported list
    if (!input.IsTypeSupported(type)) return SVL_FAIL;

    svlStreamType output_type;

    if (type == svlTypeImageRGB) {
        output_type = svlTypeImageMono8;
    }
    else if (type == svlTypeImageRGBStereo) {
        output_type = svlTypeImageMono8Stereo;
    }
    else return SVL_FAIL;

    SetOutputType("output", output_type);

    delete OutputImage;
    OutputImage = dynamic_cast<svlSampleImage*>(svlSample::GetNewFromType(output_type));

    return SVL_OK;
}

int svlFilterImageColorSegmentation::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    OutputImage->SetSize(syncInput);
    syncOutput = OutputImage;

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = img->GetVideoChannels();

    DistanceMap.SetSize(videochannels);
    for (unsigned int vch = 0; vch < videochannels; vch ++) {
        DistanceMap[vch].SetSize(img->GetHeight(vch), img->GetWidth(vch));
    }

    return SVL_OK;
}

int svlFilterImageColorSegmentation::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputImage;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        ComputeSegmentation(img, idx);
    }

    return SVL_OK;
}

void svlFilterImageColorSegmentation::ComputeSegmentation(svlSampleImage* image, unsigned int videoch)
{
    const unsigned int colorcount = static_cast<unsigned int>(Color.size());
    const unsigned int pixelcount = image->GetWidth(videoch) * image->GetHeight(videoch);
    unsigned char *inbuf, *outbuf, *distbuf;
    unsigned char label, dist;
    unsigned int i, c;
    int cx, cy, cz, thrsh2, xyz, dist2;
    bool enx, eny, enz;

    memset(OutputImage->GetUCharPointer(videoch), 0, pixelcount);
    memset(DistanceMap[videoch].Pointer(), 255, pixelcount);

    for (c = 0; c < colorcount; c ++) {

        cx      = Color[c][0];
        cy      = Color[c][1];
        cz      = Color[c][2];
        label   = ColorLabel[c];
        thrsh2  = ColorThreshold[c];
        thrsh2 *= thrsh2;
        inbuf   = image->GetUCharPointer(videoch);
        outbuf  = OutputImage->GetUCharPointer(videoch);
        distbuf = DistanceMap[videoch].Pointer();

        (cx >= 0) ? enx = true : enx = false;
        (cy >= 0) ? eny = true : eny = false;
        (cz >= 0) ? enz = true : enz = false;

        switch (ColorSpace[c]) {
            case svlColorSpaceRGB:
                for (i = 0; i < pixelcount; i ++) {
                    dist2 = 0;

                    if (enx) {
                        xyz = *inbuf;
                        xyz -= cx;
                        dist2 += xyz * xyz;
                    }
                    inbuf ++;

                    if (eny) {
                        xyz = *inbuf;
                        xyz -= cy;
                        dist2 += xyz * xyz;
                    }
                    inbuf ++;

                    if (enz) {
                        xyz = *inbuf;
                        xyz -= cz;
                        dist2 += xyz * xyz;
                    }
                    inbuf ++;

                    if (dist2 < thrsh2) {
                        dist = NormSqrtLUT[dist2];
                        if (dist < *distbuf) {
                            *outbuf = label;
                            *distbuf = dist;
                        }
                    }

                    outbuf ++;
                    distbuf ++;
                }
            break;

            case svlColorSpaceHSV:
                for (i = 0; i < pixelcount; i ++) {
                }
            break;

            case svlColorSpaceHSL:
                for (i = 0; i < pixelcount; i ++) {
                }
            break;

            case svlColorSpaceYUV:
                for (i = 0; i < pixelcount; i ++) {
                }
            break;

            default:
                return;
        }
    }
}

