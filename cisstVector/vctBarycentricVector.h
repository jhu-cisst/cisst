/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:	2003-12-02

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctBarycentricVector_h
#define _vctBarycentricVector_h

/*!
  \file
  \brief Defines vctBarycentricVector
*/

#include <cisstCommon/cmnTypeTraits.h>
#include <cisstVector/vctFixedSizeVector.h>

/*!
  \ingroup cisstVector

  class vctBarycentricVector is derived from vctFixedSizeVector and
  has the same basic functionality.  It contains a few additional
  functions meant to simplify dealing with vectors of barycentric
  coordinates.  For our general definition, a set of coordinates is
  barycentric if it sums to 1.  When a barycentric vector is used to
  parametrize a simplex geometrical object, all interior points have
  all-positive coordinates.  All member points (including the
  boundary) have non-negative coordinates.  A boundary point has one
  or more of its coordinates zero.  A vertex has one coordinate equal
  to 1, and the others zero.

  I have decided to derive this class from vctFixedSizeVector, which
  means that the operations can only be performed on locally owned
  memory (as opposed to a referenced vector).  The reason was that it
  would be too hard to regenerate this set of operations as a subclass
  for each vector type.  In addition, most of the operations here are
  const methods, and in the case of computing a new set of
  coordinates, they return it by value.  Therefore,
  vctBarycentricVector is safer to use than other library objects, but
  on the other hand the operations may be somewhat slower.

  I did not provide the full set of constructors as I have with
  vctFixedSizeVector, and therefore the use of a vctBarycentricVector
  should be more explicit in terms of type conversions than
  vctFixedSizeVector.

  Many operations on vctBarycentricVector require a tolerance
  argument, which is given with a default value.  The tolerance
  argument must be non-negative, or else the result is undefined. The
  current implementation does not test for it, but assumes it.  In
  most cases, we compare absolute values of numbers to the tolerance.
 */
template <class _elementType, vct::size_type _size>
class vctBarycentricVector : public vctFixedSizeVector<_elementType, _size>
{
public:
    typedef vctFixedSizeVector<_elementType, _size> BaseType;
    typedef vctBarycentricVector<_elementType, _size> ThisType;

    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef class cmnTypeTraits<value_type> TypeTraits;

    /*! Default constructor -- call base class def. ctor */
    vctBarycentricVector()
        : BaseType()
    {}

    /*! copy constructor for vctBarycentricVector object */
    vctBarycentricVector(const ThisType & other)
        : BaseType( static_cast<const BaseType &>(other) )
    {}

    /*! copy constructor for vctFixedSizeVector object */
    vctBarycentricVector(const BaseType & other)
        : BaseType(other)
    {}

    /*! copy constructor from a general vector */
    template <stride_type __stride, class __dataPtrType>
    vctBarycentricVector(const vctFixedSizeVectorBase<_size, __stride, _elementType, __dataPtrType> & other)
        : BaseType(other)
    {}

    vctBarycentricVector(_elementType value)
        : BaseType(value)
    {}

    vctBarycentricVector(_elementType element0, _elementType element1)
        : BaseType(element0, element1)
    {}

    vctBarycentricVector(_elementType element0, _elementType element1, _elementType element2)
        : BaseType(element0, element1, element2)
    {}

    vctBarycentricVector(_elementType element0, _elementType element1, _elementType element2,
                         _elementType element3)
        : BaseType(element0, element1, element2, element3)
    {}

    /* This one cannot call BaseType constructor as we cannot identify the
     unknown arguments */
    vctBarycentricVector(_elementType element0, _elementType element1, _elementType element2,
                         _elementType element3, _elementType element4, ...)
    {
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
        (*this)[3] = element3;
        (*this)[4] = element4;
        va_list nextArg;
        va_start(nextArg, element4);
        for (index_type i = 5; i < _size; ++i) {
            (*this)[i] = va_arg(nextArg, value_type);
        }
        va_end(nextArg);
    }

    /*! return true iff the sum of elements of this vector is
      equal to 1 up to the given tolerance. See class documentation. */
    bool IsBarycentric(const _elementType tolerance = TypeTraits::Tolerance() ) const
    {
        const _elementType diff = this->SumOfElements() - 1;
        return ( (-tolerance <= diff) && (diff <= tolerance) );
    }

    /*! return true iff all the coordinates are greater than or
      equal to the given tolerance. See class documentation. */
    bool IsInterior(const _elementType tolerance = TypeTraits::Tolerance()) const
    {
        if (!IsBarycentric())
            return false;
        const bool result = ((*this).GreaterOrEqual(tolerance));
        return result;
    }

    /*! return true iff all the coodinates are greater than
      -tolerance. See class documentation. */
    bool IsMember(const _elementType tolerance = TypeTraits::Tolerance()) const
    {
        if (!IsBarycentric())
            return false;
        const bool result = ((*this).GreaterOrEqual(-tolerance));
        return result;
    }

    /*! return true iff one of the coodinates is zero, up to
      the given tolerance.  This is not a membership test. */
    bool HasZero(const _elementType tolerance = TypeTraits::Tolerance()) const
    {
        return
            vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViSi< typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::Bound>::
            Unfold( (*this), tolerance );
    }

    /*! return (IsMember(tolerance) && HasZero(tolerance)); */
    bool IsBoundary(const _elementType tolerance = TypeTraits::Tolerance()) const
    {
        return (IsMember(tolerance) && HasZero(tolerance));
    }

    /*! return true iff a the vector is a member and a coordinate is equal to
      1 up to a given tolerance */
    bool IsVertex(const _elementType tolerance = TypeTraits::Tolerance()) const
    {
        if (!IsMember(tolerance))
            return false;
        const ThisType diff((*this) - _elementType(1));
        return (diff.HasZero(tolerance));
    }

    /*! return a barycentric scaled version of this vector, that is,
      a vector whose sum of elements is 1.  If the sum of this vector
      is zero up to the given tolerance, return a zero vector (which is
      not barycentric).
     */
    ThisType ScaleToBarycentric(const _elementType tolerance = TypeTraits::Tolerance()) const
    {
        _elementType scale = this->SumOfElements();
        if ( (-tolerance <= scale) && (scale <= tolerance) )
            return ThisType(0);
        ThisType result(*this);
        result /= scale;
        return result;
    }

};


#endif // _vctBarycentricVector_h

