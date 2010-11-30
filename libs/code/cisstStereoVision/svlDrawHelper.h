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

#ifndef _svlDrawHelper_h
#define _svlDrawHelper_h

#include <cisstStereoVision/svlTypes.h>


class svlDrawInternals
{
protected:
    // Protected default constructor:
    //   Class can only be instantiated by derived class
    svlDrawInternals();

private:
    // Private copy constructor:
    //   No one else will be able to call it
    svlDrawInternals(const svlDrawInternals& internals);

public:
    // Virtual destructor:
    //   Will force the compiler to create the virtual function table
    virtual ~svlDrawInternals();
};


namespace svlDrawHelper
{
    //////////////////////
    // Triangle Drawing //
    //////////////////////

    class TriangleInternals : public svlDrawInternals
    {
    public:
        TriangleInternals();

        bool SetImage(svlSampleImage* image, unsigned int channel = 0);
        void Draw(int x1, int y1, int x2, int y2, int x3, int y3, svlRGB color);

    private:
        void SampleLine(vctDynamicVector<int>& samples, int x1, int y1, int x2, int y2);

    private:
        svlSampleImage* Image;
        unsigned int Channel;
        int Width;
        int Height;
        vctDynamicVector<int> LeftSamples;
        vctDynamicVector<int> RightSamples;
    };
};

#endif // _svlDrawHelper_h

