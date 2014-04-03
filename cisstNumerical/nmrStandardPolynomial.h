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


#ifndef _nmrStandardPolynomial_h
#define _nmrStandardPolynomial_h


#include <cisstNumerical/nmrDynAllocPolynomialContainer.h>
#include <cisstNumerical/nmrExport.h>


/*!
  \ingroup cisstNumerical

  class nmrStandardPolynomial represents a polynomial in standard
  (power) basis.  Each term consists of a power index and a
  coefficient. The structure of nmrStandardPolynomial is based on
  nmrPolynomialContainer, and it implements the required methods in a
  straightforward way.

  Note: some methods include assertions. To avoid including the
  <assert.h> header file by all the files that use polynomials, I put
  these methods, and some which were more than one-liners, in the .cpp
  file. But in every practical way, nmrStandardPolynomial could be
  implemented completely as inline.


  Note: We use dynamic allocation for the term information (Coefficient), which
  is separate from the STL provided dynamic allocation for the container elements. We
  store the pointer to the dynamically allocated term info in the container.  Therefore:
  1. We had to override the base-class's RemoveTerm() and Clear() methods.
  2. Do not even try to make a copy of a nmrBernsteinPolynomial using copy-ctor or operator=
     (yeah, like you would).  Just for safety, we declare these operations protected, and
     do not provide implementation.
  3. A better solution may be to define a common base type for TermInfo, and have it declare
  a virtual dtor. Go for it, if you have time. */
class CISST_EXPORT nmrStandardPolynomial : public nmrDynAllocPolynomialContainer
{
public:
	typedef nmrDynAllocPolynomialContainer BaseType;

	/*! Constructor determines the number of variables and the degree
      of the polynomial. */
	nmrStandardPolynomial(VariableIndexType numVariables, PowerType minDegree, PowerType maxDegree)
		: BaseType(numVariables, minDegree, maxDegree)
#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
        , PowerBasis(numVariables, maxDegree)
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE

    {
#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(push)
#pragma warning(disable:4996)
#endif  // ifdef CISST_COMPILER_IS_MSVC
        VariablePowers = &PowerBasis;
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(pop)
#endif  // ifdef CISST_COMPILER_IS_MSVC
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE
    }

	virtual ~nmrStandardPolynomial()
	{}

#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(push)
#pragma warning(disable:4996)
#endif  // ifdef CISST_COMPILER_IS_MSVC
	virtual void CISST_DEPRECATED SetVariable(VariableIndexType varIndex, VariableType value);

	virtual void CISST_DEPRECATED SetVariables(const VariableType vars[]);

	virtual bool CISST_DEPRECATED CanSetVariable(VariableIndexType varIndex) const
	{
		return ( (0 <= varIndex) && (varIndex < GetNumVariables()) );
	}
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(pop)
#endif  // ifdef CISST_COMPILER_IS_MSVC
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE

	/*! Set a coefficient for the given term. */
	virtual InsertStatus SetCoefficient(const nmrPolynomialTermPowerIndex & where, CoefficientType coefficient);

	virtual InsertStatus SetCoefficient(TermIteratorType & where, CoefficientType coefficient)
	{ 
		DereferenceIterator(where) = coefficient;
		return INSERT_REPLACE;
	}

	
	/*! Retrieve the value of the user defined coefficient for a given term. */
	virtual CoefficientType GetCoefficient(const nmrPolynomialTermPowerIndex& where) const
	{
		TermConstIteratorType it = FindTerm(where);

		return (it != EndTermIterator())
			? DereferenceConstIterator(it)
			: 0;
	}

	virtual CoefficientType GetCoefficient(const TermConstIteratorType & where) const
	{
		return DereferenceConstIterator(where);
	}

	virtual CoefficientType GetCoefficient(const TermIteratorType & where) const
	{
		return DereferenceConstIterator(where);
	}


	
	/*! Remove a term from the polynomial. The term is given by
      iterator.  The function also reclaims the space allocated for
      the term. */
	virtual void RemoveTerm(TermIteratorType & where);

	/*! Remove all the terms to make an empty (zero) polynomial We
      cannot use nmrPolynomialContainer::Clear(), since it does not
      reclaim the space allocated for the term. This implementation
      does. */
	virtual void Clear();
	
    virtual ValueType EvaluateBasis(const nmrPolynomialTermPowerIndex & where,
        const nmrMultiVariablePowerBasis & variables) const
    {
        assert(where.GetNumVariables() == variables.GetNumVariables());
        assert(where.GetDegree() <= variables.GetMaxDegree());
        return variables.EvaluatePowerProduct(where.GetPowers());
    }

	virtual ValueType EvaluateBasis(const TermConstIteratorType & where,
        const nmrMultiVariablePowerBasis & variables) const
	{ 
        return EvaluateBasis( where->first, variables );
    }

	virtual ValueType EvaluateBasis(const TermIteratorType & where,
        const nmrMultiVariablePowerBasis & variables) const
	{ 
        return EvaluateBasis( where->first, variables ); 
    }

    /*!  This function is overridden to store the term coefficient to
      the stream.  It is called from
      nmrPolynomialContainer::SerializeRaw().
    */
    virtual void SerializeTermInfo(std::ostream & output, const TermConstIteratorType & termIterator) const;

    /*!  This function is overridden to allocate memory for and read
      the term coefficient.  It is called from
      nmrPolynomialContainer::DeserializeRaw().
    */
    virtual void DeserializeTermInfo(std::istream & input, TermIteratorType & termIterator);

    /*! An actual implementation overriding nmrPolynomialBase.  In
      this implementation, the coefficient of the term with degree
      zero is increased by the given shift amount.
    */
    virtual void AddConstant(CoefficientType shiftAmount);

    /*! Overloaded from nmrPolynomialBase.  This implementation
      verifies that this polynomial contains a term of degree zero,
      and adds the shift amount to the corresponding external
      coefficient.
    */
    virtual void AddConstantToCoefficients(CoefficientType coefficients[],
        CoefficientType shiftAmount) const;

protected:

	CoefficientType & DereferenceIterator(const TermIteratorType & it)
	{ 
		CoefficientType * pCoeff = (CoefficientType *)(it->second);
		return *pCoeff; 
	}

	CoefficientType DereferenceIterator(const TermIteratorType & it) const
	{ return *((CoefficientType *)(it->second)); }

	CoefficientType DereferenceConstIterator(const TermConstIteratorType & it) const
	{ return *((CoefficientType *)(it->second)); }

#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
    CISST_DEPRECATED nmrMultiVariablePowerBasis::StandardPowerBasis PowerBasis;
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE

};


#endif // _nmrStandardPolynomial_h

