/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrBernsteinPolynomial.h,v 1.10 2007/04/26 19:33:57 anton Exp $

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


#ifndef _nmrBernsteinPolynomial_h
#define _nmrBernsteinPolynomial_h

#include <cisstNumerical/nmrDynAllocPolynomialContainer.h>

#include <assert.h>

#include <cisstNumerical/nmrExport.h>

/*!
  \ingroup cisstNumerical

  class nmrBernsteinPolynomial defines a polynomial in Bernstein basis.
  In a Bernstein polynomial, all the terms are of equal degree (that is, the sum of
  powers is a constant), and the variables sum up to a unity. Each term is associated
  with a scalar coefficient and with a multinomial factor, which reflects the relative
  weight of the term in the expression:

  \f$1 = (x_0 + ... + x_{n-1}) ^ d = \sum_{(p_0 + ... + p_{n-1}) = d} \choose{d}{p_0 p_1 ... p_{n-1}} x_0^{p_0} ... x_{n-1}^{p_{n-1}}\f$
  
  To make repeated evaluations quicker, we cache the multinomial
  factor along with the coefficient of the term in a BernsteinTermInfo
  object. Appropriate accessors are defined.

  Since the sum of the variables is 1, one of the variables depends on
  the others. Typically, it is either the last or the first, but the
  user can choose any variable to be the ``implicit'' variable. Once
  the implicit variable has been determined, its value cannot be
  updated directly, and instead it is reassigned every time one of the
  other variables is set. However, the user of the class still has the
  flexibility to replace the choice of the implicit variable at any
  time. The only thing that matters is that the variables sum to 1.

  Note: We use dynamic allocation for the term information
  (BernsteinTermInfo), which is separate from the STL provided dynamic
  allocation for the container elements. We store the pointer to the
  dynamically allocated term info in the container.  Therefore:

  1. We had to override the base-class's RemoveTerm() and Clear() methods.
  2. Do not even try to make a copy of a nmrBernsteinPolynomial using copy-ctor or operator=
     (yeah, like you would).  Just for safety, we declare these operations protected, and
     do not provide implementation.
  3. A better solution may be to define a common base type for TermInfo, and have it declare
  a virtual dtor. Go for it, if you have time. */
class CISST_EXPORT nmrBernsteinPolynomial : public nmrDynAllocPolynomialContainer
{
public:
	typedef nmrDynAllocPolynomialContainer BaseType;
    
	typedef std::pair<nmrPolynomialBase::CoefficientType, 
		nmrPolynomialTermPowerIndex::MultinomialCoefficientType> BernsteinTermInfo;

#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(push)
#pragma warning(disable:4996)
#endif  // ifdef CISST_COMPILER_IS_MSVC
	/*!  Constructor determines the number of variables and the degree
      of the polynomial.  Note that Bernstein polynomial contains an
      additional implicit variable.  The argument numVariables must
      include the implicit variable, since we want to be consistent
      with the GetNumVariables() method. The degree defines both
      maximum and minimum degrees for the terms, as all terms are of
      equal degree.  The constructor initializes the free variables to
      zero, and the implicit variable to 1, so that they all sum up to
      1.  The constructor sets the implicit variable to be the last
      one (the numVariable - 1) by default. The user can change this
      setting by calling SetImplicitVarIndex(). */
	nmrBernsteinPolynomial(VariableIndexType numVariables, PowerType degree)
		: BaseType(numVariables, degree, degree)
#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
        , PowerBasis(numVariables, degree, numVariables - 1)
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE
	{
#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
        VariablePowers = &PowerBasis;
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE
	}

#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
	/*! Set the values of the polynomial variables to a specific
      n-dimensional point. This enables to pre-calculate the power
      basis for future evaluations. Even though the values[] array may
      contain a value for the implicit variable, it is not set
      directly, but rather assigned at the end to be 1 -
      sum_of_others. */
	virtual void CISST_DEPRECATED SetVariables(const VariableType values[])
    {
        PowerBasis.SetVariables(values);
    }

	/*! Set the value of one variable. Note that this affects the
      value of the implicit parameter.  The implicit variable cannot
      be changed directly. */
	virtual void CISST_DEPRECATED SetVariable(VariableIndexType varIndex, VariableType value)
    {
        PowerBasis.SetVariable(varIndex, value);
    }


	virtual bool CISST_DEPRECATED CanSetVariable(VariableIndexType varIndex) const
	{
		return ( (0 <= varIndex) && 
			(varIndex < GetNumVariables()) && 
			(varIndex != GetImplicitVarIndex()) );
	}

	/*! Return the value of the implicit variable. */
	VariableType CISST_DEPRECATED GetImplicitVariable() const
	{ 
        return PowerBasis.GetImplicitVariable();
    }

	/*! Return the index of the implicit variable */
	VariableIndexType CISST_DEPRECATED GetImplicitVarIndex() const
	{ 
        return PowerBasis.GetImplicitVarIndex();
    }

	/*! Choose which of the variables is the implicit one. */
	void CISST_DEPRECATED SetImplicitVarIndex(VariableIndexType newIndex)
	{ 
        PowerBasis.SetImplicitVarIndex(newIndex);
	}
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(pop)
#endif  // ifdef CISST_COMPILER_IS_MSVC
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE

	/*! Overriding nmrPolynomial::SetCoefficient(). This
      implementation computes the multinomial factor for the term. */
	virtual InsertStatus SetCoefficient(const nmrPolynomialTermPowerIndex & where, 
                                        CoefficientType coefficient);

	/*! Set a coefficient for the given term iterator.
      Implemented from nmrPolynomialContainer. */
	virtual InsertStatus SetCoefficient(TermIteratorType & where, CoefficientType coefficient);
    
	/*! Retrieve the value of the user defined coefficient for a given term. */
	virtual CoefficientType GetCoefficient(const nmrPolynomialTermPowerIndex & where) const;

	/*! Retrieve the value of the user defined coefficient for a term given by iterator. */
	virtual CoefficientType GetCoefficient(const TermConstIteratorType & where) const
	{ return GetTermCoefficient(where); }
	virtual CoefficientType GetCoefficient(const TermIteratorType & where) const
	{ return GetTermCoefficient(where); }

	/*! Remove a term from the polynomial. The term is given by
      iterator.  The function also reclaims the space allocated for
      the term. */
	virtual void RemoveTerm(TermIteratorType & where);

	/*! Remove all the terms to make an empty (zero) polynomial We
      cannot use nmrPolynomialContainer::Clear(), since it does not
      reclaim the space allocated for the term. This implementation
      does. */
	virtual void Clear();

	/*! Evaluate the basis function for a term.  This DOES NOT include
      the term's coefficient, but DOES include the multinomial
      factor. */
	virtual ValueType EvaluateBasis(const nmrPolynomialTermPowerIndex & where,
        const nmrMultiVariablePowerBasis & variables) const;
	virtual ValueType EvaluateBasis(const TermConstIteratorType & where,
        const nmrMultiVariablePowerBasis & variables) const;
	virtual ValueType EvaluateBasis(const TermIteratorType & where,
        const nmrMultiVariablePowerBasis & variables) const;

    /*!  This function is overridden to store the term coefficient to
      the stream.  It is called from
      nmrPolynomialContainer::SerializeRaw().
    */
    virtual void SerializeTermInfo(std::ostream & output, const TermConstIteratorType & termIterator) const;

    /*!  This function is overridden to allocate memory for the
      multinomial factor and the term coefficient, read the latter
      from the stream and calculate the former from the term power
      index.  It is called from
      nmrPolynomialContainer::DeserializeRaw().
    */
    virtual void DeserializeTermInfo(std::istream & input, TermIteratorType & termIterator);

    /*!  Overloaded from nmrPolynomialBase.  This implementation goes
    over all the possible for this polynomial, reads their current
    coefficient (which may be zero if the term is not included so far)
    then adds the given shift amount.  It may not be the most
    efficient method, but it should not be called too many times.
    */
    virtual void AddConstant(CoefficientType shiftAmount);

    /*! Overloaded from nmrPolynomialBase.  This implementation
      asserts that this polynomial contains all the possible terms,
      and then iterates on all the terms and adds the shift amount to
      the corresponding external coefficient.
    */
    virtual void AddConstantToCoefficients(CoefficientType coefficients[],
        CoefficientType shiftAmount) const;

	const nmrPolynomialTermPowerIndex::MultinomialCoefficientType &
		GetMultinomialCoefficient(const TermIteratorType & where) const
	{
        return ((BernsteinTermInfo *)(where->second))->second;
    }

	const nmrPolynomialTermPowerIndex::MultinomialCoefficientType & 
		GetMultinomialCoefficient(const TermConstIteratorType & where) const
	{
        return ((BernsteinTermInfo *)(where->second))->second;
    }


protected:
	nmrPolynomialTermPowerIndex::MultinomialCoefficientType &
		RefMultinomialCoefficient(const TermIteratorType & where)
	{
        return ((BernsteinTermInfo *)(where->second))->second;
    }

	const CoefficientType & GetTermCoefficient(const TermConstIteratorType & where) const
	{
        return ((BernsteinTermInfo *)(where->second))->first;
    }

	const CoefficientType & GetTermCoefficient(const TermIteratorType & where) const
	{
        return ((BernsteinTermInfo *)(where->second))->first;
    }

	CoefficientType & RefTermCoefficient(const TermIteratorType & where)
	{
        return ((BernsteinTermInfo *)(where->second))->first;
    }

#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
    CISST_DEPRECATED nmrMultiVariablePowerBasis::BarycentricBasis PowerBasis;
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE

};


#endif // _nmrBernsteinPolynomial_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrBernsteinPolynomial.h,v $
//  Revision 1.10  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.9  2006/11/20 20:33:19  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
//
//  Revision 1.8  2006/09/26 21:47:47  anton
//  cisstVector and cisstNumerical: Use #ifdef CISST_COMPILER_IS_MSVC instead
//  of #if (CISST_COMPILER == CISST_XYZ) for XYZ in Visual Studio 7~2000, 7.1~2003
//  and 8~2005.
//
//  Revision 1.7  2006/07/10 19:16:39  ofri
//  cisstNumerical polynomial classes: The code that had been declared
//  deprecated is now marked-out through #if brackets.  It will be removed
//  from the next release.
//
//  Revision 1.6  2006/06/12 20:21:19  ofri
//  cisstNumerical polynomial classes.  The internal cache of variable powers
//  (lookup table) is now declared deprecated.  The functionality should be
//  factored out to nmrMultiVariablePowerBasis objects.  Pragmas set for MSVC
//  to reduce the number of compilation warnings.  The inner
//  #define FACTOR_OUT_POLYNOMIAL_VARIABLES has been removed, as well as all
//  references to it.
//
//  Revision 1.5  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.4  2005/06/03 18:20:58  anton
//  cisstNumerical: Added license.
//
//  Revision 1.3  2004/11/08 18:06:53  anton
//  cisstNumerical: Major Doxygen update based on Ankur's notes and Ofri's
//  comments in the code.
//
//  Revision 1.2  2004/10/22 01:24:37  ofri
//  Cleanup following migration to cisst, and rebuild with cygwin/gcc
//
//  Revision 1.1  2004/10/21 19:52:51  ofri
//  Adding polynomial-related header files to the repository -- imported from Ofri.
//  See ticket #78.
//
//  Revision 1.10  2003/12/01 16:10:40  ofri
//  Cross compiler compatibility with gcc/.net
//
//  Revision 1.9  2003/10/15 19:09:12  ofri
//  Added some line break for easier debugging
//
//  Revision 1.8  2003/09/22 18:28:47  ofri
//  1. Replace macro name cmnExport with CISST_EXPORT (following Anton's
//  update).
//  2. Added abstract methods Scale(), ScaleCoefficients(), AddConstant(),
//  AddConstantToCoefficients() to the base class nmrPolynomialBase,
//  and actual implementations in nmrStandardPolynomial and
//  nmrBernsteinPolynomial.
//
//  Submitted after successful build but no testing yet.
//
//  Revision 1.7  2003/08/26 20:20:51  ofri
//  Factored out the evaluation of polynomial to use variables from
//  nmrMultiVariablePowerBasis.  This affected methods in nmrPolynomialBase,
//  nmrPolynomialContainer, nmrStandardPolynomial, nmrBernsteinPolynomial .
//  The revision was tested successfully.
//
//  Revision 1.6  2003/08/21 19:04:48  ofri
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
//  Revision 1.5  2003/07/21 17:44:34  ofri
//  1. Added documentation for the Serialize and Deserialize protocols/methods
//  in the various polynomial classes.
//  2. Extracted method nmrPolynomialPowerTermIndex::SerializeIndexRaw()
//  from SerializeRaw()
//
//  Revision 1.4  2003/07/16 22:07:02  ofri
//  Adding serialization functionality to the polynomial classes and the
//  nmrPolynomialTermPowerIndex class
//
//  Revision 1.3  2003/07/10 15:50:16  ofri
//  Adding change history log to the files in cisstNumerical. I guess in a few files
//  there was replacement of spaces and tabs, which was recorded in CVS as
//  a big change, unfortunately. But for most, it's just adding the tags.
//
//
// ****************************************************************************
