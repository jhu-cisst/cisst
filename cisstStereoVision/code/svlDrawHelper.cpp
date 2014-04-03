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

#include "svlDrawHelper.h"

#define __LARGE_NUMBER   100000000
#define __SMALL_NUMBER  -100000000


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


/*******************************************/
/*** svlDrawHelper::ShapeInternals class ***/
/*******************************************/

svlDrawHelper::ShapeInternals::ShapeInternals() :
    svlDrawInternals(),
    Image(0),
    Channel(0),
    Width(0),
    Height(0)
{
}

bool svlDrawHelper::ShapeInternals::SetImage(svlSampleImage* image, unsigned int channel)
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

void svlDrawHelper::ShapeInternals::SampleLine(vctDynamicVector<int>& samples, int x1, int y1, int x2, int y2)
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

void svlDrawHelper::ShapeInternals::SampleLine(int x1, int y1, int x2, int y2)
{
    if (x1 == x2 && y1 == y2) {
        if (y1 >= 0 && y1 < Height) {
            if (LeftSamples[y1] == __SMALL_NUMBER) {
                LeftSamples[y1] = RightSamples[y1] = x1;
            }
            else {
                if (x1 < LeftSamples[y1])  LeftSamples[y1]  = x1;
                if (x1 > RightSamples[y1]) RightSamples[y1] = x1;
            }
        }
        return;
    }

    int x = x1, y = y1;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int eps = 0;

    if (dy > 0) {
        if (dx >= dy) {
            for (x = x1; x <= x2; x ++) {
                if (y >= 0 && y < Height) {
                    if (LeftSamples[y] == __SMALL_NUMBER) {
                        LeftSamples[y] = RightSamples[y] = x;
                    }
                    else {
                        if (x < LeftSamples[y])  LeftSamples[y]  = x;
                        if (x > RightSamples[y]) RightSamples[y] = x;
                    }
                }
                eps += dy;
                if ((eps << 1) >= dx) {
                    y ++;
                    eps -= dx;
                }
            }
        }
        else {
            for (y = y1; y <= y2; y ++) {
                if (y >= 0 && y < Height) {
                    if (LeftSamples[y] == __SMALL_NUMBER) {
                        LeftSamples[y] = RightSamples[y] = x;
                    }
                    else {
                        if (x < LeftSamples[y])  LeftSamples[y]  = x;
                        if (x > RightSamples[y]) RightSamples[y] = x;
                    }
                }
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
                if (y >= 0 && y < Height) {
                    if (LeftSamples[y] == __SMALL_NUMBER) {
                        LeftSamples[y] = RightSamples[y] = x;
                    }
                    else {
                        if (x < LeftSamples[y])  LeftSamples[y]  = x;
                        if (x > RightSamples[y]) RightSamples[y] = x;
                    }
                }
                eps += dy;
                if ((eps << 1) <= -dx) {
                    y --;
                    eps += dx;
                }
            }
        }
        else {
            for (y = y1; y >= y2; y --) {
                if (y >= 0 && y < Height) {
                    if (LeftSamples[y] == __SMALL_NUMBER) {
                        LeftSamples[y] = RightSamples[y] = x;
                    }
                    else {
                        if (x < LeftSamples[y])  LeftSamples[y]  = x;
                        if (x > RightSamples[y]) RightSamples[y] = x;
                    }
                }
                eps += dx;
                if ((eps << 1) >= -dy) {
                    x ++;
                    eps -= -dy;
                }
            }
        }
    }
}

void svlDrawHelper::ShapeInternals::DrawMono8(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char value)
{
    if (!Image || Width == 0 || Height == 0) return;

    unsigned char *tdata, *tdata2, *imgdata = Image->GetUCharPointer(Channel);
    int i, t, top, bottom, left, right, len;

    // If triangle is a horizontal line
    if (y1 == y2 && y2 == y3) {
        if (y1 < 0 || y1 >= Height) return;

        // If triangle is one pixel
        if (x1 == x2 && x2 == x3 && x1 >= 0 && x1 < Width) {
            imgdata[y1 * Width + x1] = value;
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
            *tdata = value; tdata ++; len --;
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
            *tdata = value; tdata += Width; len --;
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
            *tdata2 = value; tdata2 ++; len --;
        }
    }
}

void svlDrawHelper::ShapeInternals::DrawRGB(int x1, int y1, int x2, int y2, int x3, int y3, svlRGB color)
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

void svlDrawHelper::ShapeInternals::DrawMono8(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned char value)
{
    if (!Image || Width == 0 || Height == 0) return;

    unsigned char *tdata, *tdata2, *imgdata = Image->GetUCharPointer(Channel);
    int i, top, bottom, left, right, len;

    // If quad is a horizontal line
    if (y1 == y2 && y2 == y3 && y3 == y4) {
        if (y1 < 0 || y1 >= Height) return;

        // If quad is one pixel
        if (x1 == x2 && x2 == x3 && x3 == x4 && x1 >= 0 && x1 < Width) {
            imgdata[y1 * Width + x1] = value;
            return;
        }

        // Find leftmost corner
        left = x1;
        if (x2 < left) left = x2;
        if (x3 < left) left = x3;
        if (x4 < left) left = x4;
        // Find rightmost corner
        right = x1;
        if (x2 > right) right = x2;
        if (x3 > right) right = x3;
        if (x4 > right) right = x4;

        if (left < 0) left = 0;
        if (right >= Width) right = Width - 1;

        len = right - left;
        if (len < 0) return;

        // Draw horizontal line
        tdata = imgdata + (y1 * Width + left);
        while (len >= 0) {
            *tdata = value; tdata ++; len --;
        }
        return;
    }

    // Find topmost corner
    top = y1;
    if (y2 < top) top = y2;
    if (y3 < top) top = y3;
    if (y4 < top) top = y4;
    // Find bottommost corner
    bottom = y1;
    if (y2 > bottom) bottom = y2;
    if (y3 > bottom) bottom = y3;
    if (y4 > bottom) bottom = y4;

    // If quad is a vertical line
    if (x1 == x2 && x2 == x3 && x3 == x4) {
        if (x1 < 0 || x1 >= Width) return;

        if (top < 0) top = 0;
        if (bottom >= Height) bottom = Height - 1;

        len = bottom - top;
        if (len < 0) return;

        // Draw vertical line
        tdata = imgdata + (top * Width + x1);
        while (len >= 0) {
            *tdata = value; tdata += Width; len --;
        }
        return;
    }

    if (top < 0) top = 0;
    if (bottom >= Height) bottom = Height - 1;

    // Initialize line sampling table
    for (i = top; i <= bottom; i ++) {
        LeftSamples[i] = __SMALL_NUMBER;
    }

    if (x1 < x2) SampleLine(x1, y1, x2, y2);
    else SampleLine(x2, y2, x1, y1);
    if (x2 < x3) SampleLine(x2, y2, x3, y3);
    else SampleLine(x3, y3, x2, y2);
    if (x3 < x4) SampleLine(x3, y3, x4, y4);
    else SampleLine(x4, y4, x3, y3);
    if (x4 < x1) SampleLine(x4, y4, x1, y1);
    else SampleLine(x1, y1, x4, y4);

    // Fill quad
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
            *tdata2 = value; tdata2 ++; len --;
        }
    }
}

void svlDrawHelper::ShapeInternals::DrawRGB(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, svlRGB color)
{
    if (!Image || Width == 0 || Height == 0) return;

    svlRGB *tdata, *tdata2, *imgdata = reinterpret_cast<svlRGB*>(Image->GetUCharPointer(Channel));
    int i, top, bottom, left, right, len;

    // If quad is a horizontal line
    if (y1 == y2 && y2 == y3 && y3 == y4) {
        if (y1 < 0 || y1 >= Height) return;

        // If quad is one pixel
        if (x1 == x2 && x2 == x3 && x3 == x4 && x1 >= 0 && x1 < Width) {
            imgdata[y1 * Width + x1] = color;
            return;
        }

        // Find leftmost corner
        left = x1;
        if (x2 < left) left = x2;
        if (x3 < left) left = x3;
        if (x4 < left) left = x4;
        // Find rightmost corner
        right = x1;
        if (x2 > right) right = x2;
        if (x3 > right) right = x3;
        if (x4 > right) right = x4;

        if (left < 0) left = 0;
        if (right >= Width) right = Width - 1;

        len = right - left;
        if (len < 0) return;

        // Draw horizontal line
        tdata = imgdata + (y1 * Width + left);
        while (len >= 0) {
            *tdata = color; tdata ++; len --;
        }
        return;
    }

    // Find topmost corner
    top = y1;
    if (y2 < top) top = y2;
    if (y3 < top) top = y3;
    if (y4 < top) top = y4;
    // Find bottommost corner
    bottom = y1;
    if (y2 > bottom) bottom = y2;
    if (y3 > bottom) bottom = y3;
    if (y4 > bottom) bottom = y4;

    // If quad is a vertical line
    if (x1 == x2 && x2 == x3 && x3 == x4) {
        if (x1 < 0 || x1 >= Width) return;

        if (top < 0) top = 0;
        if (bottom >= Height) bottom = Height - 1;

        len = bottom - top;
        if (len < 0) return;

        // Draw vertical line
        tdata = imgdata + (top * Width + x1);
        while (len >= 0) {
            *tdata = color; tdata += Width; len --;
        }
        return;
    }

    if (top < 0) top = 0;
    if (bottom >= Height) bottom = Height - 1;

    // Initialize line sampling table
    for (i = top; i <= bottom; i ++) {
        LeftSamples[i] = __SMALL_NUMBER;
    }

    if (x1 < x2) SampleLine(x1, y1, x2, y2);
    else SampleLine(x2, y2, x1, y1);
    if (x2 < x3) SampleLine(x2, y2, x3, y3);
    else SampleLine(x3, y3, x2, y2);
    if (x3 < x4) SampleLine(x3, y3, x4, y4);
    else SampleLine(x4, y4, x3, y3);
    if (x4 < x1) SampleLine(x4, y4, x1, y1);
    else SampleLine(x1, y1, x4, y4);

    // Fill quad
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


/******************************************/
/*** svlDrawHelper::WarpInternals class ***/
/******************************************/

svlDrawHelper::WarpInternals::WarpInternals(unsigned int vertices) :
    svlDrawInternals(),
    Input(0),
    Output(0),
    _ilen(0),
    _olen(0),
    _in_idxs(0),
    _out_idxs(0),
    _lm_x(0),
    _rm_x(0),
    _lm_id(0),
    _rm_id(0),
    _lm_pos(0),
    _rm_pos(0)
{
    AllocateBuffers(vertices, MAX_DIMENSION * 2);
}

svlDrawHelper::WarpInternals::~WarpInternals()
{
    ReleaseBuffers();
}

bool svlDrawHelper::WarpInternals::SetInputImage(svlSampleImage* image, unsigned int channel)
{
    if (!image || channel >= image->GetVideoChannels()) return false;

    InWidth = image->GetWidth(channel);
    InHeight = image->GetHeight(channel);

    if (InWidth < 1 || InHeight < 1) {
        Input = 0;
        return false;
    }

    Input = image->GetUCharPointer(channel);
    InPixelType = image->GetPixelType();

    return true;
}

bool svlDrawHelper::WarpInternals::SetOutputImage(svlSampleImage* image, unsigned int channel)
{
    if (!image || channel >= image->GetVideoChannels()) return false;

    OutWidth = image->GetWidth(channel);
    OutHeight = image->GetHeight(channel);

    if (OutWidth < 1 || OutHeight < 1) {
        Output = 0;
        return false;
    }

    Output = image->GetUCharPointer(channel);
    OutPixelType = image->GetPixelType();

    return true;
}

void svlDrawHelper::WarpInternals::Draw(unsigned int thread_count, unsigned int thread_id,
                                        int ix1, int iy1, int ix2, int iy2, int ix3, int iy3,
                                        int ox1, int oy1, int ox2, int oy2, int ox3, int oy3,
                                        unsigned int alpha)
{
    if (_ixs.size() < 3) return;
    if (!Input || !Output || alpha == 0) return;
    if (InPixelType != svlPixelMono8 &&
        InPixelType != svlPixelRGB &&
        InPixelType != svlPixelRGBA) return;
    if ((InPixelType == svlPixelMono8 || InPixelType == svlPixelRGB) && InPixelType != OutPixelType) return;
    if (InPixelType == svlPixelRGBA && OutPixelType != svlPixelRGB) return;

    ix1 <<= 1; iy1 <<= 1; ix2 <<= 1; iy2 <<= 1; ix3 <<= 1; iy3 <<= 1;
    ox1 <<= 1; oy1 <<= 1; ox2 <<= 1; oy2 <<= 1; ox3 <<= 1; oy3 <<= 1;

    const int out_height_dbl = OutHeight << 1;
    int miny = MIN3(oy1, oy2, oy3);
    int maxy = MAX3(oy1, oy2, oy3);
    if (miny < 0) miny = 0;
    if (maxy >= out_height_dbl) maxy = out_height_dbl - 1;

    int i, j, x, y, len;
    int *xs, *ys;

    // Trace source and destination triangle contours
    if (iy1 <= iy2) {
        _ilen[0] = GetLinePixels(_ixs[0], _iys[0], ix1, iy1, ix2, iy2);
        _olen[0] = GetLinePixels(_oxs[0], _oys[0], ox1, oy1, ox2, oy2);
    }
    else {
        _ilen[0] = GetLinePixels(_ixs[0], _iys[0], ix2, iy2, ix1, iy1);
        _olen[0] = GetLinePixels(_oxs[0], _oys[0], ox2, oy2, ox1, oy1);
    }
    if (iy2 <= iy3) {
        _ilen[1] = GetLinePixels(_ixs[1], _iys[1], ix2, iy2, ix3, iy3);
        _olen[1] = GetLinePixels(_oxs[1], _oys[1], ox2, oy2, ox3, oy3);
    }
    else {
        _ilen[1] = GetLinePixels(_ixs[1], _iys[1], ix3, iy3, ix2, iy2);
        _olen[1] = GetLinePixels(_oxs[1], _oys[1], ox3, oy3, ox2, oy2);
    }
    if (iy3 <= iy1) {
        _ilen[2] = GetLinePixels(_ixs[2], _iys[2], ix3, iy3, ix1, iy1);
        _olen[2] = GetLinePixels(_oxs[2], _oys[2], ox3, oy3, ox1, oy1);
    }
    else {
        _ilen[2] = GetLinePixels(_ixs[2], _iys[2], ix1, iy1, ix3, iy3);
        _olen[2] = GetLinePixels(_oxs[2], _oys[2], ox1, oy1, ox3, oy3);
    }

    for (i = miny; i <= maxy; i ++) {
        _lm_x[i] = __LARGE_NUMBER;
        _rm_x[i] = __SMALL_NUMBER;
    }

    for (j = 0; j < 3; j ++) {
        len = _olen[j];
        xs = _oxs[j];
        ys = _oys[j];

        for (i = 0; i < len; i ++) {
            x = *xs; xs ++;
            y = *ys; ys ++;
            if (y < 0 || y >= out_height_dbl) continue;

            if (x < _lm_x[y]) {
                _lm_x[y] = x;
                _lm_id[y] = j;
                _lm_pos[y] = i;
            }
            if (x > _rm_x[y]) {
                _rm_x[y] = x;
                _rm_id[y] = j;
                _rm_pos[y] = i;
            }
        }
    }

    vctInt3 ratio;
    ratio[0] = (_ilen[0] << 10) / _olen[0];
    ratio[1] = (_ilen[1] << 10) / _olen[1];
    ratio[2] = (_ilen[2] << 10) / _olen[2];

    int id1, id2, pos1, pos2;

    // Calculate range for multithreaded processing
    int full_range = maxy - miny;
    int _to = full_range / thread_count + 1;
    int _from = thread_id * _to;
    _to += _from;
    if (_to > full_range) _to = full_range;
    _from += miny + 1;
    _to += miny + 1;

    for (i = _from; i < _to; i += 2) {
        id1 = _lm_id[i];
        id2 = _rm_id[i];

        pos1 = (_lm_pos[i] * ratio[id1] + 512) >> 10;
        pos2 = (_rm_pos[i] * ratio[id2] + 512) >> 10;
        if (pos1 >= _ilen[id1]) pos1 = _ilen[id1] - 1;
        if (pos2 >= _ilen[id2]) pos2 = _ilen[id2] - 1;

        if (_lm_x[i] != __LARGE_NUMBER &&
            _rm_x[i] != __SMALL_NUMBER) {

            if (InPixelType != svlPixelRGBA) {
                if (alpha == 256) {
                    if (InPixelType == svlPixelMono8) {
                        ResampleLineMono8((_ixs[id1][pos1] + 1) >> 1, (_iys[id1][pos1] + 1) >> 1,
                                          (_ixs[id2][pos2] + 1) >> 1, (_iys[id2][pos2] + 1) >> 1,
                                          (_lm_x[i] + 1) >> 1, (i + 1) >> 1,
                                          (_rm_x[i] + 1) >> 1, (i + 1) >> 1);
                    }
                    else {
                        ResampleLineRGB((_ixs[id1][pos1] + 1) >> 1, (_iys[id1][pos1] + 1) >> 1,
                                        (_ixs[id2][pos2] + 1) >> 1, (_iys[id2][pos2] + 1) >> 1,
                                        (_lm_x[i] + 1) >> 1, (i + 1) >> 1,
                                        (_rm_x[i] + 1) >> 1, (i + 1) >> 1);
                    }
                }
                else {
                    if (InPixelType == svlPixelMono8) {
                        ResampleLineAlphaMono8((_ixs[id1][pos1] + 1) >> 1, (_iys[id1][pos1] + 1) >> 1,
                                               (_ixs[id2][pos2] + 1) >> 1, (_iys[id2][pos2] + 1) >> 1,
                                               (_lm_x[i] + 1) >> 1, (i + 1) >> 1,
                                               (_rm_x[i] + 1) >> 1, (i + 1) >> 1,
                                               alpha);
                    }
                    else if (InPixelType == svlPixelRGB) {
                        ResampleLineAlphaRGB((_ixs[id1][pos1] + 1) >> 1, (_iys[id1][pos1] + 1) >> 1,
                                             (_ixs[id2][pos2] + 1) >> 1, (_iys[id2][pos2] + 1) >> 1,
                                             (_lm_x[i] + 1) >> 1, (i + 1) >> 1,
                                             (_rm_x[i] + 1) >> 1, (i + 1) >> 1,
                                             alpha);
                    }
                }
            }
            else {
                ResampleLineRGBA((_ixs[id1][pos1] + 1) >> 1, (_iys[id1][pos1] + 1) >> 1,
                                 (_ixs[id2][pos2] + 1) >> 1, (_iys[id2][pos2] + 1) >> 1,
                                 (_lm_x[i] + 1) >> 1, (i + 1) >> 1,
                                 (_rm_x[i] + 1) >> 1, (i + 1) >> 1);
            }
        }
    }
}

void svlDrawHelper::WarpInternals::Draw(unsigned int thread_count, unsigned int thread_id,
                                        int ix1, int iy1, int ix2, int iy2, int ix3, int iy3, int ix4, int iy4,
                                        int ox1, int oy1, int ox2, int oy2, int ox3, int oy3, int ox4, int oy4,
                                        unsigned int alpha)
{
    if (_ixs.size() < 4) return;
    if (!Input || !Output || alpha == 0) return;
    if (InPixelType != svlPixelMono8 &&
        InPixelType != svlPixelRGB &&
        InPixelType != svlPixelRGBA) return;
    if ((InPixelType == svlPixelMono8 || InPixelType == svlPixelRGB) && InPixelType != OutPixelType) return;
    if (InPixelType == svlPixelRGBA && OutPixelType != svlPixelRGB) return;

    ix1 <<= 1; iy1 <<= 1; ix2 <<= 1; iy2 <<= 1; ix3 <<= 1; iy3 <<= 1; ix4 <<= 1; iy4 <<= 1;
    ox1 <<= 1; oy1 <<= 1; ox2 <<= 1; oy2 <<= 1; ox3 <<= 1; oy3 <<= 1; ox4 <<= 1; oy4 <<= 1;

    const int out_height_dbl = OutHeight << 1;
    int miny = oy1, maxy = oy1;
    if (oy2 < miny) miny = oy2;
    if (oy3 < miny) miny = oy3;
    if (oy4 < miny) miny = oy4;
    if (oy2 > maxy) maxy = oy2;
    if (oy3 > maxy) maxy = oy3;
    if (oy4 > maxy) maxy = oy4;
    if (miny < 0) miny = 0;
    if (maxy >= out_height_dbl) maxy = out_height_dbl - 1;

    int i, j, x, y, len;
    int *xs, *ys;

    // Trace source and destination quad contours
    if (iy1 <= iy2) {
        _ilen[0] = GetLinePixels(_ixs[0], _iys[0], ix1, iy1, ix2, iy2);
        _olen[0] = GetLinePixels(_oxs[0], _oys[0], ox1, oy1, ox2, oy2);
    }
    else {
        _ilen[0] = GetLinePixels(_ixs[0], _iys[0], ix2, iy2, ix1, iy1);
        _olen[0] = GetLinePixels(_oxs[0], _oys[0], ox2, oy2, ox1, oy1);
    }
    if (iy2 <= iy3) {
        _ilen[1] = GetLinePixels(_ixs[1], _iys[1], ix2, iy2, ix3, iy3);
        _olen[1] = GetLinePixels(_oxs[1], _oys[1], ox2, oy2, ox3, oy3);
    }
    else {
        _ilen[1] = GetLinePixels(_ixs[1], _iys[1], ix3, iy3, ix2, iy2);
        _olen[1] = GetLinePixels(_oxs[1], _oys[1], ox3, oy3, ox2, oy2);
    }
    if (iy3 <= iy4) {
        _ilen[2] = GetLinePixels(_ixs[2], _iys[2], ix3, iy3, ix4, iy4);
        _olen[2] = GetLinePixels(_oxs[2], _oys[2], ox3, oy3, ox4, oy4);
    }
    else {
        _ilen[2] = GetLinePixels(_ixs[2], _iys[2], ix4, iy4, ix3, iy3);
        _olen[2] = GetLinePixels(_oxs[2], _oys[2], ox4, oy4, ox3, oy3);
    }
    if (iy4 <= iy1) {
        _ilen[3] = GetLinePixels(_ixs[3], _iys[3], ix4, iy4, ix1, iy1);
        _olen[3] = GetLinePixels(_oxs[3], _oys[3], ox4, oy4, ox1, oy1);
    }
    else {
        _ilen[3] = GetLinePixels(_ixs[3], _iys[3], ix1, iy1, ix4, iy4);
        _olen[3] = GetLinePixels(_oxs[3], _oys[3], ox1, oy1, ox4, oy4);
    }

    for (i = miny; i <= maxy; i ++) {
        _lm_x[i] = __LARGE_NUMBER;
        _rm_x[i] = __SMALL_NUMBER;
    }

    for (j = 0; j < 4; j ++) {
        len = _olen[j];
        xs = _oxs[j];
        ys = _oys[j];

        for (i = 0; i < len; i ++) {
            x = *xs; xs ++;
            y = *ys; ys ++;
            if (y < 0 || y >= out_height_dbl) continue;

            if (x < _lm_x[y]) {
                _lm_x[y] = x;
                _lm_id[y] = j;
                _lm_pos[y] = i;
            }
            if (x > _rm_x[y]) {
                _rm_x[y] = x;
                _rm_id[y] = j;
                _rm_pos[y] = i;
            }
        }
    }

    vctInt4 ratio;
    ratio[0] = (_ilen[0] << 10) / _olen[0];
    ratio[1] = (_ilen[1] << 10) / _olen[1];
    ratio[2] = (_ilen[2] << 10) / _olen[2];
    ratio[3] = (_ilen[3] << 10) / _olen[3];

    int id1, id2, pos1, pos2;

    // Calculate range for multithreaded processing
    int full_range = maxy - miny;
    int _to = full_range / thread_count + 1;
    int _from = thread_id * _to;
    _to += _from;
    if (_to > full_range) _to = full_range;
    _from += miny + 1;
    _to += miny + 1;

    for (i = _from; i < _to; i += 2) {
        id1 = _lm_id[i];
        id2 = _rm_id[i];

        pos1 = (_lm_pos[i] * ratio[id1] + 512) >> 10;        
        pos2 = (_rm_pos[i] * ratio[id2] + 512) >> 10;
        if (pos1 >= _ilen[id1]) pos1 = _ilen[id1] - 1;
        if (pos2 >= _ilen[id2]) pos2 = _ilen[id2] - 1;

        if (_lm_x[i] != __LARGE_NUMBER &&
            _rm_x[i] != __SMALL_NUMBER) {

            if (InPixelType != svlPixelRGBA) {
                if (alpha == 256) {
                    if (InPixelType == svlPixelMono8) {
                        ResampleLineMono8((_ixs[id1][pos1] + 1) >> 1, (_iys[id1][pos1] + 1) >> 1,
                                          (_ixs[id2][pos2] + 1) >> 1, (_iys[id2][pos2] + 1) >> 1,
                                          (_lm_x[i] + 1) >> 1, (i + 1) >> 1,
                                          (_rm_x[i] + 1) >> 1, (i + 1) >> 1);
                    }
                    else {
                        ResampleLineRGB((_ixs[id1][pos1] + 1) >> 1, (_iys[id1][pos1] + 1) >> 1,
                                        (_ixs[id2][pos2] + 1) >> 1, (_iys[id2][pos2] + 1) >> 1,
                                        (_lm_x[i] + 1) >> 1, (i + 1) >> 1,
                                        (_rm_x[i] + 1) >> 1, (i + 1) >> 1);
                    }
                }
                else {
                    if (InPixelType == svlPixelMono8) {
                        ResampleLineAlphaMono8((_ixs[id1][pos1] + 1) >> 1, (_iys[id1][pos1] + 1) >> 1,
                                               (_ixs[id2][pos2] + 1) >> 1, (_iys[id2][pos2] + 1) >> 1,
                                               (_lm_x[i] + 1) >> 1, (i + 1) >> 1,
                                               (_rm_x[i] + 1) >> 1, (i + 1) >> 1,
                                               alpha);
                    }
                    else {
                        ResampleLineAlphaRGB((_ixs[id1][pos1] + 1) >> 1, (_iys[id1][pos1] + 1) >> 1,
                                             (_ixs[id2][pos2] + 1) >> 1, (_iys[id2][pos2] + 1) >> 1,
                                             (_lm_x[i] + 1) >> 1, (i + 1) >> 1,
                                             (_rm_x[i] + 1) >> 1, (i + 1) >> 1,
                                             alpha);
                    }
                }
            }
            else {
                ResampleLineRGBA((_ixs[id1][pos1] + 1) >> 1, (_iys[id1][pos1] + 1) >> 1,
                                 (_ixs[id2][pos2] + 1) >> 1, (_iys[id2][pos2] + 1) >> 1,
                                 (_lm_x[i] + 1) >> 1, (i + 1) >> 1,
                                 (_rm_x[i] + 1) >> 1, (i + 1) >> 1);
            }
        }
    }
}

int svlDrawHelper::WarpInternals::GetLinePixels(int* xs, int* ys, int x1, int y1, int x2, int y2)
{
    if (x1 == x2 && y1 == y2) {
        *xs = x1; *ys = y1;
        return 1;
    }

    int x = x1, y = y1, dx, dy = y2 - y1, eps = 0, len = 0;

    if (y1 == y2) {
    // Horizontal line
        if (x2 > x1) {
            for (x = x1; x <= x2; x ++) {
                *xs = x; *ys = y1; xs ++; ys ++; len ++;
            }
        }
        else {
            for (x = x1; x >= x2; x --) {
                *xs = x; *ys = y1; xs ++; ys ++; len ++;
            }
        }
        return len;
    }
    else if (x1 == x2) {
    // Vertical line
        if (y2 > y1) {
            for (y = y1; y <= y2; y ++) {
                *xs = x1; *ys = y; xs ++; ys ++; len ++;
            }
        }
        else {
            for (y = y1; y >= y2; y --) {
                *xs = x1; *ys = y; xs ++; ys ++; len ++;
            }
        }
        return len;
    }

    if (x1 < x2) {

        dx = x2 - x1;

        if (dy > 0) {
            if (dx >= dy) {
                for (x = x1; x <= x2; x ++) {
                    *xs = x; *ys = y; xs ++; ys ++; len ++;
                    
                    eps += dy;
                    if ((eps << 1) >= dx) {
                        y ++;
                        eps -= dx;
                    }
                }
            }
            else {
                for (y = y1; y <= y2; y ++) {
                    *xs = x; *ys = y; xs ++; ys ++; len ++;
                    
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
                    *xs = x; *ys = y; xs ++; ys ++; len ++;
                    
                    eps += dy;
                    if ((eps << 1) <= -dx) {
                        y --;
                        eps += dx;
                    }
                }
            }
            else {
                for (y = y1; y >= y2; y --) {
                    *xs = x; *ys = y; xs ++; ys ++; len ++;
                    
                    eps += dx;
                    if ((eps << 1) >= -dy) {
                        x ++;
                        eps -= -dy;
                    }
                }
            }
        }
    }
    else {

        dx = x1 - x2;

        if (dy > 0) {
            if (dx >= dy) {
                for (x = x1; x >= x2; x --) {
                    *xs = x; *ys = y; xs ++; ys ++; len ++;
                    
                    eps += dy;
                    if ((eps << 1) >= dx) {
                        y ++;
                        eps -= dx;
                    }
                }
            }
            else {
                for (y = y1; y <= y2; y ++) {
                    *xs = x; *ys = y; xs ++; ys ++; len ++;
                    
                    eps += dx;
                    if ((eps << 1) >= dy) {
                        x --;
                        eps -= dy;
                    }
                }
            }
        }
        else {
            if (dx >= abs(dy)) {
                for (x = x1; x >= x2; x --) {
                    *xs = x; *ys = y; xs ++; ys ++; len ++;
                    
                    eps += dy;
                    if ((eps << 1) <= -dx) {
                        y --;
                        eps += dx;
                    }
                }
            }
            else {
                for (y = y1; y >= y2; y --) {
                    *xs = x; *ys = y; xs ++; ys ++; len ++;
                    
                    eps += dx;
                    if ((eps << 1) >= -dy) {
                        x --;
                        eps -= -dy;
                    }
                }
            }
        }
    }

    return len;
}

int svlDrawHelper::WarpInternals::GetLinePixelsMono8(int* idxs, int x1, int y1, int x2, int y2, const int w, const int h)
{
    const int stride = w;

    if (x1 == x2 && y1 == y2) {

        if (x1 >= 0 && x1 < w && y1 >= 0 && y1 < h) *idxs = y1 * stride + x1;
        else *idxs = -1;

        return 1;
    }

    int x = x1, y = y1, dx, dy = y2 - y1, eps = 0, len = 0;

    if (y1 == y2) {
    // Horizontal line
        if (x2 > x1) {
            for (x = x1; x <= x2; x ++) {
                if (x >= 0 && x < w && y1 >= 0 && y1 < h) *idxs = y1 * stride + x;
                else *idxs = -1;
                idxs ++; len ++;
            }
        }
        else {
            for (x = x1; x >= x2; x --) {
                if (x >= 0 && x < w && y1 >= 0 && y1 < h) *idxs = y1 * stride + x;
                else *idxs = -1;
                idxs ++; len ++;
            }
        }
        return len;
    }
    else if (x1 == x2) {
    // Vertical line
        if (y2 > y1) {
            for (y = y1; y <= y2; y ++) {
                if (x1 >= 0 && x1 < w && y >= 0 && y < h) *idxs = y * stride + x1;
                else *idxs = -1;
                idxs ++; len ++;
            }
        }
        else {
            for (y = y1; y >= y2; y --) {
                if (x1 >= 0 && x1 < w && y >= 0 && y < h) *idxs = y * stride + x1;
                else *idxs = -1;
                idxs ++; len ++;
            }
        }
        return len;
    }

    if (x1 < x2) {

        dx = x2 - x1;

        if (dy > 0) {
            if (dx >= dy) {
                for (x = x1; x <= x2; x ++) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dy;
                    if ((eps << 1) >= dx) {
                        y ++;
                        eps -= dx;
                    }
                }
            }
            else {
                for (y = y1; y <= y2; y ++) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x;
                    else *idxs = -1;
                    idxs ++; len ++;

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

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dy;
                    if ((eps << 1) <= -dx) {
                        y --;
                        eps += dx;
                    }
                }
            }
            else {
                for (y = y1; y >= y2; y --) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dx;
                    if ((eps << 1) >= -dy) {
                        x ++;
                        eps -= -dy;
                    }
                }
            }
        }
    }
    else {

        dx = x1 - x2;

        if (dy > 0) {
            if (dx >= dy) {
                for (x = x1; x >= x2; x --) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dy;
                    if ((eps << 1) >= dx) {
                        y ++;
                        eps -= dx;
                    }
                }
            }
            else {
                for (y = y1; y <= y2; y ++) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dx;
                    if ((eps << 1) >= dy) {
                        x --;
                        eps -= dy;
                    }
                }
            }
        }
        else {
            if (dx >= abs(dy)) {
                for (x = x1; x >= x2; x --) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dy;
                    if ((eps << 1) <= -dx) {
                        y --;
                        eps += dx;
                    }
                }
            }
            else {
                for (y = y1; y >= y2; y --) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dx;
                    if ((eps << 1) >= -dy) {
                        x --;
                        eps -= -dy;
                    }
                }
            }
        }
    }

    return len;
}

int svlDrawHelper::WarpInternals::GetLinePixelsRGB(int* idxs, int x1, int y1, int x2, int y2, const int w, const int h)
{
    const int stride = w * 3;

    if (x1 == x2 && y1 == y2) {

        if (x1 >= 0 && x1 < w && y1 >= 0 && y1 < h) *idxs = y1 * stride + x1 * 3;
        else *idxs = -1;

        return 1;
    }

    int x = x1, y = y1, dx, dy = y2 - y1, eps = 0, len = 0;

    if (y1 == y2) {
    // Horizontal line
        if (x2 > x1) {
            for (x = x1; x <= x2; x ++) {
                if (x >= 0 && x < w && y1 >= 0 && y1 < h) *idxs = y1 * stride + x * 3;
                else *idxs = -1;
                idxs ++; len ++;
            }
        }
        else {
            for (x = x1; x >= x2; x --) {
                if (x >= 0 && x < w && y1 >= 0 && y1 < h) *idxs = y1 * stride + x * 3;
                else *idxs = -1;
                idxs ++; len ++;
            }
        }
        return len;
    }
    else if (x1 == x2) {
    // Vertical line
        if (y2 > y1) {
            for (y = y1; y <= y2; y ++) {
                if (x1 >= 0 && x1 < w && y >= 0 && y < h) *idxs = y * stride + x1 * 3;
                else *idxs = -1;
                idxs ++; len ++;
            }
        }
        else {
            for (y = y1; y >= y2; y --) {
                if (x1 >= 0 && x1 < w && y >= 0 && y < h) *idxs = y * stride + x1 * 3;
                else *idxs = -1;
                idxs ++; len ++;
            }
        }
        return len;
    }

    if (x1 < x2) {

        dx = x2 - x1;

        if (dy > 0) {
            if (dx >= dy) {
                for (x = x1; x <= x2; x ++) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 3;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dy;
                    if ((eps << 1) >= dx) {
                        y ++;
                        eps -= dx;
                    }
                }
            }
            else {
                for (y = y1; y <= y2; y ++) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 3;
                    else *idxs = -1;
                    idxs ++; len ++;

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

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 3;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dy;
                    if ((eps << 1) <= -dx) {
                        y --;
                        eps += dx;
                    }
                }
            }
            else {
                for (y = y1; y >= y2; y --) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 3;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dx;
                    if ((eps << 1) >= -dy) {
                        x ++;
                        eps -= -dy;
                    }
                }
            }
        }
    }
    else {

        dx = x1 - x2;

        if (dy > 0) {
            if (dx >= dy) {
                for (x = x1; x >= x2; x --) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 3;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dy;
                    if ((eps << 1) >= dx) {
                        y ++;
                        eps -= dx;
                    }
                }
            }
            else {
                for (y = y1; y <= y2; y ++) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 3;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dx;
                    if ((eps << 1) >= dy) {
                        x --;
                        eps -= dy;
                    }
                }
            }
        }
        else {
            if (dx >= abs(dy)) {
                for (x = x1; x >= x2; x --) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 3;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dy;
                    if ((eps << 1) <= -dx) {
                        y --;
                        eps += dx;
                    }
                }
            }
            else {
                for (y = y1; y >= y2; y --) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 3;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dx;
                    if ((eps << 1) >= -dy) {
                        x --;
                        eps -= -dy;
                    }
                }
            }
        }
    }

    return len;
}

int svlDrawHelper::WarpInternals::GetLinePixelsRGBA(int* idxs, int x1, int y1, int x2, int y2, const int w, const int h)
{
    const int stride = w * 4;

    if (x1 == x2 && y1 == y2) {

        if (x1 >= 0 && x1 < w && y1 >= 0 && y1 < h) *idxs = y1 * stride + x1 * 4;
        else *idxs = -1;

        return 1;
    }

    int x = x1, y = y1, dx, dy = y2 - y1, eps = 0, len = 0;

    if (y1 == y2) {
    // Horizontal line
        if (x2 > x1) {
            for (x = x1; x <= x2; x ++) {
                if (x >= 0 && x < w && y1 >= 0 && y1 < h) *idxs = y1 * stride + x * 4;
                else *idxs = -1;
                idxs ++; len ++;
            }
        }
        else {
            for (x = x1; x >= x2; x --) {
                if (x >= 0 && x < w && y1 >= 0 && y1 < h) *idxs = y1 * stride + x * 4;
                else *idxs = -1;
                idxs ++; len ++;
            }
        }
        return len;
    }
    else if (x1 == x2) {
    // Vertical line
        if (y2 > y1) {
            for (y = y1; y <= y2; y ++) {
                if (x1 >= 0 && x1 < w && y >= 0 && y < h) *idxs = y * stride + x1 * 4;
                else *idxs = -1;
                idxs ++; len ++;
            }
        }
        else {
            for (y = y1; y >= y2; y --) {
                if (x1 >= 0 && x1 < w && y >= 0 && y < h) *idxs = y * stride + x1 * 4;
                else *idxs = -1;
                idxs ++; len ++;
            }
        }
        return len;
    }

    if (x1 < x2) {

        dx = x2 - x1;

        if (dy > 0) {
            if (dx >= dy) {
                for (x = x1; x <= x2; x ++) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 4;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dy;
                    if ((eps << 1) >= dx) {
                        y ++;
                        eps -= dx;
                    }
                }
            }
            else {
                for (y = y1; y <= y2; y ++) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 4;
                    else *idxs = -1;
                    idxs ++; len ++;

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

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 4;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dy;
                    if ((eps << 1) <= -dx) {
                        y --;
                        eps += dx;
                    }
                }
            }
            else {
                for (y = y1; y >= y2; y --) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 4;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dx;
                    if ((eps << 1) >= -dy) {
                        x ++;
                        eps -= -dy;
                    }
                }
            }
        }
    }
    else {

        dx = x1 - x2;

        if (dy > 0) {
            if (dx >= dy) {
                for (x = x1; x >= x2; x --) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 4;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dy;
                    if ((eps << 1) >= dx) {
                        y ++;
                        eps -= dx;
                    }
                }
            }
            else {
                for (y = y1; y <= y2; y ++) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 4;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dx;
                    if ((eps << 1) >= dy) {
                        x --;
                        eps -= dy;
                    }
                }
            }
        }
        else {
            if (dx >= abs(dy)) {
                for (x = x1; x >= x2; x --) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 4;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dy;
                    if ((eps << 1) <= -dx) {
                        y --;
                        eps += dx;
                    }
                }
            }
            else {
                for (y = y1; y >= y2; y --) {

                    if (x >= 0 && x < w && y >= 0 && y < h) *idxs = y * stride + x * 4;
                    else *idxs = -1;
                    idxs ++; len ++;

                    eps += dx;
                    if ((eps << 1) >= -dy) {
                        x --;
                        eps -= -dy;
                    }
                }
            }
        }
    }

    return len;
}

void svlDrawHelper::WarpInternals::ResampleLineMono8(int ix1, int iy1, int ix2, int iy2,
                                                     int ox1, int oy1, int ox2, int oy2)
{
    int ilen = GetLinePixelsMono8(_in_idxs,  ix1, iy1, ix2, iy2, InWidth, InHeight);
    int olen = GetLinePixelsMono8(_out_idxs, ox1, oy1, ox2, oy2, OutWidth, OutHeight);

    int* in_idxs = _in_idxs;
    int* out_idxs = _out_idxs;
    unsigned char *in_buf, *out_buf;
    int ipix, opix;

    if (olen == 1) {

        opix = *out_idxs;
        if (opix >= 0) {
            out_buf = Output + opix;

            ipix = *in_idxs;
            if (ipix >= 0) {
                in_buf  = Input + ipix;
                *out_buf = *in_buf;
            }
            else {
                *out_buf = 0;
            }
        }

        return;
    }

    int i, eps = 0;

    if (ilen >= olen) {
        for (i = 0; i < ilen; i ++) {
            eps += olen;
            if ((eps << 1) >= ilen) {

                opix = *out_idxs;
                if (opix >= 0) {
                    out_buf = Output + opix;

                    ipix = *in_idxs;
                    if (ipix >= 0) {
                        in_buf  = Input + ipix;
                        *out_buf = *in_buf;
                    }
                    else {
                        *out_buf = 0;
                    }
                }
                out_idxs ++;

                eps -= ilen;
            }
            in_idxs ++;
        }
    }
    else {
        for (i = 0; i < olen; i ++) {

            opix = *out_idxs;
            if (opix >= 0) {
                out_buf = Output + opix;

                ipix = *in_idxs;
                if (ipix >= 0) {
                    in_buf  = Input + ipix;
                    *out_buf = *in_buf;
                }
                else {
                    *out_buf = 0;
                }
            }
            out_idxs ++;

            if ((eps << 1) >= olen) {
                eps -= olen;
                in_idxs ++;
            }
            eps += ilen;
        }
    }
}

void svlDrawHelper::WarpInternals::ResampleLineRGB(int ix1, int iy1, int ix2, int iy2,
                                                   int ox1, int oy1, int ox2, int oy2)
{
    int ilen = GetLinePixelsRGB(_in_idxs,  ix1, iy1, ix2, iy2, InWidth, InHeight);
    int olen = GetLinePixelsRGB(_out_idxs, ox1, oy1, ox2, oy2, OutWidth, OutHeight);

    int* in_idxs = _in_idxs;
    int* out_idxs = _out_idxs;
    unsigned char *in_buf, *out_buf;
    int ipix, opix;

    if (olen == 1) {

        opix = *out_idxs;
        if (opix >= 0) {
            out_buf = Output + opix;

            ipix = *in_idxs;
            if (ipix >= 0) {
                in_buf  = Input + ipix;
                *out_buf = *in_buf; out_buf ++; in_buf ++;
                *out_buf = *in_buf; out_buf ++; in_buf ++;
                *out_buf = *in_buf;
            }
            else {
                *out_buf = 0; out_buf ++;
                *out_buf = 0; out_buf ++;
                *out_buf = 0;
            }
        }

        return;
    }

    int i, eps = 0;

    if (ilen >= olen) {
        for (i = 0; i < ilen; i ++) {
            eps += olen;
            if ((eps << 1) >= ilen) {

                opix = *out_idxs;
                if (opix >= 0) {
                    out_buf = Output + opix;

                    ipix = *in_idxs;
                    if (ipix >= 0) {
                        in_buf  = Input + ipix;
                        *out_buf = *in_buf; out_buf ++; in_buf ++;
                        *out_buf = *in_buf; out_buf ++; in_buf ++;
                        *out_buf = *in_buf;
                    }
                    else {
                        *out_buf = 0; out_buf ++;
                        *out_buf = 0; out_buf ++;
                        *out_buf = 0;
                    }
                }
                out_idxs ++;

                eps -= ilen;
            }
            in_idxs ++;
        }
    }
    else {
        for (i = 0; i < olen; i ++) {

            opix = *out_idxs;
            if (opix >= 0) {
                out_buf = Output + opix;

                ipix = *in_idxs;
                if (ipix >= 0) {
                    in_buf  = Input + ipix;
                    *out_buf = *in_buf; out_buf ++; in_buf ++;
                    *out_buf = *in_buf; out_buf ++; in_buf ++;
                    *out_buf = *in_buf;
                }
                else {
                    *out_buf = 0; out_buf ++;
                    *out_buf = 0; out_buf ++;
                    *out_buf = 0;
                }
            }
            out_idxs ++;

            if ((eps << 1) >= olen) {
                eps -= olen;
                in_idxs ++;
            }
            eps += ilen;
        }
    }
}

void svlDrawHelper::WarpInternals::ResampleLineRGBA(int ix1, int iy1, int ix2, int iy2,
                                                    int ox1, int oy1, int ox2, int oy2)
{
    int ilen = GetLinePixelsRGBA(_in_idxs,  ix1, iy1, ix2, iy2, InWidth, InHeight);
    int olen = GetLinePixelsRGB(_out_idxs, ox1, oy1, ox2, oy2, OutWidth, OutHeight);

    int* in_idxs = _in_idxs;
    int* out_idxs = _out_idxs;
    unsigned char *in_buf, *out_buf;
    int ipix, opix, aval, iaval;

    if (olen == 1) {

        opix = *out_idxs;
        if (opix >= 0) {
            out_buf = Output + opix;

            ipix = *in_idxs;
            if (ipix >= 0) {
                in_buf  = Input + ipix;
                aval = in_buf[3]; // alpha
                iaval = 256 - aval; // inverse alpha
                *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8; out_buf ++; in_buf ++;
                *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8; out_buf ++; in_buf ++;
                *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8;
            }
        }

        return;
    }

    int i, eps = 0;

    if (ilen >= olen) {
        for (i = 0; i < ilen; i ++) {
            eps += olen;
            if ((eps << 1) >= ilen) {

                opix = *out_idxs;
                if (opix >= 0) {
                    out_buf = Output + opix;

                    ipix = *in_idxs;
                    if (ipix >= 0) {
                        in_buf  = Input + ipix;
                        aval = in_buf[3]; // alpha
                        iaval = 256 - aval; // inverse alpha
                        *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8; out_buf ++; in_buf ++;
                        *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8; out_buf ++; in_buf ++;
                        *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8;
                    }
                }
                out_idxs ++;

                eps -= ilen;
            }
            in_idxs ++;
        }
    }
    else {
        for (i = 0; i < olen; i ++) {

            opix = *out_idxs;
            if (opix >= 0) {
                out_buf = Output + opix;

                ipix = *in_idxs;
                if (ipix >= 0) {
                    in_buf  = Input + ipix;
                    aval = in_buf[3]; // alpha
                    iaval = 256 - aval; // inverse alpha
                    *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8; out_buf ++; in_buf ++;
                    *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8; out_buf ++; in_buf ++;
                    *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8;
                }
            }
            out_idxs ++;

            if ((eps << 1) >= olen) {
                eps -= olen;
                in_idxs ++;
            }
            eps += ilen;
        }
    }
}

void svlDrawHelper::WarpInternals::ResampleLineAlphaMono8(int ix1, int iy1, int ix2, int iy2,
                                                          int ox1, int oy1, int ox2, int oy2,
                                                          unsigned int alpha)
{
    int ilen = GetLinePixelsMono8(_in_idxs,  ix1, iy1, ix2, iy2, InWidth, InHeight);
    int olen = GetLinePixelsMono8(_out_idxs, ox1, oy1, ox2, oy2, OutWidth, OutHeight);

    int* in_idxs = _in_idxs;
    int* out_idxs = _out_idxs;
    unsigned char *in_buf, *out_buf;
    int ipix, opix, aval = alpha, iaval = 256 - aval;

    if (olen == 1) {

        opix = *out_idxs;
        if (opix >= 0) {
            out_buf = Output + opix;

            ipix = *in_idxs;
            if (ipix >= 0) {
                in_buf  = Input + ipix;
                *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8;
            }
//            else {
//                *out_buf = (iaval * (int)(*out_buf)) >> 8;
//            }
        }

        return;
    }

    int i, eps = 0;

    if (ilen >= olen) {
        for (i = 0; i < ilen; i ++) {
            eps += olen;
            if ((eps << 1) >= ilen) {

                opix = *out_idxs;
                if (opix >= 0) {
                    out_buf = Output + opix;

                    ipix = *in_idxs;
                    if (ipix >= 0) {
                        in_buf  = Input + ipix;
                        *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8;
                    }
//                    else {
//                        *out_buf = (iaval * (int)(*out_buf)) >> 8;
//                    }
                }
                out_idxs ++;

                eps -= ilen;
            }
            in_idxs ++;
        }
    }
    else {
        for (i = 0; i < olen; i ++) {

            opix = *out_idxs;
            if (opix >= 0) {
                out_buf = Output + opix;

                ipix = *in_idxs;
                if (ipix >= 0) {
                    in_buf  = Input + ipix;
                    *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8;
                }
//                else {
//                    *out_buf = (iaval * (int)(*out_buf)) >> 8;
//                }
            }
            out_idxs ++;

            if ((eps << 1) >= olen) {
                eps -= olen;
                in_idxs ++;
            }
            eps += ilen;
        }
    }
}

void svlDrawHelper::WarpInternals::ResampleLineAlphaRGB(int ix1, int iy1, int ix2, int iy2,
                                                        int ox1, int oy1, int ox2, int oy2,
                                                        unsigned int alpha)
{
    int ilen = GetLinePixelsRGB(_in_idxs,  ix1, iy1, ix2, iy2, InWidth, InHeight);
    int olen = GetLinePixelsRGB(_out_idxs, ox1, oy1, ox2, oy2, OutWidth, OutHeight);

    int* in_idxs = _in_idxs;
    int* out_idxs = _out_idxs;
    unsigned char *in_buf, *out_buf;
    int ipix, opix, aval = alpha, iaval = 256 - aval;

    if (olen == 1) {

        opix = *out_idxs;
        if (opix >= 0) {
            out_buf = Output + opix;

            ipix = *in_idxs;
            if (ipix >= 0) {
                in_buf  = Input + ipix;
                *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8; out_buf ++; in_buf ++;
                *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8; out_buf ++; in_buf ++;
                *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8;
            }
//            else {
//                *out_buf = (iaval * (int)(*out_buf)) >> 8; out_buf ++;
//                *out_buf = (iaval * (int)(*out_buf)) >> 8; out_buf ++;
//                *out_buf = (iaval * (int)(*out_buf)) >> 8;
//            }
        }

        return;
    }

    int i, eps = 0;

    if (ilen >= olen) {
        for (i = 0; i < ilen; i ++) {
            eps += olen;
            if ((eps << 1) >= ilen) {

                opix = *out_idxs;
                if (opix >= 0) {
                    out_buf = Output + opix;

                    ipix = *in_idxs;
                    if (ipix >= 0) {
                        in_buf  = Input + ipix;
                        *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8; out_buf ++; in_buf ++;
                        *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8; out_buf ++; in_buf ++;
                        *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8;
                    }
//                    else {
//                        *out_buf = (iaval * (int)(*out_buf)) >> 8; out_buf ++;
//                        *out_buf = (iaval * (int)(*out_buf)) >> 8; out_buf ++;
//                        *out_buf = (iaval * (int)(*out_buf)) >> 8;
//                    }
                }
                out_idxs ++;

                eps -= ilen;
            }
            in_idxs ++;
        }
    }
    else {
        for (i = 0; i < olen; i ++) {

            opix = *out_idxs;
            if (opix >= 0) {
                out_buf = Output + opix;

                ipix = *in_idxs;
                if (ipix >= 0) {
                    in_buf  = Input + ipix;
                    *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8; out_buf ++; in_buf ++;
                    *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8; out_buf ++; in_buf ++;
                    *out_buf = (aval * (int)(*in_buf) + iaval * (int)(*out_buf)) >> 8;
                }
//                else {
//                    *out_buf = (iaval * (int)(*out_buf)) >> 8; out_buf ++;
//                    *out_buf = (iaval * (int)(*out_buf)) >> 8; out_buf ++;
//                    *out_buf = (iaval * (int)(*out_buf)) >> 8;
//                }
            }
            out_idxs ++;

            if ((eps << 1) >= olen) {
                eps -= olen;
                in_idxs ++;
            }
            eps += ilen;
        }
    }
}

void svlDrawHelper::WarpInternals::AllocateBuffers(unsigned int vertices, unsigned int size)
{
    ReleaseBuffers();

    _ilen     = new int[vertices];
    _olen     = new int[vertices];
    _in_idxs  = new int[size];
    _out_idxs = new int[size];
    _lm_x     = new int[size];
    _rm_x     = new int[size];
    _lm_id    = new int[size];
    _rm_id    = new int[size];
    _lm_pos   = new int[size];
    _rm_pos   = new int[size];

    _ixs.SetSize(vertices);
    _iys.SetSize(vertices);
    _oxs.SetSize(vertices);
    _oys.SetSize(vertices);

    for (unsigned int i = 0; i < vertices; i ++) {
        _ixs[i] = new int[size];
        _iys[i] = new int[size];
        _oxs[i] = new int[size];
        _oys[i] = new int[size];
    }
}

void svlDrawHelper::WarpInternals::ReleaseBuffers()
{
    if (_ilen) {
        delete [] _ilen;
        _ilen = 0;
    }
    if (_olen) {
        delete [] _olen;
        _olen = 0;
    }
    if (_in_idxs) {
        delete [] _in_idxs;
        _in_idxs = 0;
    }
    if (_out_idxs) {
        delete [] _out_idxs;
        _out_idxs = 0;
    }
    if (_lm_x) {
        delete [] _lm_x;
        _lm_x = 0;
    }
    if (_rm_x) {
        delete [] _rm_x;
        _rm_x = 0;
    }
    if (_lm_id) {
        delete [] _lm_id;
        _lm_id = 0;
    }
    if (_rm_id) {
        delete [] _rm_id;
        _rm_id = 0;
    }
    if (_lm_pos) {
        delete [] _lm_pos;
        _lm_pos = 0;
    }
    if (_rm_pos) {
        delete [] _rm_pos;
        _rm_pos = 0;
    }

    for (unsigned int i = 0; i < _ixs.size(); i ++) {
        if (_ixs[i]) {
            delete [] _ixs[i];
            _ixs[i] = 0;
        }
    }
    for (unsigned int i = 0; i < _iys.size(); i ++) {
        if (_iys[i]) {
            delete [] _iys[i];
            _iys[i] = 0;
        }
    }
    for (unsigned int i = 0; i < _oxs.size(); i ++) {
        if (_oxs[i]) {
            delete [] _oxs[i];
            _oxs[i] = 0;
        }
    }
    for (unsigned int i = 0; i < _oys.size(); i ++) {
        if (_oys[i]) {
            delete [] _oys[i];
            _oys[i] = 0;
        }
    }
}

