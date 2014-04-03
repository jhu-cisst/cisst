/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
