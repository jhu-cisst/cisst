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


#ifndef _TestBernsteinPolynomialLineIntegral_h
#define _TestBernsteinPolynomialLineIntegral_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstCommon/cmnRandomSequence.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstNumerical/nmrBernsteinPolynomialLineIntegral.h>
#include <cisstNumerical/nmrBernsteinPolynomial.h>


class nmrBernsteinPolynomialLineIntegralTest : public CppUnit::TestCase
{
public:
	typedef nmrBernsteinPolynomial::PowerType PowerType;
    typedef nmrBernsteinPolynomial::VariableIndexType VariableIndexType;
	typedef nmrBernsteinPolynomial::VariableType VariableType;
	typedef nmrBernsteinPolynomialLineIntegral::ValueType ValueType;
	typedef nmrBernsteinPolynomial::CoefficientType CoefficientType;

    static const VariableIndexType DefaultNumVariables;

	static const PowerType DegreeMax;

	static const double Tolerance;

    typedef vctDynamicVector<VariableType> IntegrationPointType;

    static void RandomIntegrationPoint(IntegrationPointType & integrationPoint,
        cmnRandomSequence & randomGenerator);

	// Choose a random scalar and set it as the integrand function in the shape
	// of a Bernstein polynomial.
	// Choose two random points and evaluate the integral between them.
	// Test that the integral is equal to the scalar times the distance
	// between the points.
	static void TestScalarIntegration(nmrBernsteinPolynomial & integrand,
        nmrBernsteinPolynomialLineIntegral & integrator,
        IntegrationPointType & p0,
        IntegrationPointType & p1,
        cmnRandomSequence & randomGenerator);
    void TestScalarIntegration()
    {
        TestScalarIntegration(Integrand, Integrator, 
            IntegrationPoint0, IntegrationPoint1, RandomGenerator);
    }

	// Choose two random endpoints, and evaluate the integral between them.
	// Then choose a random point on the segment between them, and integrate
	// between each endpoint and the midpoint. Test that the sum of midpoint
	// integrals is equal (within tolerance) to the segment integral.
    static void TestMidpointIntegration(nmrBernsteinPolynomial & integrand,
        nmrBernsteinPolynomialLineIntegral & integrator,
        IntegrationPointType & p0,
        IntegrationPointType & p1,
        cmnRandomSequence & randomGenerator);
    void TestMidpointIntegration()
    {
        TestMidpointIntegration(Integrand, Integrator, 
            IntegrationPoint0, IntegrationPoint1, RandomGenerator);
    }

    void setUp();

    nmrBernsteinPolynomialLineIntegralTest()
        : RandomGenerator( cmnRandomSequence::GetInstance() )
        , Integrand( DefaultNumVariables, RandomGenerator.ExtractRandomInt(1, DegreeMax+1) )
        , Integrator( Integrand )
        , IntegrationPoint0( DefaultNumVariables, VariableType(0) )
        , IntegrationPoint1( DefaultNumVariables, VariableType(0) )
    {}

    CPPUNIT_TEST_SUITE( nmrBernsteinPolynomialLineIntegralTest );
    CPPUNIT_TEST( TestScalarIntegration );
    CPPUNIT_TEST( TestMidpointIntegration );
    CPPUNIT_TEST_SUITE_END();

protected:
    cmnRandomSequence & RandomGenerator;

    nmrBernsteinPolynomial Integrand;

    nmrBernsteinPolynomialLineIntegral Integrator;

    IntegrationPointType IntegrationPoint0;
    IntegrationPointType IntegrationPoint1;

};


#endif  // _nmrBernsteinPolynomialLineIntegralTest_h

