/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2005-02-23

  (C) Copyright 2005-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctMatrixRotation2_h
#define _vctMatrixRotation2_h

/*!
  \file
  \brief Declaration of vctMatrixRotation2
 */

#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctMatrixRotation2Base.h>

#include <cisstVector/vctExport.h>

/*!
  \brief Define a rotation matrix for a space of dimension 2

  \ingroup cisstVector

  This class is templated by the element type.  It is derived from
  vctMatrixRotation2Base and uses a vctFixedSizeMatrix as underlying
  container.  It provides a more humain interface for programmers only
  interested in templating by _elementType.

  \param _elementType The type of elements of the matrix.

  \sa vctMatrixRotation2Base vctFixedSizeMatrix
*/
template <class _elementType>
class vctMatrixRotation2: public vctMatrixRotation2Base<vctFixedSizeMatrix<_elementType, 2, 2> >
{
public:
    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {ROWS = 2, COLS = 2};
    enum {DIMENSION = 2};
    typedef vctFixedSizeMatrix<value_type, ROWS, COLS> ContainerType;
    typedef vctMatrixRotation2Base<ContainerType> BaseType;
    typedef vctMatrixRotation2<value_type> ThisType;

    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<value_type> TypeTraits;

    /*! Default constructor. Sets the rotation matrix to identity. */
    inline vctMatrixRotation2():
        BaseType()
    {}

    inline vctMatrixRotation2(const ThisType & other):
        BaseType(other)
    {}

    inline vctMatrixRotation2(const BaseType & other):
        BaseType(other)
    {}

    /*! The assignment from BaseType (i.e. a 2 by 2 fixed size matrix)
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

    /* documented in base class */
    inline vctMatrixRotation2(const value_type & element00, const value_type & element01,
                              const value_type & element10, const value_type & element11)
        CISST_THROW(std::runtime_error):
        BaseType(element00, element01,
                 element10, element11)
    {}

    template <stride_type __stride1, class __dataPtrType1,
              stride_type __stride2, class __dataPtrType2>
    inline vctMatrixRotation2(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1>& v1,
                              const vctFixedSizeConstVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2>& v2,
                              bool vectorsAreColumns = true)
        CISST_THROW(std::runtime_error):
        BaseType(v1, v2, vectorsAreColumns)
    {}

    template <class __vectorOwnerType1,
              class __vectorOwnerType2>
    inline vctMatrixRotation2(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                              const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                              bool vectorsAreColumns = true)
        CISST_THROW(std::runtime_error):
        BaseType(v1, v2, vectorsAreColumns)
    {}

    inline vctMatrixRotation2(const vctAngleRotation2 & angleRotation)
        CISST_THROW(std::runtime_error):
        BaseType(angleRotation)
    {}

    inline vctMatrixRotation2(const value_type & element00, const value_type & element01,
                              const value_type & element10, const value_type & element11,
                              bool normalizeInput):
        BaseType(element00, element01,
                 element10, element11,
                 normalizeInput)
    {}

    template <stride_type __stride1, class __dataPtrType1,
              stride_type __stride2, class __dataPtrType2>
    inline vctMatrixRotation2(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1>& v1,
                              const vctFixedSizeConstVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2>& v2,
                              bool vectorsAreColumns, bool normalizeInput):

        BaseType(v1, v2, vectorsAreColumns, normalizeInput)
    {}

    template <class __vectorOwnerType1,
              class __vectorOwnerType2>
    inline vctMatrixRotation2(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                              const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                              bool vectorsAreColumns, bool normalizeInput):
        BaseType(v1, v2, vectorsAreColumns, normalizeInput)
    {}

    inline vctMatrixRotation2(const vctAngleRotation2 & angleRotation,
                              bool normalizeInput):
        BaseType(angleRotation, normalizeInput)
    {}

    /*! Initialize this rotation matrix with a 2x2 matrix.  This
      constructor only takes a matrix of the same element type.

      \note This constructor does not verify normalization.  It is
      introduced to allow using results of matrix operations and
      assign them to a rotation matrix.

      \note The constructor is declared explicit, to force the user to
      be aware of the conversion being made.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    explicit inline
    vctMatrixRotation2(const vctFixedSizeMatrixBase<ROWS, COLS, __rowStride, __colStride, value_type, __dataPtrType> & matrix):
        BaseType(matrix)
    {}

};


#endif  // _vctMatrixRotation2_h
