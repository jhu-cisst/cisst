/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2004-01-12

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctMatrixRotation3ConstRef_h
#define _vctMatrixRotation3ConstRef_h

/*!
  \file
  \brief Declaration of vctMatrixRotation3Ref
 */

#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctMatrixRotation3Base.h>

#include <cisstVector/vctExport.h>


/*!
  \brief Define a rotation matrix for a space of dimension 3

  \ingroup cisstVector

  This class is templated by the element type.  It is derived from
  vctMatrixRotation3Base and uses a vctFixedSizeMatrix as underlying
  container.  It provides a more humain interface for programmers only
  interested in templating by _elementType.

  \param _elementType The type of elements of the matrix.

  \sa vctMatrixRotation3Base vctFixedSizeMatrix
*/
template <class _elementType, vct::stride_type _rowStride, vct::stride_type _colStride>
class vctMatrixRotation3ConstRef: public vctMatrixRotation3Base<vctFixedSizeConstMatrixRef<_elementType, 3, 3, _rowStride, _colStride> >
{
 public:
    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {ROWS = 3, COLS = 3};
    enum {DIMENSION = 3};
    typedef vctFixedSizeConstMatrixRef<value_type, ROWS, COLS, _rowStride, _colStride> ContainerType;
    typedef vctMatrixRotation3Base<ContainerType> BaseType;
    typedef vctMatrixRotation3ConstRef<value_type, _rowStride, _colStride> ThisType;
    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<value_type> TypeTraits;

    /*! Default constructor. Sets the rotation matrix to identity. */
    inline vctMatrixRotation3ConstRef(void)
    {}


    // #if 0 // default one works?
    // inline vctMatrixRotation3ConstRef(const ThisType & other) {
    //     this->SetRef(other);
    // }
    // #endif

    inline vctMatrixRotation3ConstRef(const BaseType & other) {
        this->SetRef(other);
    }

    template <class __containerType>
    inline vctMatrixRotation3ConstRef(const vctMatrixRotation3ConstBase<__containerType> & other) {
        this->SetRef(other.Pointer());
    }
};


#endif  // _vctMatrixRotation3ConstRef_h

