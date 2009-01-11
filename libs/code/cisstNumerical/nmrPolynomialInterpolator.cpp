/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrPolynomialInterpolator.cpp,v 1.5 2007/04/26 19:33:57 anton Exp $

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


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrPolynomialInterpolator.cpp,v $
//  Revision 1.5  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.4  2006/11/20 20:33:19  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
//
//  Revision 1.3  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.2  2005/06/03 18:20:58  anton
//  cisstNumerical: Added license.
//
//  Revision 1.1  2004/10/21 19:53:07  ofri
//  Adding polynomial-related cpp files to the repository -- imported from Ofri.
//  See ticket #78.
//
//  Revision 1.5  2003/12/24 17:57:46  ofri
//  Updates to cisNumerical:
//  1) Using #include <...> instead of #include "..." for cisst header files
//  2) Getting rid of cisMatrixVector in most places (if not all), replacing it with
//  vnl
//
//  Revision 1.4  2003/09/29 23:29:23  ofri
//  changed the definition of nmrPolynomialInterpolator::InterpolationPointType
//  and added some subsequent chages.  This due to const compilation issues.
//
//  Revision 1.3  2003/09/26 01:14:36  ofri
//  Changed interface of nmrPolynomialInterpolator
//
//  Revision 1.2  2003/07/16 22:05:45  ofri
//  Adding serialization functionality to the polynomial classes and the
//  nmrPolynomialTermPowerIndex class
//
//  Revision 1.1  2003/07/10 15:45:04  ofri
//  Adding class nmrPolynomialInterpolator to the repository
//
//
// ****************************************************************************
