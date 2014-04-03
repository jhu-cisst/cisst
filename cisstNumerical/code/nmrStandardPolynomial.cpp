/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:   2003

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstNumerical/nmrStandardPolynomial.h>


#include <assert.h>
#include <iostream>

#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(push)
#pragma warning(disable:4996)
#endif  // ifdef CISST_COMPILER_IS_MSVC
void nmrStandardPolynomial::SetVariable(VariableIndexType varIndex, VariableType value)
{
    VariablePowers->SetVariable(varIndex, value);
}

void nmrStandardPolynomial::SetVariables(const VariableType vars[])
{
	VariableIndexType v;
	VariableIndexType numVars = GetNumVariables();
	for (v = 0; v < numVars; v++) {
		SetVariable(v, vars[v]);
	}
}
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(pop)
#endif  // ifdef CISST_COMPILER_IS_MSVC
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE

nmrStandardPolynomial::InsertStatus nmrStandardPolynomial::SetCoefficient(const nmrPolynomialTermPowerIndex & where, CoefficientType coefficient)
{
	if (!this->CanIncludeIndex(where))
		return INSERT_FAIL;

#if (CONTAINER_TYPE == MAP_CONTAINER)

	TermIteratorType foundTerm = FindTerm(where);
	if (foundTerm != EndTermIterator()) {
		DereferenceIterator(foundTerm) = coefficient;
		return INSERT_REPLACE;
	}

	TermType term(where, new CoefficientType(coefficient) );
	std::pair<TermIteratorType, bool> result = Terms.insert(term);
	return INSERT_NEW;

#elif (CONTAINER_TYPE == LIST_CONTAINER)

    TermIteratorType foundTerm = std::find_if(FirstTermIterator(), EndTermIterator(), LessOrEqualityTester(where));
    if (foundTerm == EndTermIterator()) {
        Terms.insert( EndTermIterator(), ContainerElementType(where, new CoefficientType(coefficient)) );
        return INSERT_NEW;
    }

    if (where.Compare( (*foundTerm).first ) == 0) {
        DereferenceIterator(foundTerm) = coefficient;
        return INSERT_REPLACE;
    }

    Terms.insert( foundTerm, ContainerElementType(where, new CoefficientType(coefficient)) );
    return INSERT_NEW;
#endif
}


void nmrStandardPolynomial::RemoveTerm(TermIteratorType & where)
{
	delete &(DereferenceIterator(where));
	BaseType::RemoveTerm(where);
}


void nmrStandardPolynomial::Clear()
{
	TermIteratorType termIt = FirstTermIterator();
	while (termIt != EndTermIterator()) {
		CoefficientType * pCoeff = &(DereferenceIterator(termIt));
		delete pCoeff;
		termIt++;
	}

	BaseType::Clear();
}


void nmrStandardPolynomial::SerializeTermInfo(std::ostream & output, const TermConstIteratorType & termIterator) const
{
    const CoefficientType pCoeff = DereferenceConstIterator(termIterator);
    output.write( (const char *)&pCoeff, sizeof(CoefficientType) );
}

void nmrStandardPolynomial::DeserializeTermInfo(std::istream & input, TermIteratorType & termIterator)
{
    termIterator->second = new CoefficientType();
    input.read( (char *)termIterator->second, sizeof(CoefficientType) );
}


void nmrStandardPolynomial::AddConstant(CoefficientType shiftAmount)
{
    nmrPolynomialTermPowerIndex termIndex(*this);
    termIndex.SetDegree(0);
    CoefficientType currentConstant = GetCoefficient(termIndex);
    SetCoefficient(termIndex, currentConstant + shiftAmount);
}

void nmrStandardPolynomial::AddConstantToCoefficients(CoefficientType coefficients[],
    CoefficientType shiftAmount) const
{
    TermConstIteratorType termIt = FirstTermIterator();
    TermConstIteratorType endIt = EndTermIterator();
    TermCounterType termCounter = 0;

    for(; termIt != endIt; ++termIt, ++termCounter) {
        if (GetTermPowerIndex(termIt).GetDegree() == 0) {
            coefficients[termCounter] += shiftAmount;
            return;
        }
    }

    assert(false);
}

#define STANDARD_POLYNOMIAL_TEST_CONSTRUCTOR 0

#if STANDARD_POLYNOMIAL_TEST_CONSTRUCTOR
static nmrStandardPolynomial g_TestStandardPolynomial(4, 0, 3);
#endif
