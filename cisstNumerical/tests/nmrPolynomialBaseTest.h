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


#ifndef _TestPolynomialBase_h
#define _TestPolynomialBase_h

#include <cisstCommon/cmnPortability.h>
#include <cisstNumerical/nmrPolynomialBase.h>
#include <cisstCommon/cmnRandomSequence.h>

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>


class nmrPolynomialBaseTest : public CppUnit::TestCase
{
public:
	typedef nmrPolynomialBase::VariableType VariableType;
	typedef nmrPolynomialBase::PowerType PowerType;
	typedef nmrPolynomialBase::TermCounterType TermCounterType;
	typedef nmrPolynomialBase::CoefficientType CoefficientType;
    typedef nmrPolynomialBase::VariableIndexType VariableIndexType;

    static const VariableIndexType DefaultNumVariables;
	static const VariableType VariableValueMax;
	static const PowerType DegreeMax;
	static const double Tolerance;


	// Generate valid random terms for a polynomial, and insert them with coefficient 0
	// into the polynomial. As we do not test for repeating random terms, we can only
	// specify the maximum number of terms to insert.  Note that we do not clear the 
	// polynomial here before inserting the terms.
	//
	// Return the actual number of terms that were successfully inserted into the polynomial.
	// This is a utility function and not a test.
	static TermCounterType InsertRandomTerms(nmrPolynomialBase & polynomial, TermCounterType maxNumTerms,
        cmnRandomSequence & randomGenerator);

	// Return a random value for a variable of a polynomial. This is a utility function
	// and not a test.
	static VariableType ChooseRandomVariable(cmnRandomSequence & randomGenerator)
    {
        return randomGenerator.ExtractRandomDouble(-VariableValueMax, VariableValueMax);
    }

	static void RandomVariables(VariableType vars[], int numVariables,
        cmnRandomSequence & randomGenerator)
	{
		for (int i = 0; i < numVariables; ++i)
            vars[i] = ChooseRandomVariable(randomGenerator);
	}

	// Set random values to the variables of the polynomial. This is a utility function
	// and not a test.
    static void SetRandomVariables(nmrMultiVariablePowerBasis & powerBasis,
        cmnRandomSequence & randomGenerator);

	// Test all criteria of emptiness of a polynomial:
	// IsEmpty()
	// GetNumberOfTerms() == 0
	//
	// The given polynomial must be empty for the test to have a chance of success.
	// This can serve both as a utility function and as a test.
	static void TestEmptiness(const nmrPolynomialBase & polynomial)
	{
        CPPUNIT_ASSERT( polynomial.IsEmpty() );
        CPPUNIT_ASSERT( polynomial.GetNumberOfTerms() == 0 );
	}

	// Fill all possible terms into the polynomial, then Clear() the polynomial.
	// Verify that the polynomial is empty.  Note that Clear() is not necessarily identical
	// to removing all the terms one by one, and needs to be tested separately (following
	// bug discovery).
	static void TestFillClear(nmrPolynomialBase & polynomial)
	{
		polynomial.FillAllTerms();

        CPPUNIT_ASSERT( polynomial.GetNumberOfTerms() == polynomial.GetMaxNumberOfTerms() );
		
		polynomial.Clear();
		
		TestEmptiness(polynomial);
	}
    void TestFillClear()
    {
        TestFillClear(*TestTarget);
    }

	// Insert a random term with a random coefficient into the polynomial and test 
	// that it's there and that is has the correct coefficient.  Then remove it and
	// test that it's not there, and consequently, that its coefficient is zero.
	static void TestSingleRandomTerm(nmrPolynomialBase & polynomial,
        cmnRandomSequence & randomGenerator);
    void TestSingleRandomTerm()
    {
        TestSingleRandomTerm(*TestTarget, RandomGenerator);
    }

    /*! Compare two methods of evaluating the polynomial: one that uses
      internally stored coefficients ( nmrPolynomialBase::Evaluate() ) and
      one that uses external coefficients 
      ( nmrPolynomialBase::EvaluateForCoefficients() ).
      The comparison is done by creating a set of random terms and choosing
      random coefficients, then evaluating the polynomial using the two 
      methods.  This function does not test for correctness of the evaluated
      number, but on the other hand, the two evaluations should yield exactly
      identical output, with zero tolerance for error.  If this test succeeds,
      then it is enough to test for the correctness of only one evaluation 
      method, and we know that the evaluations are consistent.

      NOTE: Some compilers create a different order of evaluation for
      the two expressions, even though they should be the same.  For
      example: gcc 4.0.  Therefore, we provide some tolerance on the
      equality of the two values.
    */
    static void TestCompareEvaluations(nmrPolynomialBase & polynomial,
        nmrMultiVariablePowerBasis & powerBasis,
        cmnRandomSequence & randomGenerator);
    void TestCompareEvaluations()
    {
        TestCompareEvaluations(*TestTarget, *TestTargetVariables, RandomGenerator);
    }

    /*! Create a random polynomial and initialize it with random coefficients and
      variable values.  Evaluate the polynomial.  Scale the polynomial by a random
      scale factor and evaluate again.  Test that the last value is equal to the
      scale factor times the first value.

      Repeat the procedure with external coefficients.

      NOTE: The function evaluates the polynomial with internal and external
      coefficients.  As sepcified in the note for TestCompareEvaluations,
      some compilers create a different evaluation order, which would cause
      this test to fail.  We therefore introduce some tolerance in the equality
      test.
    */
    static void TestScalePolynomial(nmrPolynomialBase & polynomial,
        nmrMultiVariablePowerBasis & powerBasis,
        cmnRandomSequence & randomGenerator);
    void TestScalePolynomial()
    {
        TestScalePolynomial(*TestTarget, *TestTargetVariables, RandomGenerator);
    }

    /*! Create a random polynomial and initialize it with random coefficients and
      variable values.  Evaluate the polynomial.  Add a constant to the polynomial
      and evaluate again.  Test that the difference between the last value and
      the first is equal to the constant.
      Repeat the procedure with external coefficients.  This requires recovering the
      coefficients after the constant has been added, then subtract the constant
      value from the coefficients, and evaluate the polynomial for the
      external coefficients.  Test that we get the initial value.
    */
    static void TestAddConstant(nmrPolynomialBase & polynomial,
        nmrMultiVariablePowerBasis & powerBasis,
        cmnRandomSequence & randomGenerator);
    void TestAddConstant()
    {
        TestAddConstant(*TestTarget, *TestTargetVariables, RandomGenerator);
    }

    void setUp()
    {}


    void tearDown()
    {}


    nmrPolynomialBaseTest()
        : RandomGenerator( cmnRandomSequence::GetInstance() )
    {}

    virtual ~nmrPolynomialBaseTest()
    {}

    CPPUNIT_TEST_SUITE( nmrPolynomialBaseTest );
    CPPUNIT_TEST( TestSingleRandomTerm );
    CPPUNIT_TEST( TestFillClear );
    CPPUNIT_TEST( TestCompareEvaluations );
    CPPUNIT_TEST( TestScalePolynomial );
    CPPUNIT_TEST( TestAddConstant );
    CPPUNIT_TEST_SUITE_END();

protected:
    cmnRandomSequence & RandomGenerator;

    /*! This member is to be initialized by the test class of the concrete 
     polynomial
    */
    nmrPolynomialBase * TestTarget;

    nmrMultiVariablePowerBasis * TestTargetVariables;

};


#endif

