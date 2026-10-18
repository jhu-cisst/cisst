/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2004-07-09

  (C) Copyright 2004-2022 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _vctGenericVectorTest_h
#define _vctGenericVectorTest_h

#include <cppunit/extensions/HelperMacros.h>

#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnPortability.h>


#include <iostream>
#include <algorithm>
#include <stdexcept>


/* Please check vctGenericContainerTest.h for some basic
   guidelines. */
class vctGenericVectorTest
{
 public:

    /*! Test assignements */
    template <class _containerType1, class _containerType2>
    static void TestAssignment(const _containerType1 & vector1,
                               _containerType2 & vector2) {
        size_t index;
        const size_t size = vector1.size();
        vector2.Assign(vector1);
        CPPUNIT_ASSERT(vector2.size() == size);
        for (index = 0; index < size; index++) {
            CPPUNIT_ASSERT(vector1[index] == vector2[index]);
        }
    }


    /* Test access methods */
    template <class _containerType1, class _containerType2>
        static void TestAccessMethods(_containerType1 & vector1,
                                      const _containerType2 & vector2) {
        size_t index;
        const size_t size = vector1.size();
        typedef typename _containerType1::value_type value_type;
        value_type element;

        for (index = 0; index < size; index++) {
            element = vector1[index];
            CPPUNIT_ASSERT(vector2[index] == element);
            CPPUNIT_ASSERT(vector1.at(index) == element);
            CPPUNIT_ASSERT(vector2.at(index) == element);
            CPPUNIT_ASSERT(vector1(index) == element);
            CPPUNIT_ASSERT(vector2(index) == element);
            CPPUNIT_ASSERT(vector1.Element(index) == element);
            CPPUNIT_ASSERT(vector2.Element(index) == element);
            CPPUNIT_ASSERT(*(vector1.Pointer(index)) == element);
            CPPUNIT_ASSERT(*(vector2.Pointer(index)) == element);
            CPPUNIT_ASSERT(vector1.ValidIndex(index));
        }

        CPPUNIT_ASSERT(!vector1.ValidIndex(vector1.size()));

        bool gotException = false;
        try {
            vector2.at(size);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

        gotException = false;
        try {
            vector1.at(size);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

        gotException = false;
        try {
            vector2(size);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

        gotException = false;
        try {
            vector1(size);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

    }


    /*! Test special element name methods: X(), Y(), XY(), XYZ(), etc.
      The test asserts that each element of these subvectors has the
      same address as may be expected with respect to the input
      vector.  Note that the element address of some subvectors, such
      as YZ(), must be offset with respect to the input.
    */
    template<class _containerType>
    static void TestConstXYZW(const _containerType & vector1)
    {
        if (vector1.size() > 0) {
            CPPUNIT_ASSERT(&(vector1.X()) == vector1.Pointer(0));
        }

        if (vector1.size() > 1) {
            CPPUNIT_ASSERT(&(vector1.Y()) == vector1.Pointer(1));
            CPPUNIT_ASSERT(vector1.XY().Pointer(0) == vector1.Pointer(0));
            CPPUNIT_ASSERT(vector1.XY().Pointer(1) == vector1.Pointer(1));
        }

        if (vector1.size() > 2) {
            CPPUNIT_ASSERT(&(vector1.Z()) == vector1.Pointer(2));
            CPPUNIT_ASSERT(vector1.XZ().Pointer(0) == vector1.Pointer(0));
            CPPUNIT_ASSERT(vector1.XZ().Pointer(1) == vector1.Pointer(2));
            CPPUNIT_ASSERT(vector1.YZ().Pointer(0) == vector1.Pointer(1));
            CPPUNIT_ASSERT(vector1.YZ().Pointer(1) == vector1.Pointer(2));
            CPPUNIT_ASSERT(vector1.XYZ().Pointer(0) == vector1.Pointer(0));
            CPPUNIT_ASSERT(vector1.XYZ().Pointer(1) == vector1.Pointer(1));
            CPPUNIT_ASSERT(vector1.XYZ().Pointer(2) == vector1.Pointer(2));
        }

        if (vector1.size() > 3) {
            CPPUNIT_ASSERT(&(vector1.W()) == vector1.Pointer(3));
            CPPUNIT_ASSERT(vector1.XW().Pointer(0) == vector1.Pointer(0));
            CPPUNIT_ASSERT(vector1.XW().Pointer(1) == vector1.Pointer(3));
            CPPUNIT_ASSERT(vector1.YW().Pointer(0) == vector1.Pointer(1));
            CPPUNIT_ASSERT(vector1.YW().Pointer(1) == vector1.Pointer(3));
            CPPUNIT_ASSERT(vector1.ZW().Pointer(0) == vector1.Pointer(2));
            CPPUNIT_ASSERT(vector1.ZW().Pointer(1) == vector1.Pointer(3));
            CPPUNIT_ASSERT(vector1.YZW().Pointer(0) == vector1.Pointer(1));
            CPPUNIT_ASSERT(vector1.YZW().Pointer(1) == vector1.Pointer(2));
            CPPUNIT_ASSERT(vector1.YZW().Pointer(2) == vector1.Pointer(3));
            CPPUNIT_ASSERT(vector1.XYZW().Pointer(0) == vector1.Pointer(0));
            CPPUNIT_ASSERT(vector1.XYZW().Pointer(1) == vector1.Pointer(1));
            CPPUNIT_ASSERT(vector1.XYZW().Pointer(2) == vector1.Pointer(2));
            CPPUNIT_ASSERT(vector1.XYZW().Pointer(3) == vector1.Pointer(3));
        }

    }  // TestConstXYZW


    /*! Test special element name methods: X(), Y(), XY(), XYZ(), etc.
      The test asserts that each element of these subvectors has the
      same address as may be expected with respect to the input
      vector.  Note that the element address of some subvectors, such
      as YZ(), must be offset with respect to the input.  This test
      can only be applied to non-const input, sinc it attempts to
      write to the X() element of the input.
    */
    template<class _containerType>
    static void TestNonConstXYZW(_containerType & vector1)
    {
        typedef typename _containerType::value_type value_type;

        if (vector1.size() > 0) {
            CPPUNIT_ASSERT(&(vector1.X()) == vector1.Pointer(0));
            const value_type newX = vector1.X() - value_type(1);
            CPPUNIT_ASSERT(vector1.X() != newX);
            vector1.X() = newX;
            CPPUNIT_ASSERT(vector1.X() == newX);
        }

        if (vector1.size() > 1) {
            CPPUNIT_ASSERT(&(vector1.Y()) == vector1.Pointer(1));
            CPPUNIT_ASSERT(vector1.XY().Pointer(0) == vector1.Pointer(0));
            CPPUNIT_ASSERT(vector1.XY().Pointer(1) == vector1.Pointer(1));
        }

        if (vector1.size() > 2) {
            CPPUNIT_ASSERT(&(vector1.Z()) == vector1.Pointer(2));
            CPPUNIT_ASSERT(vector1.XZ().Pointer(0) == vector1.Pointer(0));
            CPPUNIT_ASSERT(vector1.XZ().Pointer(1) == vector1.Pointer(2));
            CPPUNIT_ASSERT(vector1.YZ().Pointer(0) == vector1.Pointer(1));
            CPPUNIT_ASSERT(vector1.YZ().Pointer(1) == vector1.Pointer(2));
            CPPUNIT_ASSERT(vector1.XYZ().Pointer(0) == vector1.Pointer(0));
            CPPUNIT_ASSERT(vector1.XYZ().Pointer(1) == vector1.Pointer(1));
            CPPUNIT_ASSERT(vector1.XYZ().Pointer(2) == vector1.Pointer(2));
        }

        if (vector1.size() > 3) {
            CPPUNIT_ASSERT(&(vector1.W()) == vector1.Pointer(3));
            CPPUNIT_ASSERT(vector1.XW().Pointer(0) == vector1.Pointer(0));
            CPPUNIT_ASSERT(vector1.XW().Pointer(1) == vector1.Pointer(3));
            CPPUNIT_ASSERT(vector1.YW().Pointer(0) == vector1.Pointer(1));
            CPPUNIT_ASSERT(vector1.YW().Pointer(1) == vector1.Pointer(3));
            CPPUNIT_ASSERT(vector1.ZW().Pointer(0) == vector1.Pointer(2));
            CPPUNIT_ASSERT(vector1.ZW().Pointer(1) == vector1.Pointer(3));
            CPPUNIT_ASSERT(vector1.YZW().Pointer(0) == vector1.Pointer(1));
            CPPUNIT_ASSERT(vector1.YZW().Pointer(1) == vector1.Pointer(2));
            CPPUNIT_ASSERT(vector1.YZW().Pointer(2) == vector1.Pointer(3));
            CPPUNIT_ASSERT(vector1.XYZW().Pointer(0) == vector1.Pointer(0));
            CPPUNIT_ASSERT(vector1.XYZW().Pointer(1) == vector1.Pointer(1));
            CPPUNIT_ASSERT(vector1.XYZW().Pointer(2) == vector1.Pointer(2));
            CPPUNIT_ASSERT(vector1.XYZW().Pointer(3) == vector1.Pointer(3));
        }

    }  // TestNonConstXYZW


    /*! Test SelectFrom and Select operations. */
    template <class _inputContainerType, class _indexContainerType, class _outputContainerType>
    static void TestSelect(const _inputContainerType  & input,
                           const _indexContainerType  & index,
                           _outputContainerType & output)
    {
        CPPUNIT_ASSERT(index.size() == output.size());

        CPPUNIT_ASSERT(index.Lesser( input.size() ));

        const typename _outputContainerType::const_iterator endOut = output.end();
        typename _outputContainerType::const_iterator iterOut;
        typename _indexContainerType::const_iterator iterInd;

        output.SelectFrom(input, index);

        iterOut = output.begin();
        iterInd = index.begin();

        for (; iterOut != endOut; ++iterOut, ++iterInd) {
            CPPUNIT_ASSERT((*iterOut) == input.Element(*iterInd));
        }
    }

    /*! Test cross product. */
    template <class _containerType1, class _containerType2, class _containerType3>
    static void TestCrossProduct(const _containerType1 & vector1,
                                 const _containerType2 & vector2,
                                 _containerType3 & vector3,
                                 typename _containerType1::value_type tolerance
                                 = cmnTypeTraits<typename _containerType1::value_type>::Tolerance()) {
        typedef typename _containerType1::value_type value_type;
        const int numOperations = 27; // 3 * 3 * 3
        const value_type actTolerance = tolerance * value_type(numOperations);

        vector3.CrossProductOf(vector1, vector2);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(vector3.DotProduct(vector1), value_type(0), actTolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(vector3.DotProduct(vector2), value_type(0), actTolerance);

        vector3 = vector1 % vector2;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(vector3.DotProduct(vector1), value_type(0), actTolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(vector3.DotProduct(vector2), value_type(0), actTolerance);

        vector3 = vctCrossProduct(vector1, vector2);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(vector3.DotProduct(vector1), value_type(0), actTolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(vector3.DotProduct(vector2), value_type(0), actTolerance);
    }



    /*! Test dot product. */
    template <class _containerType1, class _containerType2>
    static void TestDotProduct(const _containerType1 & vector1,
                               const _containerType2 & vector2,
                               typename _containerType1::value_type tolerance
                               = cmnTypeTraits<typename _containerType1::value_type>::Tolerance()) {

        CPPUNIT_ASSERT(vector1.size() == vector2.size());
        typedef typename _containerType1::value_type value_type;

        const size_t size = vector1.size();
        value_type result = value_type(0);
        size_t index;
        for (index = 0; index < size; index++) {
            result += (vector1[index] * vector2[index]);
        }

        VCT_CPPUNIT_ASSERT_DOUBLES_EQUAL_CAST(result, vector1.DotProduct(vector2), tolerance * size);
        VCT_CPPUNIT_ASSERT_DOUBLES_EQUAL_CAST(result, vector2.DotProduct(vector1), tolerance * size);
        VCT_CPPUNIT_ASSERT_DOUBLES_EQUAL_CAST(result, vctDotProduct(vector1, vector2), tolerance * size);
        VCT_CPPUNIT_ASSERT_DOUBLES_EQUAL_CAST(result, vctDotProduct(vector2, vector1), tolerance * size);
        VCT_CPPUNIT_ASSERT_DOUBLES_EQUAL_CAST(result, vector1 * vector2, tolerance * size);
        VCT_CPPUNIT_ASSERT_DOUBLES_EQUAL_CAST(result, vector2 * vector1, tolerance * size);
    }

    /*! Store copies of vector1 and vector2 in local variables.  Swap
      the elements of vector1 with vector2, and assert that the swapped
      vectors are equal to the copies.  Swap again the elements of
      vector2 with vector1, and assert that the vectors are equal to
      the copies. */
    template<class _containerType1, class _containerType2>
    static void TestSwapElements(_containerType1 & vector1, _containerType2 & vector2)
    {
        typename _containerType1::CopyType tmp1(vector1);
        typename _containerType2::CopyType tmp2(vector2);

        vector1.SwapElementsWith(vector2);
        CPPUNIT_ASSERT(tmp1 == vector2);
        CPPUNIT_ASSERT(tmp2 == vector1);

        vector2.SwapElementsWith(vector1);
        CPPUNIT_ASSERT(tmp1 == vector1);
        CPPUNIT_ASSERT(tmp2 == vector2);
    }

    /*! Test the normalization */
    template<class _containerType1>
    static void TestNormalization(_containerType1 & vector1,
                                  typename _containerType1::value_type tolerance
                                  = cmnTypeTraits<typename _containerType1::value_type>::Tolerance())
    {
        const typename _containerType1::CopyType vector1Copy(vector1);
        typename _containerType1::CopyType tmp1(vector1);
        typedef typename _containerType1::value_type value_type;
        value_type vector1Norm = static_cast<value_type>(vector1.Norm());

        // Make sure the vector's norm is nonzero
        if (vector1Norm < tolerance) {
            vector1.Add(value_type(1));
            vector1Norm = static_cast<value_type>(vector1.Norm());
        }

        // Make sure this vector is not already normalized
        if (vector1.IsNormalized()) {
            vector1.Add(value_type(2));
            vector1Norm = static_cast<value_type>(vector1.Norm());
        }
        CPPUNIT_ASSERT(!vector1.IsNormalized());

        tmp1.NormalizedOf(vector1);
        CPPUNIT_ASSERT(tmp1.IsNormalized());
        tmp1.Multiply(vector1Norm);
        tmp1.Subtract(vector1);
        CPPUNIT_ASSERT(tmp1.Norm() <= tolerance);

        tmp1 = vector1.Normalized();
        CPPUNIT_ASSERT(tmp1.IsNormalized());
        tmp1.Multiply(vector1Norm);
        tmp1.Subtract(vector1);
        CPPUNIT_ASSERT(tmp1.Norm() <= tolerance);

        vector1.NormalizedSelf();
        CPPUNIT_ASSERT(vector1.IsNormalized());

        // Restore the old value of vector1
        vector1.Assign(vector1Copy);
    }
};

#endif // _vctGenericVectorTest_h
