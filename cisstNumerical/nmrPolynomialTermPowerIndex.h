/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:   2001-10-21

  (C) Copyright 2001-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief Declare class nmrPolynomialTermPowerIndex to represent the power index of a
  single term in a multi-variable polynomial.
*/

#ifndef _nmrPolynomialTermPowerIndex_h
#define _nmrPolynomialTermPowerIndex_h

#include <vector>
#include <stdexcept>
#include <cisstCommon/cmnPortability.h>
#include <cisstNumerical/nmrExport.h>

class nmrPolynomialBase;

/*! This macro controls the conditional compilation of formatted output
 * of a nmrPolynomialTermPowerIndex . The macro and its use should be erased
 * when output format is decided on.
 */
#ifndef DEFINE_FORMATTED_OUTPUT
#define DEFINE_FORMATTED_OUTPUT 0
#endif

/*!
  \brief Represents the power index of a single term in a multi-variable polynomial.

  \ingroup cisstNumerical
  
  Represents the power index of a single term in a multi-variable polynomial.
  For example, if the polynomial has four variables, then the power index of the
  term 
  \f$c * (x0^2)*(x1)*(x2^3)*(x3)\f$
  is (2, 1, 3, 1). The power index of the term
  \f$d * (x1^2)*(x2)\f$
  is (0, 2, 1, 0), and so on.
  
  A power index can be used to reference a term in a polynomial, and
  can also be used as an iterator. We define increment and decrement
  rules for a power index and they are described in the Compare()
  method.
  
  To define a complete term in a polynomial, you need at least a pair
  consisting of a scalar coefficient and a power index. For other
  forms of polynomials, more parameters are used. For example, for
  Bernstein basis polynomials, you need a term coefficient, a
  multinomial coefficient, and the power index.
  
  We define the term power index by the number of variables, the
  minimal degree, and the maximal degree. This way we can iterate over
  power indexes within a certain range of degrees.  This becomes
  handy, for example, with Bernstein basis polynomials, where all the
  terms are of an equal degree.  To define a Bernstein term index for
  n variables of degree d, just create nmrPolynomialTermPowerIndex
  termIndex(n, d, d);
     
  \note for runtime efficiency reasons, the current implementation of
  nmrPolynomialTermPowerIndex caches the degree of the term, that is,
  the sum of all powers. This means that whenever the index is
  modified, the sum needs to be updated. Fortunately, this takes a
  constant number of steps, regardless of the size (number of
  variables) of the term. Just add the difference between the new
  variable degree and the old variable degree to the total.  Degree
  comparison is essential to establish the order relation between
  terms, which later determines where a term is inserted in the
  polynomial. Therefore we do not want to re-evaluate the degree every
  time we compare terms for order.
*/
class CISST_EXPORT nmrPolynomialTermPowerIndex
{
public:
    /*! type of an index to a variable. A variable is x0, x1, x2, ... and the 
      corresponding index is 0, 1, 2, ...
    */
	typedef int VariableIndexType;
    /*! type of a power of a varible -- a non-negative integer
    */
	typedef int PowerType;
    /*! type of a combinatorial result, such as multinomial factor
    */
	typedef unsigned long MultinomialCoefficientType;


	/*!
      Ctor: construct a term, defining the number of variables, the minimal degree
	  and the maximal degree.
	  The default values for the parameter are only to enable constructing a default object.
	  Be especially careful with the maxDegree argument, whose default value is arbitrary.
    */
	nmrPolynomialTermPowerIndex(VariableIndexType numVariables = 1, PowerType minDegree = 0, PowerType maxDegree = 4);

	/*!
      Construct a term valid for a specific Polynomial object
    */
	nmrPolynomialTermPowerIndex(const nmrPolynomialBase & p);

    /*!
	  retrieve the degree of the term (sum of all powers). May be negative (!) when the
	  term is invalid
     */
	int GetDegree() const
	{
		return Degree;
	}

	PowerType GetMaxDegree() const
	{
		return MaxDegree;
	}

	PowerType GetMinDegree() const
	{
		return MinDegree;
	}

    void SetMaxDegree(PowerType newMax) CISST_THROW(std::runtime_error)
    {
        if (newMax < this->Degree) {
            throw std::runtime_error("nmrPolynomialTermPowerIndex: Attempt to set max degree below current degree");
        }
        if (newMax < this->MinDegree) {
            throw std::runtime_error("nmrPolynomialTermPowerIndex: Attempt to set max degree below set minimum");
        }
        MaxDegree = newMax;
    }

    void SetMinDegree(PowerType newMin) CISST_THROW(std::runtime_error)
    {
        if (newMin > this->Degree) {
            throw std::runtime_error("nmrPolynomialTermPowerIndex: Attempt to set min degree above current degree");
        }
        if (newMin > this->MaxDegree) {
            throw std::runtime_error("nmrPolynomialTermPowerIndex: Attempt to set max degree above set maximum");
        }
        MinDegree = newMin;
    }

    /*!
	  return true if the degree of this term is valid, i.e., in the range minDegree..maxDegree
    */
	bool IsValid() const
	{
		return (MinDegree <= Degree) && (Degree <= MaxDegree);
	}
		
	VariableIndexType GetNumVariables() const
	{
		return Powers.size();
	}
	
    /*!
	  retrieve the power of one variable in the current term.
	  the variable index is zero based!
    */
	PowerType GetPower(VariableIndexType variable) const
	{
		return Powers[variable];
	}

	/*! return a pointer to the array of all powers. */
	const PowerType * GetPowers() const
	{
		return &(Powers[0]); 
	}

	/*! return the same pointer as GetPowers(), but pointing signed int. */
	const int * GetPowersAsSigned() const
	{
		return reinterpret_cast<const int *>(GetPowers());
	}

    /*!
	  set the power of one variable in the current term.
	  the variable index is zero based!
    */
	void SetPower(VariableIndexType variable, PowerType power)
	{ 
		Degree += power - Powers[variable];
		Powers[variable] = power; 
	}

	/*! set all the powers of the term using an array of powers. */
	void SetPowers(const PowerType powers[])
	{
		VariableIndexType i;
		Degree = 0;
		for (i = 0; i < GetNumVariables(); i++) {
			Powers[i] = powers[i];
			Degree += powers[i];
		}
	}

	/*! copy the powers from another term into this one.
	  This operation is valid only if the other term has the same number
      of variables as this one
    */
	void CopyPowers(const nmrPolynomialTermPowerIndex & other);

	/*! Move to the first possible term of the given degree. That is, (degree, 0, 0, ..., 0)
    */
	void SetDegree(PowerType degree);

	/*! Move to the first possible term of degree MinDegree */
	void GoBegin();
	/*! Move to the last possible term of degree MaxDegree */
	void GoEnd();

    /*!
	  Move to the next possible term. The rule is:
	  1) Remove all the power from the last variable (index n-1), and store it.
	  2) Find the rightmost positive power in the remaining term.
	  3) Move one power from the right of the remaining term to the variable just after.
	  4) Return the powers from the last variable to the new rightmost positive power.
	 
	  For example, if we had a term (3, 3, 0, 2)
	  1) have (3, 3, 0, 0), store 2
	  2) the last positive degree is of variable #1 (zero based index)
	  3) change to (3, 2, 1, 0)
	  4) return the stored 2: (3, 2, 3, 0)
	 
	  The process is identical if the last power is zero. If in step (2) we are left with
	  no positive powers, we skip the removal of power from the right of the term, and
	  step (3) becomes making the term (1, 0, ..., 0).
    */
	void Increment();

    /*!
	  Move to the previous possible term. The rules are the reverse of the Increment()
	  process. That is:
	  1) Remove all the power from the rightmost positive power, and store it in p.
	  2) Add 1 to the power of the variable just left of where power was removed.
	  3) Add (p-1) to the last variable.
	 
	  For more details, see the Increment() method.
    */
	void Decrement();  

	/*! tells if this is the first possible term for the given degree limits. */
	bool IsBegin() const
	{
		return (GetDegree() == GetMinDegree()) && (GetPower(0) == GetMinDegree());
	}
	
	/*! tells if this is the last possible term for the given degree limits. */
	bool IsEnd() const
	{
		return (GetDegree() == GetMaxDegree()) && (GetPower(GetNumVariables() - 1) == GetMaxDegree());
	}

    /*!
	  compare terms according to the order set by the successor function Increment()
	  return zero if the terms are equal, negative value if this term is before
	  the other, and positive value if this term is after the other.
	 
	  The order relation is quasi-lexicographic. Number of variables is compared first,
	  the greater number of variables has greater term index. Degree is compared next.
	  The greater degree has greater term index. If the degree and number of variables
	  are equal, we compare the individual powers from left to right. The first occurence
	  of unequal powers makes the term index with lower (first different) power greater.
	  That is because the Increment() function "pushes" the power to the right. Then,
	  if term index a has lower power on a left variable than term index b, it means
	  that power was pushed to the left from a to obtain b. That is, b is greater than a.
    */
	int Compare(const nmrPolynomialTermPowerIndex & other) const;

    /*!
	  decide if this index is before the other according to the successor function
	  Increment().
    */
	bool operator< (const nmrPolynomialTermPowerIndex& other) const
	{
		return (this->Compare(other) < 0);
	}

    /*!
	  decide if this index is after the other according to the successor function
	  Increment().
    */
	bool operator > (const nmrPolynomialTermPowerIndex& other) const
	{
		return (other < *this);
	}

    bool operator<= (const nmrPolynomialTermPowerIndex & other) const
    {
        return (this->Compare(other) <= 0);
    }

    bool operator>= (const nmrPolynomialTermPowerIndex & other) const
    {
        return (other <= *this);
    }

    /*! decide if this index is equal to the other. 
      Currently, the operation is not implemented, as it was not needed and its
      semantics is questionable.  Does == mean identity, or does it mean equivalence
      of index?
    */
	bool operator== (const nmrPolynomialTermPowerIndex& other) const;
    // This is an incorrect definition of operator==
	//{ return (this->Compare(other) == 0); }


	// For a set of powers 'p1',...,'pn' whose sum is 'd', return
	// d! / (p1! p2! ... pn!).
	// In this function, d is given as an argument, so there is assumed to
	// be an implicit (non-independent) variable in the polynomial.
	static MultinomialCoefficientType 
		EvaluateMultinomialCoefficient(VariableIndexType numIndices, 
		PowerType chooseFrom, const PowerType indices[]);

	// For a set of powers 'p1',...,'pn' whose sum is 'd', return
	// d! / (p1! p2! ... pn!).
	// In this function, d is the sum of all the powers stored in 'indices'
	static MultinomialCoefficientType 
		EvaluateMultinomialCoefficient(VariableIndexType numIndices, 
		const PowerType indices[]);


	// For a set of powers 'p1',...,'pn' whose sum is 'd', return
	// d! / (p1! p2! ... pn!).
	// In this function, d is the sum of all the powers stored in this term.
	MultinomialCoefficientType GetMultinomialCoefficient() const
	{
		return EvaluateMultinomialCoefficient(GetNumVariables(), GetPowers());
	}

	// For a set of powers 'p1',...,'pn' whose sum is 'd', return
	// d! / (p1! p2! ... pn!).
	// In this function, d is given as an argument, so there is assumed to
	// be an implicit (non-independent) variable in the polynomial.
	MultinomialCoefficientType GetMultinomialCoefficient(PowerType d) const
	{
		return EvaluateMultinomialCoefficient(GetNumVariables(), d, GetPowers());
	}

	// Return the combinatorial number of possible power sets for the term.
	//
	// For a fixed degree d, and n variables, the number of combinations
	// is the number of multisets of size d over n elements.
	// First we choose k = number of variables in the term. if d<n we can choose
	// k=1..d, otherwise we can choose k=1..n . But for simplicity and arithmetic
	// reasons we will assume k=1..d. We will see that it does not affect the 
	// final result.  Next, we have to choose the variables in the term. We have
	// \choose{n}{k} combinations.  Finally we choose the power for each variable.
	// We have \choose{d-1}{k-1} combinations. The entire expression is
	//
	//   \sum_{k=1}^{d} \choose{n}{k} \choose{d-1}{k-1}
	//
	// Note that for any k>n, \choose{n}{k} = 0, so we don't need to worry about
	// d>n.
	// The entire sum can be simplified into \choose{n+d-1}{d}. However, for a term
	// index of n variables, we have to sum over all possible degrees:
	//
	//   \sum_{d=MinDegree}^{MaxDegree} \choose{n+d-1}{d}.
	//
	// \choose{n+d}{d+1} = (n+d)! / ( (d+1)! (n-1)! ) = ((n+d)(n+d-1)) / ((d+1) d! (n-1)!) 
	//                   = ((n+d)/(d+1)) * ( (n+d-1)! d! (n-1)! ) = ((n+d)/(d+1)) * \choose{n+d-1}{d}
	//
	// Therefore, we only need to make a long computation for the first element of the
	// sum, and the rest are received incrementally.
	MultinomialCoefficientType CountPowerCombinations() const;

    /*! Return the number of possible term indices whose corresponding powers are
      less than or equal to the powers in this term index.  The result is the product
      of all the powers in this term index, adding one to each. */
    MultinomialCoefficientType CountLowerOrderTerms() const;

#if DEFINE_FORMATTED_OUTPUT
	// Print the powers of the term index in sprintf format into a character
	// buffer. No assertion is made to ensure that the buffer has enough capacity
	// to store the formatted message. All powers are printed, included zeroes.
	// Parameters:
	//   buffer [i/o]   -- destination buffer for the output
	//   widthFormat [i] -- a format specification to enable equal column width.
	//
	// Return: a pointer to the character immediately following the formatted output
	char * FormatPowers(char * buffer, const char * widthFormat = "%d") const;
#endif


#if DEFINE_FORMATTED_OUTPUT
	// Print the entire term index in formatted output. The format is:
	// [numVariables minDegree maxDegree] powers...
	// See FormatPowers for parameter and return value specification.
	char * FormatTermIndex(char * buffer, const char * widthFormat = "%d") const;
#endif

#if DEFINE_FORMATTED_OUTPUT
	// Return the estimated number of characters required to store the formatted
	// powers (from FormatPowers())
	int CalculateFormatPowerLength(const char * widthFormat = "%d") const
	{
		char buff[10];
		sprintf( buff, widthFormat, GetMaxDegree() );
		int varLength = strlen(buff) + 1;
		return varLength * GetNumVariables();
	}
#endif

    /*! Serialize the power-index object into a stream.
      This function is not completely standardized, as the output is in 
      machine-dependent format.
      Output order is similar to member order.
    */
    void SerializeRaw(std::ostream & output) const;

    /*! Deserialize a power index from a stream.  The function should
      work as the opposite of SerializeRaw().
    */
    void DeserializeRaw(std::istream & input);

    /*! Serialize only the power index, without degree limits or number of
      variables. This function is declared public so that we can save space
      by calling this one instead of SerializeRaw when serializing a whole 
      polynomial.
    */
    void SerializeIndexRaw(std::ostream & output) const;

    /*! Deserialize only the power index.  \see SerializeIndexRaw().
    */
    void DeserializeIndexRaw(std::istream & input);

protected:
	std::vector<PowerType> Powers;		// the powers of all the variables in the term
	PowerType MinDegree;	// the minimal possible degree of the term
	PowerType MaxDegree;	// the maximal possible degree of the term

	// the degree of the term. Note that it may be negative, in which case the term is
	// invalid.  This member should always be equal to the sum of all powers.  It
    // is therefore redundant information is only serves as a cache for sorting
    // purposes.  It is not serialized.
	int Degree;

};

#endif // _nmrPolynomialTermPowerIndex_h
