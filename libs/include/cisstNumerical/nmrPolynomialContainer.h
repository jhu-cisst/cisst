/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrPolynomialContainer.h,v 1.8 2007/04/26 19:33:57 anton Exp $

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


#ifndef _nmrPolynomialContainer_h
#define _nmrPolynomialContainer_h

#include <cisstNumerical/nmrPolynomialBase.h>

#define MAP_CONTAINER 1
#define LIST_CONTAINER 2
#define CONTAINER_TYPE LIST_CONTAINER

#if (CONTAINER_TYPE == MAP_CONTAINER)
#include <map>
#elif (CONTAINER_TYPE == LIST_CONTAINER)
#include <list>
#include <algorithm>
#endif

#include <iostream>

#include <cisstNumerical/nmrExport.h>

// class nmrPolynomialContainer is a templated class to contain the terms of a
// multi-variable polynomial. The concrete polynomial may still define properties
// such as basis functions (standard, Bernstein, etc.). The terms are indexed
// by a nmrPolynomialTermPowerIndex object, and ordered according to the order relation
// of class nmrPolynomialTermPowerIndex. The current implementation uses STL map to
// contain all the terms. The key is the power index, and the value varies by the concrete
// polynomial class. That is, nmrPolynomialContainer is templated over the additional
// information associated with a term. For a standard polynomial, it is only the coefficient.
// For a Bernstein polynomial, it is the coefficient and a multinomial factor.
//
// nmrPolynomialContainer defines iterators for quick and sequential access/manipulation
// of data in the polynomial. The user can index a term by its powers, whether it's stored
// in the container or not, in logarithmic time. Referencing a term which is already in
// the polynomial by an iterator is quicker. Therefore, some operations which were defined
// in the base class nmrPolynomialBase to use a nmrPolynomialTermPowerIndex argument are
// redefined here with an iterator argument for faster processing.
template<class _TermInfo>
class CISST_EXPORT nmrPolynomialContainer : public nmrPolynomialBase
{
public:
	typedef nmrPolynomialBase BaseType;

	typedef _TermInfo TermInfoType;

#if (CONTAINER_TYPE == MAP_CONTAINER)
	typedef std::map<nmrPolynomialTermPowerIndex, TermInfoType> TermContainerType;
#elif (CONTAINER_TYPE == LIST_CONTAINER)
    typedef std::pair<nmrPolynomialTermPowerIndex, TermInfoType> ContainerElementType;
    typedef std::list<ContainerElementType> TermContainerType;
#endif

	typedef typename TermContainerType::value_type TermType;
	typedef typename TermContainerType::iterator TermIteratorType;
	typedef typename TermContainerType::const_iterator TermConstIteratorType;


#if (CONTAINER_TYPE == LIST_CONTAINER)
    /*! Here we define an auxiliary functor class to compare objects of type
      nmrPolynomialTermPowerIndex . The functor class can be used in STL algorithms
      such as find_if.  The member operator() returns true if the input target term
      index is equal to the term index of a pair in the container.
    */
    class EqualityTester
    {
    public:
        inline EqualityTester(const nmrPolynomialTermPowerIndex & target)
            : Target(target)
        {}

        inline bool operator() (const ContainerElementType & element)
        {
            return (Target.Compare(element.first) == 0);
        }
    private:
        const nmrPolynomialTermPowerIndex & Target;
    };

    /*! Here we define an auxiliary functor class to compare objects of type
      nmrPolynomialTermPowerIndex . The functor class can be used in STL algorithms
      such as find_if.  The member operator() returns true if the input target term
      index is less than or equal to the term index of a pair in the container.
    */
    class LessOrEqualityTester
    {
    public:
        inline LessOrEqualityTester(const nmrPolynomialTermPowerIndex & target)
            : Target(target)
        {}

        inline bool operator() (const ContainerElementType & element)
        {
            return (Target.Compare(element.first) <= 0);
        }
    private:
        const nmrPolynomialTermPowerIndex & Target;
    };

#endif

	// Ctor determines the number of variables and the degree of the polynomial
	nmrPolynomialContainer(VariableIndexType numVariables, PowerType minDegree, PowerType maxDegree)
		: BaseType(numVariables, minDegree, maxDegree)
	{}

	virtual ~nmrPolynomialContainer()
	{
		Clear();
	}

	virtual TermCounterType GetNumberOfTerms() const
	{
		return Terms.size();
	}

	// Tell if there are any terms in the polynomial
	virtual bool IsEmpty() const
	{
        return Terms.empty();
    }

    //: Implemented from nmrPolynomialBase
    virtual void SetMinDegree(PowerType newMin) throw(std::runtime_error)
    {
        if (newMin > this->MaxDegree) {
            throw std::runtime_error("nmrPolynomialContainer: Attempt to set the min degree higher than max");
        }
        if (this->IsEmpty()) {
            this->MinDegree = newMin;
            return;
        }
        const TermType & firstTerm = *(Terms.begin());
        const nmrPolynomialTermPowerIndex & termPowers = firstTerm.first;
        const PowerType firstDegree = termPowers.GetDegree();
        if (newMin > firstDegree) {
            throw std::runtime_error("nmrPolynomialContainer: Attempt to set the min degree above existing term");
        }
        MinDegree = newMin;
    }

    //: Implemented from nmrPolynomialBase
    virtual void SetMaxDegree(PowerType newMax) throw(std::runtime_error)
    {
        if (newMax < this->MinDegree) {
            throw std::runtime_error("nmrPolynomialContainer: Attempt to set the max degree lower than min");
        }
        if (this->IsEmpty()) {
            this->MaxDegree = newMax;
            return;
        }
        const TermType & lastTerm = *(Terms.rbegin());
        const nmrPolynomialTermPowerIndex & termPowers = lastTerm.first;
        const PowerType lastDegree = termPowers.GetDegree();
        if (newMax < lastDegree) {
            throw std::runtime_error("nmrPolynomialContainer: Attempt to set the max degree below existing term");
        }
        MaxDegree = newMax;
    }

	// Generate all possible terms for this polynomial, and store them for later
	// use.
	virtual void FillAllTerms()
	{
		nmrPolynomialTermPowerIndex termIndex( *this );
		termIndex.GoBegin();
		
		while (termIndex.IsValid()) {
			TermIteratorType termIterator = FindTerm(termIndex);
			if (termIterator == EndTermIterator())
				SetCoefficient(termIndex, 0);
			termIndex.Increment();
		}
	}
	
	// Find a term by its index. Return true is the term is found.
	// return 'false' otherwise.
	virtual bool IncludesIndex(const nmrPolynomialTermPowerIndex& target) const
	{ 
#if (CONTAINER_TYPE == MAP_CONTAINER)
        return Terms.find(target) != Terms.end(); 
#elif (CONTAINER_TYPE == LIST_CONTAINER)
        return std::find_if(Terms.begin(), Terms.end(), EqualityTester(target)) != Terms.end();
#endif
    }

    // Find a term by its coefficients.  If the term is found, return the sequential
    // position (zero-based) of the term in the sequence of terms for this polynomial.
    // If the term is not found, return a number greater than or equal to the current number 
    // of terms in the sequence.
    virtual TermCounterType GetIndexPosition(const nmrPolynomialTermPowerIndex & term) const
    {
        if (this->IsEmpty()) {
            return 0;
        }

        TermConstIteratorType termIterator = FirstTermIterator();
        const TermConstIteratorType endTermIterator = EndTermIterator();
        TermCounterType result = 0;
        for (; termIterator != endTermIterator; ++termIterator) {
            if ( (termIterator->first).Compare(term) == 0 ) {
                break;
            }
            ++result;
        }
        return result;
    }

    /*!  Return the iterator for a term identified by its sequential number (as in GetIndexPosition).
    */
    TermIteratorType GetTermIteratorForPosition(TermCounterType position)
    {
        TermIteratorType result = FirstTermIterator();
        TermCounterType index = 0;
        const TermIteratorType endTermIterator = EndTermIterator();
        for (; (index < position) && (result != endTermIterator); ++index, ++result)
        {}

        return result;
    }

	// Returns an iterator for the first actual term in the polynomial.
	TermIteratorType FirstTermIterator()
	{ return Terms.begin(); }
	TermConstIteratorType FirstTermIterator() const
	{ return Terms.begin(); }

	// Returns an iterator to the end of the polynomial container. See the end()
	// method for STL containers.
	TermIteratorType EndTermIterator()
	{ return Terms.end(); }
	TermConstIteratorType EndTermIterator() const
	{ return Terms.end(); }

	// Returns an iterator that refers to the term of the given power index. If this
	// polynomial does not contain a term with the given index, returns EndTermIterator().
	TermIteratorType FindTerm(const nmrPolynomialTermPowerIndex& target)
	{ 
#if (CONTAINER_TYPE == MAP_CONTAINER)
        return Terms.find(target); 
#elif (CONTAINER_TYPE == LIST_CONTAINER)
        return std::find_if(Terms.begin(), Terms.end(), EqualityTester(target));
#endif
    }
	TermConstIteratorType FindTerm(const nmrPolynomialTermPowerIndex& target) const
	{ 
#if (CONTAINER_TYPE == MAP_CONTAINER)
        return Terms.find(target); 
#elif (CONTAINER_TYPE == LIST_CONTAINER)
        return std::find_if(Terms.begin(), Terms.end(), EqualityTester(target));
#endif
    }

	// Remove a term from the polynomial. The term is given by power index.
	// Equivalent to setting the coefficient of the term to zero, except that this
	// function also reclaims the space allocated for the term.
	virtual void RemoveTerm(const nmrPolynomialTermPowerIndex & where)
	{
		TermIteratorType foundTerm = FindTerm(where);
		if (foundTerm != EndTermIterator()) {
			RemoveTerm(foundTerm);
		}
	}

	// Remove a term from the polynomial. The term is given by iterator.
	// The function also reclaims the space allocated for the term.
	virtual void RemoveTerm(TermIteratorType & where)
	{
		Terms.erase(where);
	}

	virtual void Clear()
	{
		Terms.clear();
	}

	// Re-implemented from nmrPolynomialBase to provide v-table access.
	virtual InsertStatus SetCoefficient(const nmrPolynomialTermPowerIndex & where, CoefficientType coefficient) = 0;

	// Set a coefficient for a term given by iterator.
	// Implemented for each concrete polynomial class.
	virtual InsertStatus SetCoefficient(TermIteratorType & where, CoefficientType coefficient) = 0;
	
	// Retrieve the value of the user defined coefficient for a term given by iterator.
	virtual CoefficientType GetCoefficient(const TermConstIteratorType & where) const = 0;
	virtual CoefficientType GetCoefficient(const TermIteratorType & where) const = 0;

    // Collect all the coefficients into an array. Re-implemented from nmrPolynomialBase
    virtual void CollectCoefficients(CoefficientType target[]) const
    {
        TermConstIteratorType termIterator = FirstTermIterator();
        int coefficientIndex = 0;
        for (; termIterator != EndTermIterator(); ++termIterator, ++coefficientIndex) {
            target[coefficientIndex] = GetCoefficient(termIterator);
        }
    }

    // Restore all the coefficients from an array. 
    // Re-implemented from nmrPolynomialBase
    virtual void RestoreCoefficients(const CoefficientType source[])
    {
        TermIteratorType termIterator = FirstTermIterator();
        int coefficientIndex = 0;
        for (; termIterator != EndTermIterator(); ++termIterator, ++coefficientIndex) {
            SetCoefficient(termIterator, source[coefficientIndex]);
        }
    }

	const nmrPolynomialTermPowerIndex & GetTermPowerIndex(const TermConstIteratorType & where) const
	{ return where->first; }
	const nmrPolynomialTermPowerIndex & GetTermPowerIndex(const TermIteratorType & where) const
	{ return where->first; }

#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
	// Evaluate the basis function for a term given by iterator. The use of iterator can
	// optimize evaluation by caching elements such as multinomial factor with the term
	// coefficient, eliminating the need to re-evaluate it for every evaluation of
	// the basis function.
	ValueType EvaluateBasis(const TermConstIteratorType & where) const
    {
        return EvaluateBasis(where, *VariablePowers);
    }
	ValueType EvaluateBasis(const TermIteratorType & where) const
    {
        return EvaluateBasis(where, *VariablePowers);
    }
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE
	virtual ValueType EvaluateBasis(const TermConstIteratorType & where,
        const nmrMultiVariablePowerBasis & variables) const = 0;
	virtual ValueType EvaluateBasis(const TermIteratorType & where,
        const nmrMultiVariablePowerBasis & variables) const = 0;

	// Evaluate a single term referenced by iterator.
	ValueType EvaluateTerm(const TermConstIteratorType & where) const
	{ 
        return GetCoefficient(where) * EvaluateBasis(where); 
    }
	ValueType EvaluateTerm(const TermIteratorType & where) const
	{
        return GetCoefficient(where) * EvaluateBasis(where);
    }
	ValueType EvaluateTerm(const TermConstIteratorType & where,
        const nmrMultiVariablePowerBasis & variables) const
	{ 
        return GetCoefficient(where) * EvaluateBasis(where, variables); 
    }
	ValueType EvaluateTerm(const TermIteratorType & where,
        const nmrMultiVariablePowerBasis & variables) const
	{
        return GetCoefficient(where) * EvaluateBasis(where, variables);
    }
	
	// Evaluate the polynomial at the currently point (specified by the values of the variables).
	virtual ValueType Evaluate(const nmrMultiVariablePowerBasis & variables) const
	{
        ValueType polyVal = 0;
        
        TermConstIteratorType termIt = FirstTermIterator();
        TermConstIteratorType endIt = EndTermIterator();
        while (termIt != endIt) {
            polyVal += EvaluateTerm(termIt, variables);
            ++termIt;
        }
        
        return polyVal;
	}

    /*! Evaluate the basis functions of the terms as a vector of values. */
    virtual void EvaluateBasisVector(const nmrMultiVariablePowerBasis & variables,
        ValueType termBaseValues[]) const
    {
        TermConstIteratorType termIt = FirstTermIterator();
        const TermConstIteratorType endIt = EndTermIterator();
        ValueType * valuePtr = termBaseValues;

        for (; termIt != endIt; ++termIt, ++valuePtr) {
            *valuePtr = EvaluateBasis(termIt, variables);
        }
    }

    // Evaluate the polynomial using externally defined coefficients.
    virtual ValueType EvaluateForCoefficients(const nmrMultiVariablePowerBasis & variables,
        const CoefficientType coefficients[]) const
    {
        ValueType polyVal = 0;

        CoefficientType const * coefficientIndex = coefficients;
        
        TermConstIteratorType termIt = FirstTermIterator();
        TermConstIteratorType endIt = EndTermIterator();
        for (; termIt != endIt; ++termIt, ++coefficientIndex) {
            polyVal += BaseType::EvaluateTerm(termIt->first, 
                variables, *coefficientIndex);
        }
        
        return polyVal;
    }

    /*! Overloaded from nmrPolynomialBase */
    virtual void Scale(CoefficientType scaleFactor)
    {
        TermIteratorType termIt = FirstTermIterator();
        TermIteratorType endIt = EndTermIterator();
        for (; termIt != endIt; ++termIt) {
            CoefficientType currentCoefficient = GetCoefficient(termIt);
            SetCoefficient(termIt, scaleFactor * currentCoefficient);
        }
    }

    /*! Overloaded from nmrPolynomialBase and declared abstract as the implementation
      varies in subtypes.  See subtype implementations */
    virtual void AddConstant(CoefficientType shiftAmount) = 0;

    /*! Overloaded from nmrPolynomialBase.  See subtype implementations */
    virtual void AddConstantToCoefficients(CoefficientType coefficients[],
        CoefficientType shiftAmount) const = 0;


#if DEFINE_FORMATTED_OUTPUT
	virtual char * Format(char * buffer) const
	{
		TermConstIteratorType termIterator = FirstTermIterator();
		TermConstIteratorType endIterator = EndTermIterator();
		while (termIterator != endIterator) {
			sprintf(buffer, "%.4f ", GetCoefficient(termIterator));
			buffer += strlen(buffer);
			sprintf(buffer, "{");
			buffer+=strlen(buffer);
			buffer = GetTermPowerIndex(termIterator).FormatPowers(buffer);
			sprintf(buffer, "}\n");
			buffer += strlen(buffer);

			termIterator++;
		}

		return buffer;
	}
#endif

    /*! Serialize the TermInfo member associated with the power index of a term.
      The implementation of this function is to be specific per TermInfo concrete
      type, and so it should be overridden in the derived classes.  This function
      is called from SerializeRaw().

      \param output the output serializetion stream
      \param termIterator an iterator referring to the term whose contents are being
      serialized.
    */
    virtual void SerializeTermInfo(std::ostream & output, const TermConstIteratorType & termIterator) const = 0;

    /*! Deserialize and do other necessary initializations for the TermInfoMember
      associated with the power index of a term.  This function should be complementart
      to SerializeTermInfo.

      \param input the input deserialization stream
      \param termIterator an iterator referreing to the term whose contents are being
      deserialized.
    */
    virtual void DeserializeTermInfo(std::istream & input, TermIteratorType & termIterator) = 0;

    /*! nmrPolynomialBase::SerializeRaw() plus serialize for each term
     the power index in nmrPolynomialTermPowerIndex::SerializeIndexRaw()
     format, and the specific term information by calling the abstract 
     SerializeTermInfo for that term.
    */
    virtual void SerializeRaw(std::ostream & output) const
    {
        BaseType::SerializeRaw(output);
        TermCounterType numTerms = GetNumberOfTerms();
        output.write( (const char *)&numTerms, sizeof(numTerms));
        TermConstIteratorType termIterator = FirstTermIterator();
        for (; termIterator != EndTermIterator(); ++termIterator) {
            GetTermPowerIndex(termIterator).SerializeIndexRaw(output);
            SerializeTermInfo(output, termIterator);
        }
    }

    /*! nmrPolynomialBase::DeserializeRaw() plus deserialize all the terms.
      For each term we use nmrPolynomialTermPowerIndex::DeserializeIndexRaw()
      and then call the abstract DeserializeTermInfo() to complete the rest
      of the term information.
    */
    virtual void DeserializeRaw(std::istream & input)
    {
        // Restore NumVariables, MinDegree, MaxDegree
        BaseType::DeserializeRaw(input);
        TermCounterType numTermsLeft;
        input.read( (char *)&numTermsLeft, sizeof(numTermsLeft));
        for (; numTermsLeft > 0; --numTermsLeft) {
            nmrPolynomialTermPowerIndex termIndex(GetNumVariables(), GetMinDegree(), GetMaxDegree());
            termIndex.DeserializeIndexRaw(input);
            TermInfoType termInfo;
#if (CONTAINER_TYPE == MAP_CONTAINER)
            Container[termIndex] = termInfo;
#elif (CONTAINER_TYPE == LIST_CONTAINER)
            Terms.push_back( ContainerElementType(termIndex, termInfo) );
#endif
            TermIteratorType termIterator = FindTerm(termIndex);
            DeserializeTermInfo(input, termIterator);
        }
    }

protected:
	 TermContainerType Terms;

     const TermInfoType & GetTermInfo(const TermConstIteratorType & termIterator) const
     {
         // good for both map and list!
         return termIterator->second;
     }

     TermInfoType & GetTermInfo(const TermConstIteratorType & termIterator)
     {
         // good for both map and list!
         return termIterator->second;
     }

};


#endif // _nmrPolynomialContainer_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrPolynomialContainer.h,v $
//  Revision 1.8  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.7  2006/11/20 20:33:19  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
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
//  Revision 1.2  2004/10/27 02:41:59  anton
//  cisstNumerical: Code to compile as a DLL.  Most of the code was in place but
//  never tested.  Minor additions including to disable some warnings.
//
//  Revision 1.1  2004/10/21 19:52:51  ofri
//  Adding polynomial-related header files to the repository -- imported from Ofri.
//  See ticket #78.
//
//  Revision 1.11  2004/04/19 14:45:31  ofri
//  1) Added implementation for virtual method GetIndexPosition
//  2) Added complementary method GetTermIteratorForPosition
//
//  Revision 1.10  2003/12/24 17:57:58  ofri
//  Updates to cisNumerical:
//  1) Using #include <...> instead of #include "..." for cisst header files
//  2) Getting rid of cisMatrixVector in most places (if not all), replacing it with
//  vnl
//
//  Revision 1.9  2003/09/22 18:28:46  ofri
//  1. Replace macro name cmnExport with CISST_EXPORT (following Anton's
//  update).
//  2. Added abstract methods Scale(), ScaleCoefficients(), AddConstant(),
//  AddConstantToCoefficients() to the base class nmrPolynomialBase,
//  and actual implementations in nmrStandardPolynomial and
//  nmrBernsteinPolynomial.
//
//  Submitted after successful build but no testing yet.
//
//  Revision 1.8  2003/08/26 20:20:51  ofri
//  Factored out the evaluation of polynomial to use variables from
//  nmrMultiVariablePowerBasis.  This affected methods in nmrPolynomialBase,
//  nmrPolynomialContainer, nmrStandardPolynomial, nmrBernsteinPolynomial .
//  The revision was tested successfully.
//
//  Revision 1.7  2003/08/20 18:23:58  ofri
//  Added nmrPolynomialBase::EvaluateBasisVector() [abstract] with
//  implementation in nmrPolynomialContainer.
//
//  Revision 1.6  2003/07/21 17:44:34  ofri
//  1. Added documentation for the Serialize and Deserialize protocols/methods
//  in the various polynomial classes.
//  2. Extracted method nmrPolynomialPowerTermIndex::SerializeIndexRaw()
//  from SerializeRaw()
//
//  Revision 1.5  2003/07/16 22:07:02  ofri
//  Adding serialization functionality to the polynomial classes and the
//  nmrPolynomialTermPowerIndex class
//
//  Revision 1.4  2003/07/10 15:50:16  ofri
//  Adding change history log to the files in cisstNumerical. I guess in a few files
//  there was replacement of spaces and tabs, which was recorded in CVS as
//  a big change, unfortunately. But for most, it's just adding the tags.
//
//
// ****************************************************************************


