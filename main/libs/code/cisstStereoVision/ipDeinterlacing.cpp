/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ipDeinterlacing.cpp 75 2009-02-24 16:47:20Z adeguet1 $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "ipDeinterlacing.h"

void Blending(unsigned char* buffer, const int width, const int height)
{
    int i, j;
    int ar, ag, ab;
    unsigned char *r0, *g0, *b0;
    unsigned char *r1, *g1, *b1;
    const int colstride = width * 3;

    r0 = buffer;
    g0 = r0 + 1;
    b0 = g0 + 1;

    r1 = r0 + colstride;
    g1 = r1 + 1;
    b1 = g1 + 1;

    for (j = 0; j < height; j += 2) {
        for (i = 0; i < width; i ++) {
            ar = (*r0 + *r1) >> 1; ag = (*g0 + *g1) >> 1; ab = (*b0 + *b1) >> 1;
            *r0 = ar; *g0 = ag; *b0 = ab;
            *r1 = ar; *g1 = ag; *b1 = ab;

            r0 += 3; g0 += 3; b0 += 3;
            r1 += 3; g1 += 3; b1 += 3;
        }
        r0 += colstride; g0 += colstride; b0 += colstride;
        r1 += colstride; g1 += colstride; b1 += colstride;
    }
}

void Discarding(unsigned char* buffer, const int width, const int height)
{
    int i, j;
    unsigned char *r0, *g0, *b0;
    unsigned char *r1, *g1, *b1;
    const int colstride = width * 3;

    r0 = buffer;
    g0 = r0 + 1;
    b0 = g0 + 1;

    r1 = r0 + colstride;
    g1 = r1 + 1;
    b1 = g1 + 1;

    for (j = 0; j < height; j += 2) {
        for (i = 0; i < width; i ++) {
            *r1 = *r0; *g1 = *g0; *b1 = *b0;

            r0 += 3; g0 += 3; b0 += 3;
            r1 += 3; g1 += 3; b1 += 3;
        }
        r0 += colstride; g0 += colstride; b0 += colstride;
        r1 += colstride; g1 += colstride; b1 += colstride;
    }
}

void AdaptiveBlending(unsigned char* buffer, const int width, const int height)
{
    int i, j;
    int ar, ag, ab;
    unsigned int diff, diffinv;
    unsigned char *r0, *g0, *b0;
    unsigned char *r1, *g1, *b1;
    unsigned char *r2, *g2, *b2;
    const int colstride = width * 3;

    r0 = buffer;
    g0 = r0 + 1;
    b0 = g0 + 1;

    r1 = r0 + colstride;
    g1 = r1 + 1;
    b1 = g1 + 1;

    r2 = r1 + colstride;
    g2 = r2 + 1;
    b2 = g2 + 1;

    for (j = 0; j < height; j += 2) {
        for (i = 0; i < width; i ++) {
            ar = (*r0 + *r2) >> 1; ag = (*g0 + *g2) >> 1; ab = (*b0 + *b2) >> 1;

            diff = (abs(*r1 - ar) + abs(*g1 - ag) + abs(*b1 - ab)) * 4;
            if (diff > 765) diff = 765;
            diffinv = 765 - diff;
            *r1 = (diff * ar + diffinv * (*r1)) / 765;
            *g1 = (diff * ag + diffinv * (*g1)) / 765;
            *b1 = (diff * ab + diffinv * (*b1)) / 765;

            r0 += 3; g0 += 3; b0 += 3;
            r1 += 3; g1 += 3; b1 += 3;
            r2 += 3; g2 += 3; b2 += 3;
        }
        r0 += colstride; g0 += colstride; b0 += colstride;
        r1 += colstride; g1 += colstride; b1 += colstride;
        r2 += colstride; g2 += colstride; b2 += colstride;
    }
}

void AdaptiveDiscarding(unsigned char* buffer, const int width, const int height)
{
    int i, j;
    int ar, ag, ab;
    unsigned int diff, diffinv;
    unsigned char *r0, *g0, *b0;
    unsigned char *r1, *g1, *b1;
    unsigned char *r2, *g2, *b2;
    const int colstride = width * 3;

    r0 = buffer;
    g0 = r0 + 1;
    b0 = g0 + 1;

    r1 = r0 + colstride;
    g1 = r1 + 1;
    b1 = g1 + 1;

    r2 = r1 + colstride;
    g2 = r2 + 1;
    b2 = g2 + 1;

    for (j = 0; j < height; j += 2) {
        for (i = 0; i < width; i ++) {
            ar = (*r0 + *r2) >> 1; ag = (*g0 + *g2) >> 1; ab = (*b0 + *b2) >> 1;

            diff = (abs(*r1 - ar) + abs(*g1 - ag) + abs(*b1 - ab)) * 2;
            if (diff > 765) diff = 765;
            diffinv = 765 - diff;
            *r1 = (diff * ar + diffinv * (*r1)) / 765;
            *g1 = (diff * ag + diffinv * (*g1)) / 765;
            *b1 = (diff * ab + diffinv * (*b1)) / 765;

            r0 += 3; g0 += 3; b0 += 3;
            r1 += 3; g1 += 3; b1 += 3;
            r2 += 3; g2 += 3; b2 += 3;
        }
        r0 += colstride; g0 += colstride; b0 += colstride;
        r1 += colstride; g1 += colstride; b1 += colstride;
        r2 += colstride; g2 += colstride; b2 += colstride;
    }
}

