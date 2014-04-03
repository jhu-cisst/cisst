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

/* TODO List
01 [X]	Add iterators. See vctVarStrideMatrixIterator.h
02 [X]	Add specilization that allows chaining.
03 [ ]	Typedefs for matrix reference returns.
Later...
01 [ ]	Fix Element default versions, once the rest of the library is updated.
02 [ ]	Fix Dynamic Vector Typedefs + Classes for vector reference returns
03 [ ]	Better Transpose code
04 [ ]	Actual return type classes (Post Matrix Bases)
05 [ ]	Reference access not complete
*/ 

#ifndef _vctDynamicFunctionalMatrixOwner_h
#define _vctDynamicFunctionalMatrixOwner_h

/*!
  \file
  \brief Declaration of vctDynamicFunctionalMatrixOwner
*/

#include <cisstVector/vctContainerTraits.h>
#include "vctFunctionalMatrixIterator.h"

/*! 
  This templated class decorates another concrete data structure.
  It is directly based off of vctDynamicMatrixRefOwner.
  It provides the ability to filter data in various manners by
  Class extension. Arbitrary shapes can be achieved with 1xN matrixes.
  
  \param _baseMatrixType the type of matrix to decorate
*/
template< class _baseMatrixType, class _elementType = typename _baseMatrixType::value_type >
class vctDynamicFunctionalMatrixOwner
{
public:
    /*! define most types from vctContainerTraits */
	VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

	/*! The underlying type of matrix this function decorates, should be a reference type*/
	typedef _baseMatrixType MatrixBaseType;

    /*! Type of the matrix owner itself. */
	typedef vctDynamicFunctionalMatrixOwner<_baseMatrixType, _elementType> ThisType;

    /*! Constant foward iterator type */    
    typedef vctFunctionalMatrixConstIterator<ThisType> const_iterator;
    /*! Constant reverse iterator type */    
    typedef vctFunctionalMatrixConstIterator<ThisType> const_reverse_iterator;
    /*! foward iterator type */    
    typedef vctFunctionalMatrixConstIterator<ThisType> iterator;
    /*! reverse iterator type */    
    typedef vctFunctionalMatrixConstIterator<ThisType> reverse_iterator;


    /*! The type indicating a vector of this matrix accessed by (const)
      reference */
    typedef vctDynamicConstVectorRef<_elementType> ConstVectorRefType;
    
    /*! The type indicating a vector of this matrix accessed by
      (non-const) reference */
    typedef vctDynamicVectorRef<_elementType> VectorRefType;
    
	/*! The type of matrix indicating matrix access by (const)
	  reference */
    typedef ThisType ConstMatrixRefType;

	/*! The type of matrix indicating matrix access by (const)
	  reference */
    typedef ThisType MatrixRefType;

    /*! The type of a matrix returned by value from operations on this object */
    typedef ThisType MatrixValueType;

	/*! Default Initilization. Not valid to use */
	vctDynamicFunctionalMatrixOwner() :
        Rows(0),
        Cols(0),
        Data(0)
    {}
	
	/*! Initialize to a given size and underlying Matrix */
    vctDynamicFunctionalMatrixOwner(size_type rows, size_type cols, MatrixBaseType * data):
        Rows(rows),
        Cols(cols),
        Data(data)
    {}

	/*! Simple set routine for all data values */
    void SetRef(size_type rows, size_type cols, MatrixBaseType * data) {
        Rows = rows;
        Cols = cols;
        Data = data;
    }
	/*! Simple set routine for all data values */
    void SetRef(size_type rows, size_type cols, MatrixBaseType & data) {
        SetRef(rows,cols,&data);
    }
	
	/*! Set only the underlying data, useful for shifting / loop structures */
    void SetRef(MatrixBaseType * data) {
        Data = data;
    }
	/*! Set only the underlying data, useful for shifting / loop structures */
    void SetRef(MatrixBaseType & data) {
        SetRef(&data);
    }

	/*! The total size of the matrix */
    size_type size(void) const {
        return Rows * Cols;
    }
	
	/*! The number of rows in the matrix */
    size_type rows(void) const {
        return Rows;
    }
	
	/*! The number of columns in the matrix */
    size_type cols(void) const {
        return Cols;
    }

  	/*! A constant forward column major iterator at the root of the matrix */
	const_iterator begin(void) const {
        return const_iterator(this);
    }

  	/*! A constant forward column major iterator at the end of the matrix */
	const_iterator end(void) const {
        return const_iterator(this,rows());
    }

  	/*! A constant reverse column major iterator at the root of the reverse matrix */
	const_reverse_iterator rbegin(void) const {
        return const_reverse_iterator(this,rows() - 1,cols() - 1,-1);
    }

  	/*! A constant reverse column major iterator at the end of the reverse matrix */
	const_reverse_iterator rend(void) const {
        return const_reverse_iterator(this,-1,cols() - 1,-1);
    }

	/*! Accessor for constant matrixes, virtual to facilitate 
	specilization based upon functional access type. Basic implementation simply
	returns the underline structure. Default implementation should be changed to
	use Data.Element as soon as possible */
	const_reference Element(index_type rowIndex, index_type colIndex) const {
		return value_type( Data->Element(rowIndex, colIndex) );
	}	

protected:
	/*! The number of rows in this pseudo matrix*/
 	size_type Rows;
	/*! The number of columns in this pseudo matrix*/
    size_type Cols;

	/*! The underlying matrix.
	*/
	MatrixBaseType * Data;
};


#endif // _vctDynamicFunctionalMatrixOwner_h

