/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2009-04-29
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsVectorTest.h"
#include <cisstVector/vctRandomDynamicVector.h>

template <class _elementType>
void mtsVectorTest::TestSetSizeFrom(void)
{
    typedef _elementType value_type;
    typedef mtsVector<value_type> VectorType;
    VectorType original(10);
    vctRandom(original, static_cast<value_type>(0), static_cast<value_type>(10));
    CPPUNIT_ASSERT(original.size() == 10);
    VectorType copyByConstructor(original);
    CPPUNIT_ASSERT(copyByConstructor.Equal(original));
    VectorType copy;
    CPPUNIT_ASSERT(copy.size() == 0);
    new(&copy) VectorType(original);
    CPPUNIT_ASSERT(copy.Equal(original));
}

void mtsVectorTest::TestSetSizeFromDouble(void)
{
    TestSetSizeFrom<double>();
}

void mtsVectorTest::TestSetSizeFromInt(void)
{
    TestSetSizeFrom<int>();
}



template <class _elementType>
void mtsVectorTest::TestConversion(void)
{
    typedef _elementType value_type;
    typedef mtsVector<value_type> VectorType;
    typedef typename VectorType::VectorType InternalVectorType;
    InternalVectorType original(10);
    vctRandom(original, static_cast<value_type>(0), static_cast<value_type>(10));
    CPPUNIT_ASSERT(original.size() == 10);
    // test copy ctor from internal type
    VectorType copyByConstructor(original);
    CPPUNIT_ASSERT(copyByConstructor.Equal(original));
    // test assign from internal type
    VectorType copyByAssign = original;
    CPPUNIT_ASSERT(copyByConstructor.Equal(original));
    // test assign to internal type
    InternalVectorType copy;
    CPPUNIT_ASSERT(copy.size() == 0);
    copy = copyByConstructor;
    CPPUNIT_ASSERT(copy.Equal(copyByConstructor));
}

void mtsVectorTest::TestConversionDouble(void)
{
    TestConversion<double>();
}

void mtsVectorTest::TestConversionInt(void)
{
    TestConversion<int>();
}
