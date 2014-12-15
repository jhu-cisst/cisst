/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-11-02

  (C) Copyright 2013-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnDataGeneratorTest.h"
#include "cmnDataGeneratorTestTypes.h"

#include <limits>
#include <algorithm>

void cmnDataGeneratorTest::TestInheritance(void)
{
    cmnDataGeneratorTestC * objectC = new cmnDataGeneratorTestC;
    CPPUNIT_ASSERT(dynamic_cast<cmnDataGeneratorTestA *>(objectC));
    CPPUNIT_ASSERT(dynamic_cast<cmnDataGeneratorTestB *>(objectC));
    CPPUNIT_ASSERT(dynamic_cast<cmnGenericObject *>(objectC));
    delete objectC;
}

void cmnDataGeneratorTest::TestAccessors(void)
{
    cmnDataGeneratorTestC objectC;

    // string from base class has default value, cover all possible accessors
    CPPUNIT_ASSERT_EQUAL(std::string("DefaultA"), objectC.StringA());
    std::string newValue = "cow, jumping over the moon";
    objectC.StringA() = newValue;
    std::string placeHolder;
    objectC.GetStringA(placeHolder);
    CPPUNIT_ASSERT_EQUAL(newValue, placeHolder);
    newValue = "can this handle back slash \\ or forward slash /";
    objectC.SetStringA(newValue);
    CPPUNIT_ASSERT_EQUAL(newValue, objectC.StringA());

    // std::vector, public data member w/o accessors
    CPPUNIT_ASSERT(objectC.StdVector.empty());
    objectC.StdVector.resize(3, 1234);
    CPPUNIT_ASSERT_EQUAL(1234, objectC.StdVector[2]);

    // C array of 3 elements, pointer arrays only
    CPPUNIT_ASSERT(cmnDataGeneratorTestC::SIZE == 3);
    for (size_t index = 0; index < cmnDataGeneratorTestC::SIZE; ++index) {
        objectC.CArray[index] = index + 0.5;
    }
    for (size_t index = 0; index < cmnDataGeneratorTestC::SIZE; ++index) {
        CPPUNIT_ASSERT(objectC.CArray[index] == (index + 0.5));
    }

    // 2D C array
    for (size_t row = 0; row < cmnDataGeneratorTestC::ROW; ++row) {
        for (size_t col = 0; col < cmnDataGeneratorTestC::COL; ++col) {
            objectC.CArray2D[row][col] = row * 10 + col;
        }
    }
    for (size_t row = 0; row < cmnDataGeneratorTestC::ROW; ++row) {
        for (size_t col = 0; col < cmnDataGeneratorTestC::COL; ++col) {
            CPPUNIT_ASSERT(objectC.CArray2D[row][col] == (row * 10 + col));
        }
    }
}

void cmnDataGeneratorTest::TestCopy(void)
{
    cmnDataGeneratorTestC source, destination;
    // modify source
    source.StringA() = "source with a comma , a back slash \\ and a forward slash /";
    source.StdVector.resize(10, 5);
    for (size_t index = 0; index < cmnDataGeneratorTestC::SIZE; ++index) {
        source.CArray[index] = index + 10.0;
    }
    for (size_t row = 0; row < cmnDataGeneratorTestC::ROW; ++row) {
        for (size_t col = 0; col < cmnDataGeneratorTestC::COL; ++col) {
            source.CArray2D[row][col] = row * 10 + col;
        }
    }
    // copy
    cmnData<cmnDataGeneratorTestC>::Copy(destination, source);
    // check result
    CPPUNIT_ASSERT_EQUAL(source.StringA(), destination.StringA());
    CPPUNIT_ASSERT_EQUAL(source.StdVector.size(), destination.StdVector.size());
    CPPUNIT_ASSERT(std::equal(source.StdVector.begin(), source.StdVector.end(), destination.StdVector.begin()));
    for (size_t index = 0; index < cmnDataGeneratorTestC::SIZE; ++index) {
        CPPUNIT_ASSERT_EQUAL(source.CArray[index], destination.CArray[index]);
    }
    for (size_t row = 0; row < cmnDataGeneratorTestC::ROW; ++row) {
        for (size_t col = 0; col < cmnDataGeneratorTestC::COL; ++col) {
            CPPUNIT_ASSERT_EQUAL(source.CArray2D[row][col], destination.CArray2D[row][col]);
        }
    }

}

void cmnDataGeneratorTest::TestBinarySerialization(void)
{
    cmnDataGeneratorTestC source, destination;
    // modify source
    source.StringA() = "source with a comma , a back slash \\ and a forward slash /";
    source.StdVector.resize(10, 5);
    for (size_t index = 0; index < cmnDataGeneratorTestC::SIZE; ++index) {
        source.CArray[index] = index + 10.0;
    }
    for (size_t row = 0; row < cmnDataGeneratorTestC::ROW; ++row) {
        for (size_t col = 0; col < cmnDataGeneratorTestC::COL; ++col) {
            source.CArray2D[row][col] = row * 10 + col;
        }
    }
    // serialize/de-serialize
    cmnDataFormat local, remote; // same!
    std::stringstream stream;
    cmnData<cmnDataGeneratorTestC>::SerializeBinary(source, stream);
    cmnData<cmnDataGeneratorTestC>::DeSerializeBinary(destination, stream, local, remote);
    // check result
    CPPUNIT_ASSERT_EQUAL(source.StringA(), destination.StringA());
    CPPUNIT_ASSERT_EQUAL(source.StdVector.size(), destination.StdVector.size());
    CPPUNIT_ASSERT(std::equal(source.StdVector.begin(), source.StdVector.end(), destination.StdVector.begin()));
    for (size_t index = 0; index < cmnDataGeneratorTestC::SIZE; ++index) {
        CPPUNIT_ASSERT_EQUAL(source.CArray[index], destination.CArray[index]);
    }
    for (size_t row = 0; row < cmnDataGeneratorTestC::ROW; ++row) {
        for (size_t col = 0; col < cmnDataGeneratorTestC::COL; ++col) {
            CPPUNIT_ASSERT_EQUAL(source.CArray2D[row][col], destination.CArray2D[row][col]);
        }
    }
}

void cmnDataGeneratorTest::TestTextSerialization(void)
{
    cmnDataGeneratorTestC source, destination;
    // modify source
    source.StringA() = "source with a comma , a back slash \\ and a forward slash /";
    source.StdVector.resize(10, 5);
    for (size_t index = 0; index < cmnDataGeneratorTestC::SIZE; ++index) {
        source.CArray[index] = index + 10.0;
    }
    for (size_t row = 0; row < cmnDataGeneratorTestC::ROW; ++row) {
        for (size_t col = 0; col < cmnDataGeneratorTestC::COL; ++col) {
            source.CArray2D[row][col] = row * 10 + col;
        }
    }
    // serialize/de-serialize
    cmnDataFormat local, remote; // same!
    std::stringstream stream;
    cmnData<cmnDataGeneratorTestC>::SerializeText(source, stream);
    CPPUNIT_ASSERT(!stream.fail());
    cmnData<cmnDataGeneratorTestC>::DeSerializeText(destination, stream);
    CPPUNIT_ASSERT(!stream.fail());
    // check result
    CPPUNIT_ASSERT_EQUAL(source.StringA(), destination.StringA());
    CPPUNIT_ASSERT_EQUAL(source.StdVector.size(), destination.StdVector.size());
    CPPUNIT_ASSERT(std::equal(source.StdVector.begin(), source.StdVector.end(), destination.StdVector.begin()));
    for (size_t index = 0; index < cmnDataGeneratorTestC::SIZE; ++index) {
        CPPUNIT_ASSERT_EQUAL(source.CArray[index], destination.CArray[index]);
    }
    for (size_t row = 0; row < cmnDataGeneratorTestC::ROW; ++row) {
        for (size_t col = 0; col < cmnDataGeneratorTestC::COL; ++col) {
            CPPUNIT_ASSERT_EQUAL(source.CArray2D[row][col], destination.CArray2D[row][col]);
        }
    }
}

void cmnDataGeneratorTest::TestScalars(void)
{
    cmnDataGeneratorTestC objectC;
    // starting with size of dynamic vector + 3 elements in C array + 12 in CArray2D
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1 + 0 + cmnDataGeneratorTestC::SIZE + cmnDataGeneratorTestC::ROW * cmnDataGeneratorTestC::COL),
                         cmnData<cmnDataGeneratorTestC>::ScalarNumber(objectC));
    // resize std::vector
    objectC.StdVector.resize(cmnDataGeneratorTestC::SIZE);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1 + cmnDataGeneratorTestC::SIZE + cmnDataGeneratorTestC::SIZE + cmnDataGeneratorTestC::ROW * cmnDataGeneratorTestC::COL),
                         cmnData<cmnDataGeneratorTestC>::ScalarNumber(objectC));
    // modify content directly
    for (size_t index = 0; index < cmnDataGeneratorTestC::SIZE; ++index) {
        objectC.StdVector.at(index) = static_cast<int>(index + 10);
        objectC.CArray[index] = static_cast<int>(index + 20);
    }
    for (size_t row = 0; row < cmnDataGeneratorTestC::ROW; ++row) {
        for (size_t col = 0; col < cmnDataGeneratorTestC::COL; ++col) {
            objectC.CArray2D[row][col] = row * 10 + col;
        }
    }
    // test scalars
    // first should be size of std vector
    CPPUNIT_ASSERT_EQUAL(static_cast<double>(cmnDataGeneratorTestC::SIZE), cmnData<cmnDataGeneratorTestC>::Scalar(objectC, 0));
    for (size_t index = 0; index < cmnDataGeneratorTestC::SIZE; ++index) {
        // std::vector, from 1 to 4, values 10 to 12
        CPPUNIT_ASSERT_EQUAL(10.0 + index, cmnData<cmnDataGeneratorTestC>::Scalar(objectC, 1 + index));
        // C array, from 5 to 7, values 20 to 22
        CPPUNIT_ASSERT_EQUAL(20.0 + index, cmnData<cmnDataGeneratorTestC>::Scalar(objectC,
                                                                                  1 + cmnDataGeneratorTestC::SIZE + index));
    }
    for (size_t row = 0; row < cmnDataGeneratorTestC::ROW; ++row) {
        for (size_t col = 0; col < cmnDataGeneratorTestC::COL; ++col) {
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(row * 10 + col),
                                 cmnData<cmnDataGeneratorTestC>::Scalar(objectC, 1 + 2 * cmnDataGeneratorTestC::SIZE + row * cmnDataGeneratorTestC::COL + col));
        }
    }
}
