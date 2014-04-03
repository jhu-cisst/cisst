/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Ofri Sadowsky
  Created on: 2004-10-21
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "nmrBernsteinPolynomialLineIntegralTest.h"

#include "nmrPolynomialBaseTest.h"
#include "nmrBernsteinPolynomialTest.h"

#include <math.h>

const nmrBernsteinPolynomialLineIntegralTest::VariableIndexType 
nmrBernsteinPolynomialLineIntegralTest::DefaultNumVariables = 4;

const nmrBernsteinPolynomialLineIntegralTest::PowerType 
nmrBernsteinPolynomialLineIntegralTest::DegreeMax = 6;

const double nmrBernsteinPolynomialLineIntegralTest::Tolerance = 1.0e-5;

void nmrBernsteinPolynomialLineIntegralTest::setUp()
{
    RandomIntegrationPoint(IntegrationPoint0, RandomGenerator);
    RandomIntegrationPoint(IntegrationPoint1, RandomGenerator);
}

void 
nmrBernsteinPolynomialLineIntegralTest::
RandomIntegrationPoint(IntegrationPointType & integrationPoint,
                       cmnRandomSequence & randomGenerator)
{
    const unsigned int numVariables = integrationPoint.size();
    nmrPolynomialBaseTest::RandomVariables( &(integrationPoint[0]), numVariables, randomGenerator );
    VariableType sum = integrationPoint.SumOfElements();
    while (sum < 1) {
        integrationPoint += VariableType(1);
        sum += numVariables;
    }

    integrationPoint /= sum;
}

void nmrBernsteinPolynomialLineIntegralTest::TestScalarIntegration(nmrBernsteinPolynomial & integrand,
                                                                nmrBernsteinPolynomialLineIntegral & integrator,
                                                                IntegrationPointType & p0,
                                                                IntegrationPointType & p1,
                                                                cmnRandomSequence & randomGenerator)
{
    CoefficientType coefficient = nmrPolynomialBaseTest::ChooseRandomVariable(randomGenerator);
    nmrBernsteinPolynomialTest::MakeScalarPolynomial(integrand, coefficient);
    integrator.UpdateIntegrationTableau();

    const ValueType p1p0distance = (p1 - p0).Norm();
    const ValueType expectedResult = coefficient * p1p0distance;

    const ValueType valueFrom0To1 = integrator.EvaluateForSegment( &(p0[0]), &(p1[0]), p1p0distance );
    const ValueType valueFrom1To0 = integrator.EvaluateForSegment( &(p1[0]), &(p0[0]), p1p0distance );

    const double error1 = fabs( (valueFrom0To1 - expectedResult) );
    const double errorScale1 = (fabs(expectedResult) >= 1.0) ? fabs(expectedResult) : 1.0;
    const double relativeError1 = error1 / errorScale1;

    const double error2 = fabs( (valueFrom1To0 - expectedResult) );
    const double errorScale2 = (fabs(expectedResult) >= 1.0) ? fabs(expectedResult) : 1.0;
    const double relativeError2 = error2 / errorScale2;

    CPPUNIT_ASSERT( relativeError1 <= Tolerance );
    CPPUNIT_ASSERT( relativeError2 <= Tolerance );
}



void nmrBernsteinPolynomialLineIntegralTest::TestMidpointIntegration(nmrBernsteinPolynomial & integrand,
                                                                  nmrBernsteinPolynomialLineIntegral & integrator,
                                                                  IntegrationPointType & p0,
                                                                  IntegrationPointType & p1,
                                                                  cmnRandomSequence & randomGenerator)
{
    unsigned long maxTerms = integrand.GetMaxNumberOfTerms();
    integrand.Clear();
    nmrPolynomialBaseTest::InsertRandomTerms(integrand, int(sqrt(static_cast<double>(maxTerms))) + 1, randomGenerator);
    nmrDynAllocPolynomialContainerTest::SetRandomCoefficients(integrand, randomGenerator);

    double midPointRatio = randomGenerator.ExtractRandomFloat();
    IntegrationPointType midPoint = p0 * (1 - midPointRatio) + p1 * midPointRatio;

    const ValueType p0p1Distance = (p1 - p0).Norm();
    const ValueType p0midDistance = (midPoint - p0).Norm();
    const ValueType midp1Distance = (p1 - midPoint).Norm();

    ValueType valueP0P1 = integrator.EvaluateForSegment( &(p0[0]), &(p1[0]), p0p1Distance );

    ValueType valueP0MidPoint = integrator.EvaluateForSegment( &(p0[0]), &(midPoint[0]), p0midDistance );
    ValueType valueMidPointP1 = integrator.EvaluateForSegment( &(midPoint[0]), &(p1[0]), midp1Distance );

    ValueType sumValues = valueP0MidPoint + valueMidPointP1;

    double error = fabs( (sumValues - valueP0P1) );
    double errorScale = (fabs(valueP0P1) >= 1.0) ? fabs(valueP0P1) : 1.0;
    double relativeError = error / errorScale;

    CPPUNIT_ASSERT( relativeError <= Tolerance );
}

CPPUNIT_TEST_SUITE_REGISTRATION(nmrBernsteinPolynomialLineIntegralTest);

