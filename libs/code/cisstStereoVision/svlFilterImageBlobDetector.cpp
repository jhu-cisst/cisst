/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
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

#include <cisstStereoVision/svlFilterImageBlobDetector.h>
#include <cisstStereoVision/svlFilterInput.h>

#define FILL_BUFFER_SIZE    100000


/****************************************/
/*** svlFilterImageBlobDetector class ***/
/****************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageBlobDetector)

svlFilterImageBlobDetector::svlFilterImageBlobDetector() :
    svlFilterBase(),
    OutputBlobIDs(0),
    OutputBlobs(0),
    MaxBlobCount(1000),
    FiltMinArea(0),
    FiltMaxArea(0),
    FiltMinCompactness(0.0),
    FiltMaxCompactness(0.0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageMono8Stereo);

    AddOutput("blobsmap", true);
    AddOutput("blobs", false);
    SetOutputType("blobs", svlTypeBlobs);
}

svlFilterImageBlobDetector::~svlFilterImageBlobDetector()
{
    delete OutputBlobIDs;
    delete OutputBlobs;
}

int svlFilterImageBlobDetector::SetMaxBlobCount(unsigned int max_blobs)
{
    if (IsInitialized()) return SVL_FAIL;
    MaxBlobCount = max_blobs;
    return SVL_OK;
}

void svlFilterImageBlobDetector::SetFilterArea(unsigned int min_area, unsigned int max_area)
{
    FiltMinArea = min_area;
    FiltMaxArea = max_area;
}

void svlFilterImageBlobDetector::SetFilterCompactness(double min_compactness, double max_compactness)
{
    FiltMinCompactness = min_compactness;
    FiltMaxCompactness = max_compactness;
}

int svlFilterImageBlobDetector::OnConnectInput(svlFilterInput &input, svlStreamType type)
{
    // Check if type is on the supported list
    if (!input.IsTypeSupported(type)) return SVL_FAIL;

    svlStreamType output_type;
    unsigned int channelcount = 0;

    if (type == svlTypeImageMono8) {
        output_type = svlTypeImageMono32;
        channelcount = 1;
    }
    else if (type == svlTypeImageMono8Stereo) {
        output_type = svlTypeImageMono32Stereo;
        channelcount = 2;
    }
    else return SVL_FAIL;

    SetOutputType("blobsmap", output_type);

    delete OutputBlobIDs;
    OutputBlobIDs = dynamic_cast<svlSampleImage*>(svlSample::GetNewFromType(output_type));

    delete OutputBlobs;
    OutputBlobs = new svlSampleBlobs;
    OutputBlobs->SetChannelCount(channelcount);

    FillBuffer.SetSize(channelcount * 4, FILL_BUFFER_SIZE);

    return SVL_OK;
}

int svlFilterImageBlobDetector::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    OutputBlobIDs->SetSize(syncInput);
    syncOutput = OutputBlobIDs;

    BlobsOutputConnected = GetOutput("blobs")->IsConnected();
    if (BlobsOutputConnected) {
        for (unsigned int i = 0; i < OutputBlobs->GetChannelCount(); i ++) {
            OutputBlobs->SetBufferSize(MaxBlobCount, i);
        }
    }

    return SVL_OK;
}

int svlFilterImageBlobDetector::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputBlobIDs;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx, maxblobid;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        maxblobid = ComputeBlobIDs(img, idx);

        if (BlobsOutputConnected) {
            ComputeBlobStats(maxblobid, img, idx);
        }
    }

    if (BlobsOutputConnected)
    {
        _SynchronizeThreads(procInfo);
        _OnSingleThread(procInfo)
        {
            GetOutput("blobs")->PushSample(OutputBlobs);
        }
    }

    return SVL_OK;
}

unsigned int svlFilterImageBlobDetector::ComputeBlobIDs(svlSampleImage* input, unsigned int videoch)
{
    int *fx  = FillBuffer.Pointer() + videoch * FILL_BUFFER_SIZE * 4;
    int *fy  = fx + FILL_BUFFER_SIZE;
    int *fnx = fy + FILL_BUFFER_SIZE;
    int *fny = fnx + FILL_BUFFER_SIZE;
    unsigned int fu, fnu;

    unsigned int *blobids = reinterpret_cast<unsigned int*>(OutputBlobIDs->GetUCharPointer(videoch));
    memset(blobids, 0, OutputBlobIDs->GetDataSize(videoch));

    unsigned char *image = input->GetUCharPointer(videoch);
    const int width  = static_cast<int>(input->GetWidth(videoch));
    const int height = static_cast<int>(input->GetHeight(videoch));
    const int width_m1  = width - 1;
    const int height_m1 = height - 1;

    const unsigned int FBSm1  = FILL_BUFFER_SIZE - 1;
    unsigned int k, off, off2, imgval, c = 0, maxblobid = 1;
    int i, j, x, y;
    int *tptr;


    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {
            imgval = image[c];

            if (imgval == 0 || blobids[c] != 0) {
                c ++;
                continue;
            }

            // Fill current pixel
            *fnx = i; *fny = j; fnu = 1;
            blobids[c] = maxblobid;

            while (fnu) {
                // Swap fill buffers
                tptr = fx; fx = fnx; fnx = tptr;
                tptr = fy; fy = fny; fny = tptr;
                fu = fnu; fnu = 0;

                for (k = 0; k < fu; k ++) {
                    x = fx[k]; y = fy[k];
                    off = y * width + x;

                    // Check left neighbor
                    if (x > 0) {
                        off2 = off - 1;
                        if (blobids[off2] == 0 && image[off2] == imgval && fnu < FBSm1) {
                            // Fill pixel
                            fnx[fnu] = x - 1; fny[fnu] = y; fnu ++;
                            blobids[off2] = maxblobid;
                        }
                    }

                    // Check right neighbor
                    if (x < width_m1) {
                        off2 = off + 1;
                        if (blobids[off2] == 0 && image[off2] == imgval && fnu < FBSm1) {
                            // Fill pixel
                            fnx[fnu] = x + 1; fny[fnu] = y; fnu ++;
                            blobids[off2] = maxblobid;
                        }
                    }

                    // Check top neighbor
                    if (y > 0) {
                        off2 = off - width;
                        if (blobids[off2] == 0 && image[off2] == imgval && fnu < FBSm1) {
                            // Fill pixel
                            fnx[fnu] = x; fny[fnu] = y - 1; fnu ++;
                            blobids[off2] = maxblobid;
                        }
                    }

                    // Check bottom neighbor
                    if (y < height_m1) {
                        off2 = off + width;
                        if (blobids[off2] == 0 && image[off2] == imgval && fnu < FBSm1) {
                            // Fill pixel
                            fnx[fnu] = x; fny[fnu] = y + 1; fnu ++;
                            blobids[off2] = maxblobid;
                        }
                    }
                }
            }

            maxblobid ++;
            c ++;
        }
    }

    return maxblobid - 1;
}

void svlFilterImageBlobDetector::ComputeBlobStats(unsigned int maxblobid, svlSampleImage* input, unsigned int videoch)
{
    if (maxblobid > MaxBlobCount) maxblobid = MaxBlobCount;

    unsigned int *blobids = reinterpret_cast<unsigned int*>(OutputBlobIDs->GetUCharPointer(videoch));
    unsigned char *image = input->GetUCharPointer(videoch);
    svlBlob *blobs = OutputBlobs->GetBlobsPointer(videoch);
    const int width  = static_cast<int>(input->GetWidth(videoch));
    const int height = static_cast<int>(input->GetHeight(videoch));
    const int width_m1  = width - 1;
    const int height_m1 = height - 1;

    bool do_filtering = false;
    double compactness, db_area, db_circumference;
    unsigned int k;
    svlBlob *blob;
    int i, j;


    blob = blobs;
    for (k = 0; k < maxblobid; k ++) {
        blob->id            = k + 1;
        blob->used          = true;
        blob->left          = 100000;
        blob->right         = -1;
        blob->top           = 100000;
        blob->bottom        = -1;
        blob->center_x      = 0;
        blob->center_y      = 0;
        blob->area          = 0;
        blob->circumference = 0;
        blob->label         = 0;
        blob ++;
    }

    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {

            k = *blobids;
            if (k > 0 && k <= MaxBlobCount) {

                blob = blobs + k - 1;

                // Bounding rectangle
                if (i < blob->left) blob->left = i;
                if (i > blob->right) blob->right = i;
                if (j < blob->top) blob->top = j;
                if (j > blob->bottom) blob->bottom = j;

                // Center of weight
                blob->center_x += i;
                blob->center_y += j;

                // Area
                blob->area ++;

                // Circumference
                if ((i > 0         && blobids[    -1] != k) ||
                    (i < width_m1  && blobids[     1] != k) ||
                    (j > 0         && blobids[-width] != k) ||
                    (j < height_m1 && blobids[ width] != k)) {
                    blob->circumference ++;
                }

                // Label
                if (blob->label == 0) blob->label = *image;
            }

            image ++;
            blobids ++;
        }
    }

    blob = blobs;
    for (k = 0; k < maxblobid; k ++) {
        blob->center_x /= blob->area;
        blob->center_y /= blob->area;
        blob ++;
    }

    if (FiltMinArea > 0 || FiltMaxArea > 0) {
        do_filtering = true;

        blob = blobs;
        for (k = 0; k < maxblobid; k ++) {
            if ((FiltMaxArea && blob->area > FiltMaxArea) ||
                                blob->area < FiltMinArea) {
                blob->used = false;
            }
            blob ++;
        }
    }

    if (FiltMinCompactness > 0.0 || FiltMaxCompactness > 0.0) {
        do_filtering = true;

        blob = blobs;
        for (k = 0; k < maxblobid; k ++) {

            // r=sqrt(area/pi)
            // compactness=(area/circumference)/r
            // compactness=1.0 (if prefect circular disk)
            // compactness<1.0 (if any other shape)

            db_area          = static_cast<double>(blob->area);
            db_circumference = static_cast<double>(blob->circumference);
            compactness = std::min(1.0, db_area / (db_circumference * std::sqrt(db_area * 0.318309886183791)));

            if ((FiltMinCompactness > 0.0 && compactness < FiltMinCompactness) ||
                (FiltMaxCompactness > 0.0 && compactness > FiltMaxCompactness)) {
                blob->used = false;
            }
            blob ++;
        }
    }

    if (do_filtering) {
        blobids = reinterpret_cast<unsigned int*>(OutputBlobIDs->GetUCharPointer(videoch));

        for (j = 0; j < height; j ++) {
            for (i = 0; i < width; i ++) {

                k = *blobids;
                if (k > 0 && k <= MaxBlobCount) {
                    if (blobs[k - 1].used == false) *blobids = 0;
                }

                blobids ++;
            }
        }
    }

    OutputBlobs->SetBufferUsed(maxblobid, videoch);
}

