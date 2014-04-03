/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky, Anton Deguet
  Created on: 2004-07-01

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicConstMatrixRef_h
#define _vctDynamicConstMatrixRef_h

/*!
  \file
  \brief Declaration of vctDynamicConstMatrixRef
*/

#include <cisstVector/vctDynamicConstMatrixBase.h>
#include <cisstVector/vctDynamicMatrixRefOwner.h>

/*!
  \ingroup cisstVector
  \brief Dynamic matrix referencing existing memory (const)

  The class vctDynamicConstMatrixRef allows to create a matrix
  overlaying an existing block of memory.  It can be used to
  manipulate a container created by another toolkit (i.e. OpenGL, vtk,
  ...) or a cisstVector container using different parameters
  (different size, storage order or stride).

  To setup the overlay, one can use either the constructors or the
  SetRef methods.  When used with a cisst container, the SetRef
  methods can infer some of the memory layout information (pointer on
  first element, sizes, strides).  When used to overlay existing
  memory referenced by a pointer, the user has to provide all the
  required information.  In any case, the Ref object doesn't allocate
  nor free memory.

  Finally, as this overlay is const, it can be used to make sure the
  referenced content is not modified.

  For example to manipulate the first five by five elements of a
  matrix, one can do:
  \code
  vctDynamicMatrix<double> wholeMatrix(10, 10);
  vctRandom(wholeMatrix, -10.0, 10.0);
  vctDynamicConstMatrixRef<double> first5x5;
  first5x5.SetRef(wholeMatrix, 0, 0, 5, 5);
  std::cout << first5x5 << " " << first5x5.SumOfElements() << std::endl;
  \endcode

  \note Make sure the underlying memory is not freed after the Ref
  object is created and before it is used.  This would lead to faulty
  memory access and potential bugs.

  \note vctDynamicConstMatrixRef only performs const operations even
  if it stores a non-const <code>value_type *</code>.  It can be
  initialized with either <code>value_type *</code> or <code>const
  value_type *</code>.  For a non-const Ref, see vctDynamicMatrixRef.

  \sa vctDynamicMatrix, vctDynamicMatrixRef

  \param _elementType Type of elements referenced.  Also defined as
  <code>value_type</code>.
*/
template <class _elementType>
class vctDynamicConstMatrixRef :
    public vctDynamicConstMatrixBase<vctDynamicMatrixRefOwner<_elementType>, _elementType>
{
public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {DIMENSION = 2};
    VCT_NARRAY_TRAITS_TYPEDEFS(DIMENSION);

    typedef vctDynamicConstMatrixRef<_elementType> ThisType;
    typedef vctDynamicMatrixRefOwner<_elementType> MatrixOwnerType;
    typedef vctDynamicConstMatrixBase<vctDynamicMatrixRefOwner<_elementType>, _elementType> BaseType;
    typedef typename MatrixOwnerType::iterator iterator;
    typedef typename MatrixOwnerType::const_iterator const_iterator;
    typedef typename MatrixOwnerType::reverse_iterator reverse_iterator;
    typedef typename MatrixOwnerType::const_reverse_iterator const_reverse_iterator;


    vctDynamicConstMatrixRef() {
        SetRef(0, 0, 1, 1, static_cast<pointer>(0));
    }

    /*! Copy constructor */
    vctDynamicConstMatrixRef(const ThisType & other):
        BaseType()
    {
        SetRef(other.sizes(), other.strides(), other.Pointer());
    }

    /*! Constructor requiring the user to provide all the necessary
        information, i.e. the size (number of rows and columns), the
        strides (increment from row to row and from column to column)
        as well as a pointer on the first element. */
    //@{
    vctDynamicConstMatrixRef(size_type rows, size_type cols,
                             stride_type rowStride, stride_type colStride,
                             pointer dataPointer)
    {
        SetRef(rows, cols, rowStride, colStride, dataPointer);
    }
    vctDynamicConstMatrixRef(size_type rows, size_type cols,
                             stride_type rowStride, stride_type colStride,
                             const_pointer dataPointer)
    {
        SetRef(rows, cols, rowStride, colStride, dataPointer);
    }

    vctDynamicConstMatrixRef(const nsize_type & matrixSize, stride_type rowStride, stride_type colStride,
                             pointer dataPointer)
    {
        SetRef(matrixSize, rowStride, colStride, dataPointer);
    }
    vctDynamicConstMatrixRef(const nsize_type & matrixSize, stride_type rowStride, stride_type colStride,
                             const_pointer dataPointer)
    {
        SetRef(matrixSize, rowStride, colStride, dataPointer);
    }

    vctDynamicConstMatrixRef(const nsize_type & matrixSize, const nstride_type & matrixStride,
                             pointer dataPointer)
    {
        SetRef(matrixSize, matrixStride, dataPointer);
    }
    vctDynamicConstMatrixRef(const nsize_type & matrixSize, const nstride_type & matrixStride,
                             const_pointer dataPointer)
    {
        SetRef(matrixSize, matrixStride, dataPointer);
    }
    //@}


    /*! Constructor requiring the user to provide the size of the
        matrix (number of rows and columns), a pointer on the first
        element and the desired storage order.  If the storage order
        is omitted, the default is used (row major). */
    //@{
    vctDynamicConstMatrixRef(size_type rows, size_type cols, pointer dataPointer,
                             bool storageOrder = VCT_DEFAULT_STORAGE)
    {
        SetRef(rows, cols, dataPointer, storageOrder);
    }
    vctDynamicConstMatrixRef(size_type rows, size_type cols, const_pointer dataPointer,
                             bool storageOrder = VCT_DEFAULT_STORAGE)
    {
        SetRef(rows, cols, dataPointer, storageOrder);
    }

    vctDynamicConstMatrixRef(const nsize_type & matrixSize, pointer dataPointer,
                             bool storageOrder = VCT_DEFAULT_STORAGE)
    {
        SetRef(matrixSize, dataPointer, storageOrder);
    }
    vctDynamicConstMatrixRef(const nsize_type & matrixSize, const_pointer dataPointer,
                             bool storageOrder = VCT_DEFAULT_STORAGE)
    {
        SetRef(matrixSize, dataPointer, storageOrder);
    }
    //@}


    /*! Construct a dynamic reference to a fixed-size matrix.
      \note the strides of this reference will be identical to the strides of
      the input matrix.  For a more advanced access, customize the parameters
      yourself (see the other ctors).
    */
    template <size_type __rows, size_type __cols, stride_type __rowStride, stride_type __colStride, typename __dataPtrType>
    inline vctDynamicConstMatrixRef(const vctFixedSizeConstMatrixBase<__rows, __cols, __rowStride, __colStride, _elementType, __dataPtrType>
                                    & otherMatrix)
    {
        SetRef(otherMatrix);
    }

    /*! Construct a dynamic reference to a submatrix of a fixed-size matrix.
      \param otherMatrix a fixed size matrix to which a reference is created.
      \param startRow row index of the position where the reference will start.
      \param startCol column index of the position where the reference will start.
      \param rows the number of rows being referenced
      \param cols the number of columns being referenced
      \note the strides of this reference will be identical to the strides of
      the input matrix.  For a more advanced access, customize the parameters
      yourself (see the other ctors).
    */
    template <size_type __rows, size_type __cols, stride_type __rowStride, stride_type __colStride, typename __dataPtrType>
    inline vctDynamicConstMatrixRef(const vctFixedSizeConstMatrixBase<__rows, __cols, __rowStride, __colStride, _elementType, __dataPtrType> & otherMatrix,
                                    size_type startRow, size_type startCol, size_type rows, size_type cols)
    {
        SetRef(otherMatrix, startRow, startCol, rows, cols);
    }

    /*! Construct a dynamic reference to a dynamic matrix.
      \note the strides of this reference will be identical to the strides of
      the input matrix.  For a more advanced access, customize the parameters
      yourself (see the other SetRef methods).
    */
    template <class __ownerType>
    inline vctDynamicConstMatrixRef(const vctDynamicConstMatrixBase<__ownerType, _elementType> & otherMatrix)
    {
        SetRef(otherMatrix);
    }

    /*! Construct a dynamic reference to a submatrix of a dynamic matrix.
      \param otherMatrix a fixed size matrix to which a reference is created.
      \param startRow row index of the position where the reference will start.
      \param startCol column index of the position where the reference will start.
      \param rows the number of rows being referenced
      \param cols the number of columns being referenced
      \note the strides of this reference will be identical to the strides of
      the input matrix.  For a more advanced access, customize the parameters
      yourself (see the other ctors).
    */
    //@{
    template <class __ownerType>
    inline vctDynamicConstMatrixRef(const vctDynamicConstMatrixBase<__ownerType, _elementType> & otherMatrix,
                                    size_type startRow, size_type startCol, size_type rows, size_type cols)
    {
        SetRef(otherMatrix, startRow, startCol, rows, cols);
    }

    template <class __ownerType>
    inline vctDynamicConstMatrixRef(const vctDynamicConstMatrixBase<__ownerType, _elementType> & otherMatrix,
                                    const nsize_type & start, const nsize_type & matrixSize)
    {
        SetRef(otherMatrix, start, matrixSize);
    }
    //@}

    /*! Set a dynamic reference to a memory location.  The user should
      specify all the parameters of the referenced memory, including a
      start memory address. */
    //@{
    void SetRef(size_type rows, size_type cols,
                stride_type rowStride, stride_type colStride,
                pointer dataPointer)
    {
        this->Matrix.SetRef(rows, cols, rowStride, colStride, dataPointer);
    }
    void SetRef(size_type rows, size_type cols,
                stride_type rowStride, stride_type colStride,
                const_pointer dataPointer)
    {
        this->Matrix.SetRef(rows, cols, rowStride, colStride, const_cast<pointer>(dataPointer));
    }

    void SetRef(const nsize_type & matrixSize,
                stride_type rowStride, stride_type colStride,
                pointer dataPointer) {
        this->Matrix.SetRef(matrixSize, rowStride, colStride, dataPointer);
    }
    void SetRef(const nsize_type & matrixSize,
                stride_type rowStride, stride_type colStride,
                const_pointer dataPointer) {
        this->Matrix.SetRef(matrixSize, rowStride, colStride, const_cast<pointer>(dataPointer));
    }

    void SetRef(const nsize_type & matrixSize,
                const nstride_type & matrixStride,
                pointer dataPointer) {
        this->Matrix.SetRef(matrixSize, matrixStride, dataPointer);
    }
    void SetRef(const nsize_type & matrixSize,
                const nstride_type & matrixStride,
                const_pointer dataPointer) {
        this->Matrix.SetRef(matrixSize, matrixStride, const_cast<pointer>(dataPointer));
    }
    //@}


    /*! Set a dynamic reference to a memory location.  The user should
      specify the dimension, start memory address and desired storage
      order.  This method assumes there is enough memory and will
      create a compact matrix reference. */
    //@{
    void SetRef(size_type rows, size_type cols, pointer dataPointer, bool storageOrder = VCT_DEFAULT_STORAGE)
    {
        this->Matrix.SetRef(rows, cols, dataPointer, storageOrder);
    }
    void SetRef(size_type rows, size_type cols, const_pointer dataPointer, bool storageOrder = VCT_DEFAULT_STORAGE)
    {
        this->Matrix.SetRef(rows, cols, const_cast<pointer>(dataPointer), storageOrder);
    }

    void SetRef(const nsize_type & sizes, pointer dataPointer, bool storageOrder = VCT_DEFAULT_STORAGE)
    {
        this->Matrix.SetRef(sizes, dataPointer, storageOrder);
    }
    void SetRef(const nsize_type & sizes, const_pointer dataPointer, bool storageOrder = VCT_DEFAULT_STORAGE)
    {
        this->Matrix.SetRef(sizes, const_cast<pointer>(dataPointer), storageOrder);
    }
    //@}


    /*! Set a dynamic reference to a fixed-size matrix.  The reference
      will have identical dimensions and strides as the input matrix.
    */
    template <size_type __rows, size_type __cols, stride_type __rowStride, stride_type __colStride, typename __dataPtrType>
    inline void SetRef(const vctFixedSizeConstMatrixBase<__rows, __cols, __rowStride, __colStride, _elementType, __dataPtrType>
                       & otherMatrix)
    {
        SetRef(otherMatrix.rows(), otherMatrix.cols(), otherMatrix.row_stride(), otherMatrix.col_stride(),
               otherMatrix.Pointer());
    }


    /*! Set a dynamic reference to a submatrix of a fixed-size matrix.
      \param otherMatrix a fixed size matrix to which a reference is created.
      \param startRow row index of the position where the reference will start.
      \param startCol column index of the position where the reference will start.
      \param rows the number of rows being referenced
      \param cols the number of columns being referenced
      \note the strides of this reference will be identical to the strides of
      the input matrix.  For a more advanced access, customize the parameters
      yourself (see the other SetRef methods).
      \note This method verifies that the size of this matrix does not exceed the
      size of the input matrix (otherwise cmnThrow is used to throw std::out_of_range).
    */
    template <size_type __rows, size_type __cols, stride_type __rowStride, stride_type __colStride, typename __dataPtrType>
    inline void SetRef(const vctFixedSizeConstMatrixBase<__rows, __cols, __rowStride, __colStride, _elementType, __dataPtrType> & otherMatrix,
                       size_type startRow, size_type startCol,
                       size_type rows, size_type cols)
    {
        if ( (startRow + rows > otherMatrix.rows()) || (startCol + cols > otherMatrix.cols()) ) {
            cmnThrow(std::out_of_range("vctDynamicConstMatrixRef SetRef out of range"));
        }
        SetRef(rows, cols, otherMatrix.row_stride(), otherMatrix.col_stride(), otherMatrix.Pointer(startRow, startCol));
    }

    /*! Set a dynamic reference to a dynamic matrix.  The reference
      will have identical dimensions and strides as the input matrix.
    */
    template <class __ownerType>
    inline void SetRef( const vctDynamicConstMatrixBase<__ownerType, _elementType> & otherMatrix)
    {
        SetRef(otherMatrix.rows(), otherMatrix.cols(), otherMatrix.row_stride(), otherMatrix.col_stride(),
               otherMatrix.Pointer());
    }

    /*! Set a dynamic reference to a submatrix of a dynamic matrix.
      \param otherMatrix a fixed size matrix to which a reference is created.
      \param startRow row index of the position where the reference will start.
      \param startCol column index of the position where the reference will start.
      \param rows the number of rows being referenced
      \param cols the number of columns being referenced
      \note the strides of this reference will be identical to the strides of
      the input matrix.  For a more advanced access, customize the parameters
      yourself (see the other SetRef methods).
      \note This method verifies that the size of this matrix does not exceed the
      size of the input matrix (otherwise cmnThrow is used to throw std::out_of_range).
    */
    //@{
    template <class __ownerType>
    inline void SetRef( const vctDynamicConstMatrixBase<__ownerType, _elementType> & otherMatrix,
                        size_type startRow, size_type startCol,
                        size_type rows, size_type cols)
    {
        if ( (startRow + rows > otherMatrix.rows()) || (startCol + cols > otherMatrix.cols()) ) {
            cmnThrow(std::out_of_range("vctDynamicConstMatrixRef SetRef out of range"));
        }
        SetRef(rows, cols, otherMatrix.row_stride(), otherMatrix.col_stride(),
               otherMatrix.Pointer(startRow, startCol));
    }

    template <class __ownerType>
    inline void SetRef(const vctDynamicConstMatrixBase<__ownerType, _elementType> & otherMatrix,
                       const nsize_type & start, const nsize_type & matrixSize)
    {
        SetRef(otherMatrix,
               start.Element(0), start.Element(1), matrixSize.Element(0), matrixSize.Element(1));
    }
    //@}
};


#endif // _vctDynamicConstMatrixRef_h

