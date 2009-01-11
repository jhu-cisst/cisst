/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrBernsteinPolynomial.cpp,v 1.7 2007/04/26 19:33:57 anton Exp $

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

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrBernsteinPolynomial.cpp,v $
//  Revision 1.7  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.6  2006/11/20 20:33:19  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
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
//  Revision 1.9  2003/12/24 17:57:46  ofri
//  Updates to cisNumerical:
//  1) Using #include <...> instead of #include "..." for cisst header files
//  2) Getting rid of cisMatrixVector in most places (if not all), replacing it with
//  vnl
//
//  Revision 1.8  2003/09/22 18:29:22  ofri
//  Added abstract methods Scale(), ScaleCoefficients(), AddConstant(),
//  AddConstantToCoefficients() to the base class nmrPolynomialBase,
//  and actual implementations in nmrStandardPolynomial and
//  nmrBernsteinPolynomial.
//
//  Submitted after successful build but no testing yet.
//
//  Revision 1.7  2003/08/26 20:21:17  ofri
//  Factored out the evaluation of polynomial to use variables from
//  nmrMultiVariablePowerBasis.  This affected methods in nmrPolynomialBase,
//  nmrPolynomialContainer, nmrStandardPolynomial, nmrBernsteinPolynomial .
//  The revision was tested successfully.
//
//  Revision 1.6  2003/08/25 19:45:15  ofri
//  Turned off test construction of nmrBernsteinPolynomial
//
//  Revision 1.5  2003/08/21 19:04:58  ofri
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
//  Revision 1.4  2003/07/16 22:05:45  ofri
//  Adding serialization functionality to the polynomial classes and the
//  nmrPolynomialTermPowerIndex class
//
//  Revision 1.3  2003/07/10 15:50:15  ofri
//  Adding change history log to the files in cisstNumerical. I guess in a few files
//  there was replacement of spaces and tabs, which was recorded in CVS as
//  a big change, unfortunately. But for most, it's just adding the tags.
//
//
// ****************************************************************************
