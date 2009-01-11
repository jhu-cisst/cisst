/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrStandardPolynomial.h,v 1.9 2007/04/26 19:33:57 anton Exp $

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


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrStandardPolynomial.h,v $
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
//  Revision 1.6  2006/07/10 19:16:39  ofri
//  cisstNumerical polynomial classes: The code that had been declared
//  deprecated is now marked-out through #if brackets.  It will be removed
//  from the next release.
//
//  Revision 1.5  2006/06/12 20:21:19  ofri
//  cisstNumerical polynomial classes.  The internal cache of variable powers
//  (lookup table) is now declared deprecated.  The functionality should be
//  factored out to nmrMultiVariablePowerBasis objects.  Pragmas set for MSVC
//  to reduce the number of compilation warnings.  The inner
//  #define FACTOR_OUT_POLYNOMIAL_VARIABLES has been removed, as well as all
//  references to it.
//
//  Revision 1.4  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.3  2005/06/03 18:20:58  anton
//  cisstNumerical: Added license.
//
//  Revision 1.2  2004/11/08 18:06:54  anton
//  cisstNumerical: Major Doxygen update based on Ankur's notes and Ofri's
//  comments in the code.
//
//  Revision 1.1  2004/10/21 19:52:51  ofri
//  Adding polynomial-related header files to the repository -- imported from Ofri.
//  See ticket #78.
//
//  Revision 1.9  2004/04/19 14:45:54  ofri
//  Cosmetic changes.
//
//  Revision 1.8  2003/09/22 18:28:46  ofri
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
//  Revision 1.6  2003/08/21 19:04:49  ofri
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
//  Revision 1.4  2003/07/16 22:07:03  ofri
//  Adding serialization functionality to the polynomial classes and the
//  nmrPolynomialTermPowerIndex class
//
//  Revision 1.3  2003/07/10 15:50:17  ofri
//  Adding change history log to the files in cisstNumerical. I guess in a few files
//  there was replacement of spaces and tabs, which was recorded in CVS as
//  a big change, unfortunately. But for most, it's just adding the tags.
//
//
// ****************************************************************************
