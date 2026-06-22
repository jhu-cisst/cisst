/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2007-02-11

  (C) Copyright 2003-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctRandomFixedSizeVector_h
#define _vctRandomFixedSizeVector_h

/*!
  \file
  \brief Definition of vctRandom for fixed size vectors.
*/

#include <cisstCommon/cmnRandomSequence.h>
#include <cisstCommon/cmnPortability.h>

#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctFixedSizeVector.h>

/*!
  \ingroup cisstVector

  Define the global function vctRandom to initialize a fixed size
  vector with random elements.  The function takes a range from which
  to choose random elements.

  \note The function uses the global instance of cmnRandomSequence to
  extract random values.  As we have a vague plan to allow for
  multiple random sequence objects to coexist, these interfaces may
  need to be changed.
*/
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
void vctRandom(vctFixedSizeVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
               const _elementType min,
               const _elementType max) {
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    typedef vctFixedSizeVectorBase<_size, _stride, _elementType, _dataPtrType> VectorType;
    const typename VectorType::iterator end = vector.end();
    typename VectorType::iterator iter;
    for (iter = vector.begin(); iter != end; ++iter) {
        randomSequence.ExtractRandomValue(min, max,
                                          *iter);
    }
}

template <class _elementType, vct::size_type _size, vct::stride_type _stride>
void vctRandom(vctFixedSizeVectorRef<_elementType, _size, _stride> && vector,
               const _elementType min,
               const _elementType max) {
    typename vctFixedSizeVectorRef<_elementType, _size, _stride>::BaseType & base = vector;
    vctRandom(base, min, max);
}

#endif  // _vctRandomFixedSizeVector_h
