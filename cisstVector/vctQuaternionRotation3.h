/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2004-01-15

  (C) Copyright 2004-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctQuaternionRotation3_h
#define _vctQuaternionRotation3_h

/*!
  \file
  \brief Declaration of vctQuaternionRotation3
 */

#include <cisstVector/vctQuaternionRotation3Base.h>
#include <cisstVector/vctFixedSizeVector.h>

#include <cisstVector/vctExport.h>


/*!
  \brief Define a rotation quaternion for a space of dimension 3

  \ingroup cisstVector

  This class is templated by the element type.  It is derived from
  vctMatrixRotation2Base and uses a vctFixedSizeMatrix as underlying
  container.  It provides a more humain interface for programmers only
  interested in templating by _elementType.

  \param _elementType The type of elements of the quaternion.

  \sa vctQuaternionRotation3Base vctFixedSizeVector
*/
template <class _elementType>
class vctQuaternionRotation3: public vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> >
{
 public:
    /* no need to document, inherit doxygen documentation from vctFixedSizeVectorBase */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {SIZE = 4};
    enum {DIMENSION = 3};
    typedef vctFixedSizeVector<value_type, SIZE> ContainerType;
    typedef vctQuaternionRotation3Base<ContainerType> BaseType;
    typedef vctQuaternionRotation3<value_type> ThisType;
    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<value_type> TypeTraits;


    /* documented in base class */
    inline vctQuaternionRotation3():
        BaseType()
    {}

    inline vctQuaternionRotation3(const ThisType & quaternionRotation):
        BaseType(quaternionRotation)
    {}

    ThisType & operator = (const ThisType & quaternionRotation)
    {
        BaseType::operator = (quaternionRotation);
        return *this;
    }

    template <class __containerType>
    inline
    vctQuaternionRotation3(const vctQuaternionRotation3Base<__containerType> & other)
        CISST_THROW(std::runtime_error):
        BaseType(other)
    {}

    inline
    vctQuaternionRotation3(const value_type & x, const value_type & y, const value_type & z,
                           const value_type & r)
        CISST_THROW(std::runtime_error):
        BaseType(x, y, z, r)
    {}

    template <class __containerType>
    explicit inline
    vctQuaternionRotation3(const vctMatrixRotation3Base<__containerType> & matrixRotation)
        CISST_THROW(std::runtime_error):
        BaseType(matrixRotation)
    {}

    explicit inline
    vctQuaternionRotation3(const vctAxisAngleRotation3<value_type> & axisAngleRotation)
        CISST_THROW(std::runtime_error):
        BaseType(axisAngleRotation)
    {}

    template <class __containerType>
    explicit inline
    vctQuaternionRotation3(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation)
        CISST_THROW(std::runtime_error):
        BaseType(rodriguezRotation)
    {}

    inline
    vctQuaternionRotation3(const ThisType & quaternionRotation,
                           bool normalizeInput):
        BaseType(quaternionRotation, normalizeInput)
    {}

    template <class _containerType>
    inline
    vctQuaternionRotation3(const _containerType & vector4,
                           bool normalizeInput):
        BaseType(vector4, normalizeInput)
    {}

    inline
    vctQuaternionRotation3(const value_type & x, const value_type & y, const value_type & z,
                           const value_type & r,
                           bool normalizeInput):
        BaseType(x, y, z, r, normalizeInput)
    {}

    template <class __containerType>
    inline explicit
    vctQuaternionRotation3(const vctMatrixRotation3Base<__containerType> & matrixRotation,
                           bool normalizeInput):
        BaseType(matrixRotation, normalizeInput)
    {}

    inline
    vctQuaternionRotation3(const vctAxisAngleRotation3<value_type> & axisAngleRotation,
                           bool normalizeInput):
        BaseType(axisAngleRotation, normalizeInput)
    {}

    template <class __containerType>
    inline
    vctQuaternionRotation3(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation,
                           bool normalizeInput):
        BaseType(rodriguezRotation, normalizeInput)
    {}


    /*! The assignment from BaseType has to be redefined for this
      class (C++ restriction).  This operator uses the Assign() method
      inherited from the ContainerType.  This operator (as well as the
      Assign method) allows to set a rotation quaternion to whatever
      value without any further validity checking.  It is recommended
      to use it with caution. */
    template <class __containerType>
    inline ThisType & operator = (const vctQuaternionRotation3Base<__containerType> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

};


#endif  // _vctQuaternionRotation3_h
