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


#include "nmrStandardPolynomialTest.h"

#include <math.h>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

const nmrStandardPolynomialTest::VariableType nmrStandardPolynomialTest::VariableValueMax = 10;
const nmrStandardPolynomialTest::PowerType nmrStandardPolynomialTest::DegreeMax = 8;
const double nmrStandardPolynomialTest::Tolerance = 1.0e-6;

nmrStandardPolynomial & 
nmrStandardPolynomialTest::MakeN_Nomial(nmrStandardPolynomial & polynomial, PowerType degree)
{
    nmrPolynomialTermPowerIndex index(polynomial.GetNumVariables(), degree, degree);
    
    index.GoBegin();
    while(index.IsValid()) {
        polynomial.SetCoefficient(index, index.GetMultinomialCoefficient());
        index.Increment();
    }
    
    return polynomial;
}



void nmrStandardPolynomialTest::TestN_Nomial(nmrStandardPolynomial & polynomial,
                                             nmrMultiVariablePowerBasis & powerBasis,
                                             cmnRandomSequence & randomGenerator)
{
    PowerType degree = randomGenerator.ExtractRandomInt(0, polynomial.GetMaxDegree() + 1);
    polynomial.Clear();
    MakeN_Nomial(polynomial, degree);
    
    // sum all the variables
    VariableType variableSum = powerBasis.GetSumOfVariables();
    
    // If the sum's absolute value is less than one, add one to |sum| to avoid
    // numerical instablities.
    while (fabs(variableSum) < 1.0) {
        VariableType var0 = powerBasis.GetVariable(0);
        variableSum -= var0;
        var0 += (var0 < 0) ? -1.0 : 1.0;
        variableSum += var0;
        powerBasis.SetVariable(0, var0);
    }
    
    nmrStandardPolynomial::ValueType value = ((nmrPolynomialBase & )polynomial).Evaluate( powerBasis );
    
    nmrStandardPolynomial::ValueType compareValue = pow(variableSum, degree);
    
    double error = fabs( (compareValue - value) );
    double errorScale = (fabs(value) >= 1.0) ? fabs(value) : 1.0;
    double relativeError = error / errorScale;
    
    CPPUNIT_ASSERT( relativeError < Tolerance );
}

CPPUNIT_TEST_SUITE_REGISTRATION(nmrStandardPolynomialTest);

