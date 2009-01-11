/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrBernsteinPolynomialLineIntegralTest.h,v 1.6 2007/04/26 20:12:05 anton Exp $
  
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


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrBernsteinPolynomialLineIntegralTest.h,v $
// Revision 1.6  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.5  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.4  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/09/06 15:41:37  anton
// cisstNumerical tests: Added license.
//
// Revision 1.2  2004/10/27 17:57:21  ofri
// cisstNumericalTest: Following ticket #82, I erased the use of valarray, and
// instead I am using vctDynamicVector.  Huh!
//
// Revision 1.1  2004/10/24 03:51:03  ofri
// Renamed cisstNumerical test classes and files.  Tests run but fail for some
// cases.
//
// ****************************************************************************
