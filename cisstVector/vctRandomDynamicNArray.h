/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2007-02-11

  (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctRandomDynamicNArray_h
#define _vctRandomDynamicNArray_h

/*!
  \file
  \brief Definition of vctRandom for dynamic nArrays.
*/

#include <cisstCommon/cmnRandomSequence.h>
#include <cisstCommon/cmnPortability.h>

#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctDynamicNArray.h>

/*!
  \ingroup cisstVector

  Define the global function vctRandom to initialize a dynamic
  nArray with random elements.  The function takes a range from which
  to choose random elements.

  \note The function uses the global instance of cmnRandomSequence to
  extract random values.  As we have a vague plan to allow for
  multiple random sequence objects to coexist, these interfaces may
  need to be changed.
*/

template <class _nArrayOwnerType, typename _elementType, vct::size_type _dimension>
void vctRandom(vctDynamicNArrayBase<_nArrayOwnerType, _elementType, _dimension> & nArray,
               const typename vctDynamicNArrayBase<_nArrayOwnerType, _elementType, _dimension>::value_type min,
               const typename vctDynamicNArrayBase<_nArrayOwnerType, _elementType, _dimension>::value_type max)
{
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    typedef typename vctDynamicNArrayBase<_nArrayOwnerType, _elementType, _dimension>::iterator iterator;
    iterator iter;
    const iterator end = nArray.end();
    for (iter = nArray.begin(); iter != end; iter++) {
        randomSequence.ExtractRandomValue(min, max, *iter);
    }
}


#endif  // _vctRandomDynamicNArray_h

