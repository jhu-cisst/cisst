/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrStandardPolynomialTest.h,v 1.6 2007/04/26 20:12:05 anton Exp $
  
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


#ifndef _nmrStandardPolynomialTest_h
#define _nmrStandardPolynomialTest_h

#include "nmrDynAllocPolynomialContainerTest.h"

#include <cisstNumerical/nmrStandardPolynomial.h>

class nmrStandardPolynomialTest : public nmrDynAllocPolynomialContainerTest
{
public:

	static const VariableType VariableValueMax;
	static const PowerType DegreeMax;
	static const double Tolerance;

	// Given a polynomial of the variables x_0, x_1, ... x_{n-1},
	// expand the expression (x_0 + ... + x_{n-1})^d =
	//   \sum_{ |k| = d }  \choose{d}{k} x_0^{k_0} ... x_{n-1}^{k_{n-1}}
	//
	// into a polynomial form.
	// Note: The function does not clear the polynomial before inserting the terms.
	static nmrStandardPolynomial & MakeN_Nomial(nmrStandardPolynomial & polynomial, PowerType degree);

	// Generate a random degree N-nomial and initialize it with random variables.
	// Evaluate the polynomial and compare the result with the expected (sum(variables)^d).
	static void TestN_Nomial(nmrStandardPolynomial & polynomial,
        nmrMultiVariablePowerBasis & powerBasis,
        cmnRandomSequence & randomGenerator);
    void TestN_Nomial()
    {
        TestN_Nomial(ConcreteTestTarget, ConcreteTestTargetVariables, RandomGenerator);
    }

    void setUp()
    {
        SetRandomVariables(ConcreteTestTargetVariables, RandomGenerator);
    }

    nmrStandardPolynomialTest()
        : ConcreteTestTargetVariables(DefaultNumVariables, RandomGenerator.ExtractRandomInt(1, DegreeMax + 1) )
        , ConcreteTestTarget(DefaultNumVariables, 0, ConcreteTestTargetVariables.GetMaxDegree() )
    {
        TestTarget = &ConcreteTestTarget;
        DynAllocTestTarget = & ConcreteTestTarget;
        TestTargetVariables = &ConcreteTestTargetVariables;
    }

    CPPUNIT_TEST_SUB_SUITE(nmrStandardPolynomialTest, nmrDynAllocPolynomialContainerTest);
    CPPUNIT_TEST( TestN_Nomial );
    CPPUNIT_TEST_SUITE_END();

protected:
    nmrMultiVariablePowerBasis::StandardPowerBasis ConcreteTestTargetVariables;
    nmrStandardPolynomial ConcreteTestTarget;
};

#endif  // _nmrStandardPolynomialTest_h

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrStandardPolynomialTest.h,v $
// Revision 1.6  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.5  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.4  2006/07/10 15:42:56  ofri
// cusstNumerical tests : Adapted code so that the deprecated interfaces are
// not used.  Tests passed with msvc7, gcc-3.3.
//
// Revision 1.3  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/09/06 15:41:37  anton
// cisstNumerical tests: Added license.
//
// Revision 1.1  2004/10/24 03:51:03  ofri
// Renamed cisstNumerical test classes and files.  Tests run but fail for some
// cases.
//
// ****************************************************************************
