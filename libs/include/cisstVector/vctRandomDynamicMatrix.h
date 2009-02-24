/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):	Anton Deguet
  Created on:	2007-02-11

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _vctRandomDynamicMatrix_h
#define _vctRandomDynamicMatrix_h

#include <cisstCommon/cmnRandomSequence.h>
#include <cisstCommon/cmnPortability.h>

#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctDynamicMatrix.h>
/*!
  \file
  \brief Definition of vctRandom for dynamic matrices.
*/

/*!
  \ingroup cisstVector

  Define the global function vctRandom to initialize a dynamic
  matrix with random elements.  The function takes a range from which
  to choose random elements.

  \note The function uses the global instance of cmnRandomSequence to
  extract random values.  As we have a vague plan to allow for
  multiple random sequence objects to coexist, these interfaces may
  need to be changed.
*/
template <class _matrixOwnerType, typename _elementType>
void vctRandom(vctDynamicMatrixBase<_matrixOwnerType, _elementType> & matrix,
               const typename vctDynamicMatrixBase<_matrixOwnerType, _elementType>::value_type min,
               const typename vctDynamicMatrixBase<_matrixOwnerType, _elementType>::value_type max)
{
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    const unsigned int rows = matrix.rows();
    const unsigned int cols = matrix.cols();
    unsigned int rowIndex, colIndex;
    for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
        for (colIndex = 0; colIndex < cols; ++colIndex) {
            randomSequence.ExtractRandomValue(min, max,
                                              matrix.Element(rowIndex, colIndex));    
        }
    }
}


#endif  // _vctRandomDynamicMatrix_h

