/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2004-11-11

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctContainerTraits_h
#define _vctContainerTraits_h

/*!
  \file
  \brief Basic traits for the cisstVector containers
  \ingroup cisstVector
 */

#include <cstddef>

namespace vct
{
    typedef size_t size_type;
    typedef size_t index_type;
    typedef ptrdiff_t stride_type;
    typedef ptrdiff_t difference_type;
}


/*! Macro used to define multiple types based on the type of elements.
  This will define size_type, index_type, difference_type,
  stride_type, value_type, reference, const_reference, pointer,
  const_pointer, NormType (double) and AngleType (double).  Most of
  these types are introduced and named for STL compatibility.

  \param type Type of element, e.g. double, float, char.
 */
#define VCT_CONTAINER_TRAITS_TYPEDEFS(type) \
    typedef vct::size_type size_type; \
    typedef vct::index_type index_type; \
    typedef vct::difference_type difference_type; \
    typedef vct::stride_type stride_type; \
    typedef type value_type; \
    typedef value_type & reference; \
    typedef const value_type & const_reference; \
    typedef value_type * pointer; \
    typedef const value_type * const_pointer; \
    typedef double NormType; \
    typedef double AngleType


/*! Macro used to define nArray-specific types based on the type of
  elements.  This will define nsize_type, nstride_type, and
  dimension_type.
 */
#define VCT_NARRAY_TRAITS_TYPEDEFS(dimension) \
    typedef vctFixedSizeVector<size_type, dimension> nsize_type; \
    typedef vctFixedSizeVector<stride_type, dimension> nstride_type; \
    typedef vctFixedSizeVector<index_type, dimension> nindex_type; \
    typedef size_type dimension_type; \
    typedef vctFixedSizeVector<dimension_type, dimension> ndimension_type


#endif  // _vctContainerTraits_h

