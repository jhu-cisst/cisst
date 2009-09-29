/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: imgPixelTypesTest.cpp 75 2009-02-24 16:47:20Z adeguet1 $
  
  Author(s): Anton Deguet
  Created on: 2006-03-28
  
  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "imgPixelTypesTest.h"


void imgPixelTypesTest::TestCompilerSizeOf(void) {

    // Simple tests for padding, size comparaison
    CPPUNIT_ASSERT(sizeof(imgUCharRGB::Pixel) == 3);
    imgUCharRGB::Pixel pixel;
    CPPUNIT_ASSERT(sizeof(char) == 1);
    CPPUNIT_ASSERT(sizeof(pixel) == (pixel.size() * sizeof(char)));

    // Testing with an array dynamically allocated with malloc
    imgUCharRGB::Pixel * mallocBuffer;
    const unsigned int bufferSize = 13;
    mallocBuffer = (imgUCharRGB::Pixel*) malloc(bufferSize * sizeof(imgUCharRGB::Pixel));
    CPPUNIT_ASSERT(mallocBuffer != 0);
    imgUCharRGB::Pixel * pixelPointer = mallocBuffer;
    unsigned int index;
    for (index = 0; index < bufferSize; index++) {
        CPPUNIT_ASSERT( (unsigned int)((char*)pixelPointer - (char*)mallocBuffer) == (index * sizeof(imgUCharRGB::Pixel)));
        CPPUNIT_ASSERT( (unsigned int)(pixelPointer - mallocBuffer) == index);
        CPPUNIT_ASSERT( (unsigned int)((char*)&mallocBuffer[index] - (char*)mallocBuffer) == (index * sizeof(imgUCharRGB::Pixel)));
        CPPUNIT_ASSERT( (unsigned int)(&mallocBuffer[index] - mallocBuffer) == index);
        pixelPointer++;
    }

    // Testing with an array statically allocated
    imgUCharRGB::Pixel staticBuffer[bufferSize];
    CPPUNIT_ASSERT(sizeof(staticBuffer) == bufferSize * sizeof(imgUCharRGB::Pixel));
    pixelPointer = staticBuffer;
    for (index = 0; index < bufferSize; index++) {
        CPPUNIT_ASSERT( (unsigned int)((char*)pixelPointer - (char*)staticBuffer) == (index * sizeof(imgUCharRGB::Pixel)));
        CPPUNIT_ASSERT( (unsigned int)(pixelPointer - staticBuffer) == index);
        CPPUNIT_ASSERT( (unsigned int)((char*)&staticBuffer[index] - (char*)staticBuffer) == (index * sizeof(imgUCharRGB::Pixel)));
        CPPUNIT_ASSERT( (unsigned int)(&staticBuffer[index] - staticBuffer) == index);
        pixelPointer++;
    }

    // Testing with an array allocated with new
    imgUCharRGB::Pixel * newBuffer;
    newBuffer = new imgUCharRGB::Pixel[bufferSize];
    pixelPointer = newBuffer;
    for (index = 0; index < bufferSize; index++) {
        CPPUNIT_ASSERT( (unsigned int)((char*)pixelPointer - (char*)newBuffer) == (index * sizeof(imgUCharRGB::Pixel)));
        CPPUNIT_ASSERT( (unsigned int)(pixelPointer - newBuffer) == index);
        CPPUNIT_ASSERT( (unsigned int)((char*)&newBuffer[index] - (char*)newBuffer) == (index * sizeof(imgUCharRGB::Pixel)));
        CPPUNIT_ASSERT( (unsigned int)(&newBuffer[index] - newBuffer) == index);
        pixelPointer++;
    }
}

