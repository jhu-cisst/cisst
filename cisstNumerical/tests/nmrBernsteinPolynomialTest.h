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


#ifndef _nmrBernsteinPolynomialTest_h
#define _nmrBernsteinPolynomialTest_h

#include "nmrDynAllocPolynomialContainerTest.h"
#include <cisstNumerical/nmrBernsteinPolynomial.h>

class nmrBernsteinPolynomialTest : public nmrDynAllocPolynomialContainerTest
{
public:
    
    // Make a Bernstein polynomial with all coefficients equal to a parameter.
    // The evaluation of the polynomial should yield the same scalar for any assignment
    // of variables.
    static nmrBernsteinPolynomial & MakeScalarPolynomial(nmrBernsteinPolynomial & polynomial, 
        CoefficientType coefficient);

    /*! Test that the implicit variable cannot be modified.  Then switch to another
      implicit variable, and see that the first one is modified.  Then restore
      the original implicit variable.  Check that the sum of variables is 1
      after any change.  
    */
    static void TestImplicitVariable(nmrMultiVariablePowerBasis::BarycentricBasis & powerBasis,
        cmnRandomSequence & randomGenerator);

    void TestImplicitVariable()
    {
        TestImplicitVariable(ConcreteTestTargetVariables, RandomGenerator);
    }

    // Create a polynomial that evaluates to a random scalar value. Evaluate it for
    // random variable assignments. Test that the evaluation result is as expected.
    static void TestScalarPolynomial(nmrBernsteinPolynomial & polynomial,
        nmrMultiVariablePowerBasis & powerBasis,
        cmnRandomSequence & randomGenerator);
    void TestScalarPolynomial()
    {
        TestScalarPolynomial(ConcreteTestTarget, ConcreteTestTargetVariables, RandomGenerator);
    }
    
    void setUp()
    {
        SetRandomVariables(ConcreteTestTargetVariables, RandomGenerator);
    }
    
    nmrBernsteinPolynomialTest()
        : ConcreteTestTargetVariables(DefaultNumVariables, RandomGenerator.ExtractRandomInt(0, DegreeMax+1))
        , ConcreteTestTarget(DefaultNumVariables, ConcreteTestTargetVariables.GetMaxDegree() )
    {
        TestTarget = & ConcreteTestTarget;
        DynAllocTestTarget = & ConcreteTestTarget;
        TestTargetVariables = & ConcreteTestTargetVariables;
    }

    CPPUNIT_TEST_SUB_SUITE(nmrBernsteinPolynomialTest, nmrDynAllocPolynomialContainerTest);
    CPPUNIT_TEST( TestImplicitVariable );
    CPPUNIT_TEST( TestScalarPolynomial );
    CPPUNIT_TEST_SUITE_END();
    
protected:
    nmrMultiVariablePowerBasis::BarycentricBasis ConcreteTestTargetVariables;
    nmrBernsteinPolynomial ConcreteTestTarget;
    
};


#endif  // _nmrBernsteinPolynomialTest_h

