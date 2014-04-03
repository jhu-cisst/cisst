/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2004-01-12

  (C) Copyright 2004-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctMatrixRotation3Ref_h
#define _vctMatrixRotation3Ref_h

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
class vctMatrixRotation3Ref: public vctMatrixRotation3Base<vctFixedSizeMatrixRef<_elementType, 3, 3, _rowStride, _colStride> >
{
 public:
    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {ROWS = 3, COLS = 3};
    enum {DIMENSION = 3};
    typedef vctFixedSizeMatrixRef<value_type, ROWS, COLS, _rowStride, _colStride> ContainerType;
    typedef vctMatrixRotation3Base<ContainerType> BaseType;
    typedef vctMatrixRotation3Ref<value_type, _rowStride, _colStride> ThisType;
    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<value_type> TypeTraits;

    /*! Default constructor. Sets the rotation matrix to identity. */
    inline vctMatrixRotation3Ref(void):
        BaseType()
    {}


    inline vctMatrixRotation3Ref(const ThisType & other):
        BaseType()
    {
        this->SetRef(other);
    }

    inline vctMatrixRotation3Ref(const BaseType & other):
        BaseType()
    {
        this->SetRef(other);
    }

    /*! The assignment from BaseType (i.e. a 3 by 3 fixed size matrix)
      has to be redefined for this class (C++ restriction).  This
      operator uses the Assign() method inherited from the BaseType.
      This operator (as well as the Assign method) allows to set a
      rotation matrix to whatever value without any further validity
      checking.  It is recommended to use it with caution. */
    inline ThisType & operator = (const ContainerType & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    inline ThisType & operator = (const ThisType & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    inline ThisType & operator = (const BaseType & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

};


#endif  // _vctMatrixRotation3Ref_h
