/*
  
  Author(s):	Ofri Sadowsky, Anton Deguet
  Created on: 2004-07-01

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
/*! TODO List
01 [ ]	Is MatrixValueType used correctly?
02 [ ]	240: ConstRowRefType operator[](size_type index) const {
		Probably should be implemented at the matrix owner level
		Also 305, 310, 315 : Row Column, Diagonal
03 [ ]	734 : Smart transpose generation: How to do?
04 [ ]	785 : Smart ConstSubmatrix?
Later:
01 [ ]	_elementType for otherMatrix on operations seem to require
		the same type. We might like that not to be the case. Goes
		for scaler code as well. 
02 [ ]	Elementwise logical operations just skipped for now
*/


#ifndef _vctDynamicFunctionalMatrixBase_h
#define _vctDynamicFunctionalMatrixBase_h

/*!
  \file
  \brief Declaration of vctDynamicFunctionalMatrixBase
*/

#include <cisstCommon/cmnTypeTraits.h>
#include <cisstVector/vctContainerTraits.h>
#include <cisstVector/vctDynamicVectorRef.h>
#include <cisstVector/vctDynamicConstVectorRef.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicMatrixLoopEngines.h>

#include <iostream>
#include <iomanip>


/* Forward declarations *
template<class _matrixOwnerType, class __matrixOwnerType, class _elementType,
         class _elementOperationType>
inline vctReturnDynamicMatrix<bool>
vctDynamicMatrixElementwiseCompareMatrix(const vctDynamicFunctionalMatrixBase<_matrixOwnerType> & matrix1,
                                         const vctDynamicFunctionalMatrixBase<_matrixOwnerType> & matrix2);

template<class _matrixOwnerType, class _elementType,
         class _elementOperationType>
inline vctReturnDynamicMatrix<bool>
vctDynamicMatrixElementwiseCompareScalar(const vctDynamicFunctionalMatrixBase<_matrixOwnerType> & matrix,
                                         const _elementType & scalar);


/! 
  This class is templated with the ``matrix owner type'', which may
  be a vctDynamicMatrixOwner, vctMatrixRefOwner, vctFunctionalMatrixOwner,
  or vctFunctionalMatrixOwnerRef.  It provides const operations on the
  dynamic matrix, such as SumOfElements etc.
  
  Matrix indexing is zero-based.

  The method provided for the compatibility with the STL containers
  start with a lower case.  Other methods start with a capitalilized
  letter (see CISST naming convention).

  \param _matrixOwnerType the type of matrix owner.
*/
template<class _matrixOwnerType>
class vctDynamicFunctionalMatrixBase : public _matrixOwnerType
{
public:
	/*! Type of the matrix itself. */
    typedef vctDynamicFunctionalMatrixBase<_matrixOwnerType> ThisType;
    
    /*! Type of the data owner (dynamic array or pointer) */
    typedef _matrixOwnerType MatrixOwnerType;
    
    //Owner traits have to be copied and pasted
    /*! define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename MatrixOwnerType::value_type);
    
    /*! Constant foward iterator type */    
    typedef typename MatrixOwnerType::const_iterator const_iterator;
    /*! Constant reverse iterator type */    
    typedef typename MatrixOwnerType::const_reverse_iterator const_reverse_iterator;
    /*! foward iterator type */    
    typedef typename MatrixOwnerType::iterator iterator;
    /*! reverse iterator type */    
    typedef typename MatrixOwnerType::reverse_iterator reverse_iterator;
    /*! The type indicating a vector of this matrix accessed by (const)
      reference */
    typedef typename MatrixOwnerType::ConstVectorRefType ConstVectorRefType;
    /*! The type indicating a vector of this matrix accessed by
      (non-const) reference */
    typedef typename MatrixOwnerType::VectorRefType VectorRefType;
    /*! The type of matrix indicating matrix access by (const)
      reference */
    typedef typename MatrixOwnerType::ConstMatrixRefType ConstMatrixRefType;
    /*! The type of matrix indicating matrix access by (const)
      reference */
    typedef typename MatrixOwnerType::MatrixRefType MatrixRefType;
    /*! The type of a matrix returned by value from operations on this object */
    typedef typename MatrixOwnerType::MatrixValueType MatrixValueType;
    
    /*! Row reference type, a specilization of vector reference type */
    typedef ConstVectorRefType ConstRowRefType;
    /*! Column reference type, a specilization of vector reference type */
    typedef ConstVectorRefType ConstColumnRefType;
    /*! Diagonal reference type, a specilization of vector reference type */
    typedef ConstVectorRefType ConstDiagonalRefType;
    /*! Matrix Transpose reference type, a specilization of vector reference type */
    typedef ConstMatrixRefType ConstRefTransposeType;
    
    /*! The type of object required to store a transposed copy of this matrix */
    typedef vctDynamicMatrix<value_type> TransposeValueType;
    /*! The type of a matrix of booleans returned from operations on this object, e.g.,
      ElementwiseEqual. */
    typedef vctReturnDynamicMatrix<bool> BoolMatrixValueType;
    /*! Common contaner type traits */
    typedef cmnTypeTraits<value_type> TypeTraits;
    
 protected:
    /*! Check the validity of an index. */
    inline void RangeCheck(size_type index) const throw(std::out_of_range) {
        if (index >= this->size()) {
            throw std::out_of_range("vctDynamicFunctionalMatrix access out of range");
        }
    }

    /*! Check the validity of the row and column indices. */
    inline void RangeCheck(size_type rowIndex, size_type colIndex) const throw(std::out_of_range) {
        if ((rowIndex >= this->rows()) || (colIndex >= this->cols())) {
            throw std::out_of_range("vctDynamicFunctionalMatrix access out of range");
        }
    }
    
 public:
    /*! Tell is the matrix is empty (STL compatibility).  False unless
      size is zero. */
    bool empty(void) const {
        return (this->size() == 0);
    }
    
    /*! Reference a row of this matrix by index (const).
      \return a const reference to the element[index] */
    ConstRowRefType operator[](size_type index) const {
//        return ConstRowRefType(cols(), Pointer(index, 0), col_stride());
        return ConstRowRefType(0, 0, 1);
    }
    
    /*! Access an element by index (const).  Compare with
      std::matrix::at().  This method can be a handy substitute for
      the overloaded operator [] when operator overloading is
      unavailable or inconvenient.  \return a const reference to
      element[index] */
    const_reference at(size_type index) const throw(std::out_of_range) {
        RangeCheck(index);
        return (this->begin())[index];
    }

    /*! Returns true if both rowIndex and colIndex are valid
      indices. */
    bool ValidIndex(size_type rowIndex, size_type colIndex) const {
        return ((rowIndex >= 0)
                && (rowIndex < this->rows())
                && (colIndex >= 0)
                && (colIndex < this->cols()));
    }
    
    /*! Returns true if rowIndex is a valid row index. */
    bool ValidRowIndex(size_type rowIndex) const {
        return ((rowIndex >= 0)
                && (rowIndex < this->rows()));
    }
    
    /*! Returns true if colIndex is a valid column index. */
    bool ValidColIndex(size_type colIndex) const {
        return ((colIndex >= 0)
                && (colIndex < this->cols()));
    }

    /*! Access an element by index (const).  Compare with std::vector::at().
      This method can be a handy substitute for the overloaded operator () when
      operator overloading is unavailable or inconvenient.
      \return a const reference to the element at rowIndex, colIndex */
    const_reference at(size_type rowIndex, size_type colIndex) const throw(std::out_of_range) {
        RangeCheck(rowIndex, colIndex);
        return this->Element(rowIndex, colIndex);
    }

    /*! Overloaded operator () for simplified (const) element access with bounds checking */
    const_reference operator () (size_type rowIndex, size_type colIndex) const throw(std::out_of_range) {
        return this->at(rowIndex, colIndex);
    }

    ConstRowRefType Row(size_type index) const {
//        return ConstRowRefType(cols(), Pointer(index, 0), col_stride());
		return ConstRowRefType(0,0,1);
    }
    
    ConstColumnRefType Column(size_type index) const {
 //       return ConstColumnRefType(rows(), Pointer(0, index), row_stride());
		return ConstColumnRefType(0,0,1);
    }

    ConstDiagonalRefType Diagonal() const {
		return ConstDiagonalRefType(0,0,1);
//        return ConstDiagonalRefType( std::min(rows(), cols()), Pointer(0, 0), row_stride() + col_stride() );
    }


    /*! \name Incremental operations returning a scalar.
      Compute a scalar from all the elements of the matrix. */
    //@{

    /*! Return the sum of the elements of the matrix.
      \return The sum of all the elements */
    inline value_type SumOfElements(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    } 

    /*! Return the square of the norm  of the matrix.
      \return The square of the norm */
    inline value_type NormSquare(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Square>::
            Run(*this);
    }

    /*! Return the norm of the matrix.
      \return The norm. */
    inline NormType Norm(void) const {
        return sqrt(NormType(NormSquare()));
    }

    /*! Return the L1 norm of the matrix, i.e. the sum of the absolute
      values of all the elements.
 
      \return The L1 norm. */
    inline value_type L1Norm(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the Linf norm of the matrix, i.e. the maximum of the absolute
      values of all the elements.
 
      \return The Linf norm. */
    inline value_type LinfNorm(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the maximum element of the matrix.
      \return The maximum element */
    inline value_type MaxElement(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the minimum element of the matrix.
      \return The minimum element */
    inline value_type MinElement(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Minimum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }
    
    /*! Return true if all the elements of this matrix are strictly positive, 
      false otherwise */
    inline bool IsPositive(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are non-negative, 
      false otherwise */
    inline bool IsNonNegative(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are non-positive, 
      false otherwise */
    inline bool IsNonPositive(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are strictly negative, 
      false otherwise */
    inline bool IsNegative (void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are nonzero,
      false otherwise */
    inline bool All(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    /*! Return true if any element of this matrix is nonzero, false
      otherwise */
    inline bool Any(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::Or,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }
    //@}


    /*! \name Elementwise comparisons between matrices.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between two matrices of the same size, containing
      the same type of elements.  The strides can be different.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for Equal(),
      NotEqual(), Lesser(), LesserOrEqual(), Greater() or
      GreaterOrEqual() is performed elementwise between the two
      matrices.  A logical "and" is performed (except for NotEqual
      which uses a logical "or") to accumulate the elementwise
      results.  The only operators provided are "==" and "!=" since
      the semantic is not ambiguous.

      \return A boolean.
    */
    template <class __matrixOwnerType>
    inline bool Equal(const vctDynamicFunctionalMatrixBase<__matrixOwnerType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, otherMatrix);
    }
    
    /* documented above */
    template <class __matrixOwnerType>
    inline bool operator == (const vctDynamicFunctionalMatrixBase<__matrixOwnerType> & otherMatrix) const {
        return Equal(otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool AlmostEqual(const vctDynamicFunctionalMatrixBase<__matrixOwnerType> & otherMatrix,
			    value_type tolerance) const {
        return ((*this - otherMatrix).LinfNorm() <= tolerance);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool AlmostEqual(const vctDynamicFunctionalMatrixBase<__matrixOwnerType> & otherMatrix) const {
        return ((*this - otherMatrix).LinfNorm() <= TypeTraits::Tolerance());
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool NotEqual(const vctDynamicFunctionalMatrixBase<__matrixOwnerType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, otherMatrix);
    }
    
    /* documented above */
    template <class __matrixOwnerType>
    inline bool operator != (const vctDynamicFunctionalMatrixBase<__matrixOwnerType> & otherMatrix) const {
        return NotEqual(otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool Lesser(const vctDynamicFunctionalMatrixBase<__matrixOwnerType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool LesserOrEqual(const vctDynamicFunctionalMatrixBase<__matrixOwnerType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool Greater(const vctDynamicFunctionalMatrixBase<__matrixOwnerType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, otherMatrix);
    }
    
    /* documented above */
    template <class __matrixOwnerType>
    inline bool GreaterOrEqual(const vctDynamicFunctionalMatrixBase<__matrixOwnerType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Run(*this, otherMatrix);
    }
    //@}

    /*! \name Elementwise comparisons between matrices.
      Returns the matrix of comparison's results. */
    //@{
    /*! Comparison between two matrices of the same size, containing
      the same type of elements.  The strides and the internal
      representation (_matrixOwnerType) can be different.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for
      ElementwiseEqual(), ElementwiseNotEqual(), ElementwiseLesser(),
      ElementwiseLesserOrEqual(), ElementwiseGreater() or
      ElementwiseGreaterOrEqual() is performed elementwise between the
      two matrices and stored in a newly created matrix.  There is no
      operator provided since the semantic would be ambiguous.

      \return A matrix of booleans.
    *
    template<class __matrixOwnerType>
    inline BoolMatrixValueType
    ElementwiseEqual(const vctDynamicFunctionalMatrixBase<__matrixOwnerType, value_type> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, otherMatrix);
    }

    /! documented above *
    template<class __matrixOwnerType>
    inline BoolMatrixValueType
    ElementwiseNotEqual(const vctDynamicFunctionalMatrixBase<__matrixOwnerType, value_type> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, otherMatrix);
    }

    /! documented above *
    template<class __matrixOwnerType>
    inline BoolMatrixValueType
    ElementwiseLesser(const vctDynamicFunctionalMatrixBase<__matrixOwnerType, value_type> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, otherMatrix);
    }

    /! documented above *
    template<class __matrixOwnerType>
    inline BoolMatrixValueType
    ElementwiseLesserOrEqual(const vctDynamicFunctionalMatrixBase<__matrixOwnerType, value_type> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, otherMatrix);
    }

    /! documented above *
    template<class __matrixOwnerType>
    inline BoolMatrixValueType
    ElementwiseGreater(const vctDynamicFunctionalMatrixBase<__matrixOwnerType, value_type> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, otherMatrix);
    }

    /! documented above *
    template<class __matrixOwnerType>
    inline BoolMatrixValueType
    ElementwiseGreaterOrEqual(const vctDynamicFunctionalMatrixBase<__matrixOwnerType, value_type> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>(*this, otherMatrix);
    }

    //@}


    /! \name Elementwise comparisons between a matrix and a scalar.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between a matrix and a scalar.  The type of the
      elements of the matrix and the scalar must be the same.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for Equal(),
      NotEqual(), Lesser(), LesserOrEqual(), Greater() or
      GreaterOrEqual() is performed elementwise between the matrix and
      the scalar.  A logical "and" is performed (except for NotEqual
      which uses a logical "or") to accumulate the elementwise
      results..  The only operators provided are "==" and "!=" since
      the semantic is not ambiguous.

      \return A boolean.
    */
    inline bool Equal(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, scalar);
    }
    
    /* documented above */
    inline bool operator == (const value_type & scalar) const {
        return Equal(scalar);
    }

    /* documented above */
    inline bool NotEqual(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool operator != (const value_type & scalar) const {
        return NotEqual(scalar);
    }

    /* documented above */
    inline bool Lesser(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool LesserOrEqual(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool Greater(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool GreaterOrEqual(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Run(*this, scalar);
    }
    //@}

    /*! \name Elementwise comparisons between a matrix and a scalar.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between a matrix and a scalar, containing the same
      type of elements.  The comparaison (\f$ = \neq < \leq > \geq
      \f$) for ElementwiseEqual(), ElementwiseNotEqual(),
      ElementwiseLesser(), ElementwiseLesserOrEqual(),
      ElementwiseGreater() or ElementwiseGreaterOrEqual() is performed
      elementwise between the matrix and the scalar and stored in a
      newly created matrix.  There is no operator provided since the
      semantic would be ambiguous.

      \return A matrix of booleans.
    *
    BoolMatrixValueType ElementwiseEqual(const value_type & scalar) const;

    /! documented above *
    BoolMatrixValueType ElementwiseNotEqual(const value_type & scalar) const;

    /! documented above *
    BoolMatrixValueType ElementwiseLesser(const value_type & scalar) const;

    /! documented above *
    BoolMatrixValueType ElementwiseLesserOrEqual(const value_type & scalar) const;

    /! documented above *
    BoolMatrixValueType ElementwiseGreater(const value_type & scalar) const;

    /! documented above *
    BoolMatrixValueType ElementwiseGreaterOrEqual(const value_type & scalar) const;

    //@}

    /! \name Unary elementwise operations.
      Returns the result of matrix.op(). */
    //@{
    /*! Unary elementwise operations on a matrix.  For each element of
      the matrix "this", performs \f$ this[i] \leftarrow
      op(otherMatrix[i])\f$ where \f$op\f$ can calculate the absolute
      value (Abs) or the opposite (Negation).
      
      \return A new matrix.
    */
    inline MatrixValueType Abs(void) const;
    
    /* documented above */
    inline MatrixValueType Negation(void) const;
    //@}

    /*! Return a transposed reference to this matrix.  This allows access to elements
      of this matrix in transposed order.
    */
    ConstRefTransposeType TransposeRef(void) const
    {
        return ConstRefTransposeType();
//        return ConstRefTransposeType(cols(), rows(), col_stride(), row_stride(), Pointer());
    }

    /*! Return a transposed copy of this matrix */
    inline vctReturnDynamicMatrix<value_type> Transpose() const
    {
        vctDynamicMatrix<value_type> result( this->TransposeRef() );
        return vctReturnDynamicMatrix<value_type>(result);
    }

    std::string ToString(void) {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }

    void ToStream(std::ostream & outputStream) const {
        size_type rowIndex, colIndex;
        const size_type nbRows = this->rows();
        const size_type nbCols = this->cols();
        for (rowIndex = 0; rowIndex < nbRows; ++rowIndex) {
            for (colIndex = 0; colIndex < nbCols; ++colIndex) {
                outputStream << std::setprecision(6) << std::setw(12) << std::showpoint << this->Element(rowIndex, colIndex) << " ";
            }
            // end of line between rows, not at the end
            if (rowIndex != (nbRows - 1)) {
                outputStream << std::endl;
            }
        }
    }

	/*! Output a Matlab style deliminated string, for file output, etc. with variable label */
    void ToDeliminatedStream(std::ostream & outputStream, const char * label) const {
		outputStream << label << " = ";
		ToDeliminatedStream(outputStream);
	}	

	/*! Output a Matlab style deliminated string, for file output, etc. */
    void ToDeliminatedStream(std::ostream & outputStream) const {
        size_type rowIndex, colIndex;
        const size_type nbRows = this->rows();
        const size_type nbCols = this->cols();
	outputStream << "[";
        for (rowIndex = 0; rowIndex < nbRows; ++rowIndex) {
            for (colIndex = 0; colIndex < nbCols; ++colIndex) {
		outputStream << std::setprecision(15) << this->Element(rowIndex, colIndex) << ' ';
            }
            // end of line between rows, not at the end
            if (rowIndex != (nbRows - 1)) {
                outputStream << ";" << std::endl;
	    }
        }
	outputStream << "];";
    }

    /*! Define a ConstSubmatrix class for compatibility with the fixed size matrices.
      A const submatrix has the same stride as the parent container.

      Example:

      typedef vctDynamicMatrix<double> MatrixType;
      MatrixType M(6,6);
      MatrixType::ConstSubmatrix::Type topLeft(M, 0, 0);
      MatrixType::ConstSubmatrix::Type bottomRight(M, 3, 3);

      \note There is no straightforward way to define a fixed-size
      submatrix of a dynamic matrix, because the stride of the dynamic
      matrix is not known in compilation time.  A way to do it is:

      vctFixedSizeConstMatrixRef<double, 3, 3, 1, 6> topRight(M, 0, 3);
      vctFixedSizeConstMatrixRef<double, 3, 3, 1, 6> bottomLeft(M, 3, 0);
    */
#ifndef SWIG
    class ConstSubmatrix
    {
    public:
        typedef vctDynamicConstMatrixRef<value_type> Type;
    };
#endif // SWIG
};

/* documented in class.  Implementation moved here for .Net 2003 *
template<class _matrixOwnerType, _elementType >
inline typename vctDynamicFunctionalMatrixBase<_matrixOwnerType>::BoolMatrixValueType
vctDynamicFunctionalMatrixBase<_matrixOwnerType>::ElementwiseEqual(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::Equal>(*this, scalar);
}

/! documented in class.  Implementation moved here for .Net 2003 *
template<class _matrixOwnerType, class _elementType>
inline typename vctDynamicFunctionalMatrixBase<_matrixOwnerType>::BoolMatrixValueType
vctDynamicFunctionalMatrixBase<_matrixOwnerType>::ElementwiseNotEqual(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::NotEqual>(*this, scalar);
}

/! documented in class.  Implementation moved here for .Net 2003 *
template<class _matrixOwnerType, class _elementType>
inline typename vctDynamicFunctionalMatrixBase<_matrixOwnerType>::BoolMatrixValueType
vctDynamicFunctionalMatrixBase<_matrixOwnerType>::ElementwiseLesser(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::Lesser>(*this, scalar);
}

/! documented in class.  Implementation moved here for .Net 2003 *
template<class _matrixOwnerType, class _elementType>
inline typename vctDynamicFunctionalMatrixBase<_matrixOwnerType>::BoolMatrixValueType
vctDynamicFunctionalMatrixBase<_matrixOwnerType>::ElementwiseLesserOrEqual(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::LesserOrEqual>(*this, scalar);
}

/! documented in class.  Implementation moved here for .Net 2003 *
template<class _matrixOwnerType, class _elementType>
inline typename vctDynamicFunctionalMatrixBase<_matrixOwnerType>::BoolMatrixValueType
vctDynamicFunctionalMatrixBase<_matrixOwnerType>::ElementwiseGreater(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::Greater>(*this, scalar);
}

/! documented in class.  Implementation moved here for .Net 2003 *
template<class _matrixOwnerType>
inline typename vctDynamicFunctionalMatrixBase<_matrixOwnerType>::BoolMatrixValueType
vctDynamicFunctionalMatrixBase<_matrixOwnerType>::ElementwiseGreaterOrEqual(const _matrixOwnerType::value_type & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::GreaterOrEqual>(*this, scalar);
}


/! Return true if all the elements of the matrix are nonzero, false otherwise */
template<class _matrixOwnerType>
inline bool vctAll(const vctDynamicFunctionalMatrixBase<_matrixOwnerType> & matrix) {
    return matrix.All();
}

/*! Return true if any element of the matrix is nonzero, false otherwise */
template<class _matrixOwnerType>
inline bool vctAny(const vctDynamicFunctionalMatrixBase<_matrixOwnerType> & matrix) {
    return matrix.Any();
}

/*! Stream out operator. */
template<class _matrixOwnerType>
std::ostream & operator << (std::ostream & output,
                            const vctDynamicFunctionalMatrixBase<_matrixOwnerType> & matrix) {
    matrix.ToStream(output);
    return output;
}


#endif // _vctDynamicFunctionalMatrixBase_h

