/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2003-10-07

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctQuaternion_h
#define _vctQuaternion_h

/*!
  \file
  \brief Declaration of vctQuaternion
 */

#include <cisstVector/vctQuaternionBase.h>
#include <cisstVector/vctFixedSizeVector.h>


/*!
  \brief Define a quaternion container.

  \ingroup cisstVector

  This class is templated by the element type.  It is derived from
  vctQuaternionBase and uses a vctFixedSizeVector as underlying
  container.  It provides a more humain interface for programmers only
  interested in templating by _elementType.

  It is important to note that the class vctQuaternion is for
  any quaternion, i.e. it does not necessary represent a unit
  quaternion.

  \param _elementType The type of elements of the vector.

  \sa vctQuaternionBase vctFixedSizeVector
*/
template <class _elementType>
class vctQuaternion : public vctQuaternionBase<vctFixedSizeVector<_elementType, 4> >
{
public:
    /* no need to document, inherit doxygen documentation from base class */
    enum {SIZE = 4};
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctQuaternion<value_type> ThisType;
    typedef vctFixedSizeVector<value_type, SIZE> ContainerType;
    typedef vctQuaternionBase<ContainerType> BaseType;
    typedef cmnTypeTraits<value_type> TypeTraits;

    /*! Default constructor.  Does nothing. */
    inline vctQuaternion():
        BaseType()
    {}

    /*! Constructor from 4 elements.

      \param x The first imaginary component
      \param y The second imaginary component
      \param z The third imaginary component
      \param r The real component
    */
    inline vctQuaternion(const value_type & x,
                         const value_type & y,
                         const value_type & z,
                         const value_type & r):
        BaseType(x, y, z, r)
    {}
};


#endif  // _vctQuaternion_h

