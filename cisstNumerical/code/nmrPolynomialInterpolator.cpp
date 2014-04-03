/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:   2003-07-09

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstNumerical/nmrPolynomialInterpolator.h>
#include <vnl/algo/vnl_svd.h>

std::vector<nmrPolynomialInterpolator::CoefficientType>
nmrPolynomialInterpolator::FitCoefficients(
        const nmrDynAllocPolynomialContainer & polynomial,
        unsigned int numPoints, InterpolationPointType interpolationPoints[],
        const ValueType functionValues[])
{
    const nmrPolynomialBase::TermCounterType numTerms = polynomial.GetNumberOfTerms();
    assert( numPoints >= numTerms );
    const unsigned int numVariables = polynomial.GetNumVariables();

    InterpolationMatrixType equationMatrix;

    nmrMultiVariablePowerBasis::StandardPowerBasis 
        powerBasis(polynomial.GetNumVariables(), polynomial.GetMaxDegree());

    GenerateTermMatrixForPoints(polynomial, numPoints, interpolationPoints, equationMatrix);

    std::vector<CoefficientType> result(numTerms, CoefficientType(0));

    vnl_svd<CoefficientType> vnlSvd(equationMatrix);

    int rc = vnlSvd.rank();

    vnlSvd.solve(functionValues, &(result.front()));

    return result;
}

void nmrPolynomialInterpolator::GenerateTermMatrixForPoints(
    const nmrDynAllocPolynomialContainer & polynomial,
    unsigned int numPoints,
    InterpolationPointType interpolationPoints[],
    InterpolationMatrixType & result)
{
    nmrPolynomialBase::TermCounterType numTerms = polynomial.GetNumberOfTerms();
    result.set_size(numPoints, numTerms);

    nmrMultiVariablePowerBasis::StandardPowerBasis 
        powerBasis(polynomial.GetNumVariables() , polynomial.GetMaxDegree());

    nmrDynAllocPolynomialContainer::TermConstIteratorType termIterator;


    unsigned int pointIndex = 0;
    for (pointIndex = 0; pointIndex < numPoints; ++pointIndex)
    {
        powerBasis.SetVariables( interpolationPoints[pointIndex] );
        polynomial.EvaluateBasisVector( powerBasis, result[pointIndex] );
    }
}

