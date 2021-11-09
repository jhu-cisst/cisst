/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2004-10-25

  (C) Copyright 2004-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctForwardDeclarations_h
#define _vctForwardDeclarations_h

/*!
  \file
  \brief Forward declarations and \#define for cisstVector
  \ingroup cisstVector
*/
#include <cisstCommon/cmnPortability.h>
#include <cisstVector/vctContainerTraits.h>

/*!
  \name Definition of the storage order used for the matrices.

  The default for cisstVector is row major (#VCT_ROW_MAJOR) which is
  the default convention for C/C++ code.  In order to support some
  numerical libraries in Fortran (or Fortran compatible), cisstVector
  also support column major order (#VCT_COL_MAJOR). */
//@{

/*! Value to use for a row major storage order. */
const bool VCT_ROW_MAJOR = true;

/*! Value to use for a column major storage order. */
const bool VCT_COL_MAJOR = false;

/*! Default storage order used in cisstVector. */
const bool VCT_DEFAULT_STORAGE = VCT_ROW_MAJOR;

/*! More friendly value to use for Fortran storage. Equivalent to #VCT_COL_MAJOR. */
const bool VCT_FORTRAN_ORDER = VCT_COL_MAJOR;

//@}


/*! Value used to force the normalization of the input for rotation
  constructors.  See classes vctMatrixRotation3Base,
  vctQuaternionRotation3Base, vctAxisAngleRotation3,
  vctRodriguezRotation3Base, etc. */
const bool VCT_NORMALIZE = true;

/*! Value used to NOT force the normalization of the input for
  rotation constructors.  Not only the input will not be normalized,
  but the constructor or method will not check if the input is
  normalized.  See classes vctMatrixRotation3Base,
  vctQuaternionRotation3Base, vctAxisAngleRotation3,
  vctRodriguezRotation3Base, etc. */
const bool VCT_DO_NOT_NORMALIZE = false;


// iterators
template <class _ownerType, bool _forward>
class vctVarStrideNArrayIterator;

template <class _ownerType, bool _forward>
class vctVarStrideNArrayConstIterator;


// fixed size vectors
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
class vctFixedSizeConstVectorBase;

template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
class vctFixedSizeVectorBase;

template <class _elementType, vct::size_type _size, vct::stride_type _stride>
class vctFixedSizeConstVectorRef;

template <class _elementType, vct::size_type _size, vct::stride_type _stride>
class vctFixedSizeVectorRef;

template <class _elementType, vct::size_type _size>
class vctFixedSizeVector;


// fixed size matrices
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride,
          vct::stride_type _colStride, class _elementType, class _dataPtrType>
class vctFixedSizeConstMatrixBase;

template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride,
          vct::stride_type _colStride, class _elementType, class _dataPtrType>
class vctFixedSizeMatrixBase;

template <class _elementType, vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride>
class vctFixedSizeConstMatrixRef;

template <class _elementType, vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride>
class vctFixedSizeMatrixRef;

template <class _elementType, vct::size_type _rows, vct::size_type _cols,
          bool _rowMajor = VCT_ROW_MAJOR>
class vctFixedSizeMatrix;


// dynamic vectors
template <class _vectorOwnerType, class _elementType>
class vctDynamicConstVectorBase;

template <class _vectorOwnerType, class _elementType>
class vctDynamicVectorBase;

template <class _elementType>
class vctDynamicConstVectorRef;

template <class _elementType>
class vctDynamicVectorRef;

template <class _elementType>
class vctDynamicVector;

template <class _elementType>
class vctReturnDynamicVector;

template <class _elementType>
class vctDynamicVectorOwner;

template <class _elementType>
class vctDynamicVectorRefOwner;


// dynamic matrices
template <class _matrixOwnerType, class _elementType>
class vctDynamicConstMatrixBase;

template <class _matrixOwnerType, class _elementType>
class vctDynamicMatrixBase;

template <class _elementType>
class vctDynamicConstMatrixRef;

template <class _elementType>
class vctDynamicMatrixRef;

template <class _elementType>
class vctDynamicMatrix;

template <class _elementType>
class vctReturnDynamicMatrix;

template <class _elementType>
class vctDynamicMatrixOwner;

template <class _elementType>
class vctDynamicMatrixRefOwner;


// dynamic nArrays
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
class vctDynamicConstNArrayBase;

template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
class vctDynamicNArrayBase;

template <class _elementType, vct::size_type _dimension>
class vctDynamicConstNArrayRef;

template <class _elementType, vct::size_type _dimension>
class vctDynamicNArrayRef;

template <class _elementType, vct::size_type _dimension>
class vctDynamicNArray;

template <class _elementType, vct::size_type _dimension>
class vctReturnDynamicNArray;

template <class _elementType, vct::size_type _dimension>
class vctDynamicNArrayOwner;

template <class _elementType, vct::size_type _dimension>
class vctDynamicNArrayRefOwner;


// transformations
template <class _containerType> class vctMatrixRotation3ConstBase;
template <class _containerType> class vctMatrixRotation3Base;
template <class _elementType, bool _rowMajor = VCT_ROW_MAJOR> class vctMatrixRotation3;
template <class _elementType, vct::stride_type _rowStride, vct::stride_type _colStride> class vctMatrixRotation3Ref;
template <class _elementType, vct::stride_type _rowStride, vct::stride_type _colStride> class vctMatrixRotation3ConstRef;

template <class _containerType>
class vctQuaternionBase;

template <class _containerType>
class vctQuaternionRotation3Base;

template <class _elementType>
class vctAxisAngleRotation3;

template <class _containerType>
class vctRodriguezRotation3Base;
template <class _elementType>
class vctRodriguezRotation3;

namespace vctEulerRotation3Order {
    //enum OrderType { XZX, XZY, XYX, XYZ, YXY, YXZ, YZY, YZX, ZYZ, ZYX, ZXZ, ZXY };
    typedef enum { ZYZ, ZYX, ZXZ, YZX } OrderType;
};

template <vctEulerRotation3Order::OrderType _order>
class vctEulerRotation3;

template <class _containerType>
class vctMatrixRotation2Base;

class vctAngleRotation2;

template <class _rotationType>
class vctFrameBase;

template <class _containerType> class vctFrame4x4ConstBase;
template <class _containerType> class vctFrame4x4Base;
template <class _elementType, bool _rowMajor = VCT_ROW_MAJOR> class vctFrame4x4;

// plot
class vctPlot2DBase;

#endif  // _vctForwardDeclarations_h
