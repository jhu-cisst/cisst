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
#include <cisstCommon/cmnDataFunctionsArray.h>
#include <cisstCommon/cmnDataFunctionsJSON.h>
#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>

void cmnDataFunctionsVectorTest::TestStandardContainers(void)
{
    const cmnDataFormat format;
    for (const std::vector<bool> & source : {std::vector<bool>{}, {false}, {true, false, true}}) {
        std::vector<bool> data;
        cmnData<std::vector<bool>>::Copy(data, source);
        CPPUNIT_ASSERT(data == source);

        std::stringstream text;
        cmnData<std::vector<bool>>::SerializeText(source, text, ',');
        data.clear();
        cmnData<std::vector<bool>>::DeSerializeText(data, text, ',');
        CPPUNIT_ASSERT(data == source);

        std::stringstream binary;
        cmnData<std::vector<bool>>::SerializeBinary(source, binary);
        data.clear();
        cmnData<std::vector<bool>>::DeSerializeBinary(data, binary, format, format);
        CPPUNIT_ASSERT(data == source);

        std::stringstream raw;
        cmnSerializeRaw(raw, source);
        std::stringstream expectedRaw;
        cmnSerializeSizeRaw(expectedRaw, source.size());
        for (const bool value : source) {
            cmnSerializeRaw(expectedRaw, value);
        }
        CPPUNIT_ASSERT_EQUAL(expectedRaw.str(), raw.str());
        data.clear();
        cmnDeSerializeRaw(raw, data);
        CPPUNIT_ASSERT(data == source);

#if CISST_HAS_JSON
        Json::Value json;
        cmnDataJSON<std::vector<bool>>::SerializeText(source, json);
        data.clear();
        cmnDataJSON<std::vector<bool>>::DeSerializeText(data, json);
        CPPUNIT_ASSERT(data == source);
#endif
    }

    typedef std::array<double, 3> ArrayType;
    const ArrayType source{0.3, 700.0, 0.5};
    ArrayType data{};
    cmnData<ArrayType>::Copy(data, source);
    CPPUNIT_ASSERT(data == source);
    CPPUNIT_ASSERT(cmnData<ArrayType>::ScalarNumberIsFixed(source));
    CPPUNIT_ASSERT_EQUAL(size_t(3), cmnData<ArrayType>::ScalarNumber(source));
    CPPUNIT_ASSERT_EQUAL(700.0, cmnData<ArrayType>::Scalar(source, 1));
    CPPUNIT_ASSERT(!cmnData<ArrayType>::HumanReadable(source).empty());
    CPPUNIT_ASSERT(!cmnData<ArrayType>::SerializeDescription(source, ',').empty());
    CPPUNIT_ASSERT(!cmnData<ArrayType>::ScalarDescription(source, 2).empty());
    CPPUNIT_ASSERT_THROW(cmnData<ArrayType>::Scalar(source, 3), std::out_of_range);

    std::stringstream text;
    cmnData<ArrayType>::SerializeText(source, text, ',');
    data.fill(0.0);
    cmnData<ArrayType>::DeSerializeText(data, text, ',');
    CPPUNIT_ASSERT(data == source);

    std::stringstream binary;
    cmnData<ArrayType>::SerializeBinary(source, binary);
    data.fill(0.0);
    cmnData<ArrayType>::DeSerializeBinary(data, binary, format, format);
    CPPUNIT_ASSERT(data == source);

    std::stringstream raw;
    cmnSerializeRaw(raw, source);
    data.fill(0.0);
    cmnDeSerializeRaw(raw, data);
    CPPUNIT_ASSERT(data == source);

    // Array elements can themselves own dynamic storage.
    const std::array<std::vector<double>, 2> nested{{{1.0, 2.0}, {}}};
    std::array<std::vector<double>, 2> nestedRestored;
    std::stringstream nestedRaw;
    cmnSerializeRaw(nestedRaw, nested);
    cmnDeSerializeRaw(nestedRaw, nestedRestored);
    CPPUNIT_ASSERT(nestedRestored == nested);

    typedef std::array<double, 0> EmptyArray;
    const EmptyArray empty{};
    CPPUNIT_ASSERT(cmnData<EmptyArray>::ScalarNumberIsFixed(empty));
    CPPUNIT_ASSERT_EQUAL(size_t(0), cmnData<EmptyArray>::ScalarNumber(empty));
    CPPUNIT_ASSERT_THROW(cmnData<EmptyArray>::Scalar(empty, 0), std::out_of_range);
}

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
