/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky, Anton Deguet
  Created on:	2003-09-30

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedSizeVectorTraits_h
#define _vctFixedSizeVectorTraits_h

/*!
  \file
  \brief Declaration of vctFixedSizeVectorTraits
 */

#include <cisstVector/vctFixedStrideVectorIterator.h>


/*!  \brief Define common container related types based on the
  properties of a fixed size container.

  The types are declared according to the STL requirements for the
  types declared by a container object.  This class is used as a trait
  to declare the actual containers.

  In addition to the STL required types, we declare a few more types for
  completeness.

*/
template <class _elementType, vct::size_type _size, vct::stride_type _stride>
class vctFixedSizeVectorTraits
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! Iterator, follows the STL naming convention. */
    typedef vctFixedStrideVectorIterator<_elementType, _stride> iterator;

    /*! Const iterator, follows the STL naming convention. */
    typedef vctFixedStrideVectorConstIterator<_elementType, _stride> const_iterator;

#ifndef SWIG // SWIG 1.3.21 doesn't like operations (- negation) in template declarations
    /*! Reverse iterator, follows the STL naming convention. */
    typedef vctFixedStrideVectorIterator<_elementType, -_stride> reverse_iterator;

    /*! Const reverse iterator, follows the STL naming convention. */
    typedef vctFixedStrideVectorConstIterator<_elementType, -_stride> const_reverse_iterator;
#endif // SWIG

    /*! Declared to enable inference of the size of the container. */
    enum {SIZE = _size};

    /*! Declared for completeness. */
    typedef value_type array[SIZE];

    /*! Declared for completeness. */
    typedef const value_type const_array[SIZE];

    /*! Declared to enable inference of the stride of the container. */
    enum {STRIDE = _stride};
};


#endif  // _vctFixedSizeVectorTraits_h

