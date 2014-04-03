/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2007-09-13

  (C) Copyright 2007-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFrame4x4_h
#define _vctFrame4x4_h

/*!
  \file
  \brief Declaration of vctFrame4x4
 */

#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctFrame4x4Base.h>
#include <cisstVector/vctExport.h>

/*!
  \brief Template base class for a 4x4 frame.

  This class allows to use a 4 by 4 matrix as a frame in 3D.  This is
  a limited case of the so called homegenous transformations as this
  class is intended to support only the translation and rotation parts
  of the transformation.  It is not intended to support perspective or
  scaling operations.  The different constructors and normalization
  methods provided will set the last row to [0 0 0 1].

  \param _matrixType The type of matrix used to store the elements

  \sa vctDynamicMatrix, vctFixedSizeMatrix, vctFrameBase
*/
template <class _elementType, bool _rowMajor>
class vctFrame4x4:
    public vctFrame4x4Base<vctFixedSizeMatrix<_elementType, 4, 4, _rowMajor> >
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    enum {ROWS = 4, COLS = 4};
    enum {DIMENSION = 3};

    typedef vctFixedSizeMatrix<value_type, ROWS, COLS, _rowMajor> ContainerType;
    typedef vctFrame4x4Base<ContainerType> BaseType;
    typedef vctFrame4x4<value_type, _rowMajor> ThisType;

    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<value_type> TypeTraits;


public:


    /*! Default constructor. Sets the matrix to identity. */
    vctFrame4x4(void):
        BaseType()
    {
        this->Assign(this->Identity());
    }

    /*! Copy constructor.  Uses Assign. */
    vctFrame4x4(const ThisType & other):
        BaseType()
    {
        this->Assign(other);
    }

    inline ThisType & operator = (const ThisType & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    /*! Copy constructor with different storage order, uses Assign */
    template <bool __rowMajor>
    vctFrame4x4(const vctFrame4x4<value_type, __rowMajor> & other):
        BaseType()
    {
        this->Assign(other);
    }

    template <bool __rowMajor>
    inline ThisType & operator = (const vctFrame4x4<value_type, __rowMajor> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }


    /*! Copy constructor from vctFrame4x4ConstBase with a different
      container type.  Uses Assign. */
    template <class __containerType>
    vctFrame4x4(const vctFrame4x4ConstBase<__containerType> & other):
        BaseType()
    {
        this->Assign(other);
    }

    template <class __containerType>
    inline ThisType & operator = (const vctFrame4x4ConstBase<__containerType> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }


    /*! Copy constructor from ContainerType.  Uses Assign. */
    vctFrame4x4(const ContainerType & other):
        BaseType()
    {
        this->Assign(other);
    }

    inline ThisType & operator = (const ContainerType & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }


    /*! Constructor from a translation and a rotation. */
    template <class _rotationType, class _translationType>
    vctFrame4x4(const _rotationType & rotation,
                const _translationType & translation):
        BaseType()
    {
        this->From(rotation, translation);
    }

    /*! Constructor from a vctFrameBase.  This constructor uses the
      method From which will test if the input is normalized.  If
      the input is not normalized it will throw an exception of type
      std::runtime_error. */
    template <class __containerType>
    explicit inline
    vctFrame4x4(const vctFrameBase<__containerType> & other):
        BaseType()
    {
        this->From(other);
    }

    /*! Constructor from a vctFrameBase.  This constructor uses the
      method FromNormalized or FromRaw based on the second argument
      (use VCT_NORMALIZE or VCT_DO_NOT_NORMALIZE). */
    template <class __containerType>
    inline
    vctFrame4x4(const vctFrameBase<__containerType> & other,
                bool normalizeInput):
        BaseType()
    {
        if (normalizeInput) {
            this->FromNormalized(other);
        } else {
            this->FromRaw(other);
        }
    }
};


// declared in vctFrame4x4ConstBase.h
template <class _containerType>
inline typename vctFrame4x4ConstBase<_containerType>::FrameValueType
vctFrame4x4ConstBase<_containerType>::Inverse(void) const {
    FrameValueType result;
    result.InverseOf(*this);
    return result;
}


// operators
template <class _containerType,
          vct::stride_type _stride, class _dataPtrType>
inline vctFixedSizeVector<typename _containerType::value_type, 3>
operator * (const vctFrame4x4ConstBase<_containerType> & frame,
            const vctFixedSizeConstVectorBase<3, _stride, typename _containerType::value_type, _dataPtrType> & vector) {
    vctFixedSizeVector<typename _containerType::value_type, 3> result;
    frame.ApplyTo(vector, result);
    return result;
}

template <class _containerType1, class _containerType2>
inline vctFrame4x4<typename _containerType1::value_type>
operator * (const vctFrame4x4ConstBase<_containerType1> & frame1,
            const vctFrame4x4ConstBase<_containerType2> & frame2) {
    vctFrame4x4<typename _containerType1::value_type> result;
    frame1.ApplyTo(frame2, result);
    return result;
}

template <class _containerType,
          vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _dataPtrType>
inline vctFixedSizeMatrix<typename _containerType::value_type, 3, _cols >
operator * (const vctFrame4x4ConstBase<_containerType> & frame,
            const vctFixedSizeConstMatrixBase<3, _cols, _rowStride, _colStride, typename _containerType::value_type, _dataPtrType> & matrix)
{
    vctFixedSizeMatrix<typename _containerType::value_type, 3, _cols > result;
    frame.ApplyTo(matrix, result);
    return result;
}


#endif  // _vctFrame4x4_h
