/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageCenterFinder.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlImageProcessing.h>
#include <cisstStereoVision/svlDraw.h>


/*************************************************/
/*** svlFilterImageCenterFinderInterface class ***/
/*************************************************/

int svlFilterImageCenterFinderInterface::OnChangeCenter(int CMN_UNUSED(x), int CMN_UNUSED(y), unsigned int CMN_UNUSED(videoch))
{
    return SVL_FAIL;
}

int svlFilterImageCenterFinderInterface::OnChangeCenterRect(const svlRect & CMN_UNUSED(rect), unsigned int CMN_UNUSED(videoch))
{
    return SVL_FAIL;
}

int svlFilterImageCenterFinderInterface::OnChangeCenterEllipse(const svlEllipse & CMN_UNUSED(ellipse), unsigned int CMN_UNUSED(videoch))
{
    return SVL_FAIL;
}


/*****************************************/
/*** svlFilterImageCenterFinder class ****/
/*****************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageCenterFinder, svlFilterBase)

svlFilterImageCenterFinder::svlFilterImageCenterFinder() :
    svlFilterBase(),
    Smoothing(0.0),
    ThresholdLevel(10),
    MinRadius(20),
    MassRatio(50),
    LinkHorizontally(false),
    LinkVertically(true),
    EllipseFittingEnabled(false),
    EllipseFittingDrawEllipse(false),
    EllipseFittingSlices(32),
    EllipseFittingMode(0),
    EllipseFittingEdgeThreshold(100),
    EllipseFittingErrorThreshold(18),
    EllipseMaskEnabled(false),
    EllipseMaskSlices(32),
    EllipseMaskTransitionStart(0),
    EllipseMaskTransitionEnd(0),
    EllipseMargin(0),
    MaskImage(0),
    TransitionImage(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

int svlFilterImageCenterFinder::GetCenter(int &x, int &y, unsigned int videoch) const
{
    if (!IsRunning()) return SVL_FAIL;
    if (videoch >= CenterX.size()) return SVL_FAIL;

    x = CenterX[videoch];
    y = CenterY[videoch];

    return SVL_OK;
}

int svlFilterImageCenterFinder::GetRadius(int &x, int &y, unsigned int videoch) const
{
    if (!IsRunning()) return SVL_FAIL;
    if (videoch >= RadiusX.size()) return SVL_FAIL;

    x = RadiusX[videoch];
    y = RadiusY[videoch];

    return SVL_OK;
}

int svlFilterImageCenterFinder::GetEllipse(svlEllipse &ellipse, unsigned int videoch) const
{
    if (!IsRunning()) return SVL_FAIL;
    if (videoch >= RadiusX.size()) return SVL_FAIL;

    ellipse = Ellipse[videoch];

    return SVL_OK;
}

void svlFilterImageCenterFinder::SetTrajectorySmoothing(double smoothing)
{
    Smoothing = smoothing;
}

void svlFilterImageCenterFinder::SetThreshold(unsigned char thresholdlevel)
{
    ThresholdLevel = thresholdlevel;
}

void svlFilterImageCenterFinder::SetMinRadius(unsigned int radius)
{
    MinRadius = radius;
}

void svlFilterImageCenterFinder::SetMassRatio(unsigned int ratio)
{
    if (ratio > 100) ratio = 100;
    MassRatio = ratio;
}

void svlFilterImageCenterFinder::SetHorizontalLink(bool enable)
{
    LinkHorizontally = enable;
}

void svlFilterImageCenterFinder::SetVerticalLink(bool enable)
{
    LinkVertically = enable;
}

double svlFilterImageCenterFinder::GetTrajectorySmoothing() const
{
    return Smoothing;
}

unsigned char svlFilterImageCenterFinder::GetThreshold() const
{
    return static_cast<unsigned char>(ThresholdLevel);
}

unsigned int svlFilterImageCenterFinder::GetMinRadius() const
{
    return MinRadius;
}

unsigned int svlFilterImageCenterFinder::GetMassRatio() const
{
    return MassRatio;
}

bool svlFilterImageCenterFinder::GetHorizontalLink() const
{
    return LinkHorizontally;
}

bool svlFilterImageCenterFinder::GetVerticalLink() const
{
    return LinkVertically;
}

int svlFilterImageCenterFinder::SetEnableEllipseFitting(bool enable)
{
    if (IsInitialized()) return SVL_FAIL;
    EllipseFittingEnabled = enable;
    return SVL_OK;
}

int svlFilterImageCenterFinder::SetEnableEllipseFittingDrawEllipse(bool enable)
{
    EllipseFittingDrawEllipse = enable;
    return SVL_OK;
}

int svlFilterImageCenterFinder::SetEnableEllipseMask(bool enable)
{
    if (IsInitialized()) return SVL_FAIL;
    if (enable) EllipseFittingEnabled = true;
    EllipseMaskEnabled = enable;
    return SVL_OK;
}

void svlFilterImageCenterFinder::SetEllipseMaskTransition(int start, int end)
{
    EllipseMaskTransitionStart = start;
    EllipseMaskTransitionEnd   = end;
}

void svlFilterImageCenterFinder::SetEllipseMargin(int margin)
{
    EllipseMargin = margin;
}

bool svlFilterImageCenterFinder::GetEnableEllipseFitting() const
{
    return EllipseFittingEnabled;
}

bool svlFilterImageCenterFinder::GetEnableEllipseFittingDrawEllipse() const
{
    return EllipseFittingDrawEllipse;
}

bool svlFilterImageCenterFinder::GetEnableEllipseMask() const
{
    return EllipseMaskEnabled;
}

void svlFilterImageCenterFinder::GetEllipseMaskTransition(int & start, int & end) const
{
    start = EllipseMaskTransitionStart;
    end   = EllipseMaskTransitionEnd;
}

int svlFilterImageCenterFinder::GetEllipseMargin()
{
    return EllipseMargin;
}

svlSampleImage* svlFilterImageCenterFinder::GetEllipseMask()
{
    return MaskImage;
}

void svlFilterImageCenterFinder::AddReceiver(svlFilterImageCenterFinderInterface* receiver)
{
    if (!receiver) return;

    unsigned int size = static_cast<unsigned int>(Receivers.size());
    Receivers.resize(size + 1);
    Receivers[size] = receiver;
}

int svlFilterImageCenterFinder::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    svlSampleImage* image  = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int i, videochannels = image->GetVideoChannels();
    unsigned int size;

    ProjectionV.SetSize(videochannels);
    ProjectionH.SetSize(videochannels);
    CenterXInternal.SetSize(videochannels);
    CenterYInternal.SetSize(videochannels);
    RadiusXInternal.SetSize(videochannels);
    RadiusYInternal.SetSize(videochannels);
    CenterX.SetSize(videochannels);
    CenterY.SetSize(videochannels);
    RadiusX.SetSize(videochannels);
    RadiusY.SetSize(videochannels);
    Ellipse.SetSize(videochannels);

    for (i = 0; i < videochannels; i ++) {
        size = image->GetWidth(i);
        ProjectionV[i].SetSize(size);
        RadiusXInternal[i] = CenterXInternal[i] = RadiusX[i] = CenterX[i] = static_cast<int>(size) / 2;

        size = image->GetHeight(i);
        ProjectionH[i].SetSize(size);
        RadiusYInternal[i] = CenterYInternal[i] = RadiusY[i] = CenterY[i] = static_cast<int>(size) / 2;
    }

    if (EllipseFittingEnabled) {
        // Initialize ellipse fitting
        RadialProfiles.SetSize(videochannels);
        TempProfile.SetSize(videochannels);
        StepX.SetSize(videochannels);
        StepY.SetSize(videochannels);
        EdgeX.SetSize(videochannels);
        EdgeY.SetSize(videochannels);
        EdgeRad.SetSize(videochannels);
        EdgeRadSmooth.SetSize(videochannels);
        EllipseFittingInternals.SetSize(videochannels);

        for (i = 0; i < videochannels; i ++) {
            TempProfile[i].SetSize(image->GetWidth(i) / 2);
            RadialProfiles[i].SetSize(EllipseFittingSlices, TempProfile[i].size());
            StepX[i].SetSize(EllipseFittingSlices);
            StepY[i].SetSize(EllipseFittingSlices);
            EdgeX[i].SetSize(EllipseFittingSlices);
            EdgeY[i].SetSize(EllipseFittingSlices);
            EdgeRad[i].SetSize(EllipseFittingSlices);
            EdgeRadSmooth[i].SetSize(EllipseFittingSlices);
        }

        if (EllipseMaskEnabled) {
            // Initialize mask generation
            if (videochannels == 1) MaskImage = new svlSampleImageMono8;
            else                    MaskImage = new svlSampleImageMono8Stereo;
            MaskImage->SetSize(image);
            TransitionImage = new svlSampleImageMono8;
            TransitionImage->SetSize(10, 256);
            CreateTransitionImage();
            TriangleInternals.SetSize(videochannels);
            WarpInternals.SetSize(videochannels);
            EllipseSamplesIn.SetSize(videochannels);
            EllipseSamplesOut.SetSize(videochannels);
        }
    }

    syncOutput = syncInput;

    return SVL_OK;
}

int svlFilterImageCenterFinder::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage* image  = dynamic_cast<svlSampleImage*>(syncInput);
    const unsigned int videochannels = image->GetVideoChannels();
    unsigned int a, i, j, x, y, rx, ry, width, height, vch, hsum, vsum, pix, stride;
    unsigned int *h, *h2, *v, *v2;
    unsigned char *img;
    const unsigned int thrsh = ThresholdLevel * 3;

    _ParallelLoop(procInfo, vch, videochannels)
    {
        width = image->GetWidth(vch);
        height = image->GetHeight(vch);
        stride = width * 3 - 2;

        img = image->GetUCharPointer(vch);
        v = ProjectionV[vch].Pointer();
        h = ProjectionH[vch].Pointer();

        memset(v, 0, sizeof(unsigned int) * width);

        // Generate projections
        for (j = 0; j < height; j ++) {
            v2 = v;
            hsum = 0;

            for (i = 0; i < width; i ++) {
                pix  = *img; img ++;
                pix += *img; img ++;
                pix += *img; img ++;

                if (pix >= thrsh) {
                    hsum += 255;
                    *v2 += 255;
                }

                v2 ++;
            }

            *h = hsum / width; h ++;
        }
        for (i = 0; i < width; i ++) {
            *v /= height; v ++;
        }


        // Find center of weight
        v = ProjectionV[vch].Pointer();
        h = ProjectionH[vch].Pointer();

        x = vsum = 0;
        for (i = 0; i < width; i ++) {
            a = *v; v ++; vsum += a;
            x += a * i;
        }
        if (vsum != 0) x /= vsum;
        else x = CenterXInternal[vch];

        y = hsum = 0;
        for (j = 0; j < height; j ++) {
            a = *h; h ++; hsum += a;
            y += a * j;
        }
        if (hsum != 0) y /= hsum;
        else y = CenterYInternal[vch];


        // Find radii
        v = v2 = ProjectionV[vch].Pointer() + x;
        h = h2 = ProjectionH[vch].Pointer() + y;

        // Set threshold to 60% of the whole mass
        hsum = hsum * MassRatio / 100;
        vsum = vsum * MassRatio / 100;

        a = rx = 0;
        while (a < vsum) {
            a += *v + *v2;
            v ++; v2 --;
            rx ++;
        }

        a = ry = 0;
        while (a < hsum) {
            a += *h + *h2;
            h ++; h2 --;
            ry ++;
        }

        if (rx < MinRadius) rx = MinRadius;
        if (ry < MinRadius) ry = MinRadius;

        // Smoothing results
        if (FrameCounter > 0 && Smoothing > 0.0) {
            x  = static_cast<int>((Smoothing * CenterXInternal[vch] + x ) / (1.0 + Smoothing));
            y  = static_cast<int>((Smoothing * CenterYInternal[vch] + y ) / (1.0 + Smoothing));
            rx = static_cast<int>((Smoothing * RadiusXInternal[vch] + rx) / (1.0 + Smoothing));
            ry = static_cast<int>((Smoothing * RadiusYInternal[vch] + ry) / (1.0 + Smoothing));
        }

        // Storing results
        CenterXInternal[vch] = x;
        CenterYInternal[vch] = y;
        RadiusXInternal[vch] = rx;
        RadiusYInternal[vch] = ry;

        if (EllipseFittingEnabled) {
            x = Ellipse[vch].cx;
            y = Ellipse[vch].cy;

            if (FindEllipse(image, vch, x, y, Ellipse[vch]) && Ellipse[vch].rx > 0 &&
                                                               Ellipse[vch].ry > 0) {

                // Smoothing results
                if (FrameCounter > 0 && Smoothing > 0.0) {
                    x  = static_cast<int>((Smoothing * x + Ellipse[vch].cx) / (1.0 + Smoothing));
                    y  = static_cast<int>((Smoothing * y + Ellipse[vch].cy) / (1.0 + Smoothing));
                }

                // Adjust with margin
                Ellipse[vch].rx = std::max(static_cast<int>(MinRadius), Ellipse[vch].rx - EllipseMargin);
                Ellipse[vch].ry = std::max(static_cast<int>(MinRadius), Ellipse[vch].ry - EllipseMargin);

                svlRect bounding;
                Ellipse[vch].GetBoundingRect(bounding);

                CenterXInternal[vch] = Ellipse[vch].cx;
                CenterYInternal[vch] = Ellipse[vch].cy;
                RadiusXInternal[vch] = (bounding.right - bounding.left) / 2;
                RadiusYInternal[vch] = (bounding.bottom - bounding.top) / 2;

                if (EllipseFittingDrawEllipse) svlDraw::Ellipse(image, vch, Ellipse[vch], svlRGB(255, 255, 255));
                if (EllipseMaskEnabled) UpdateMaskImage(vch, Ellipse[vch]);
            }
            else {
                Ellipse[vch].Assign(CenterXInternal[vch], CenterYInternal[vch], RadiusXInternal[vch], RadiusYInternal[vch], 0.0);
            }
        }
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo)
    {
        x = y = rx = ry = 0;

        if (LinkHorizontally) {
            for (vch = 0; vch < videochannels; vch ++) {
                x  += CenterXInternal[vch];
                rx += RadiusXInternal[vch];
            }
        }
        if (LinkVertically) {
            for (vch = 0; vch < videochannels; vch ++) {
                y  += CenterYInternal[vch];
                ry += RadiusYInternal[vch];
            }
        }

        for (vch = 0; vch < videochannels; vch ++) {
            if (LinkHorizontally) {
                CenterX[vch] = x  / videochannels;
                RadiusX[vch] = rx / videochannels;
            }
            else {
                CenterX[vch] = CenterXInternal[vch];
                RadiusX[vch] = RadiusXInternal[vch];
            }
            if (LinkVertically) {
                CenterY[vch] = y  / videochannels;
                RadiusY[vch] = ry / videochannels;
            }
            else {
                CenterY[vch] = CenterYInternal[vch];
                RadiusY[vch] = RadiusYInternal[vch];
            }
        }

        for (i = 0; i < Receivers.size(); i ++) {
            if (Receivers[i]) {
                for (vch = 0; vch < videochannels; vch ++) {
                    Receivers[i]->OnChangeCenter(CenterX[vch], CenterY[vch], vch);
                    Receivers[i]->OnChangeCenterRect(svlRect(CenterX[vch] - RadiusX[vch], CenterY[vch] - RadiusY[vch],
                                                             CenterX[vch] + RadiusX[vch], CenterY[vch] + RadiusY[vch]), vch);
                    if (EllipseFittingEnabled) {
                        Receivers[i]->OnChangeCenterEllipse(Ellipse[vch], vch);
                    }
                }
            }
        }
    }

    return SVL_OK;
}

int svlFilterImageCenterFinder::Release()
{
    if (MaskImage) {
        delete MaskImage;
        MaskImage = 0;
    }
    if (TransitionImage) {
        delete TransitionImage;
        TransitionImage = 0;
    }
    return SVL_OK;
}

bool svlFilterImageCenterFinder::FindEllipse(svlSampleImage* image, unsigned int videoch, const int cx, const int cy, svlEllipse & ellipse)
{
    const int width  = image->GetWidth(videoch);
    const int height = image->GetHeight(videoch);
    const int width_half = width / 2;

    if (cx < 0 || cx >= width || cy < 0 || cy >= height) return false;

    vctDynamicMatrixRef<int> profiles(RadialProfiles[videoch]);
    vctDynamicVectorRef<int> temp_profiles(TempProfile[videoch]);
    vctDynamicVectorRef<int> step_x(StepX[videoch]);
    vctDynamicVectorRef<int> step_y(StepY[videoch]);
    vctDynamicVectorRef<int> edge_x(EdgeX[videoch]);
    vctDynamicVectorRef<int> edge_y(EdgeY[videoch]);
    vctDynamicVectorRef<int> edge_rad(EdgeRad[videoch]);
    vctDynamicVectorRef<int> edge_rad_smooth(EdgeRadSmooth[videoch]);

    const int error_threshold = EllipseFittingErrorThreshold * height / 100;
    const int kernel_radius   = height / 80;
    const double anglestep    = (3.14159265358979 * 2) / static_cast<double>(EllipseFittingSlices);
    unsigned char* img_buf = image->GetUCharPointer(videoch);
    int i, point_count;
    double a;

    for (i = 0, a = 0.0; i < EllipseFittingSlices; i ++, a += anglestep) {
        step_x[i] = static_cast<int>(sin(a) * 1024.0);
        step_y[i] = static_cast<int>(cos(a) * 1024.0);
    }

    unsigned char* tptr;
    int x, y, c, r, k, l, acc, fr, to, value;

    // Generate radial profiles
    for (i = 0; i < EllipseFittingSlices; i ++) {
        for (r = 0; r < width_half; r ++) {

            x = cx + ((step_x[i] * r) >> 10);
            y = cy + ((step_y[i] * r) >> 10);

            if (x >= 0 && x < width && y >= 0 && y < height) {
                tptr = img_buf + (y * width + x) * 3;
                profiles.Element(i, r) = tptr[0] + tptr[1] + tptr[2];
            }
            else {
                profiles.Element(i, r) = -1;
            }
        }
        // Smooth profile
        for (r = 0; r < width_half; r ++) {
            acc = 0;
            c = 0;

            fr = r - kernel_radius;
            to = r + kernel_radius;
            if (fr < 0) fr = 0;
            if (to >= width_half) to = width_half - 1;

            for (k = fr; k <= to; k ++) {
                value = profiles.Element(i, k);
                if (value >= 0) {
                    acc += value;
                    c ++;
                }
            }
            if (c > 0) temp_profiles[r] = acc / c;
            else temp_profiles[r] = -1;
        }
        // Copy smoothed vector back to profile
        for (r = 0; r < width_half; r ++) {
            if (profiles.Element(i, r) >= 0) {
                profiles.Element(i, r) = temp_profiles[r];
            }
        }
    }

    // Find location of bright to dark transition
    if (EllipseFittingMode == 0) {
        // Find area that is darker than a threshold

        for (i = 0; i < EllipseFittingSlices; i ++) {
            for (r = 0; r < width_half - 1; r ++) {

                value = profiles.Element(i, r);
                if (value < EllipseFittingEdgeThreshold) {
                    if (profiles.Element(i, r + 1) >= 0) {
                        // This edge of ellipse is fully in bounds of image
                        // Include this edge in ellipse fitting
                        x = cx + ((step_x[i] * r) >> 10);
                        y = cy + ((step_y[i] * r) >> 10);

                        edge_x[i] = x;
                        edge_y[i] = y;
                        x -= cx;
                        y -= cy;
                        edge_rad[i] = sqrt_uint32(x * x + y * y);
                    }
                    else {
                        // This edge of ellipse is out of bounds
                        // Don't include this profile in ellipse fitting
                        edge_rad[i] = -1;
                    }
                    break;
                }
            }
            if (r >= (width_half - 1)) {
                edge_rad[i] = -1;
            }
        }
    }
    else if (EllipseFittingMode == 1) {
        // Find fast gradient change

        for (i = 0; i < EllipseFittingSlices; i ++) {
            for (r = width_half - 11; r >= 0; r --) {

                value = profiles.Element(i, r) - profiles.Element(i, r + 10);
                if (value > EllipseFittingEdgeThreshold) {
                    if (profiles.Element(i, r + 10) >= 0) {
                        // This edge of ellipse is fully in bounds of image
                        // Include this edge in ellipse fitting
                        x = cx + ((step_x[i] * r) >> 10);
                        y = cy + ((step_y[i] * r) >> 10);

                        edge_x[i] = x;
                        edge_y[i] = y;
                        x -= cx;
                        y -= cy;
                        edge_rad[i] = sqrt_uint32(x * x + y * y);
                    }
                    else {
                        // This edge of ellipse is out of bounds
                        // Don't include this profile in ellipse fitting
                        edge_rad[i] = -1;
                    }
                    break;
                }
            }
            if (r < 0) {
                edge_rad[i] = -1;
            }
        }
    }
    else {
        // Nothing here yet
    }

    // Eliminate outliers

    int err; //, errpos, maxerr;

//    std::cerr << "***" << std::endl;
//    while (1) {
//        errpos = -1;
//        maxerr = 0;

        edge_rad_smooth.SetAll(-1);

        for (i = 0; i < EllipseFittingSlices; i ++) {
            if (edge_rad[i] < 0) continue;

            acc = 0;
            c = 0;

            for (k = -7; k <= 7; k ++) {
                l = i + k;
                if (l < 0) l = EllipseFittingSlices + value;
                else if (l >= EllipseFittingSlices) l -= EllipseFittingSlices;
                if (edge_rad[l] >= 0) {
                    acc += edge_rad[l];
                    c ++;
                }
            }
            if (c <= 0) continue;

            err = (acc / c) - edge_rad[i];
            if (err < 0) err = -err;

            edge_rad_smooth[i] = err;
/*
            if (err > maxerr) {
                errpos = i;
                maxerr = err;
            }
*/
        }
        for (i = 0; i < EllipseFittingSlices; i ++) {
            if (edge_rad_smooth[i] > error_threshold) {
                // Point is an outlier
                edge_rad[i] = -1;
            }
        }
/*
        if (maxerr > error_threshold) {
            // Point is an outlier
            edge_rad[errpos] = -1;
//            std::cerr << "Removing: " << errpos << "(" << maxerr << ")" << std::endl;
        }
        else {
            break;
        }

        break;
    }
*/
    // Consolidate point cloud

    point_count = EllipseFittingSlices;
    for (i = 0; i < point_count; i ++) {
        if (edge_rad[i] < 0) {
            // Remove point
            for (k = i + 1; k < point_count; k ++) {
                edge_x[k - 1]     = edge_x[k];
                edge_y[k - 1]     = edge_y[k];
                edge_rad[k - 1]   = edge_rad[k];
            }
            point_count --;
            i --;
        }
    }
/*
    for (i = 0; i < point_count; i ++) {
        tptr = img_buf + (edge_y[i] * width + edge_x[i]) * 3;
        tptr[0] = 255; tptr[1] = 0; tptr[2] = 255;
    }
*/
    // Fit ellipse on point cloud

    if (point_count < 6) return false;

    vctDynamicVectorRef<int> ex(edge_x, 0, point_count);
    vctDynamicVectorRef<int> ey(edge_y, 0, point_count);
    if (svlImageProcessing::FitEllipse(ex, ey, ellipse, EllipseFittingInternals[videoch]) != SVL_OK) return false;

    return true;
}

void svlFilterImageCenterFinder::CreateTransitionImage()
{
    const unsigned int width = TransitionImage->GetWidth();
    const unsigned int height = TransitionImage->GetHeight();
    unsigned char* buf = TransitionImage->GetUCharPointer();
    unsigned char value;

    for (unsigned int i = 0; i < height; i ++) {
        value = static_cast<unsigned char>(127.5 * (1.0 + cos(3.14159265358979 * i / (height - 1))));
        memset(buf, value, width);
        buf += width;
    }
}

void svlFilterImageCenterFinder::UpdateMaskImage(unsigned int videoch, svlEllipse & ellipse)
{
    if (ellipse.cx < 0 || ellipse.cx >= static_cast<int>(MaskImage->GetWidth(videoch))  ||
        ellipse.cy < 0 || ellipse.cy >= static_cast<int>(MaskImage->GetHeight(videoch)) ||
        ellipse.rx < 1 || ellipse.rx > 1000 ||
        ellipse.ry < 1 || ellipse.ry > 1000) return;

    const double anglestep = (3.14159265358979 * 2) / static_cast<double>(EllipseMaskSlices);
    const double sr = sin(ellipse.angle);
    const double cr = cos(ellipse.angle);
    svlQuad quad_in, quad_out;
    double a, ca, sa;
    int i, j;

    EllipseSamplesIn[videoch].SetSize(EllipseMaskSlices);
    EllipseSamplesOut[videoch].SetSize(EllipseMaskSlices);
    vctDynamicVectorRef<vctInt2> samples_in(EllipseSamplesIn[videoch]);
    vctDynamicVectorRef<vctInt2> samples_out(EllipseSamplesOut[videoch]);

    // Initialize mask
    memset(MaskImage->GetUCharPointer(videoch), 255, MaskImage->GetDataSize(videoch));

    // Calculate slice positions
    for (i = 0, a = 0.0; i < EllipseMaskSlices; i ++, a += anglestep) {
        sa = sin(a); ca = cos(a);

        samples_in[i].X() = ellipse.cx +
                            static_cast<int>(ca * cr * (ellipse.rx + EllipseMaskTransitionStart) -
                                             sa * sr * (ellipse.ry + EllipseMaskTransitionStart));
        samples_in[i].Y() = ellipse.cy +
                            static_cast<int>(ca * sr * (ellipse.rx + EllipseMaskTransitionStart) +
                                             sa * cr * (ellipse.ry + EllipseMaskTransitionStart));

        if (EllipseMaskTransitionStart < EllipseMaskTransitionEnd) {
            samples_out[i].X() = ellipse.cx +
                                 static_cast<int>(ca * cr * (ellipse.rx + EllipseMaskTransitionEnd) -
                                                  sa * sr * (ellipse.ry + EllipseMaskTransitionEnd));
            samples_out[i].Y() = ellipse.cy +
                                 static_cast<int>(ca * sr * (ellipse.rx + EllipseMaskTransitionEnd) +
                                                  sa * cr * (ellipse.ry + EllipseMaskTransitionEnd));
        }
    }

    // Draw solid inner disc
    for (i = 0; i < EllipseMaskSlices; i ++) {
        if (i > 0) j = i - 1;
        else j = EllipseMaskSlices - 1;

        svlDraw::Triangle(MaskImage, videoch,
                          ellipse.cx, ellipse.cy,
                          (samples_in[j].X() + samples_out[j].X()) >> 1, (samples_in[j].Y() + samples_out[j].Y()) >> 1,
                          (samples_in[i].X() + samples_out[i].X()) >> 1, (samples_in[i].Y() + samples_out[i].Y()) >> 1,
                          0,
                          TriangleInternals[videoch]);
    }

    // Draw smooth transition
    if (EllipseMaskTransitionStart < EllipseMaskTransitionEnd) {
        for (i = 0; i < EllipseMaskSlices; i ++) {
            if (i > 0) j = i - 1;
            else j = EllipseMaskSlices - 1;

            quad_in.Assign (0, 0,
                            TransitionImage->GetWidth() - 1, 0,
                            TransitionImage->GetWidth() - 1, TransitionImage->GetHeight() - 1,
                            0, TransitionImage->GetHeight() - 1);
            quad_out.Assign(samples_out[j].X(), samples_out[j].Y(),
                            samples_out[i].X(), samples_out[i].Y(),
                            samples_in[i].X(),  samples_in[i].Y(),
                            samples_in[j].X(),  samples_in[j].Y());
            svlDraw::WarpQuad(TransitionImage, 0, quad_in, MaskImage, videoch, quad_out, WarpInternals[videoch]);
        }
    }
}

unsigned int svlFilterImageCenterFinder::sqrt_uint32(unsigned int value)
{
    unsigned int a, g = 0;
    unsigned int bshft = 15;
    unsigned int b = 1 << bshft;

    do {
        a = (g + g + b) << bshft;
        if (value >= a) {
            g += b;
            value -= a;
        }
        b >>= 1;
    } while (bshft --);

    return g;
}

