/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 $Id$
 
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

#include "svlDrawHelper.h"


/******************************/
/*** svlDrawInternals class ***/
/******************************/

svlDrawInternals::svlDrawInternals()
{
    // Called only by derived class
}

svlDrawInternals::svlDrawInternals(const svlDrawInternals& CMN_UNUSED(internals))
{
    // Never called
}

svlDrawInternals::~svlDrawInternals()
{
    // NOP
}


/*******************************/
/*** svlDrawHelper namespace ***/
/*******************************/


/**********************************************/
/*** svlDrawHelper::TriangleInternals class ***/
/**********************************************/

svlDrawHelper::TriangleInternals::TriangleInternals() :
    svlDrawInternals(),
    Image(0),
    Channel(0),
    Width(0),
    Height(0)
{
}

bool svlDrawHelper::TriangleInternals::SetImage(svlSampleImage* image, unsigned int channel)
{
    if (!image || channel >= image->GetVideoChannels()) return false;

    Image = image;
    Channel = channel;

    const unsigned int w = image->GetWidth(channel);
    const unsigned int h = image->GetHeight(channel);
    Width = static_cast<int>(w);
    Height = static_cast<int>(h);

    // Resize sample buffers if needed
    if (LeftSamples.size() < h) LeftSamples.SetSize(h);
    if (RightSamples.size() < h) RightSamples.SetSize(h);

    return true;
}

void svlDrawHelper::TriangleInternals::SampleLine(vctDynamicVector<int>& samples, int x1, int y1, int x2, int y2)
{
    if (x1 == x2 && y1 == y2) {
        if (y1 >= 0 && y1 < Height) samples[y1] = x1;
        return;
    }

    int x = x1, y = y1;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int eps = 0;

    if (dy > 0) {
        if (dx >= dy) {
            for (x = x1; x <= x2; x ++) {
                if (y >= 0 && y < Height) samples[y] = x;
                eps += dy;
                if ((eps << 1) >= dx) {
                    y ++;
                    eps -= dx;
                }
            }
        }
        else {
            for (y = y1; y <= y2; y ++) {
                if (y >= 0 && y < Height) samples[y] = x;
                eps += dx;
                if ((eps << 1) >= dy) {
                    x ++;
                    eps -= dy;
                }
            }
        }
    }
    else {
        if (dx >= abs(dy)) {
            for (x = x1; x <= x2; x ++) {
                if (y >= 0 && y < Height) samples[y] = x;
                eps += dy;
                if ((eps << 1) <= -dx) {
                    y --;
                    eps += dx;
                }
            }
        }
        else {
            for (y = y1; y >= y2; y --) {
                if (y >= 0 && y < Height) samples[y] = x;
                eps += dx;
                if ((eps << 1) >= -dy) {
                    x ++;
                    eps -= -dy;
                }
            }
        }
    }
}

void svlDrawHelper::TriangleInternals::Draw(int x1, int y1, int x2, int y2, int x3, int y3, const svlRGB color)
{
    if (!Image || Width == 0 || Height == 0) return;

    svlRGB *tdata, *tdata2, *imgdata = reinterpret_cast<svlRGB*>(Image->GetUCharPointer(Channel));
    int i, t, top, bottom, left, right, len;

    // If triangle is a horizontal line
    if (y1 == y2 && y2 == y3) {
        if (y1 < 0 || y1 >= Height) return;

        // If triangle is one pixel
        if (x1 == x2 && x2 == x3 && x1 >= 0 && x1 < Width) {
            imgdata[y1 * Width + x1] = color;
            return;
        }

        // Horizontal ordering of corners
        if (x2 < x1) {
            t = x1; x1 = x2; x2 = t;
        }
        if (x3 < x1) {
            t = x1; x1 = x3; x3 = t;
        }
        if (x3 < x2) {
            t = x3; x3 = x2; x2 = t;
        }

        if (x1 < 0) x1 = 0;
        if (x3 >= Width) x3 = Width - 1;

        len = x3 - x1;
        if (len < 0) return;

        // Draw horizontal line
        tdata = imgdata + (y1 * Width + x1);
        while (len >= 0) {
            *tdata = color; tdata ++; len --;
        }
        return;
    }

    // If triangle is a vertical line
    if (x1 == x2 && x2 == x3) {
        if (x1 < 0 || x1 >= Width) return;

        // Vertical ordering of corners
        if (y2 < y1) {
            t = y1; y1 = y2; y2 = t;
        }
        if (y3 < y1) {
            t = y1; y1 = y3; y3 = t;
        }
        if (y3 < y2) {
            t = y3; y3 = y2; y2 = t;
        }

        if (y1 < 0) y1 = 0;
        if (y3 >= Height) y3 = Height - 1;

        len = y3 - y1;
        if (len < 0) return;

        // Draw vertical line
        tdata = imgdata + (y1 * Width + x1);
        while (len >= 0) {
            *tdata = color; tdata += Width; len --;
        }
        return;
    }

    // Horizontal ordering of corners
    if (x2 < x1) {
        t = x1; x1 = x2; x2 = t;
        t = y1; y1 = y2; y2 = t;
    }
    if (x3 < x1) {
        t = x1; x1 = x3; x3 = t;
        t = y1; y1 = y3; y3 = t;
    }
    if (x3 < x2) {
        t = x3; x3 = x2; x2 = t;
        t = y3; y3 = y2; y2 = t;
    }

    int dx2 = x2 - x1;
    int dy2 = y2 - y1;
    int dx3 = x3 - x1;
    int dy3 = y3 - y1;

    // Sample left and right border lines of the triangle
    if (dy2 == dy3) {
        SampleLine(LeftSamples, x1, y1, x2, y2);
        SampleLine(RightSamples, x1, y1, x3, y3);
    }
    else {
        if (dy2 < 0) {
            if (dy3 > 0) {
                SampleLine(LeftSamples, x1, y1, x2, y2);
                SampleLine(LeftSamples, x1, y1, x3, y3);
                SampleLine(RightSamples, x2, y2, x3, y3);
            }
            else {
                if ((dy3 * dx2) < (dy2 * dx3)) {
                    SampleLine(RightSamples, x1, y1, x2, y2);
                    SampleLine(LeftSamples, x1, y1, x3, y3);
                    if (dy3 < dy2) {
                        SampleLine(RightSamples, x2, y2, x3, y3);
                    }
                    else {
                        SampleLine(LeftSamples, x2, y2, x3, y3);
                    }
                }
                else {
                    SampleLine(LeftSamples, x1, y1, x2, y2);
                    SampleLine(RightSamples, x1, y1, x3, y3);
                    if (dy2 < dy3) {
                        SampleLine(RightSamples, x2, y2, x3, y3);
                    }
                    else {
                        SampleLine(LeftSamples, x2, y2, x3, y3);
                    }
                }
            }
        }
        else {
            if (dy3 < 0) {
                SampleLine(LeftSamples, x1, y1, x2, y2);
                SampleLine(LeftSamples, x1, y1, x3, y3);
                SampleLine(RightSamples, x2, y2, x3, y3);
            }
            else {
                if ((dy3 * dx2) > (dy2 * dx3)) {
                    SampleLine(RightSamples, x1, y1, x2, y2);
                    SampleLine(LeftSamples, x1, y1, x3, y3);
                    if (dy3 > dy2) {
                        SampleLine(RightSamples, x2, y2, x3, y3);
                    }
                    else {
                        SampleLine(LeftSamples, x2, y2, x3, y3);
                    }
                }
                else {
                    SampleLine(LeftSamples, x1, y1, x2, y2);
                    SampleLine(RightSamples, x1, y1, x3, y3);
                    if (dy2 > dy3) {
                        SampleLine(RightSamples, x2, y2, x3, y3);
                    }
                    else {
                        SampleLine(LeftSamples, x2, y2, x3, y3);
                    }
                }
            }
        }
    }

    // Get top and bottom
    if (y1 < y2) {
        if (y1 < y3) {
            top = y1;
            if (y2 < y3) bottom = y3;
            else bottom = y2;
        }
        else {
            top = y3;
            bottom = y2;
        }
    }
    else {
        if (y2 < y3) {
            top = y2;
            if (y1 < y3) bottom = y3;
            else bottom = y1;
        }
        else {
            top = y3;
            bottom = y1;
        }
    }
    if (top < 0) top = 0;
    if (bottom >= Height) bottom = Height - 1;

    // Fill triangle
    tdata = imgdata + (top * Width);
    for (i = top; i <= bottom; i ++) {

        // Find left and right end points
        left = LeftSamples[i];
        right = RightSamples[i];
        if (left < 0) left = 0;
        if (right >= Width) right = Width - 1;

        // Set image pointer
        tdata2 = tdata + left; tdata += Width;

        // Draw horizontal line
        len = right - left;
        while (len >= 0) {
            *tdata2 = color; tdata2 ++; len --;
        }
    }
}

