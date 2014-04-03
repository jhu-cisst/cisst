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


#include "nmrBernsteinPolynomialTest.h"

#include <math.h>

#include <algorithm>

nmrBernsteinPolynomial & nmrBernsteinPolynomialTest::MakeScalarPolynomial(nmrBernsteinPolynomial & polynomial, 
																	   CoefficientType coefficient)
{
	polynomial.Clear();
	polynomial.FillAllTerms();
	nmrBernsteinPolynomial::TermIteratorType termIterator = polynomial.FirstTermIterator();
	while (termIterator != polynomial.EndTermIterator()) {
		polynomial.SetCoefficient(termIterator, coefficient);
		termIterator++;
	}

	return polynomial;
}


void nmrBernsteinPolynomialTest::TestScalarPolynomial(nmrBernsteinPolynomial & polynomial,
                                                      nmrMultiVariablePowerBasis & powerBasis,
                                                   cmnRandomSequence & randomGenerator)
{
	CoefficientType scalarValue = ChooseRandomVariable(randomGenerator);

	polynomial.Clear();
	MakeScalarPolynomial(polynomial, scalarValue);

	nmrPolynomialBase::ValueType value = ((nmrPolynomialBase &)polynomial).Evaluate(powerBasis);

	double error = fabs( (scalarValue - value) );
	double errorScale = (fabs(value) >= 1.0) ? fabs(value) : 1.0;
	double relativeError = error / errorScale;

    // I tried to set a flexible limit on the error which would be better than
    // 0.01 in most cases.  But it was too hard to analyze, so I set the hard
    // limit of 0.01.
    CPPUNIT_ASSERT( relativeError < 0.01 );
}


void nmrBernsteinPolynomialTest::TestImplicitVariable(nmrMultiVariablePowerBasis::BarycentricBasis & powerBasis,
                                                   cmnRandomSequence & randomGenerator)
{
	VariableIndexType numVars = powerBasis.GetNumVariables();

    // Save the old values.
	VariableIndexType oldImplicitVarIndex = powerBasis.GetImplicitVarIndex();
    VariableType oldImplicitVarValue = powerBasis.GetVariable(oldImplicitVarIndex);

    // Assert that we cannot set the implicit variable
    CPPUNIT_ASSERT( !powerBasis.CanSetVariable(oldImplicitVarIndex) );

    // switch to another implicit variable, and see that we can actually
    // change the old implicit variable.
	VariableIndexType newImplicitVarIndex = randomGenerator.ExtractRandomInt(0, numVars);
    VariableType newImplicitVariableValue = 
        nmrPolynomialBaseTest::ChooseRandomVariable(randomGenerator);
	powerBasis.SetImplicitVarIndex(newImplicitVarIndex);

    // and see that the old implicit variable is updated (if it's not the same as
    // the new one)
    if (newImplicitVarIndex != oldImplicitVarIndex) {
        powerBasis.SetVariable(oldImplicitVarIndex, newImplicitVariableValue);
        CPPUNIT_ASSERT( newImplicitVariableValue == powerBasis.GetVariable(oldImplicitVarIndex) );
    }

    // Also verify that the sum of all variables is still 1.
	VariableType sumVariables = powerBasis.GetSumOfVariables();
	double error1 = fabs( VariableType(1) - sumVariables );
	
    // And now restore the original values
    if (newImplicitVarIndex != oldImplicitVarIndex) {
        powerBasis.SetVariable(oldImplicitVarIndex, oldImplicitVarValue);
        powerBasis.SetImplicitVarIndex(oldImplicitVarIndex);
    }

    // And once again, verify that the sum is still 1.
    sumVariables = powerBasis.GetSumOfVariables();
	double error2 = fabs( VariableType(1) - sumVariables );

    CPPUNIT_ASSERT( error1 < Tolerance );
    CPPUNIT_ASSERT( error2 < Tolerance );


}

CPPUNIT_TEST_SUITE_REGISTRATION(nmrBernsteinPolynomialTest);

