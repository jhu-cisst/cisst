/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2005-01-13

  (C) Copyright 2005-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctRodriguezRotation3_h
#define _vctRodriguezRotation3_h

/*!
  \file
  \brief Declaration of vctRodriguezRotation3
 */

#include <cisstVector/vctRodriguezRotation3Base.h>
#include <cisstVector/vctExport.h>


/*!  \brief Define a rotation based on the rodriguez representation
  for a space of dimension 3.  This representation is based on a
  vector.  The direction of the vector determines the axis of rotation
  and its norm defines the amplitude of the rotation.

  \ingroup cisstVector

  This class is templated by the element type.

  \param _elementType The type of elements.

  \sa vctQuaternion
*/
template <class _elementType>
class vctRodriguezRotation3: public vctRodriguezRotation3Base<vctFixedSizeVector<_elementType, 3> >
{
public:
    /* no need to document, inherit doxygen documentation from vctFixedSizeVectorBase */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {DIMENSION = 3};
    typedef vctFixedSizeVector<value_type, 3> ContainerType;
    typedef vctRodriguezRotation3Base<ContainerType> BaseType;
    typedef vctRodriguezRotation3<value_type> ThisType;
    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<value_type> TypeTraits;

public:

    /*! Default constructor. Sets the rotation to (0, 0, 0). */
    inline vctRodriguezRotation3():
        BaseType(BaseType::Identity())
    {}

    inline vctRodriguezRotation3(const ThisType & other):
        BaseType(other)
    {}

    inline vctRodriguezRotation3(const BaseType & other):
        BaseType(other)
    {}


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


    /*! Constructor from a 3D vector.
      \param axis A vector of size 3, its norm represents the angle.
    */
    template <stride_type __stride, class __dataPtrType>
    explicit inline
    vctRodriguezRotation3(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & axis)
        CISST_THROW(std::runtime_error)
    {
        this->From(axis);
    }

    /*! Constructor from a elements representing a 3D vector.
      \param x, y, z  A vector of size 3, its norm represents the angle.
    */
    inline vctRodriguezRotation3(value_type x, value_type y, value_type z)
        CISST_THROW(std::runtime_error)
    {
        this->From(x, y, z);
    }

    /*! Constructor from a vctQuaternionRotation3. */
    template <class __containerType>
    explicit inline
    vctRodriguezRotation3(const vctQuaternionRotation3Base<__containerType> & quaternionRotation)
        CISST_THROW(std::runtime_error)
    {
        this->From(quaternionRotation);
    }

    /*! Constructor from a vctMatrixRotation3. */
    template <class __containerType>
    explicit inline
    vctRodriguezRotation3(const vctMatrixRotation3Base<__containerType> & matrixRotation)
        CISST_THROW(std::runtime_error)
    {
        this->From(matrixRotation);
    }

    /*! Constructor from a vctAxisAngleRotation3. */
    explicit inline
    vctRodriguezRotation3(const vctAxisAngleRotation3<value_type> & axisAngleRotation)
        CISST_THROW(std::runtime_error)
    {
        this->From(axisAngleRotation);
    }

    //@}



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
        it is important to obtain a result as "normalized" as
        possible.

      </ul>
    */
    //@{

    /*! Constructor from a 3D vector.
      \param axis A vector of size 3, its norm represents the angle.
      \param normalizeInput Normalize the input or convert as is (#VCT_NORMALIZE or #VCT_DO_NOT_NORMALIZE)
    */
    template <stride_type __stride, class __dataPtrType>
    inline vctRodriguezRotation3(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & axis,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(axis);
        } else {
            FromRaw(axis);
        }
    }

    /*! Constructor from 3 elements representing a 3D vector.
      \param x, y, z A vector of size 3, its norm represents the angle.
      \param normalizeInput Normalize the input or convert as is (#VCT_NORMALIZE or #VCT_DO_NOT_NORMALIZE)
    */
    inline vctRodriguezRotation3(value_type x, value_type y, value_type z,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(x, y, z);
        } else {
            this->FromRaw(x, y, z);
        }
    }

    /*! Constructor from a vctQuaternionRotation3. */
    template <class __containerType>
    inline vctRodriguezRotation3(const vctQuaternionRotation3Base<__containerType> & quaternionRotation,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(quaternionRotation);
        } else {
            this->FromRaw(quaternionRotation);
        }
    }

    /*! Constructor from a vctMatrixRotation3. */
    template <class __containerType>
    inline vctRodriguezRotation3(const vctMatrixRotation3Base<__containerType> & matrixRotation,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(matrixRotation);
        } else {
            this->FromRaw(matrixRotation);
        }
    }

    /*! Constructor from a vctAxisAngleRotation3. */
    inline vctRodriguezRotation3(const vctAxisAngleRotation3<value_type> & axisAngleRotation,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            this->FromNormalized(axisAngleRotation);
        } else {
            this->FromRaw(axisAngleRotation);
        }
    }

    //@}
};


#endif  // _vctRodriguezRotation3_h
