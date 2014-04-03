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


#include "TestPolynomialInterpolator.h"

#include "TestPolynomialBase.h"
#include "TestDynAllocPolynomialContainer.h"

#include <math.h>

const double TestPolynomialInterpolator::Tolerance = 1.0e-6;

TestPolynomialInterpolator::TestPolynomialInterpolator()
    : RandomGenerator( tstRandom::GetInstance() )
    , StandardPolynomial( TestPolynomialBase::DefaultNumVariables, 0, RandomGenerator.ExtractRandomInt(1, TestPolynomialBase::DegreeMax+1) )
    , BernsteinPolynomial( TestPolynomialBase::DefaultNumVariables, RandomGenerator.ExtractRandomInt(1, TestPolynomialBase::DegreeMax+1) )
{}

void TestPolynomialInterpolator::TestFitCoefficients(nmrDynAllocPolynomialContainer & polynomial,
                                                     tstRandom & randomGenerator)
{
    const unsigned int numVariables = polynomial.GetNumVariables();
    const unsigned int numTerms = polynomial.GetNumberOfTerms();
    const unsigned int numPoints = numTerms + (numTerms /  2);

    std::vector<InterpolationPointType> interpolationPoints;
    interpolationPoints.reserve(numPoints);
    std::vector<ValueType> functionValues;
    functionValues.reserve(numPoints);

    // Create a set of random sample points
    unsigned int pointIndex;
    for (pointIndex = 0; pointIndex < numPoints; ++pointIndex) {
        InterpolationPointType newPoint(numVariables, 0);
        TestPolynomialBase::RandomVariables( &(newPoint.front()) , numVariables, randomGenerator );
        interpolationPoints.push_back(newPoint);
    }

    // Evaluate the polynomial at the random points
    for (pointIndex = 0; pointIndex < numPoints; ++pointIndex) {
        polynomial.SetVariables( &(interpolationPoints[pointIndex].front()) );
        ValueType value = polynomial.Evaluate();
        functionValues.push_back(value);
    }

    // Fit coefficients to the random points and the results
    std::vector<CoefficientType> coefficients =
        nmrPolynomialInterpolator::FitCoefficients( &(interpolationPoints.front()), numPoints,
        &(functionValues.front()), polynomial);

    // Test that the found coefficients are close to the original coefficients
    std::vector<CoefficientType>::const_iterator coefficientIterator =
        coefficients.begin();
    nmrDynAllocPolynomialContainer::TermConstIteratorType termIterator = polynomial.FirstTermIterator();
    for (; termIterator != polynomial.EndTermIterator(); ++termIterator) {
        CPPUNIT_ASSERT( fabs( (*coefficientIterator) - polynomial.GetCoefficient(termIterator) ) <= Tolerance );
    }

}


void TestPolynomialInterpolator::setUp()
{
    nmrPolynomialBase::TermCounterType maxStandardTerms = StandardPolynomial.GetMaxNumberOfTerms();
    nmrPolynomialBase::TermCounterType maxBernsteinTerms = BernsteinPolynomial.GetMaxNumberOfTerms();

    nmrPolynomialBase::TermCounterType numStandardTerms =
        TestPolynomialBase::InsertRandomTerms(StandardPolynomial, maxStandardTerms / 4 + 1, RandomGenerator);

    nmrPolynomialBase::TermCounterType numBernsteinTerms =
        TestPolynomialBase::InsertRandomTerms(BernsteinPolynomial, maxBernsteinTerms / 4 + 1, RandomGenerator);

    TestDynAllocPolynomialContainer::SetRandomCoefficients(StandardPolynomial, RandomGenerator);
    TestDynAllocPolynomialContainer::SetRandomCoefficients(BernsteinPolynomial, RandomGenerator);
}


void TestPolynomialInterpolator::tearDown()
{
    StandardPolynomial.Clear();
    BernsteinPolynomial.Clear();
}

