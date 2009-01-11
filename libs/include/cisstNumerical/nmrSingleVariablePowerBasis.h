/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrSingleVariablePowerBasis.h,v 1.9 2007/04/26 19:33:57 anton Exp $

  Author(s):	Ofri Sadowsky
  Created on: 2003-08-20

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrSingleVariablePowerBasis_h
#define _nmrSingleVariablePowerBasis_h


#include <vector>
#include <algorithm>
#include <assert.h>

#include <cisstCommon/cmnPortability.h>
#include <cisstNumerical/nmrPolynomialTermPowerIndex.h>
#include <cisstNumerical/nmrExport.h>

#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(push)
#pragma warning(disable:4996)
#endif  // ifdef CISST_COMPILER_IS_MSVC
/*!  Stores the power basis for a single variable. A nmrSingleVariablePowerBasis
  is initialized with the maximal degree for which the variable is raised.
  Call SetVariable() to set the variable and calculate the sequence of powers.
*/
class CISST_EXPORT CISST_DEPRECATED nmrSingleVariablePowerBasis
{
public:
    typedef double VariableType;
    typedef nmrPolynomialTermPowerIndex::PowerType PowerType;

private:
    typedef std::vector<VariableType> PowersContainerType;
    PowersContainerType PowersContainer;

public:
    nmrSingleVariablePowerBasis(PowerType degree)
        : PowersContainer( std::max<PowerType>(degree+1,2), 0)
    {
        PowersContainer[0] = 1;
    }

    /*! This function initializes the variable to zero and the degree to the given 
      value.  It is to be called during events such as serialization.
    */
    void Initialize(PowerType degree)
    {
        PowersContainer.resize(degree+1);
        PowersContainer.assign(degree+1, 0);
        PowersContainer[0] = 1;
    }

    /*! Return the maximal degree of the variable in this power sequence */
    PowerType GetDegree() const
    {
        return PowersContainer.size() - 1;
    }

    /*! Return the current value of the variable. */
    VariableType GetVariable() const
    {
        return PowersContainer[1];
    }

    /*! Return a pre-computed power of the variable (assert limits) */
    VariableType GetVariablePower(PowerType power) const
    {
        assert(power <= GetDegree());
        return PowersContainer[power];
    }

    /*! Return a pre-computed power of the variable */
    VariableType operator[](PowerType power) const
    {
        return PowersContainer[power];
    }

    /*! Set the value of the variable and the rest of the power sequence */
    void SetVariable(VariableType variable);
};
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(pop)
#endif  // ifdef CISST_COMPILER_IS_MSVC

#endif
// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrSingleVariablePowerBasis.h,v $
//  Revision 1.9  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.8  2006/11/20 20:33:20  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
//
//  Revision 1.7  2006/09/26 21:47:47  anton
//  cisstVector and cisstNumerical: Use #ifdef CISST_COMPILER_IS_MSVC instead
//  of #if (CISST_COMPILER == CISST_XYZ) for XYZ in Visual Studio 7~2000, 7.1~2003
//  and 8~2005.
//
//  Revision 1.6  2006/06/12 19:43:28  ofri
//  nmrSingleVariablePowerBasis: class is declared deprecated and will be
//  removed from future releases.
//
//  Revision 1.5  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.4  2005/06/03 18:20:58  anton
//  cisstNumerical: Added license.
//
//  Revision 1.3  2004/10/27 02:41:59  anton
//  cisstNumerical: Code to compile as a DLL.  Most of the code was in place but
//  never tested.  Minor additions including to disable some warnings.
//
//  Revision 1.2  2004/10/22 01:24:37  ofri
//  Cleanup following migration to cisst, and rebuild with cygwin/gcc
//
//  Revision 1.1  2004/10/21 19:52:51  ofri
//  Adding polynomial-related header files to the repository -- imported from Ofri.
//  See ticket #78.
//
//  Revision 1.5  2003/10/28 16:34:03  ofri
//  Redeclaring the max macro for msvc6
//
//  Revision 1.4  2003/09/22 18:28:46  ofri
//  1. Replace macro name cmnExport with CISST_EXPORT (following Anton's
//  update).
//  2. Added abstract methods Scale(), ScaleCoefficients(), AddConstant(),
//  AddConstantToCoefficients() to the base class nmrPolynomialBase,
//  and actual implementations in nmrStandardPolynomial and
//  nmrBernsteinPolynomial.
//
//  Submitted after successful build but no testing yet.
//
//  Revision 1.3  2003/08/25 19:42:31  ofri
//  Bug fixing -- PowersContainer initialization with at least two cells.
//
//  Revision 1.2  2003/08/21 19:04:49  ofri
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
//  Revision 1.1  2003/08/21 18:00:57  ofri
//  Added classes nmrSingleVariablePowerBasis, nmrMultiVariablePowerBasis,
//  nmrMultiVariablePowerBasis::StandardPowerBasis,
//  nmrMultiVariablePowerBasis::BarycentricBasis
//
//
// ****************************************************************************
