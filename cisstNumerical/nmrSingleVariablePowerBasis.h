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
