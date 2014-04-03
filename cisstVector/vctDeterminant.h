/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:	2004-04-16

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDeterminant_h
#define _vctDeterminant_h

/*!
  \file
  \brief Defines vctDeterminant
*/

#include <cisstVector/vctFixedSizeConstMatrixBase.h>

/*!
  \brief Determinant function for fixed size matrices

  Compute the determinant of a fixed size square matrix.  This
  templated class is currently specialized for matrices of size 1 by
  1, 2 by 2 or 3 by 3.

  \param _size The size of the square matrix
*/
template <vct::size_type _size>
class vctDeterminant
{
public:
    enum {SIZE = _size};
    /*!
      Actually compute the determinant of the matrix.

      \param matrix A fixed size square matrix
    */
    template <vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
    static _elementType Compute(const vctFixedSizeConstMatrixBase<_size, _size, _rowStride, _colStride, _elementType, _dataPtrType> & matrix);
};


#ifndef DOXYGEN

template<>
class vctDeterminant<1>
{
public:
    enum {SIZE = 1};
    template <vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
    static _elementType Compute(const vctFixedSizeConstMatrixBase<SIZE, SIZE, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
    {
        return matrix.Element(0, 0);
    }
};


template<>
class vctDeterminant<2>
{
public:
    enum {SIZE = 2};
    template <vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
    static _elementType Compute(const vctFixedSizeConstMatrixBase<SIZE, SIZE, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
    {
        return matrix.Element(0, 0) * matrix.Element(1, 1) - matrix.Element(0, 1) * matrix.Element(1, 0);
    }
};


template<>
class vctDeterminant<3>
{
public:
    enum {SIZE = 3};
    template<vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
    static _elementType Compute(const vctFixedSizeConstMatrixBase<SIZE, SIZE, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
    {
        return
            matrix.Element(0, 0) * ( matrix.Element(1, 1) * matrix.Element(2, 2) - matrix.Element(1, 2) * matrix.Element(2, 1) ) -
            matrix.Element(0, 1) * ( matrix.Element(1, 0) * matrix.Element(2, 2) - matrix.Element(1, 2) * matrix.Element(2, 0) ) +
            matrix.Element(0, 2) * ( matrix.Element(1, 0) * matrix.Element(2, 1) - matrix.Element(1, 1) * matrix.Element(2, 0) );
    }
};


#endif // DOXYGEN

#endif  // _vctDeterminant_h

