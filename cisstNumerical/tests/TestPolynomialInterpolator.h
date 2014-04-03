/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Ofri Sadowsky
  Created on: 2004-01-01
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _TestPolynomialInterpolator_h
#define _TestPolynomialInterpolator_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "cisstTestAuxiliary/tstRandom.h"

#include "cisstNumerical/nmrPolynomialInterpolator.h"
#include "cisstNumerical/nmrStandardPolynomial.h"
#include "cisstNumerical/nmrBernsteinPolynomial.h"

class TestPolynomialInterpolator : public CppUnit::TestCase
{
public:
    typedef nmrPolynomialInterpolator::InterpolationPointType InterpolationPointType;
    typedef nmrPolynomialInterpolator::ValueType ValueType;
    typedef nmrPolynomialInterpolator::CoefficientType CoefficientType;

    static const double Tolerance;

    /*! Create a set of random sample points which is at least as large as
    the number of terms in the polynomial, and evaluate the polynomial at
    these points.  Then try to fit coefficients to the sample points.  Test
    that the result is with Tolerance of the original coefficients.
    */
    static void TestFitCoefficients(nmrDynAllocPolynomialContainer & polynomial,
        tstRandom & randomGenerator);
    void TestStandardFitCoefficients()
    {
        TestFitCoefficients( StandardPolynomial, RandomGenerator );
    }
    void TestBernsteinFitCoefficients()
    {
        TestFitCoefficients( BernsteinPolynomial, RandomGenerator );
    }

    TestPolynomialInterpolator();

    /*! Initialize the polynomial data members with a random set of terms
    and a random set of coefficients.
    */
    void setUp();

    /*! Clear the polynomial data members.
    */
    void tearDown();

    CPPUNIT_TEST_SUITE( TestPolynomialInterpolator );
    CPPUNIT_TEST( TestStandardFitCoefficients );
    CPPUNIT_TEST( TestBernsteinFitCoefficients );
    CPPUNIT_TEST_SUITE_END();

private:
    tstRandom & RandomGenerator;
    nmrStandardPolynomial StandardPolynomial;
    nmrBernsteinPolynomial BernsteinPolynomial;
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestPolynomialInterpolator);

#endif

