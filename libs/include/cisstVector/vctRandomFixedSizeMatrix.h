/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctRandomFixedSizeMatrix.h,v 1.3 2007/11/09 17:36:21 anton Exp $
  
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


#ifndef _vctRandomFixedSizeMatrix_h
#define _vctRandomFixedSizeMatrix_h

#include <cisstCommon/cmnRandomSequence.h>
#include <cisstCommon/cmnPortability.h>

#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctFixedSizeMatrix.h>
/*!
  \file
  \brief Definition of vctRandom for fixed size matrices.
*/

/*!
  \ingroup cisstVector
  
  Define the global function vctRandom to initialize a fixed size
  matrix with random elements.  The function takes a range from which
  to choose random elements.

  \note The function uses the global instance of cmnRandomSequence to
  extract random values.  As we have a vague plan to allow for
  multiple random sequence objects to coexist, these interfaces may
  need to be changed.
*/
template <unsigned int _rows, unsigned int _cols, int _rowStride, 
          int _colStride, class _elementType, class _dataPtrType>
void vctRandom(vctFixedSizeMatrixBase<_rows, _cols, _rowStride, _colStride,
               _elementType, _dataPtrType> & matrix,
               const _elementType min,
               const _elementType max) {
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


#endif  // _vctRandomFixedSizeMatrix_h

