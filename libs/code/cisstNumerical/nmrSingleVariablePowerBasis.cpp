/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrSingleVariablePowerBasis.cpp,v 1.8 2007/04/26 19:33:57 anton Exp $

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


#include <cisstNumerical/nmrSingleVariablePowerBasis.h>

#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(push)
#pragma warning(disable:4996)
#endif  // ifdef CISST_COMPILER_IS_MSVC

void nmrSingleVariablePowerBasis::SetVariable(VariableType variable)
{
    PowersContainerType::const_iterator formerPowerIterator =
        PowersContainer.begin();
    PowersContainerType::iterator currentPowerIterator =
        PowersContainer.begin() + 1;
    PowersContainerType::iterator endPowerIterator =
        PowersContainer.end();
    nmrPolynomialTermPowerIndex::PowerType power = 1;
    for (; currentPowerIterator != endPowerIterator; 
    ++currentPowerIterator, ++power) {
        *currentPowerIterator = (*formerPowerIterator) * (*formerPowerIterator);
        if ( (power % 2) != 0 ) {
            *currentPowerIterator *= variable;
            ++formerPowerIterator;
        }
    }
}

#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(pop)
#endif  // ifdef CISST_COMPILER_IS_MSVC

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrSingleVariablePowerBasis.cpp,v $
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
//  Revision 1.5  2006/06/12 19:43:28  ofri
//  nmrSingleVariablePowerBasis: class is declared deprecated and will be
//  removed from future releases.
//
//  Revision 1.4  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.3  2005/06/03 18:20:58  anton
//  cisstNumerical: Added license.
//
//  Revision 1.2  2004/10/22 01:24:51  ofri
//  Cleanup following migration to cisst, and rebuild with cygwin/gcc
//
//  Revision 1.1  2004/10/21 19:53:07  ofri
//  Adding polynomial-related cpp files to the repository -- imported from Ofri.
//  See ticket #78.
//
//  Revision 1.2  2003/09/29 23:28:19  ofri
//  nmrSingleVariablePowerBasis now evaluates powers of the variable
//  using p(x,n) = p(x, n/2)^2 -- nore stable numerically.
//  (not tested)
//
//  Revision 1.1  2003/08/21 18:00:43  ofri
//  Added classes nmrSingleVariablePowerBasis, nmrMultiVariablePowerBasis,
//  nmrMultiVariablePowerBasis::StandardPowerBasis,
//  nmrMultiVariablePowerBasis::BarycentricBasis
//
//
// ****************************************************************************
