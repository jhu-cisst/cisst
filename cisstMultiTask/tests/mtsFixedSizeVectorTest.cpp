/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2010-02-27
  
  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsFixedSizeVectorTest.h"
#include <cisstVector/vctRandomFixedSizeVector.h>

template <class _elementType, vct::size_type _size>
void mtsFixedSizeVectorTest::TestConversion(void)
{
    typedef _elementType value_type;
    typedef mtsFixedSizeVector<value_type, _size> VectorType;
    typedef typename VectorType::VectorType InternalVectorType;
    InternalVectorType original;
    vctRandom(original, static_cast<value_type>(0), static_cast<value_type>(10));
    // test copy ctor from internal type
    VectorType copyByConstructor(original);
    CPPUNIT_ASSERT(copyByConstructor.Equal(original));
    // test assign from internal type
    VectorType copyByAssign = original;
    CPPUNIT_ASSERT(copyByConstructor.Equal(original));
    // test assign to internal type
    InternalVectorType copy;
    copy = copyByConstructor;
    CPPUNIT_ASSERT(copy.Equal(copyByConstructor));
}

void mtsFixedSizeVectorTest::TestConversionDouble3(void)
{
    TestConversion<double, 3>();
}

void mtsFixedSizeVectorTest::TestConversionDouble6(void)
{
    TestConversion<double, 6>();
}

void mtsFixedSizeVectorTest::TestConversionInt6(void)
{
    TestConversion<int, 6>();
}

void mtsFixedSizeVectorTest::TestConversionInt3(void)
{
    TestConversion<int, 3>();
}
