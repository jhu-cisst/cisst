/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:   2003-08-20

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstNumerical/nmrMultiVariablePowerBasis.h>

void nmrMultiVariablePowerBasis::Initialize(VariableIndexType numVariables, 
                                            PowerType maxPower)
{
    const PowerType numColumns = std::max(maxPower + 1, 2);
    BasisContainer.SetSize(numVariables, numColumns);
    BasisContainer.SetAll(0.0);
    BasisContainer.Column(0).SetAll(1.0);
}


void nmrMultiVariablePowerBasis::PrivateSetVariable(VariableIndexType varIndex,
                                                    VariableType value)
{
    const PowerType maxDegree = this->GetMaxDegree();
    VariableType * endPower = BasisContainer.Pointer(varIndex, maxDegree + 1);
    VariableType * currentPower = BasisContainer.Pointer(varIndex, 1);
    BasisContainer.Element(varIndex, 0) = 1.0;
    for (; currentPower != endPower; ++currentPower) {
        *currentPower = *(currentPower - 1) * value;
    }
}

void nmrMultiVariablePowerBasis::StandardPowerBasis::SetVariables(
    const VariableType vars[])
{
    VariableIndexType v;
    VariableIndexType numVars = GetNumVariables();
    for (v = 0; v < numVars; v++) {
        SetVariable(v, vars[v]);
    }
}

nmrMultiVariablePowerBasis::BarycentricBasis::BarycentricBasis(
    VariableIndexType numVariables, PowerType maxPower, VariableIndexType implicitVarIndex)
    : BaseType(numVariables, maxPower)
    , ImplicitVarIndex( ((implicitVarIndex < 0) || (implicitVarIndex >= numVariables)) 
    ? numVariables - 1 : implicitVarIndex)
{
    PrivateSetVariable(ImplicitVarIndex, 1);
}

void nmrMultiVariablePowerBasis::BarycentricBasis::SetVariable(
    VariableIndexType varIndex, VariableType value)
{
    CMN_ASSERT(CanSetVariable(varIndex));

    // newNu stores the new value of (1 - sum_of_all_free_variables).
    const VariableType oldValue = GetVariable(varIndex);
    const VariableType oldImplicit = GetImplicitVariable();
    const VariableType newImplicit = oldImplicit + oldValue - value;

    PrivateSetVariable(varIndex, value);

    PrivateSetVariable(ImplicitVarIndex, newImplicit);

}

void nmrMultiVariablePowerBasis::BarycentricBasis::SetVariables(
    const VariableType vars[])
{
    VariableType sumValues = 0.0;
    VariableIndexType v;
    const VariableIndexType numVars = this->GetNumVariables();
    for (v = 0; v < numVars; ++v) {
        if (v == ImplicitVarIndex)
            continue;

        PrivateSetVariable(v, vars[v]);
        sumValues += vars[v];
    }

    PrivateSetVariable(ImplicitVarIndex, 1.0 - sumValues);
}


nmrMultiVariablePowerBasis::ValueType 
nmrMultiVariablePowerBasis::EvaluatePowerProduct(const PowerType powers[]) const
{
    ValueType result = 1;
    VariableIndexType v;
    const VariableIndexType numVars = GetNumVariables();
    for (v = 0; v < numVars; v++) {
        result *= GetVariablePower(v, powers[v]);
    }

    return result;
}
