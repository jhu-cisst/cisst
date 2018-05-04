/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:   2001-10-16

  (C) Copyright 2001-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrPolynomialBase_h
#define _nmrPolynomialBase_h


#include <vector>
#include <cisstCommon/cmnPortability.h>
#include <cisstNumerical/nmrPolynomialTermPowerIndex.h>

#include <cisstNumerical/nmrMultiVariablePowerBasis.h>

#include <cisstNumerical/nmrExport.h>

// This control macro defines, when set to one, the inclusion of deprecated
// code in the polynomial classes.  We everything that depends on a 'true'
// value of this macro to be erased from future releases of the cisst library.
#define INCLUDE_DEPRECATED_POLYNOMIAL_CODE 0

// class nmrPolynomialBase is an abstract base class for various polynomial classes.
// Since different polynomial classes store different information for the terms (for
// example, a standard polynomial stores a coefficient, a Bernstein polynomial stores a 
// coefficient and a multinomial factor), the base class defined the common subset of
// operations for a polynomial, but does not implement the storage for the terms, or 
// the rules that relate the variables (e.g., the sum of variables is always one for
// a Bernstein polynomial).
//
// A term in the polynomial is identified by an index, which is the set of powers for
// all the variables of the polynomial in the term. See class nmrPolynomialTermPowerIndex.
//
// An implementation of a multi-variable polynomial is in class nmrPolynomialContainer.
class CISST_EXPORT nmrPolynomialBase
{
public:
	typedef double ValueType;
	typedef double VariableType;
	typedef double CoefficientType;
	typedef nmrPolynomialTermPowerIndex::VariableIndexType VariableIndexType;
	typedef nmrPolynomialTermPowerIndex::PowerType PowerType;
	typedef nmrPolynomialTermPowerIndex::MultinomialCoefficientType TermCounterType;

	enum InsertStatus { INSERT_FAIL = -1, INSERT_NEW = 0, INSERT_REPLACE = 1 };

	// Ctor determines the number of variables and the degree of the polynomial
	nmrPolynomialBase(VariableIndexType numVariables, PowerType minDegree, PowerType maxDegree);

	virtual ~nmrPolynomialBase()
	{}

	VariableIndexType GetNumVariables() const
	{
        return NumVariables;
    }

	PowerType GetMinDegree() const
	{
        return MinDegree;
    }

	PowerType GetMaxDegree() const
	{
        return MaxDegree;
    }

	// implemented in nmrPolynomialContainer
	virtual TermCounterType GetNumberOfTerms() const = 0;

	// return the maximum possible number of terms for this polynomial, based on
	// the degrees.
	TermCounterType GetMaxNumberOfTerms() const
	{
        return (nmrPolynomialTermPowerIndex(*this)).CountPowerCombinations();
    }
	
#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
	// Set the values of the polynomial variables to a specific n-dimensional
	// point. This enables to pre-calculate the power basis for future 
	// evaluations.
	// Implemented for each concrete polynomial class
	virtual void CISST_DEPRECATED SetVariables(const VariableType vars[]) = 0;

	// Set the value of one variable
	// Implemented for each concrete polynomial class.
	virtual void CISST_DEPRECATED SetVariable(VariableIndexType varIndex, VariableType value) = 0;

	// Returns true if the value of variable indicated by the index can be set
	// using SetVariable().
	virtual bool CISST_DEPRECATED CanSetVariable(VariableIndexType varIndex) const = 0;

#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(push)
#pragma warning(disable:4996)
#endif  // ifdef CISST_COMPILER_IS_MSVC
	// Retrieve the values of the variables at the current point
	void CISST_DEPRECATED GetVariables(VariableType vars[]) const 
	{
		VariableIndexType v;
		for (v = 0; v < GetNumVariables(); ++v)
			vars[v] = GetVariable(v);
	}

	// Retrieve the value of one variable
	VariableType CISST_DEPRECATED GetVariable(VariableIndexType var) const
	{ 
		return VariablePowers->GetVariable(var);
	}

	// Return a specific power of a specific variable
	VariableType CISST_DEPRECATED GetVariablePower(VariableIndexType var, PowerType power) const
	{
        return VariablePowers->GetVariablePower(var, power);
	}

#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(pop)
#endif  // ifdef CISST_COMPILER_IS_MSVC
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE

	//: Tell if there are any nonzero terms in the polynomial.
	// Implemented in nmrPolynomialContainer.
	virtual bool IsEmpty() const = 0;

    //: Update the minimum allowable degree of this polynomial to the given
    // input degree.  The method verifies that the polynomial currently contains
    // only terms of degree greater than or equal to the input, by looking at the
    // degree of the first term, which is presumably the lowest.  Note that terms
    // already contained in this polynomial aren't modified.
    virtual void SetMinDegree(PowerType newMin) CISST_THROW(std::runtime_error) = 0;


    //: Update the maximum allowable degree of this polynomial to the given
    // input degree.  The method verifies that the polynomial currently contains
    // only terms of degree lower than or equal to the input by looking at the
    // degree of the last term, which is presumably the highest.  Note that terms
    // already contained in this polynomial aren't modified.
    virtual void SetMaxDegree(PowerType newMax) CISST_THROW(std::runtime_error) = 0;

	//: Generate all possible terms for this polynomial, and store them for later
	// use.
	// Implemented in nmrPolynomialContainer.
	virtual void FillAllTerms() = 0;
	
	// Tells if this polynomial can have the given term as one of its terms.
	// In other words, if it's a valid term of this polynomial.
	bool CanIncludeIndex(const nmrPolynomialTermPowerIndex& term) const;

	// Find a term by its coefficients. Return true is the term is found.
	// return 'false' otherwise.
	// Implemented in nmrPolynomialContainer.
	virtual bool IncludesIndex(const nmrPolynomialTermPowerIndex& target) const = 0;

    // Find a term by its coefficients.  If the term is found, return the sequential
    // position (zero-based) of the term in the sequence of terms for this polynomial.
    // If the term is not found, return a number greater than or equal to the current number 
    // of terms in the sequence.
    virtual TermCounterType GetIndexPosition(const nmrPolynomialTermPowerIndex & term) const = 0;

	// Remove a term from the polynomial.
	// Implemented in nmrPolynomialContainer.
	virtual void RemoveTerm(const nmrPolynomialTermPowerIndex & where) = 0;

	// Remove all the terms to make an empty (zero) polynomial
	// Implemented in nmrPolynomialContainer.
	virtual void Clear() = 0;

	// Set a coefficient for the given term.
	// Implemented for each concrete polynomial class.
	virtual InsertStatus SetCoefficient(const nmrPolynomialTermPowerIndex & where, CoefficientType coefficient) = 0;

	// Retrieve the value of the user defined coefficient for a given term.
	// Implemented for each concrete polynomial class.
	virtual CoefficientType GetCoefficient(const nmrPolynomialTermPowerIndex & CMN_UNUSED(where)) const
	{ 
        return 0; 
    }

    // Store all the coefficients into the target array, ordered the same
    // way as the terms in this polynomial.  The user has to make sure that
    // the target array has enough space to contain all the coefficients,
    // and note that if this polynomial is modified then the target array is
    // no longer valid.
    virtual void CollectCoefficients(CoefficientType source[]) const = 0;

    // Restore all the coefficients from the source array, ordered the same
    // way as the terms in this polynomial.  The user has to make sure that
    // the target array is ordered properly -- typically it's generated using
    // CollectCoefficients(). 
    virtual void RestoreCoefficients(const CoefficientType source[]) = 0;

#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(push)
#pragma warning(disable:4996)
#endif  // ifdef CISST_COMPILER_IS_MSVC
	// Evaluate the product of powers for a given term index and the variables
	// set in this object.
	ValueType CISST_DEPRECATED EvaluatePowerProduct(const nmrPolynomialTermPowerIndex & where) const
    {
        CMN_ASSERT(this->CanIncludeIndex(where));
        return VariablePowers->EvaluatePowerProduct(where.GetPowers());
    }

    /* The function has been factored out to nmrMultiVariablePowerBasis */
    ValueType CISST_DEPRECATED EvaluatePowerProduct(const PowerType powers[]) const
    {
        return VariablePowers->EvaluatePowerProduct(powers);
    }

    /*!
	 Evaluate the basis function for a term. The term is regarded as a list
	 of powers, without any user defined coefficient.  However, for different
	 families of polynomial the set of basis functions are different.  For 
	 example, Bezier polynomials have the multinomial coefficient in addition
	 to each user defined coefficient.
	 Implemented for each concrete polynomial class.
    */
	ValueType CISST_DEPRECATED EvaluateBasis(const nmrPolynomialTermPowerIndex & where) const
    {
        return EvaluateBasis(where, *VariablePowers);
    }
    
	// Evaluate a single term at the currently specified point, including
	// the basis function and the term's.  This function uses the internally
    // stored coefficient.
	ValueType CISST_DEPRECATED EvaluateTerm(const nmrPolynomialTermPowerIndex & where) const
	{
		CoefficientType coefficient = GetCoefficient(where);
		return EvaluateTerm(where, coefficient);
	}

	// Evaluate a single term at the currently specified point, including
	// the basis function and the term's coefficient.  This function takes
    // the coefficient as a parameter, and does not use internally stored 
    // coefficients.
	ValueType CISST_DEPRECATED EvaluateTerm(const nmrPolynomialTermPowerIndex & where, CoefficientType coefficient) const
	{
        return EvaluateTerm(where, *VariablePowers, coefficient);
	}

    /*!  Evaluate the basis function for each terms in the polynomial, and store the
      results into an array of termBaseValues.
    */
    void CISST_DEPRECATED EvaluateBasisVector(ValueType termBaseValues[]) const
    {
        EvaluateBasisVector(*VariablePowers, termBaseValues);
    }
    
	// Evaluate the polynomial at the currently specified point.
	// Implemented in nmrPolynomialContainer.
	ValueType CISST_DEPRECATED Evaluate()
    {
        return Evaluate(*VariablePowers);
    }

    // Evaluate the polynomial at the currently specified point, using
    // externally defined coefficients.  This lets the user create many
    // polynomials over the same set of basis functions without having to
    // duplicate the terms.
    // The input coefficients must be ordered the same way as the terms in this
    // object.  It is best to order them using CollectCoefficients()
    ValueType CISST_DEPRECATED EvaluateForCoefficients(const CoefficientType coefficients[]) const
    {
        return EvaluateForCoefficients(*VariablePowers, coefficients);
    }
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning(pop)
#endif  // ifdef CISST_COMPILER_IS_MSVC
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE

    /*!
	 Evaluate the basis function for a term. The term is regarded as a list
	 of powers, without any user defined coefficient.  However, for different
	 families of polynomial the set of basis functions are different.  For 
	 example, Bezier polynomials have the multinomial coefficient in addition
	 to each user defined coefficient.
	 Implemented for each concrete polynomial class.
    */
	virtual ValueType EvaluateBasis(const nmrPolynomialTermPowerIndex & where,
        const nmrMultiVariablePowerBasis & variables) const = 0;

	/*! Evaluate a single term at the point specified by the `variables'
      argument, including the basis function and the term's coefficient. 
      This function uses the internally stored coefficient.
    */
	ValueType EvaluateTerm(const nmrPolynomialTermPowerIndex & where,
        const nmrMultiVariablePowerBasis & variables) const
	{
		CoefficientType coefficient = GetCoefficient(where);
		return EvaluateTerm(where, variables, coefficient);
	}

	/*! Evaluate a single term at the point specified by the `variables'
      argument, including the basis function and the term's coefficient. 
      This function takes the coefficient as a parameter.
    */
    ValueType EvaluateTerm(const nmrPolynomialTermPowerIndex & where,
        const nmrMultiVariablePowerBasis & variables, CoefficientType coefficient) const
    {
		if (coefficient == 0)
			return 0;
		return coefficient * EvaluateBasis(where, variables);
    }

    /*! Evaluate the basis vector for extrnally given variables.
      Evaluate the basis function for each terms in the polynomial, and store the
      results into an array of termBaseValues.
    */
    virtual void EvaluateBasisVector(const nmrMultiVariablePowerBasis & variables,
        ValueType termBaseValues[]) const = 0;

    /*!
     Evaluate the polynomial at the currently specified point, using
     externally defined coefficients.  This lets the user create many
     polynomials over the same set of basis functions without having to
     duplicate the terms.
     The input coefficients must be ordered the same way as the terms in this
     object.  It is best to order them using CollectCoefficients()
    */
    virtual ValueType EvaluateForCoefficients(const nmrMultiVariablePowerBasis & variables,
        const CoefficientType coefficients[]) const = 0;

    /*! Evaluate the polynomial for externally given variables */
    virtual ValueType Evaluate(const nmrMultiVariablePowerBasis & variables)
        const = 0;


    /*! Scale the polynomial by a given scalar value.  Basically, it means multiplying
      all the term coefficient by the scale factor.
    */
    virtual void Scale(CoefficientType scaleFactor) = 0;

    /*! Scale a given set of external coefficients which correspond to the terms
      of this polynomial.  Basically, it means multiplying them all by the scale
      factor.
      \param coefficients [i/o] the external coefficients to be scales (in place).
      \param scaleFactor the scaling factor.

      \note this function is not declared virtual, as it seems to behave uniformly
      for all types of polynomial representations we use.  But it may have to be
      declared virtual eventually.  Technically, it could be declared static, but
      I find it too far fetched.

      \note The same rules that apply to evaluating the polynomial for external
      coefficients apply here as well.

      \note the function is declared as deprecated.  Generally, using the
      Multiply operation on cisstVector containers of coefficients should
      be prefered.
    */
    void CISST_DEPRECATED ScaleCoefficients(CoefficientType coefficients[], CoefficientType scaleFactor) const;

    /*! Add a constant value to this polynomial.  That is, if p is a polynomial
      and s is a constant then the following returns true:
      ValueType v0 = p.Evaluate();
      p.AddConstant(s);
      ValueType v1 = p.Evaluate();
      v1 - v0 == s   // up to floating point precision error

      \note the implementation varies between different representations of the 
      polynomial.  Specifically, terms may be added to this polynomial to support
      the addition of the constant.
    */
    virtual void AddConstant(CoefficientType shiftAmount) = 0;

    /*! Modify external coefficients so that they evaluate a new polynomial
      shifted by a constant amount.  That is, if p is a polynomial, c is an
      array of external coefficients, and s is a constant, the following returns
      true:
      ValueType v0 = p.EvaluateForCoefficients(c);
      p.AddConstantToCoefficients(c, s);
      ValueType v1 = p.EvaluateForCoefficients(c);
      v1 - v0 == s;  // up to floating point precision error

      \note The implementation of this function may vary based on the representation
      of the polynomial.  In particular, for a Bernstein polynomial it requires that
      all possible terms be present in the polynomial and in the coefficients 
      correspondingly.  For a standard polynomial, it requires that this
      polynomial actually contains a term of power zero.

      \note The current implementations assert that this polynomial and the
      external coefficients meet the preconditions for the operation.  That is,
      we do not throw an exception but rather issue an assertion error.
    */
    virtual void AddConstantToCoefficients(CoefficientType coefficients[],
        CoefficientType shiftAmount) const = 0;

#if DEFINE_FORMATTED_OUTPUT
	// Format the terms of the polynomial as a string.
	// Return: a pointer to the character immediately following the formatted output
	virtual char * Format(char * buffer) const = 0;
#endif

#if DEFINE_FORMATTED_OUTPUT
	int CalculateFormatLength() const
	{
		int termPowerLength = nmrPolynomialTermPowerIndex(*this).CalculateFormatPowerLength();
		int coefficientLength = 15;
		// <coefficient>< ><{><powers>}\n
		int termLength = coefficientLength + 1 + 2 + termPowerLength + 3;
		int numTerms = GetNumberOfTerms();
		return numTerms * termLength;
	}
#endif

    /*! Serialize NumVariables, MinDegree, MaxDegree */
    virtual void SerializeRaw(std::ostream & output) const;

    /*! Deserialize NumVariables, MinDegree, MaxDegree */
    virtual void DeserializeRaw(std::istream & input);

protected:
	VariableIndexType NumVariables;
	PowerType MinDegree;
	PowerType MaxDegree;

#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
    CISST_DEPRECATED nmrMultiVariablePowerBasis * VariablePowers;
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE

};



#endif // _nmrPolynomialBase_h

