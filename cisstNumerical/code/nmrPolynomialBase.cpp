/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:   2001-10-16

  (C) Copyright 2001-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstNumerical/nmrPolynomialBase.h>
#include <assert.h>
#include <iostream>

nmrPolynomialBase::nmrPolynomialBase(VariableIndexType numVariables, 
                                     PowerType minDegree, PowerType maxDegree)
    : NumVariables(numVariables)
    , MinDegree(minDegree)
    , MaxDegree(maxDegree)
{}


bool nmrPolynomialBase::CanIncludeIndex(const nmrPolynomialTermPowerIndex & term) const
{
	// We test for each clause in a separate line for easier debugging
	if (term.GetNumVariables() != GetNumVariables())
		return false;
	if (GetMinDegree() > term.GetMinDegree())
		return false;
	if (term.GetMaxDegree() > GetMaxDegree())
		return false;
	if (!term.IsValid())
		return false;
	return true;
}


void nmrPolynomialBase::ScaleCoefficients(CoefficientType coefficients[], 
                                          CoefficientType scaleFactor) const
{
    TermCounterType numTerms = GetNumberOfTerms();
    TermCounterType termCounter = 0;
    for (; termCounter < numTerms; ++termCounter) {
        coefficients[termCounter] *= scaleFactor;
    }
}



void nmrPolynomialBase::SerializeRaw(std::ostream & output) const
{
    output.write( (const char *)&NumVariables, sizeof(NumVariables) );
    output.write( (const char *)&MinDegree, sizeof(MinDegree) );
    output.write( (const char *)&MaxDegree, sizeof(MaxDegree) );
}


void nmrPolynomialBase::DeserializeRaw(std::istream & input)
{
    Clear();
    input.read( (char *)&NumVariables, sizeof(NumVariables) );
    input.read( (char *)&MinDegree, sizeof(MinDegree) );
    input.read( (char *)&MaxDegree, sizeof(MaxDegree) );

#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(push)
#pragma warning(disable:4996)
#endif  // ifdef CISST_COMPILER_IS_MSVC
    VariablePowers->Initialize(NumVariables, MaxDegree);
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(pop)
#endif  // ifdef CISST_COMPILER_IS_MSVC
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE
}
