/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2004-11-12

  (C) Copyright 2004-2022 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _vctGenericContainerTest_h
#define _vctGenericContainerTest_h

#include <cppunit/extensions/HelperMacros.h>

#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnPortability.h>

#include <cisstVector/vctContainerTraits.h>

#include <iostream>
#include <algorithm>
#include <stdexcept>


#define VCT_CPPUNIT_ASSERT_DOUBLES_EQUAL_CAST(goal, actual, tolerance) \
    CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(goal), static_cast<double>(actual), static_cast<double>(tolerance));

template <class _containerType>
void RemoveQuasiZero(_containerType & container) {
    typedef typename _containerType::value_type value_type;
    typedef typename _containerType::iterator iterator;
    const iterator end = container.end();
    iterator iter;
    for (iter = container.begin(); iter != end; ++iter) {
        if (((*iter) < value_type(1))
            && ((*iter) > -value_type(1))) {
            (*iter) = value_type(3);
        }
    }
}


/*!  This class is a compound of tests based on the signature of the
  methods.  These tests should work for any container of cisstVector.
  The tests themselves rely on different approaches:

  - Compute the result a different way.  E.g. for SumOfElements,
    iterate using an iterator and accumulate.

  - Tests some specific cases such as neutral elements.
    E.g. container.SetAll(0); container.Norm() == 0;

  - Find some invariants based on the operations. E.g. c = a + b; c -=
    b; c == a;

  By convention, the methods don't allocate any memory.  The caller
  must provide a container to store some temporary values.  By
  convention, this is the last container passed to each method.  The
  motivation behinbd this design is to allow to test containers
  referencing an external block of memory
  (e.g. vctFixedSizeVectorRef).  If one use a container by reference,
  it is impossible to deduct which type of container to use to
  actually store a temporary variable.

  Finally, some operations might perform some divisions by zero.  It
  is the responsability of the caller to avoid that.  The method
  RemoveQuasiZero has been written for that purpose.
*/
class vctGenericContainerTest
{
 public:

    /*! Test SoCi based operations. */
    template <class _containerType1, class _containerType2>
    static void TestSoCiOperations(_containerType1 & container1,
                                   _containerType2 & container2,
                                   typename _containerType1::value_type tolerance
                                   = cmnTypeTraits<typename _containerType1::value_type>::Tolerance()) {

        typedef typename _containerType1::value_type value_type;

        CPPUNIT_ASSERT(container1.size() == container2.size());
        CPPUNIT_ASSERT(container1.size() > 2); // to have at least three elements
        const typename _containerType1::const_iterator end1 = container1.end();
        typename _containerType1::const_iterator iter1;
        typename _containerType1::value_type resultScalar;

        resultScalar = container1.SumOfElements();
        typedef typename _containerType1::value_type value_type;
        value_type goal(0);
        value_type abs;
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            goal += (*iter1);
        }
        CPPUNIT_ASSERT(!cmnTypeTraits<value_type>::IsNaN(resultScalar));
        VCT_CPPUNIT_ASSERT_DOUBLES_EQUAL_CAST(goal, resultScalar, tolerance);

        container2.Zeros();
        VCT_CPPUNIT_ASSERT_DOUBLES_EQUAL_CAST(value_type(0), container2.SumOfElements(), tolerance);
        container2 = value_type(1);
        VCT_CPPUNIT_ASSERT_DOUBLES_EQUAL_CAST(value_type(container2.size()), container2.SumOfElements(), tolerance);

        resultScalar = container1.ProductOfElements();
        goal = value_type(1);
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            goal *= (*iter1);
        }
        CPPUNIT_ASSERT(!cmnTypeTraits<value_type>::IsNaN(resultScalar));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(goal), static_cast<double>(resultScalar), static_cast<double>(tolerance * container1.size()));

        container2.SetAll(value_type(1));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(1.0), static_cast<double>(container2.ProductOfElements()), static_cast<double>(tolerance));

        resultScalar = container1.L1Norm();
        goal = value_type(0);
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            abs = (*iter1) > 0 ? (*iter1) : -(*iter1);
            goal += abs;
        }
        CPPUNIT_ASSERT(!cmnTypeTraits<value_type>::IsNaN(resultScalar));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(goal), static_cast<double>(resultScalar), static_cast<double>(tolerance));

        container2 = value_type(0);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(0), container2.L1Norm(), tolerance);
        container2.SetAll(value_type(1));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(container2.size()), container2.L1Norm(), tolerance);

        resultScalar = container1.LinfNorm();
        goal = cmnTypeTraits<value_type>::MinPositiveValue();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            abs = (*iter1) > 0 ? (*iter1) : -(*iter1);
            if (abs > goal) {
                goal = abs;
            }
        }
        CPPUNIT_ASSERT(!cmnTypeTraits<value_type>::IsNaN(resultScalar));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(goal), resultScalar, tolerance);
        resultScalar = container1.MaxAbsElement();

        container2.Zeros();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(value_type(0), container2.LinfNorm(), tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(value_type(0), container2.MaxAbsElement(), tolerance);
        container2.SetAll(value_type(1));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(value_type(1), container2.LinfNorm(), tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(value_type(1), container2.MaxAbsElement(), tolerance);

        resultScalar = container1.NormSquare();
        goal = value_type(0);
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            goal += ((*iter1) * (*iter1));
        }
        CPPUNIT_ASSERT(!cmnTypeTraits<value_type>::IsNaN(resultScalar));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(goal, resultScalar, tolerance * goal);

        container2.Zeros();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(value_type(0), container2.NormSquare(), tolerance);
        container2.SetAll(value_type(1));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(value_type(container2.size()), container2.NormSquare(), tolerance);

        double norm = container1.Norm();
        goal = value_type(0);
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            goal += ((*iter1) * (*iter1));
        }
        CPPUNIT_ASSERT(!cmnTypeTraits<double>::IsNaN(norm));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt((double)goal), norm, tolerance * goal);

        container2.Zeros();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(value_type(0), container2.Norm(), cmnTypeTraits<double>::Tolerance());
        container2.SetAll(value_type(1));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt((double)container2.size()), container2.Norm(), cmnTypeTraits<double>::Tolerance());

        resultScalar = container1.MaxElement();
        goal = cmnTypeTraits<value_type>::MinusInfinityOrMin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            if ((*iter1) > goal) {
                goal = (*iter1);
            }
        }
        CPPUNIT_ASSERT_DOUBLES_EQUAL(goal, resultScalar, tolerance);

        container2.Zeros();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(value_type(0), container2.MaxElement(), tolerance);
        container2.SetAll(value_type(1));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(value_type(1), container2.MaxElement(), tolerance);

        resultScalar = container1.MinElement();
        goal = cmnTypeTraits<value_type>::PlusInfinityOrMax();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            if ((*iter1) < goal) {
                goal = (*iter1);
            }
        }
        CPPUNIT_ASSERT_DOUBLES_EQUAL(goal, resultScalar, tolerance);

        container2.Zeros();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(value_type(0), container2.MinElement(), tolerance);
        container2.SetAll(value_type(1));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(value_type(1), container2.MinElement(), tolerance);

        resultScalar = container1.MinAbsElement();
        goal = cmnTypeTraits<value_type>::MaxPositiveValue();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            abs = (*iter1) > 0 ? (*iter1) : -(*iter1);
            if (abs < goal) {
                goal = abs;
            }
        }
        CPPUNIT_ASSERT(!cmnTypeTraits<value_type>::IsNaN(resultScalar));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(goal, resultScalar, tolerance);
        resultScalar = container1.MinAbsElement();

        container2.Zeros();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(value_type(0), container2.MinAbsElement(), tolerance);
        container2.SetAll(value_type(1));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(value_type(1), container2.MinAbsElement(), tolerance);

        container2.Assign(container1);
        RemoveQuasiZero(container2);
        (*(container2.begin())) = value_type(-1);
        (*(container2.end() - 1)) = value_type(1);
        CPPUNIT_ASSERT(!container2.IsNonNegative());
        CPPUNIT_ASSERT(!container2.IsNonPositive());
        CPPUNIT_ASSERT(!container2.IsNegative());
        CPPUNIT_ASSERT(!container2.IsPositive());

        container2.AbsSelf();
        CPPUNIT_ASSERT(container2.IsNonNegative());
        CPPUNIT_ASSERT(!container2.IsNonPositive());
        CPPUNIT_ASSERT(!container2.IsNegative());
        CPPUNIT_ASSERT(container2.IsPositive());

        container2.NegationSelf();
        CPPUNIT_ASSERT(!container2.IsNonNegative());
        CPPUNIT_ASSERT(container2.IsNonPositive());
        CPPUNIT_ASSERT(container2.IsNegative());
        CPPUNIT_ASSERT(!container2.IsPositive());

        CPPUNIT_ASSERT(!container2.HasNaN());
        CPPUNIT_ASSERT(container2.IsFinite());

        if (cmnTypeTraits<value_type>::HasNaN()) {
            value_type oldValue = (*container2.begin());
            (*container2.begin()) = cmnTypeTraits<value_type>::NaN();
            CPPUNIT_ASSERT(container2.HasNaN());
            (*container2.begin()) = oldValue;
        }

        if (cmnTypeTraits<value_type>::HasInfinity()) {
            value_type oldValue = (*container2.begin());
            (*container2.begin()) = cmnTypeTraits<value_type>::PlusInfinityOrMax();
            CPPUNIT_ASSERT(!container2.IsFinite());
            (*container2.begin()) = oldValue;
        }

        if (cmnTypeTraits<value_type>::HasInfinity()) {
            value_type oldValue = (*container2.begin());
            (*container2.begin()) = cmnTypeTraits<value_type>::MinusInfinityOrMin();
            CPPUNIT_ASSERT(!container2.IsFinite());
            (*container2.begin()) = oldValue;
        }
    }




    /*! Test SoCiCi based operations. */
    template <class _containerType1, class _containerType2, class _containerType3>
    static void TestSoCiCiOperations(_containerType1 & container1,
                                     _containerType2 & container2,
                                     _containerType3 & container3) {

        CPPUNIT_ASSERT(container1.size() == container2.size());
        CPPUNIT_ASSERT(container2.size() == container3.size());

        typedef typename _containerType1::value_type value_type;
        container3.SumOf(container1, value_type(1));
        if (container1.size() > 0) {
            *(container3.begin()) = *(container1.begin());
        }
        CPPUNIT_ASSERT(container3.GreaterOrEqual(container1));
        CPPUNIT_ASSERT(!(container3.Greater(container1)));
        CPPUNIT_ASSERT(vctAll(container3.ElementwiseGreaterOrEqual(container1)));
        CPPUNIT_ASSERT(!(vctAll(container3.ElementwiseGreater(container1))));
        CPPUNIT_ASSERT(vctAny(container3.ElementwiseGreater(container1)));

        CPPUNIT_ASSERT(container1.LesserOrEqual(container3));
        CPPUNIT_ASSERT(!(container1.Lesser(container3)));
        CPPUNIT_ASSERT(vctAll(container1.ElementwiseLesserOrEqual(container3)));
        CPPUNIT_ASSERT(!(vctAll(container1.ElementwiseLesser(container3))));
        CPPUNIT_ASSERT(vctAny(container1.ElementwiseLesser(container3)));

        container3.Add(value_type(1));
        CPPUNIT_ASSERT(container3.NotEqual(container1));
        CPPUNIT_ASSERT(container3 != container1);
        CPPUNIT_ASSERT(vctAll(container3.ElementwiseNotEqual(container1)));
        CPPUNIT_ASSERT(!(vctAny(container3.ElementwiseEqual(container1))));

        CPPUNIT_ASSERT(container3.Greater(container1));
        CPPUNIT_ASSERT(vctAll(container3.ElementwiseGreater(container1)));
        CPPUNIT_ASSERT(container1.Lesser(container3));
        CPPUNIT_ASSERT(vctAll(container1.ElementwiseLesser(container3)));

        container3.Assign(container1);
        CPPUNIT_ASSERT(container1.Equal(container3));
        CPPUNIT_ASSERT(container1.AlmostEqual(container3));
        CPPUNIT_ASSERT(container1 == container3);
        CPPUNIT_ASSERT(vctAll(container1.ElementwiseEqual(container3)));
        CPPUNIT_ASSERT(!(vctAny(container1.ElementwiseNotEqual(container3))));
        CPPUNIT_ASSERT(container1.GreaterOrEqual(container3));
        CPPUNIT_ASSERT(container1.LesserOrEqual(container3));
        CPPUNIT_ASSERT(!(container1.Lesser(container3)));
        CPPUNIT_ASSERT(!(container1.Greater(container3)));
        CPPUNIT_ASSERT(!(container1.NotEqual(container3)));
        CPPUNIT_ASSERT(!(container1 != container3));
    }




    /*! Test CioCi based operations. */
    template <class _containerType1, class _containerType2, class _containerType3>
    static void TestCioCiOperations(_containerType1 & container1,
                                    const _containerType2 & container2,
                                    _containerType3 & container3,
                                    typename _containerType1::value_type tolerance
                                    = cmnTypeTraits<typename _containerType1::value_type>::Tolerance()) {

        CPPUNIT_ASSERT(container1.size() == container2.size());
        CPPUNIT_ASSERT(container2.size() == container3.size());

        const typename _containerType1::const_iterator end1 = container1.end();
        typename _containerType1::const_iterator iter1;
        typename _containerType2::const_iterator iter2;
        typename _containerType3::const_iterator iter3;

        container3.Assign(container1);
        container3.Add(container2);
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter3), (*iter1) + (*iter2), tolerance);
            ++iter2; ++iter3;
        }

        container3.Assign(container1);
        container3 += container2;
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter3), (*iter1) + (*iter2), tolerance);
            ++iter2; ++iter3;
        }

        container3.Assign(container1);
        container3.Subtract(container2);
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter3), (*iter1) - (*iter2), tolerance);
            ++iter2; ++iter3;
        }

        container3.Assign(container1);
        container3 -= container2;
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter3), (*iter1) - (*iter2), tolerance);
            ++iter2; ++iter3;
        }

        container3.Assign(container1);
        container3.Add(container2);
        container3.Subtract(container2);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, (container3 - container1).LinfNorm(), tolerance);

        container3.Assign(container1);
        container3 += container2;
        container3 -= container2;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, (container3 - container1).LinfNorm(), tolerance);

        container3.Assign(container1);
        container3 -= container2;
        container3 += container2;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, (container3 - container1).LinfNorm(), tolerance);

        container3.Assign(container1);
        container3.ElementwiseMultiply(container2);
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter3), (*iter1) * (*iter2), tolerance);
            ++iter2; ++iter3;
        }

        container3.Assign(container1);
        container3.ElementwiseDivide(container2);
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter3), (*iter1) / (*iter2), tolerance);
            ++iter2; ++iter3;
        }

        container3.Assign(container1);
        container3.ElementwiseMultiply(container2);
        container3.ElementwiseDivide(container2);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, (container3 - container1).LinfNorm(), tolerance);
    }




    /*! Test CoCiCi based operations. */
    template <class _containerType1, class _containerType2, class _containerType3>
    static void TestCoCiCiOperations(const _containerType1 & container1,
                                     const _containerType2 & container2,
                                     _containerType3 & container3,
                                     typename _containerType1::value_type tolerance
                                     = cmnTypeTraits<typename _containerType1::value_type>::Tolerance()) {

        CPPUNIT_ASSERT(container1.size() == container2.size());
        CPPUNIT_ASSERT(container2.size() == container3.size());

        const typename _containerType1::const_iterator end1 = container1.end();
        typename _containerType1::const_iterator iter1;
        typename _containerType2::const_iterator iter2;
        typename _containerType3::const_iterator iter3;

        container3.SumOf(container1, container2);
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter3), (*iter1) + (*iter2), tolerance);
            ++iter2; ++iter3;
        }

        container3 = container1 + container2;
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter3), (*iter1) + (*iter2), tolerance);
            ++iter2; ++iter3;
        }

        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, ((container1 + container2) - (container2 + container1)).LinfNorm(), tolerance);

        container3.DifferenceOf(container1, container2);
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter3), (*iter1) - (*iter2), tolerance);
            ++iter2; ++iter3;
        }

        container3 = container1 - container2;
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter3), (*iter1) - (*iter2), tolerance);
            ++iter2; ++iter3;
        }

        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, ((container1 - container2) + (container2 - container1)).LinfNorm(), tolerance);

        container3.ElementwiseProductOf(container1, container2);
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter3), (*iter1) * (*iter2), tolerance);
            ++iter2; ++iter3;
        }

        container3.ElementwiseRatioOf(container1, container2);
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter3), (*iter1) / (*iter2), tolerance);
            ++iter2; ++iter3;
        }

        container3.ElementwiseMinOf(container1, container2);
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT((*iter3) == std::min((*iter1), (*iter2)));
            ++iter2; ++iter3;
        }

        container3.ElementwiseMaxOf(container1, container2);
        iter2 = container2.begin(); iter3 = container3.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT((*iter3) == std::max((*iter1), (*iter2)));
            ++iter2; ++iter3;
        }
    }




    /*! Test CoCi based operations. */
    template <class _containerType1, class _containerType2>
    static void TestCoCiOperations(const _containerType1 & container1,
                                   _containerType2 & container2,
                                   typename _containerType1::value_type tolerance
                                   = cmnTypeTraits<typename _containerType1::value_type>::Tolerance()) {

        CPPUNIT_ASSERT(container1.size() == container2.size());

        const typename _containerType1::const_iterator end1 = container1.end();
        typename _containerType1::const_iterator iter1;
        typename _containerType2::const_iterator iter2;
        typedef typename _containerType1::value_type value_type;

        container2.AbsOf(container1);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter1) >= value_type(0)?
                                         (*iter1):-(*iter1),
                                         (*iter2), tolerance);
            ++iter2;
        }

        container2.NegationOf(container1);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT((*iter2) == -(*iter1));
            ++iter2;
        }

        container2.FloorOf(container1);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1, ++iter2) {
            CPPUNIT_ASSERT((*iter2) <= (*iter1));
            CPPUNIT_ASSERT((*iter2) + static_cast<value_type>(1) > (*iter1));
            CPPUNIT_ASSERT((*iter2) == static_cast<value_type>(static_cast<long int>(*iter2)));
        }

        container2.CeilOf(container1);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1, ++iter2) {
            CPPUNIT_ASSERT((*iter2) >= (*iter1));
            CPPUNIT_ASSERT((*iter2) - static_cast<value_type>(1) < (*iter1));
            CPPUNIT_ASSERT((*iter2) == static_cast<value_type>(static_cast<long int>(*iter2)));
        }

        container2 = container1.Abs();
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter1) >= value_type(0)?
                                         (*iter1):-(*iter1),
                                         (*iter2), tolerance);
            ++iter2;
        }

        container2 = container1.Negation();
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT((*iter2) == -(*iter1));
            ++iter2;
        }

        container2 = container1.Floor();
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1, ++iter2) {
            CPPUNIT_ASSERT((*iter2) <= (*iter1));
            CPPUNIT_ASSERT((*iter2) + static_cast<value_type>(1) > (*iter1));
            CPPUNIT_ASSERT((*iter2) == static_cast<value_type>(static_cast<long int>(*iter2)));
        }


        container2 = container1.Ceil();
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1, ++iter2) {
            CPPUNIT_ASSERT((*iter2) >= (*iter1));
            CPPUNIT_ASSERT((*iter2) - static_cast<value_type>(1) < (*iter1));
            CPPUNIT_ASSERT((*iter2) == static_cast<value_type>(static_cast<long int>(*iter2)));
        }

    }




    /*! Test CoCiSi based operations. */
    template <class _containerType1, class _containerType2>
    static void TestCoCiSiOperations(const _containerType1 & container1,
                                     const typename _containerType1::value_type scalar,
                                     _containerType2 & container2,
                                     typename _containerType1::value_type tolerance
                                     = cmnTypeTraits<typename _containerType1::value_type>::Tolerance()) {

        CPPUNIT_ASSERT(container1.size() == container2.size());

        const typename _containerType1::const_iterator end1 = container1.end();
        typename _containerType1::const_iterator iter1;
        typename _containerType2::const_iterator iter2;

        container2.SumOf(container1, scalar);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) + scalar, tolerance);
            ++iter2;
        }
        container2 = container1 + scalar;
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) + scalar, tolerance);
            ++iter2;
        }

        container2.DifferenceOf(container1, scalar);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) - scalar, tolerance);
            ++iter2;
        }
        container2 = container1 - scalar;
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) - scalar, tolerance);
            ++iter2;
        }

        container2.ProductOf(container1, scalar);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) * scalar, tolerance);
            ++iter2;
        }
        container2 = container1 * scalar;
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) * scalar, tolerance);
            ++iter2;
        }

        container2.RatioOf(container1, scalar);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) / scalar, tolerance);
            ++iter2;
        }
        container2 = container1 / scalar;
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) / scalar, tolerance);
            ++iter2;
        }

        container2.ClippedAboveOf(container1, scalar);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT((*iter2) == std::min((*iter1), scalar));
            ++iter2;
        }

        container2.ClippedBelowOf(container1, scalar);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT((*iter2) == std::max((*iter1), scalar));
            ++iter2;
        }
    }


    /*! Test CioSiCi based operations. */
    template <class _containerType1, class _containerType2, class _containerType3>
    static void TestCioSiCiOperations(
        const _containerType1 & container1,
        const typename _containerType1::value_type scalar,
        const _containerType2 & container2,
        _containerType3 & container3,
        typename _containerType1::value_type tolerance
        = cmnTypeTraits<typename _containerType1::value_type>::Tolerance())
    {
        CPPUNIT_ASSERT(container1.size() == container2.size());

        const typename _containerType1::const_iterator end1 = container1.end();
        typename _containerType1::const_iterator iter1;
        typename _containerType2::const_iterator iter2;
        typename _containerType3::const_iterator iter3;

        container3.Assign(container2);
        container3.AddProductOf(scalar, container1);
        iter1 = container1.begin();
        iter2 = container2.begin();
        iter3 = container3.begin();
        for (; iter1 != end1; ++iter1, ++iter2, ++iter3) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter3), (*iter2) + scalar * (*iter1), tolerance);
        }
    }


    /*! Test CioCiCi based operations. */
    template <class _containerType1, class _containerType2, class _containerType3, class _containerType4>
    static void TestCioCiCiOperations(
        const _containerType1 & container1,
        const _containerType2 & container2,
        const _containerType3 & container3,
        _containerType4 & container4,
        typename _containerType1::value_type tolerance
        = cmnTypeTraits<typename _containerType1::value_type>::Tolerance())
    {
        CPPUNIT_ASSERT(container1.size() == container2.size());
        CPPUNIT_ASSERT(container1.size() == container3.size());

        const typename _containerType1::const_iterator end1 = container1.end();
        typename _containerType1::const_iterator iter1;
        typename _containerType2::const_iterator iter2;
        typename _containerType3::const_iterator iter3;
        typename _containerType4::const_iterator iter4;

        container4.Assign(container1);
        container4.AddElementwiseProductOf(container2, container3);
        iter1 = container1.begin();
        iter2 = container2.begin();
        iter3 = container3.begin();
        iter4 = container4.begin();
        for (; iter1 != end1; ++iter1, ++iter2, ++iter3, ++iter4) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter4), (*iter1) + (*iter2) * (*iter3), tolerance);
        }
    }


    /*! Test CoSiCi based operations. */
    template <class _containerType1, class _containerType2>
    static void TestCoSiCiOperations(const typename _containerType1::value_type scalar,
                                     const _containerType1 & container1,
                                     _containerType2 & container2,
                                     typename _containerType1::value_type tolerance
                                     = cmnTypeTraits<typename _containerType1::value_type>::Tolerance()) {

        CPPUNIT_ASSERT(container1.size() == container2.size());

        const typename _containerType1::const_iterator end1 = container1.end();
        typename _containerType1::const_iterator iter1;
        typename _containerType2::const_iterator iter2;

        container2.SumOf(scalar, container1);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), scalar + (*iter1), tolerance);
            ++iter2;
        }
        container2 = scalar + container1;
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), scalar + (*iter1), tolerance);
            ++iter2;
        }

        container2.DifferenceOf(scalar, container1);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), scalar - (*iter1), tolerance);
            ++iter2;
        }
        container2 = scalar - container1;
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), scalar - (*iter1), tolerance);
            ++iter2;
        }

        container2.ProductOf(scalar, container1);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), scalar * (*iter1), tolerance);
            ++iter2;
        }

        container2 = scalar * container1;
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), scalar * (*iter1), tolerance);
            ++iter2;
        }

        container2.RatioOf(scalar, container1);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), scalar / (*iter1), tolerance);
            ++iter2;
        }
        container2 = scalar / container1;
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), scalar / (*iter1), tolerance);
            ++iter2;
        }

        container2.ClippedAboveOf(scalar, container1);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT((*iter2) == std::min(scalar, (*iter1)));
            ++iter2;
        }

        container2.ClippedBelowOf(scalar, container1);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT((*iter2) == std::max(scalar, (*iter1)));
            ++iter2;
        }
    }




    /*! Test CioSi based operations. */
    template <class _containerType1, class _containerType2>
    static void TestCioSiOperations(_containerType1 & container1,
                                    const typename _containerType1::value_type & scalar,
                                    _containerType2 & container2,
                                    typename _containerType1::value_type tolerance
                                    = cmnTypeTraits<typename _containerType1::value_type>::Tolerance()) {

        CPPUNIT_ASSERT(container1.size() == container2.size());

        const typename _containerType1::const_iterator end1 = container1.end();
        const typename _containerType2::const_iterator end2 = container2.end();
        typename _containerType1::const_iterator iter1;
        typename _containerType2::const_iterator iter2;

        // test the Assign first
        container2.Assign(container1);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT((*iter2) == (*iter1));
            ++iter2;
        }

        container2.Assign(container1);
        container2.Add(scalar);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) + scalar, tolerance);
            ++iter2;
        }

        container2.Assign(container1);
        container2 += scalar;
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) + scalar, tolerance);
            ++iter2;
        }

        container2.Assign(container1);
        container2.Subtract(scalar);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) - scalar, tolerance);
            ++iter2;
        }

        container2.Assign(container1);
        container2 -= scalar;
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) - scalar, tolerance);
            ++iter2;
        }

        container2.Assign(container1);
        container2.Multiply(scalar);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) * scalar, tolerance);
            ++iter2;
        }

        container2.Assign(container1);
        container2 *= scalar;
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) * scalar, tolerance);
            ++iter2;
        }

        container2.Assign(container1);
        container2.Divide(scalar);
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) / scalar, tolerance);
            ++iter2;
        }

        container2.Assign(container1);
        container2 /= scalar;
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter2), (*iter1) / scalar, tolerance);
            ++iter2;
        }

        container2.Assign(container1);
        container2.ClipAbove(scalar);
        for (iter2 = container2.begin(); iter2 != end2; ++iter2) {
            CPPUNIT_ASSERT((*iter2) <= scalar);
        }

        container2.Assign(container1);
        container2.ClipBelow(scalar);
        for (iter2 = container2.begin(); iter2 != end2; ++iter2) {
            CPPUNIT_ASSERT((*iter2) >= scalar);
        }

    }



    /*! Test Cio based operations. */
    template <class _containerType1, class _containerType2>
    static void TestCioOperations(const _containerType1 & container1,
                                  _containerType2 & container2,
                                  typename _containerType1::value_type tolerance
                                  = cmnTypeTraits<typename _containerType1::value_type>::Tolerance()) {

        CPPUNIT_ASSERT(container1.size() == container2.size());

        const typename _containerType1::const_iterator end1 = container1.end();
        typename _containerType1::const_iterator iter1;
        typename _containerType2::const_iterator iter2;
        typedef typename _containerType1::value_type value_type;

        container2.Assign(container1);
        container2.AbsSelf();
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL((*iter1) >= value_type(0)?
                                         (*iter1):-(*iter1),
                                         (*iter2), tolerance);
            ++iter2;
        }
        container2.Assign(container1);
        container2.NegationSelf();
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT((*iter2) == -(*iter1));
            ++iter2;
        }

        container2 = -container1;
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1) {
            CPPUNIT_ASSERT((*iter2) == -(*iter1));
            ++iter2;
        }

        container2.Assign(container1);
        container2.FloorSelf();
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1, ++iter2) {
            CPPUNIT_ASSERT((*iter2) <= (*iter1));
            CPPUNIT_ASSERT((*iter2) + static_cast<value_type>(1) > (*iter1));
            CPPUNIT_ASSERT((*iter2) == static_cast<value_type>(static_cast<long int>(*iter2)));
        }

        container2.Assign(container1);
        container2.CeilSelf();
        iter2 = container2.begin();
        for (iter1 = container1.begin(); iter1 != end1; ++iter1, ++iter2) {
            CPPUNIT_ASSERT((*iter2) >= (*iter1));
            CPPUNIT_ASSERT((*iter2) - static_cast<value_type>(1) < (*iter1));
            CPPUNIT_ASSERT((*iter2) == static_cast<value_type>(static_cast<long int>(*iter2)));
        }

    }




    /*! Test SoCiSi based operations. */
    template <class _containerType1, class _containerType2>
    static void TestSoCiSiOperations(const _containerType1 & container1,
                                     const typename _containerType1::value_type & scalar,
                                     _containerType2 & container2) {

        CPPUNIT_ASSERT(container1.size() == container2.size());
        typedef typename _containerType1::value_type value_type;

        container2.Assign(container1);
        value_type scalar2 = container2.MinElement();

        CPPUNIT_ASSERT(!(container2.Greater(scalar2)));
        CPPUNIT_ASSERT(!(vctAll(container2.ElementwiseGreater(scalar2))));
        CPPUNIT_ASSERT(vctAny(container2.ElementwiseGreater(scalar2)));
        CPPUNIT_ASSERT(container2.GreaterOrEqual(scalar2));
        CPPUNIT_ASSERT(vctAll(container2.ElementwiseGreaterOrEqual(scalar2)));
        CPPUNIT_ASSERT(!(container2.Lesser(scalar2)));
        CPPUNIT_ASSERT(!(vctAny(container2.ElementwiseLesser(scalar2))));

        scalar2 = container2.MaxElement();
        CPPUNIT_ASSERT(!(container2.Lesser(scalar2)));
        CPPUNIT_ASSERT(container2.LesserOrEqual(scalar2));
        CPPUNIT_ASSERT(!(container2.Greater(scalar2)));

        container2.SetAll(scalar);
        CPPUNIT_ASSERT(container2.Equal(scalar));
        CPPUNIT_ASSERT(container2 == scalar);
        CPPUNIT_ASSERT(vctAll(container2.ElementwiseEqual(scalar)));
        CPPUNIT_ASSERT(!(vctAny(container2.ElementwiseNotEqual(scalar))));

        CPPUNIT_ASSERT(container2.LesserOrEqual(scalar));
        CPPUNIT_ASSERT(vctAll(container2.ElementwiseLesserOrEqual(scalar)));
        CPPUNIT_ASSERT(container2.GreaterOrEqual(scalar));
        CPPUNIT_ASSERT(vctAll(container2.ElementwiseGreaterOrEqual(scalar)));
        CPPUNIT_ASSERT(!(container2.NotEqual(scalar)));
        CPPUNIT_ASSERT(!(container2 != scalar));

        scalar2 = scalar + value_type(1);
        CPPUNIT_ASSERT(container2.NotEqual(scalar2));
        CPPUNIT_ASSERT(container2 != scalar2);
        CPPUNIT_ASSERT(!(container2.Equal(scalar2)));
        CPPUNIT_ASSERT(!(container2 == scalar2));
        CPPUNIT_ASSERT(container2.Lesser(scalar2));
        CPPUNIT_ASSERT(vctAll(container2.ElementwiseLesser(scalar2)));

        scalar2 = scalar - value_type(1);
        CPPUNIT_ASSERT(container2.Greater(scalar2));
        CPPUNIT_ASSERT(vctAll(container2.ElementwiseGreater(scalar2)));
    }


    template <class _containerType>
    static void TestMinAndMax(const _containerType & container)
    {
        typedef typename _containerType::value_type value_type;

        const value_type containerMin = container.MinElement();
        const value_type containerMax = container.MaxElement();
        const bool minIsLowerBound = container.GreaterOrEqual(containerMin);
        const bool maxIsUpperBound = container.LesserOrEqual(containerMax);
        CPPUNIT_ASSERT(minIsLowerBound);
        CPPUNIT_ASSERT(maxIsUpperBound);

        const bool minIsFound = (container.ElementwiseEqual(containerMin)).Any();
        const bool maxIsFound = (container.ElementwiseEqual(containerMax)).Any();
        CPPUNIT_ASSERT(minIsFound);
        CPPUNIT_ASSERT(maxIsFound);

        value_type minWithMax = containerMax;
        value_type maxWithMin = containerMin;
        container.MinAndMaxElement(minWithMax, maxWithMin);
        CPPUNIT_ASSERT(containerMin == minWithMax);
        CPPUNIT_ASSERT(containerMax == maxWithMin);
    }


    /*! Test STL functions */
    template <class _containerType>
    static void TestIterators(_containerType & container) {
        const typename _containerType::const_iterator const_end = container.end();
        const typename _containerType::iterator non_const_end = container.end();
        const typename _containerType::const_iterator const_begin = container.begin();
        const typename _containerType::iterator non_const_begin = container.begin();
        const typename _containerType::const_reverse_iterator const_rend = container.rend();
        const typename _containerType::reverse_iterator non_const_rend = container.rend();
        const typename _containerType::const_reverse_iterator const_rbegin = container.rbegin();
        const typename _containerType::reverse_iterator non_const_rbegin = container.rbegin();

        typename _containerType::iterator non_const_iter;
        typename _containerType::const_iterator const_iter;
        typename _containerType::reverse_iterator non_const_riter;
        typename _containerType::const_reverse_iterator const_riter;

        const typename _containerType::size_type size = container.size();
        typename _containerType::index_type index;

        /* go thru all elements from begin to end */
        non_const_iter = non_const_begin;
        const_iter = const_begin;
        non_const_riter = non_const_rend;
        const_riter = const_rend;
        index = 0;
        while (index < size) {
            non_const_riter--;
            const_riter--;
            // same iterators, same values, same memory address
            CPPUNIT_ASSERT(non_const_iter == const_iter);
            CPPUNIT_ASSERT((*non_const_iter) == (*const_iter));
            CPPUNIT_ASSERT(&(*non_const_iter) == &(*const_iter));
            CPPUNIT_ASSERT(&(*const_iter) == &(container.at(index)));
            CPPUNIT_ASSERT(non_const_riter == const_riter);
            CPPUNIT_ASSERT((*non_const_riter) == (*const_riter));
            CPPUNIT_ASSERT(&(*non_const_riter) == &(*const_riter));
            CPPUNIT_ASSERT(&(*const_riter) == &(container.at(index)));
            non_const_iter++;
            const_iter++;
            index++;
        }

        /* go thru all elements from end to begin */
        non_const_iter = non_const_end;
        const_iter = const_end;
        non_const_riter = non_const_rbegin;
        const_riter = const_rbegin;
        index = 0;
        while (index < size) {
            non_const_iter--;
            const_iter--;
            index++;
            // same iterators, same values, same memory address
            CPPUNIT_ASSERT(non_const_iter == const_iter);
            CPPUNIT_ASSERT((*non_const_iter) == (*const_iter));
            CPPUNIT_ASSERT(&(*non_const_iter) == &(*const_iter));
            CPPUNIT_ASSERT(&(*const_iter) == &(container.at(size - index)));
            CPPUNIT_ASSERT(non_const_riter == const_riter);
            CPPUNIT_ASSERT((*non_const_riter) == (*const_riter));
            CPPUNIT_ASSERT(&(*non_const_riter) == &(*const_riter));
            CPPUNIT_ASSERT(&(*const_riter) == &(container.at(size - index)));
            non_const_riter++;
            const_riter++;
        }
    }



    /*! Test STL functions */
    template <class _containerType1, class _containerType2>
    static void TestSTLFunctions(const _containerType1 & container1,
                                 _containerType2 & container2) {
        CPPUNIT_ASSERT(container1.size() == container2.size());

        const typename _containerType2::const_iterator end2 = container2.end();
        typename _containerType2::const_iterator iter2;

        container2.Assign(container1);
        std::sort(container2.begin(), container2.end());

        if (container2.size() > 1) {
            for (iter2 = (container2.begin() + 1); iter2 != end2; ++iter2) {
                CPPUNIT_ASSERT((*(iter2 - 1)) <= (*iter2));
            }
        }
    }



    /*! Test FastCopyOf */
    template <class _containerType1, class _containerType2>
    static void TestFastCopyOf(const _containerType1 & container1,
                               _containerType2 & container2) {
        // if containers have the same size, a copy should be performed.
        const typename _containerType1::const_iterator end1 = container1.end();
        typename _containerType1::const_iterator iter1;
        typename _containerType2::const_iterator iter2;

        CPPUNIT_ASSERT(container2.FastCopyCompatible(container1));
        CPPUNIT_ASSERT(container2.FastCopyOf(container1));
        iter1 = container1.begin();
        iter2 = container2.begin();
        for (; iter1 != end1; ++iter1, ++iter2) {
            CPPUNIT_ASSERT(*iter1 == *iter2);
        }
    }



    /*! Test FastCopyOf exception if different size*/
    template <class _containerType1, class _containerType2>
    static void TestFastCopyOfException(const _containerType1 & container1,
                                        _containerType2 & container2) {
        // an exception should be thrown
        CPPUNIT_ASSERT(!container2.FastCopyCompatible(container1));
        bool gotException = false;
        try {
            container2.FastCopyOf(container1);
        } catch(std::runtime_error &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);
    }

};

#endif // _vctGenericContainerTest_h
