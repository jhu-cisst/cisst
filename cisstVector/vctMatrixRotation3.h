/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2004-01-12

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctMatrixRotation3_h
#define _vctMatrixRotation3_h

/*!
  \file
  \brief Declaration of vctMatrixRotation3
 */

#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrixRef.h>
#include <cisstVector/vctMatrixRotation3Base.h>

// Always include last
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
template <class _elementType, bool _rowMajor>
class vctMatrixRotation3: public vctMatrixRotation3Base<vctFixedSizeMatrix<_elementType, 3, 3, _rowMajor> >
{
 public:
    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {ROWS = 3, COLS = 3};
    enum {DIMENSION = 3};
    typedef vctFixedSizeMatrix<value_type, ROWS, COLS, _rowMajor> ContainerType;
    typedef vctMatrixRotation3Base<ContainerType> BaseType;
    typedef vctMatrixRotation3ConstBase<ContainerType> ConstBaseType;
    typedef vctMatrixRotation3<value_type, _rowMajor> ThisType;
    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<value_type> TypeTraits;

    /*! Default constructor. Sets the rotation matrix to identity. */
    inline vctMatrixRotation3(void):
        BaseType()
    {
        this->Assign(this->Identity());
    }

    inline vctMatrixRotation3(const ThisType & other):
        BaseType()
    {
        this->Assign(other);
    }

    inline vctMatrixRotation3(const BaseType & other):
        BaseType()
    {
        this->Assign(other);
    }

    inline vctMatrixRotation3(const ConstBaseType & other):
        BaseType()
    {
        this->Assign(other);
    }

    explicit inline vctMatrixRotation3(const vctDynamicMatrixRef<value_type> & other):
        BaseType()
    {
        this->Assign(other);
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


    /*!
      \name Constructors with normalization test.

      These constructors will check that the input is valid,
      i.e. normalized.  If the input is not normalized, an exception
      (of type \c std::runtime_error) will be thrown.  Each
      constructor uses the corresponding From() method based on the
      input type.

      \note See the cmnThrow() function if an \c abort is better than
      an exception for your application.
    */
    //@{


    /*! Constructor from 9 elements.

      The parameters are given row first so that the code remains
      human readable:

      \code
      vctMatrixRotation3<double> matrix( 0.0, 1.0, 0.0,
                                        -1.0, 0.0, 0.0,
                                         0.0, 0.0, 1.0);
      \endcode
    */
    inline vctMatrixRotation3(const value_type & element00, const value_type & element01, const value_type & element02,
                              const value_type & element10, const value_type & element11, const value_type & element12,
                              const value_type & element20, const value_type & element21, const value_type & element22)
        CISST_THROW(std::runtime_error)
    {
        this->From(element00, element01, element02,
                   element10, element11, element12,
                   element20, element21, element22);
    }

    /*!
      Constructor from 3 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <stride_type __stride1, class __dataPtrType1,
              stride_type __stride2, class __dataPtrType2,
              stride_type __stride3, class __dataPtrType3>
    inline vctMatrixRotation3(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1>& v1,
                              const vctFixedSizeConstVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2>& v2,
                              const vctFixedSizeConstVectorBase<DIMENSION, __stride3, value_type, __dataPtrType3>& v3,
                              bool vectorsAreColumns = true)
        CISST_THROW(std::runtime_error)
    {
        this->From(v1, v2, v3, vectorsAreColumns);
    }

    /*!
      Constructor from 3 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2,
              class __vectorOwnerType3>
    inline vctMatrixRotation3(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                              const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                              const vctDynamicConstVectorBase<__vectorOwnerType3, value_type>& v3,
                              bool vectorsAreColumns = true)
        CISST_THROW(std::runtime_error)
    {
        this->From(v1, v2, v3, vectorsAreColumns);
    }

    /*! Construction from a vctAxisAngleRotation3. */
    explicit inline vctMatrixRotation3(const vctAxisAngleRotation3<value_type> & axisAngleRotation)
        CISST_THROW(std::runtime_error)
    {
        this->From(axisAngleRotation);
    }

    /*! Constructor from a rotation quaternion. It is important to
      note that this constructor doesn't normalize the rotation
      quaternion but asserts that it is normalized (in debug mode
      only).  See also the method From(quaternion).

      \param quaternionRotation A unit quaternion
    */
    template <class __containerType>
    explicit inline vctMatrixRotation3(const vctQuaternionRotation3Base<__containerType> & quaternionRotation)
        CISST_THROW(std::runtime_error)
    {
        this->From(quaternionRotation);
    }

    /*! Constructor from a vctRodriguezRotation3. */
    template <class __containerType>
    explicit inline vctMatrixRotation3(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation)
        CISST_THROW(std::runtime_error)
    {
        this->From(rodriguezRotation);
    }

    /*! Constructor from a vctEulerRotation3. */
    template <vctEulerRotation3Order::OrderType __order>
    explicit inline vctMatrixRotation3(const vctEulerRotation3<__order> & eulerRotation)
        CISST_THROW(std::runtime_error)
    {
        this->From(eulerRotation);
    }

    /*!
      \name Constructors without normalization test

      These constructors will either assume that the input is
      normalized or normalize the input (a copy of it, if required)
      based on the last parameter provided.

      <ul>

      <li>If the normalization flag is set to #VCT_DO_NOT_NORMALIZE,
      the input is considered already normalized and the constructor
      will not perform any sanity check.  This can lead to numerical
      instabilities which have to be handled by the caller.

      <li>If the normalization flag is set to #VCT_NORMALIZE, the
      input will be normalized.  This option should be used whenever
      it is important to obtain a result as "normalized" as possible.

      </ul>
    */
    //@{


    /*! Constructor from 9 elements.

      The parameters are given row first so that the code remains
      human readable:

      \code
      vctMatrixRotation3<double> matrix( 0.0, 1.0, 0.0,
                                        -1.0, 0.0, 0.0,
                                         0.0, 0.0, 1.0);
      \endcode
    */
    inline vctMatrixRotation3(const value_type & element00, const value_type & element01, const value_type & element02,
                              const value_type & element10, const value_type & element11, const value_type & element12,
                              const value_type & element20, const value_type & element21, const value_type & element22,
                              bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(element00, element01, element02,
                                 element10, element11, element12,
                                 element20, element21, element22);
        } else {
            this->FromRaw(element00, element01, element02,
                          element10, element11, element12,
                          element20, element21, element22);
        }
    }

    /*!
      Constructor from 3 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <stride_type __stride1, class __dataPtrType1,
              stride_type __stride2, class __dataPtrType2,
              stride_type __stride3, class __dataPtrType3>
    inline vctMatrixRotation3(const vctFixedSizeConstVectorBase<DIMENSION, __stride1, value_type, __dataPtrType1>& v1,
                              const vctFixedSizeConstVectorBase<DIMENSION, __stride2, value_type, __dataPtrType2>& v2,
                              const vctFixedSizeConstVectorBase<DIMENSION, __stride3, value_type, __dataPtrType3>& v3,
                              bool vectorsAreColumns, bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(v1, v2, v3, vectorsAreColumns);
        } else {
            this->FromRaw(v1, v2, v3, vectorsAreColumns);
        }
    }

    /*!
      Constructor from 3 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2,
              class __vectorOwnerType3>
    inline vctMatrixRotation3(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                              const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                              const vctDynamicConstVectorBase<__vectorOwnerType3, value_type>& v3,
                              bool vectorsAreColumns, bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(v1, v2, v3, vectorsAreColumns);
        } else {
            this->FromRaw(v1, v2, v3, vectorsAreColumns);
        }
    }

   /*! Construction from a vctAxisAngleRotation3. */
    inline vctMatrixRotation3(const vctAxisAngleRotation3<value_type> & axisAngleRotation,
                              bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(axisAngleRotation);
        } else {
            this->FromRaw(axisAngleRotation);
        }
    }

    /*! Constructor from a rotation quaternion. It is important to
      note that this constructor doesn't normalize the rotation
      quaternion but asserts that it is normalized (in debug mode
      only).  See also the method From(quaternion).

      \param quaternionRotation A unit quaternion
      \param normalizeInput Normalize the input or convert as is (#VCT_NORMALIZE or #VCT_DO_NOT_NORMALIZE)
    */
    template <class __containerType>
    inline vctMatrixRotation3(const vctQuaternionRotation3Base<__containerType> & quaternionRotation,
                              bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(quaternionRotation);
        } else {
            this->FromRaw(quaternionRotation);
        }
    }

    /*! Constructor from a vctRodriguezRotation3. */
    template <class __containerType>
    inline vctMatrixRotation3(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation,
                              bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(rodriguezRotation);
        } else {
            this->FromRaw(rodriguezRotation);
        }
    }

    /*! Construction from a vctEulerRotation3. */
    template <vctEulerRotation3Order::OrderType __order>
    inline vctMatrixRotation3(const vctEulerRotation3<__order> & eulerRotation,
                              bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(eulerRotation);
        } else {
            this->FromRaw(eulerRotation);
        }
    }

    //@}

    /*! Initialize this rotation matrix with a 3x3 matrix.  This
      constructor only takes a matrix of the same element type.

      \note This constructor does not verify normalization.  It is
      introduced to allow using results of matrix operations and
      assign them to a rotation matrix.

      \note The constructor is declared explicit, to force the user to
      be aware of the conversion being made.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    explicit inline
    vctMatrixRotation3(const vctFixedSizeMatrixBase<ROWS, COLS, __rowStride, __colStride, value_type, __dataPtrType> & matrix)
    {
        this->Assign(matrix);
    }

};


// declared in vctMatrixRotation3ConstBase.h
template <class _containerType>
typename vctMatrixRotation3ConstBase<_containerType>::RotationValueType
vctMatrixRotation3ConstBase<_containerType>::Normalized(void) const {
    RotationValueType result(*this);
    result.NormalizedSelf();
    return result;
}

// declared in vctMatrixRotation3ConstBase.h
template <class _containerType>
typename vctMatrixRotation3ConstBase<_containerType>::RotationValueType
vctMatrixRotation3ConstBase<_containerType>::Inverse(void) const {
    RotationValueType result(*this);
    result.InverseSelf();
    return result;
}


#endif  // _vctMatrixRotation3_h
