/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:   2001-10-17

  (C) Copyright 2001-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstNumerical/nmrBernsteinPolynomial.h>

#include <iostream>

//***************************************************************************
// Methods of class nmrBernsteinPolynomial
//***************************************************************************


// Evaluate a single term at the currently specified point. This DOES NOT
// include the term's coefficient.
nmrBernsteinPolynomial::ValueType 
nmrBernsteinPolynomial::EvaluateBasis(const nmrPolynomialTermPowerIndex & where,
    const nmrMultiVariablePowerBasis & variables) const
{
    CMN_ASSERT(where.GetNumVariables() == variables.GetNumVariables());
    CMN_ASSERT(where.GetDegree() <= variables.GetMaxDegree());
	const ValueType powerProduct = variables.EvaluatePowerProduct(where.GetPowers());

	const nmrPolynomialTermPowerIndex::MultinomialCoefficientType 
		multinomial = where.GetMultinomialCoefficient();

	return powerProduct * multinomial;
}

// Evaluate a single term at the currently specified point. This DOES NOT
// include the term's coefficient.
nmrBernsteinPolynomial::ValueType 
nmrBernsteinPolynomial::EvaluateBasis(const TermConstIteratorType & where,
    const nmrMultiVariablePowerBasis & variables) const
{
	const nmrPolynomialTermPowerIndex::MultinomialCoefficientType 
		multinomial = GetMultinomialCoefficient(where);

	const nmrPolynomialTermPowerIndex & powers = where->first;

    assert(powers.GetNumVariables() == variables.GetNumVariables());
    assert(powers.GetDegree() <= variables.GetMaxDegree());
	const ValueType powerProduct = variables.EvaluatePowerProduct(powers.GetPowers());

	return powerProduct * multinomial;
}


// Evaluate a single term at the currently specified point. This DOES NOT
// include the term's coefficient.
nmrBernsteinPolynomial::ValueType 
nmrBernsteinPolynomial::EvaluateBasis(const TermIteratorType & where,
    const nmrMultiVariablePowerBasis & variables) const
{
	const nmrPolynomialTermPowerIndex::MultinomialCoefficientType 
		multinomial = GetMultinomialCoefficient(where);

	const nmrPolynomialTermPowerIndex & powers = where->first;
    assert(powers.GetNumVariables() == variables.GetNumVariables());
    assert(powers.GetDegree() <= variables.GetMaxDegree());
	const ValueType powerProduct = variables.EvaluatePowerProduct(powers.GetPowers());

	return powerProduct * multinomial;
}


nmrPolynomialBase::InsertStatus
nmrBernsteinPolynomial::SetCoefficient(const nmrPolynomialTermPowerIndex & where, 
											CoefficientType coefficient)
{
	if (!this->CanIncludeIndex(where) )
		return INSERT_FAIL;

#if (CONTAINER_TYPE == MAP_CONTAINER)

	TermIteratorType foundTerm = FindTerm(where);
	if (foundTerm != EndTermIterator()) {
		RefTermCoefficient(foundTerm) = coefficient;
		return INSERT_REPLACE;
	}

	TermType term( where, new BernsteinTermInfo(coefficient, where.GetMultinomialCoefficient()) );
	std::pair<TermIteratorType, bool> result(Terms.insert(term));
	return INSERT_NEW;

#elif (CONTAINER_TYPE == LIST_CONTAINER)

    TermIteratorType foundTerm = std::find_if(FirstTermIterator(), EndTermIterator(), LessOrEqualityTester(where));
    if (foundTerm == EndTermIterator()) {
        Terms.insert( EndTermIterator(), 
            ContainerElementType(where, 
            new BernsteinTermInfo(coefficient, where.GetMultinomialCoefficient()) 
            )
            );
        return INSERT_NEW;
    }

    if (where.Compare( (*foundTerm).first ) == 0) {
		RefTermCoefficient(foundTerm) = coefficient;
        return INSERT_REPLACE;
    }

    Terms.insert( foundTerm, 
        ContainerElementType(where, 
        new BernsteinTermInfo(coefficient, where.GetMultinomialCoefficient()) 
        )
        );
    return INSERT_NEW;
#endif
}


nmrPolynomialBase::InsertStatus
nmrBernsteinPolynomial::SetCoefficient(TermIteratorType & where, CoefficientType coefficient)
{
	RefTermCoefficient(where) = coefficient;
	return INSERT_REPLACE;
}

nmrBernsteinPolynomial::CoefficientType
nmrBernsteinPolynomial::GetCoefficient(const nmrPolynomialTermPowerIndex & where) const
{
	assert(this->CanIncludeIndex(where));

	TermConstIteratorType termIt = FindTerm(where);
	if (termIt != EndTermIterator()) {
		return GetTermCoefficient(termIt);
	}

	return 0;

}


void nmrBernsteinPolynomial::RemoveTerm(TermIteratorType & where)
{
	delete ((BernsteinTermInfo *)(where->second));
	BaseType::RemoveTerm(where);
}


void nmrBernsteinPolynomial::Clear()
{
	TermIteratorType termIt = FirstTermIterator();
	while (termIt != EndTermIterator()) {
		delete ((BernsteinTermInfo *)(termIt->second));
		termIt++;
	}

	BaseType::Clear();
}

void nmrBernsteinPolynomial::SerializeTermInfo(std::ostream & output, const TermConstIteratorType & termIterator) const
{
    const CoefficientType coefficient = GetTermCoefficient(termIterator);
    output.write( (const char *)&coefficient, sizeof(coefficient) );
}

void nmrBernsteinPolynomial::DeserializeTermInfo(std::istream & input, TermIteratorType & termIterator)
{
    CoefficientType coefficient;
    input.read( (char *)&coefficient, sizeof(coefficient) );
    BernsteinTermInfo * newTermInfo = 
        new BernsteinTermInfo( coefficient,
        GetTermPowerIndex(termIterator).GetMultinomialCoefficient() );
    termIterator->second = newTermInfo;
}

void nmrBernsteinPolynomial::AddConstant(CoefficientType shiftAmount)
{
    nmrPolynomialTermPowerIndex termIndex(*this);
    termIndex.GoBegin();
    while (termIndex.IsValid()) {
        CoefficientType termCoefficient = GetCoefficient(termIndex);
        SetCoefficient(termIndex, termCoefficient + shiftAmount);

        termIndex.Increment();
    }
}

void nmrBernsteinPolynomial::AddConstantToCoefficients(CoefficientType coefficients[],
                                                       CoefficientType shiftAmount) const
{
    assert( GetNumberOfTerms() == GetMaxNumberOfTerms() );

    TermConstIteratorType termIt = FirstTermIterator();
    TermConstIteratorType endIt = EndTermIterator();
    TermCounterType termCounter = 0;

    for (; termIt != endIt; ++termIt, ++termCounter) {
        coefficients[termCounter] += shiftAmount;
    }
}

#define BERNSTEIN_POLYNOMIAL_TEST_CONSTRUCTOR 0

#if BERNSTEIN_POLYNOMIAL_TEST_CONSTRUCTOR
static nmrBernsteinPolynomial g_TestBernsteinPolynomial(4, 3);
#endif
