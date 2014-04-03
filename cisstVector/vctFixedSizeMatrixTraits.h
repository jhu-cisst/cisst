/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:	2003-11-04

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedSizeMatrixTraits_h
#define _vctFixedSizeMatrixTraits_h

/*!
  \file
  \brief Declaration of vctFixedSizeMatrixTraits
 */

#include <cisstVector/vctFixedStrideMatrixIterator.h>

/*!  \brief Define common container related types based on the
  properties of a fixed size container.

  The types are declared according to the STL requirements for the
  types declared by a container object.  This class is used as a trait
  to declare the actual containers.

  In addition to the STL required types, we declare a few more types for
  completeness.

*/
template <class _elementType,
          vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride>
class vctFixedSizeMatrixTraits
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! Iterator, follows the STL naming convention. */
    typedef vctFixedStrideMatrixIterator<_elementType, _colStride, _cols, _rowStride> iterator;

    /*! Const iterator, follows the STL naming convention. */
    typedef vctFixedStrideMatrixConstIterator<_elementType, _colStride, _cols, _rowStride> const_iterator;

    /*! Reverse iterator, follows the STL naming convention. */
    typedef vctFixedStrideMatrixIterator<_elementType, -_colStride, _cols, -_rowStride> reverse_iterator;

    /*! Const reverse iterator, follows the STL naming convention. */
    typedef vctFixedStrideMatrixConstIterator<_elementType, -_colStride, _cols, -_rowStride> const_reverse_iterator;

    /*! Define the dimensions of the matrix: number of rows, number of
      columns, and the total length of the matrix, which is their product
    */
    enum {ROWS = _rows, COLS = _cols, LENGTH = ROWS*COLS};
    /*! Declared for completeness. */
    typedef value_type array[LENGTH];
    /*! Declared for completeness. */
    typedef const value_type const_array[LENGTH];

    /*! Declared to enable inference of the stride of the container. */
    enum {ROWSTRIDE = _rowStride, COLSTRIDE = _colStride};
};


#endif // _vctFixedSizeMatrixTraits_h

