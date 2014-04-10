/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2011-06-27

  (C) Copyright 2011-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnDataFunctionsVectorTest.h"

#include <cisstCommon/cmnDataFunctionsString.h>
#include <cisstCommon/cmnDataFunctionsVector.h>

void cmnDataFunctionsVectorTest::TestCopy(void)
{
    {
        typedef std::vector<int> VectorType;
        VectorType source, data;
        source.resize(10);
        for (size_t index = 0; index < source.size(); ++index) {
            source[index] = static_cast<int>(index);
        }
        cmnData<VectorType>::Copy(data, source);
        CPPUNIT_ASSERT_EQUAL(data.size(), source.size());
        for (size_t index = 0; index < source.size(); ++index) {
            CPPUNIT_ASSERT_EQUAL(data[index], source[index]);
        }
    }

    {
        typedef std::vector<double> VectorType;
        VectorType source, data;
        source.resize(10);
        for (size_t index = 0; index < source.size(); ++index) {
            source[index] = 1.0 / static_cast<double>(index);
        }
        cmnData<VectorType>::Copy(data, source);
        CPPUNIT_ASSERT_EQUAL(data.size(), source.size());
        for (size_t index = 0; index < source.size(); ++index) {
            CPPUNIT_ASSERT_EQUAL(data[index], source[index]);
        }
    }

    {
        typedef std::vector<double> SubVectorType;
        typedef std::vector<SubVectorType> VectorType;
        VectorType source, data;
        source.resize(10);
        for (size_t index = 0; index < source.size(); ++index) {
            source[index].resize(index);
            for (size_t subIndex = 0; subIndex < source[index].size(); ++subIndex) {
                source[index][subIndex] = static_cast<double>(subIndex);
            }
        }
        cmnData<VectorType>::Copy(data, source);
        CPPUNIT_ASSERT_EQUAL(data.size(), source.size());
        for (size_t index = 0; index < source.size(); ++index) {
            CPPUNIT_ASSERT_EQUAL(data[index].size(), source[index].size());
            for (size_t subIndex = 0; subIndex < source[index].size(); ++subIndex) {
                CPPUNIT_ASSERT_EQUAL(data[index][subIndex], source[index][subIndex]);
            }
        }
    }
}


void cmnDataFunctionsVectorTest::TestBinarySerializationStream(void)
{
    cmnDataFormat local, remote;

    std::stringstream stream;

    {
        typedef std::vector<double> SubVectorType;
        typedef std::vector<SubVectorType> VectorType;
        VectorType source, destination;
        source.resize(10);
        for (size_t index = 0; index < source.size(); ++index) {
            source[index].resize(index);
            for (size_t subIndex = 0; subIndex < source[index].size(); ++subIndex) {
                source[index][subIndex] = static_cast<double>(subIndex);
            }
        }

        cmnData<VectorType>::SerializeBinary(source, stream);
        cmnData<VectorType>::DeSerializeBinary(destination, stream, local, remote);
        CPPUNIT_ASSERT_EQUAL(destination.size(), source.size());
        for (size_t index = 0; index < source.size(); ++index) {
            CPPUNIT_ASSERT_EQUAL(destination[index].size(), source[index].size());
            for (size_t subIndex = 0; subIndex < source[index].size(); ++subIndex) {
                CPPUNIT_ASSERT_EQUAL(destination[index][subIndex], source[index][subIndex]);
            }
        }
    }
}


void cmnDataFunctionsVectorTest::TestTextSerializationStream(void)
{
}


void cmnDataFunctionsVectorTest::TestScalars(void)
{
    {
        typedef std::vector<int> VectorType;
        VectorType source;
        source.resize(10);
        for (size_t index = 0; index < source.size(); ++index) {
            source[index] = static_cast<int>(index);
        }
        CPPUNIT_ASSERT_EQUAL(source.size() + 1, cmnData<VectorType>::ScalarNumber(source));
        for (size_t index = 0; index < source.size(); ++index) {
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(source[index]), cmnData<VectorType>::Scalar(source, index + 1));
        }
    }

    {
        typedef std::vector<std::string> VectorType;
        VectorType source;
        source.resize(10);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), cmnData<VectorType>::ScalarNumber(source));
    }

    {
        typedef std::vector<double> SubVectorType;
        typedef std::vector<SubVectorType> VectorType;
        VectorType source;
        size_t nbScalars = 0;
        source.resize(10);
        nbScalars += 1 + source.size(); // all sizes, one for the vector of vector and one for each sub-vector
        for (size_t index = 0; index < source.size(); ++index) {
            source[index].resize(index);
            nbScalars += source[index].size();
            for (size_t subIndex = 0; subIndex < source[index].size(); ++subIndex) {
                source[index][subIndex] = static_cast<double>(subIndex);
            }
        }
        CPPUNIT_ASSERT_EQUAL(nbScalars, cmnData<VectorType>::ScalarNumber(source));
        size_t counter = 0;
        // size of vector of vectors
        CPPUNIT_ASSERT_EQUAL(static_cast<double>(10), cmnData<VectorType>::Scalar(source, counter));
        counter++;
        for (size_t index = 0; index < source.size(); ++index) {
            // size of sub-vector
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(index), cmnData<VectorType>::Scalar(source, counter));
            counter++;
            for (size_t subIndex = 0; subIndex < source[index].size(); ++subIndex) {
                CPPUNIT_ASSERT_EQUAL(source[index][subIndex], cmnData<VectorType>::Scalar(source, counter));
                counter++;
            }
        }
    }
}
