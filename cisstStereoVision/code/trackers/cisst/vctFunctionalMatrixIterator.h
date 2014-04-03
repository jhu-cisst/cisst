/*
  
  Author(s):	Robert Jacques
  Created on:	2005-07-21

--- begin cisst license - do not edit ---

CISST Software License Agreement(c)

Copyright 2005 Johns Hopkins University (JHU) All Rights Reserved.

This software ("CISST Software") is provided by The Johns Hopkins
University on behalf of the copyright holders and
contributors. Permission is hereby granted, without payment, to copy,
modify, display and distribute this software and its documentation, if
any, for research purposes only, provided that (1) the above copyright
notice and the following four paragraphs appear on all copies of this
software, and (2) that source code to any modifications to this
software be made publicly available under terms no more restrictive
than those in this License Agreement. Use of this software constitutes
acceptance of these terms and conditions.

The CISST Software has not been reviewed or approved by the Food and
Drug Administration, and is for non-clinical, IRB-approved Research
Use Only.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY
EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

--- end cisst license ---
*/

/*! 
  \file 
  \brief Declaration of vctFunctionalMatrixIterator and vctFunctionalMatrixConstIterator
 */


#ifndef _vctFunctionalMatrixIterator_h
#define _vctFunctionalMatrixIterator_h

#include <iterator>
#include <cisstVector/vctContainerTraits.h>


/*!
	A random access iterator for use with functional matrixes,
	i.e. a seperate element access must be done for each row/column.

	Equal or greater speed over vctVarStrideMatrixIterator.h.

  \param _baseMatrixType the type of the matrix that the iterator refers to.

  \sa vctFunctionalMatrixConstIterator
*/
template<class _baseMatrixType>
class vctFunctionalMatrixConstIterator:
	public std::iterator<std::random_access_iterator_tag, typename _baseMatrixType::value_type>
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename _baseMatrixType::value_type);
    
    /*! The type of the iterator itself. */
    typedef vctFunctionalMatrixConstIterator<_baseMatrixType> ThisType;
    
    /*! Base type for this iterator, i.e. std::iterator. */
    typedef std::iterator<std::random_access_iterator_tag,typename _baseMatrixType::value_type> BaseType;

    /*! Type (i.e. category) of iterator,
      i.e. std::random_access_iterator_tag. */
    typedef typename BaseType::iterator_category iterator_category;
    
	/*! The matrix the Iterator is based upon. */
    typedef _baseMatrixType BaseMatrixType;

protected:
	/*! The data member points to the object being refered by this
      iterator.  It is not declared const, so we will be able to use
      it in the non-const iterator class.  But all the methods in
      vctVarStrideMatrixConstIterator are declared const to protect
      the data from writing.
    */
    BaseMatrixType * DataPtr;
	
	/*! Current Row in the psuedo Matrix */
	difference_type Row;

	/*! Current Column in the psuedo Matrix */
	difference_type Col;

	/*! The returned functional result*/
//	value_type result;

	/*! The incremental jump in the psuedo Matrix */
	difference_type Stride;

	/*! Number of columns in the matrix */
	difference_type NumColumns;

	/*! Helper function, has been speed tested. Approximately equal to any other formulation*/
	inline ThisType & Plus(difference_type difference) {
		Col += difference;
		while(Col >= NumColumns) { //Oversteped a row, so increase
			Row++;
			Col -= NumColumns;
		}
		while(Col < 0) { //As the difference could be negative
			Row--;
			Col += NumColumns;
		}
        return *this;
	}

public:
    /*! Constructor taking a non-const element pointer.  Note that
      only read operations will be performed! */
    vctFunctionalMatrixConstIterator(BaseMatrixType * dataPtr, 
		index_type row = 0, index_type col = 0, difference_type increment = 1):
        DataPtr(dataPtr),
        Row(row),
        Col(col),
		Stride(increment),
		NumColumns(dataPtr->cols())
        {}
   
    /*! Constructor taking a const element pointer.  We need it in
      order to deal with const containers being refered by this
      iterator.  We have to perform const_cast to convert back to the
      stored non-const member pointer of this iterator.  However, the
      non-const iterator class completely shadows the constructors of
      the const-iterator base class, and so we don't have to worry
      about creating a non-const iterator over a const container.
    */
    vctFunctionalMatrixConstIterator(const BaseMatrixType * dataPtr, 
		index_type row = 0, index_type col = 0, difference_type increment = 1):
        DataPtr(const_cast<BaseMatrixType *>(dataPtr)),
        Row(row),
        Col(col),
		Stride(increment),
		NumColumns(dataPtr->cols())
	    {}
	/*! Copy Cosntructor */
    vctFunctionalMatrixConstIterator(const vctFunctionalMatrixConstIterator & copy):
		DataPtr(copy.DataPtr),
        Row(copy.Row),
		Col(copy.Col),
		Stride(copy.Stride),
		NumColumns(copy.NumColumns)
	    {}

    /*! Pre-increment. */
    ThisType & operator++() {
        return Plus(Stride);
    }

    /*! Post-increment. */
    ThisType operator++(int) {
        ThisType tmp(*this);
        ++(*this);
        return tmp;
    }

    /*! Pre-decrement. */
    ThisType & operator--() {
        return Plus(-Stride);
    }

    /*! Post-decrement. */
    ThisType operator--(int) {
        ThisType tmp(*this);
        --(*this);
        return tmp;
    }

    /*! Increment by offset.
      \param difference offset to increment by
    */
    ThisType & operator +=(difference_type difference) {
        return Plus(difference * Stride);
    }

    /*! Decrement by offset.
     \param difference offset to decrement by
    */
    ThisType & operator -=(difference_type difference) {
        return Plus(-difference* Stride);
    }

    /*! Subtraction between iterators returns the number of increments needed
      for the second operand to reach the first operand, if it is reachable.

      \note this operation assumes reachability and does not test for it.
    */
    difference_type operator-(const ThisType & other) const {
		return ((Col - other.Col) + (Row - other.Row) * NumColumns) / other.Stride;
    }

    /*! Random access (return const reference). */
    const value_type & operator[](difference_type index) const {
        ThisType ptrCalc(*this);
        ptrCalc += index;
        return *ptrCalc;
    }

    /*! Dereference (const). */
    const value_type & operator* () const {
		return DataPtr->Element(Row,Col);
    }

    /*! Order relation between iterators, required by STL.

      \note The STL manual states that "if j is reachable from i then
      i<j".  This does not imply the converse: "if i<j then j is
      reachable from i".  In the case here, the converse does not
      hold.
    */
    bool operator< (const ThisType & other) const {
        return ((*this) - other) < 0;
    }

    /*! Equality of iterators, required by STL. */
    bool operator== (const ThisType & other) const {
		return (DataPtr == other.DataPtr) && (Row == other.Row) && (Col == other.Col) && (Stride == other.Stride);
    }

	/*! Complementary operation to operator <. */
    bool operator> (const ThisType & other) const {
        return other < *this;
    }

    /*! Complementary operation to operator ==. */
    bool operator != (const ThisType & other) const {
        return !( (*this) == other );
    }
};


/*!
	Non-const version
	A random access iterator for use with functional matrixes,
	i.e. a seperate element access must be done for each row/column.

  \param _baseMatrixType the type of the matrix that the iterator refers to.

  \sa vctFunctionalMatrixIterator
*
template<class _baseMatrixType>
class vctFunctionalMatrixIterator : public vctFunctionalMatrixConstIterator<_baseMatrixType>
{
public:
    /! define most types from vctContainerTraits *
 //   VCT_CONTAINER_TRAITS_TYPEDEFS(_baseMatrixType::value_type);
    
    /! The type of the iterator itself. *
    typedef vctFunctionalMatrixIterator<_baseMatrixType> ThisType;
    
    /! Base type for this iterator, i.e. std::iterator. *
    typedef vctFunctionalMatrixConstIterator<_baseMatrixType> BaseType;
    
    /! Type (i.e. category) of iterator,
      i.e. std::random_access_iterator_tag. *
    typedef typename BaseType::iterator_category iterator_category;
	
	/! The matrix the Iterator is based upon. *
    typedef _baseMatrixType BaseMatrixType;

protected:
	/! Overloaded Helper function for non-const*
	inline ThisType & Plus(difference_type difference) {
		Col += difference;
		Row += Col / NumColumns;
		Col  = Col % NumColumns;
        return *this;
	}

public:
    /! Default constructor: create an uninitialized object *
    vctFunctionalMatrixIterator():
        BaseType()
        {}

    /! Constructor taking a non-const element pointer.  Read and
      write operations on the refered object are permitted.  Note that
      there is no constructor that takes a const element pointer.
    *
    vctFunctionalMatrixIterator(BaseMatrixType * dataPtr, index_type row = 0, 
		index_type col = 0, difference_type increment = 1):
        BaseType(dataPtr, row, col, increment)
        {}

    /! Redefine operator++ to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator *
    ThisType & operator++() {
        return Plus(Stride);
    }

    /! Redefine operator++ to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator *
    ThisType operator++(int) {
        ThisType tmp(*this);
        ++(*this);
        return tmp;
    }

    /! Redefine operator-- to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator *
    ThisType & operator--() {
        return Plus(-Stride);
    }

    /! Redefine operator-- to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator *
    ThisType operator--(int) {
        ThisType tmp(*this);
        --(*this);
        return tmp;
    }

    /! Redefine operator+= to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator *
    ThisType & operator+=(difference_type difference) {
        return Plus(difference*Stride);
    }


    /! Redefine operator-= to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator *
    ThisType & operator-=(difference_type difference) {
		return Plus(-difference*Stride);
    }


    /! Add non-const version of operator [] *
    value_type & operator[](difference_type index) const {
        ThisType ptrCalc(*this);
        ptrCalc += index;
        return *ptrCalc;
    }


    /! Add non-const version of unary operator * *
    value_type & operator* () {
        result = DataPtr->Element(Row,Col);
    }
};

/! const_iterator + difference_type required by STL */
template<class _baseMatrixType>
vctFunctionalMatrixConstIterator<_baseMatrixType>
operator+(const vctFunctionalMatrixConstIterator<_baseMatrixType> & iterator,
          typename vctFunctionalMatrixConstIterator<_baseMatrixType>::difference_type difference)
{
    vctFunctionalMatrixConstIterator<_baseMatrixType> result(iterator);
    return result += difference;
}


/*! difference_type + const_iterator required by STL */
template<class _baseMatrixType>
vctFunctionalMatrixConstIterator<_baseMatrixType> 
operator+(typename vctFunctionalMatrixConstIterator<_baseMatrixType>::difference_type difference,
          const vctFunctionalMatrixConstIterator<_baseMatrixType> & iterator)
{
    vctFunctionalMatrixConstIterator<_baseMatrixType> result(iterator);
    return result += difference;
}


/*! const_iterator - difference_type required by STL */
template<class _baseMatrixType>
vctFunctionalMatrixConstIterator<_baseMatrixType> 
operator-(const vctFunctionalMatrixConstIterator<_baseMatrixType> & iterator,
          typename vctFunctionalMatrixConstIterator<_baseMatrixType>::difference_type difference)
{
    vctFunctionalMatrixConstIterator<_baseMatrixType> result(iterator);
    return result -= difference;
}


/*! iterator + difference_type required by STL.  Note that this method
  returns a non-const iterator, which is why it is declared separately
  from the other operator +.
*
template<class _baseMatrixType>
vctFunctionalMatrixIterator<_baseMatrixType>
operator+(const vctFunctionalMatrixIterator<_baseMatrixType> & iterator,
          typename vctFunctionalMatrixIterator<_baseMatrixType>::difference_type difference)
{
    vctFunctionalMatrixIterator<_baseMatrixType> result(iterator);
    return result += difference;
}


/! difference_type + iterator required by STL *
template<class _baseMatrixType>
vctFunctionalMatrixIterator<_baseMatrixType> 
operator+(typename vctFunctionalMatrixIterator<_baseMatrixType>::difference_type difference,
          const vctFunctionalMatrixIterator<_baseMatrixType> & iterator)
{
    vctFunctionalMatrixIterator<_baseMatrixType> result(iterator);
    return result += difference;
}


/! iterator - difference_type required by STL *
template<class _baseMatrixType>
vctFunctionalMatrixIterator<_baseMatrixType> 
operator-(const vctFunctionalMatrixIterator<_baseMatrixType> & iterator,
          typename vctFunctionalMatrixIterator<_baseMatrixType>::difference_type difference)
{
    vctFunctionalMatrixIterator<_baseMatrixType> result(iterator);
    return result -= difference;
}
/! */
#endif // _vctFunctionalMatrixIterator_h
