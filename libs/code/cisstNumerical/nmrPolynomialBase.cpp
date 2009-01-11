/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrPolynomialBase.cpp,v 1.8 2007/04/26 19:33:57 anton Exp $

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

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrPolynomialBase.cpp,v $
//  Revision 1.8  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.7  2006/11/20 20:33:19  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
//
//  Revision 1.6  2006/09/26 21:47:47  anton
//  cisstVector and cisstNumerical: Use #ifdef CISST_COMPILER_IS_MSVC instead
//  of #if (CISST_COMPILER == CISST_XYZ) for XYZ in Visual Studio 7~2000, 7.1~2003
//  and 8~2005.
//
//  Revision 1.5  2006/07/10 19:16:39  ofri
//  cisstNumerical polynomial classes: The code that had been declared
//  deprecated is now marked-out through #if brackets.  It will be removed
//  from the next release.
//
//  Revision 1.4  2006/06/12 20:22:50  ofri
//  cisstNumerical polynomial classes.  The internal cache of variable powers
//  (lookup table) is now declared deprecated.  The functionality should be
//  factored out to nmrMultiVariablePowerBasis objects.  Pragmas set for MSVC
//  to reduce the number of compilation warnings.  The inner
//  #define FACTOR_OUT_POLYNOMIAL_VARIABLES has been removed, as well as all
//  references to it.
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
//  Revision 1.10  2003/12/24 17:57:46  ofri
//  Updates to cisNumerical:
//  1) Using #include <...> instead of #include "..." for cisst header files
//  2) Getting rid of cisMatrixVector in most places (if not all), replacing it with
//  vnl
//
//  Revision 1.9  2003/09/22 18:29:22  ofri
//  Added abstract methods Scale(), ScaleCoefficients(), AddConstant(),
//  AddConstantToCoefficients() to the base class nmrPolynomialBase,
//  and actual implementations in nmrStandardPolynomial and
//  nmrBernsteinPolynomial.
//
//  Submitted after successful build but no testing yet.
//
//  Revision 1.8  2003/08/26 16:28:44  ofri
//  Fixed bug related to initialization of 0-degree polynomial when variables are
//  not factored out.
//
//  Revision 1.7  2003/08/25 22:15:12  ofri
//  Factored out nmrPolynomialBase::EvaluatePowerProduct() to
//  nmrMultiVariablePowerBasis
//
//  Revision 1.6  2003/08/21 19:04:58  ofri
//  Completed factoring the variable container out of the polynomial classes.
//  In this submission, the user can control conditional compilation of the library
//  with or without the use of the refactored classes, by setting the flag
//  FACTOR_OUT_POLYNOMIAL_VARIABLES .
//  The next step is to run tests and then remove the conditional compilation
//  option.
//  After that, I may completely remove the reference to the variable container
//  from the polynomial class, and have only evaulation parametrized by
//  a variable container.
//
//  Revision 1.5  2003/07/16 22:05:45  ofri
//  Adding serialization functionality to the polynomial classes and the
//  nmrPolynomialTermPowerIndex class
//
//  Revision 1.4  2003/07/10 15:50:16  ofri
//  Adding change history log to the files in cisstNumerical. I guess in a few files
//  there was replacement of spaces and tabs, which was recorded in CVS as
//  a big change, unfortunately. But for most, it's just adding the tags.
//
//
// ****************************************************************************
