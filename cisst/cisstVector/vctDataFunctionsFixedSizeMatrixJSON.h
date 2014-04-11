/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#pragma once

#ifndef _vctDataFunctionsFixedSizeMatrixJSON_h
#define _vctDataFunctionsFixedSizeMatrixJSON_h

#include <cisstCommon/cmnDataFunctionsJSON.h>
#include <cisstVector/vctFixedSizeMatrixBase.h>

#if CISST_HAS_JSON
template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
void cmnDataToJSON(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix,
                   Json::Value & jsonValue) {
    const size_t numberOfRows = matrix.rows();
    int jsonRowIndex = 0;
    for (size_t rowIndex = 0;
         rowIndex < numberOfRows;
         ++rowIndex, ++jsonRowIndex) {
        cmnDataToJSON(matrix.Row(rowIndex), jsonValue[jsonRowIndex]);
    }
}

template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
inline void cmnDataFromJSON(vctFixedSizeMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix,
                            const Json::Value & jsonValue)
    throw (std::runtime_error)
{
    // make sure both matrices have the same number of rows
    if (matrix.rows() != jsonValue.size()) {
        cmnThrow("cmnDataFromJSON: matrix number of rows don't match");
    }
    const size_t numberOfRows = matrix.rows();
    int jsonRowIndex = 0;
    for (size_t rowIndex = 0;
         rowIndex < numberOfRows;
         ++rowIndex, ++jsonRowIndex) {
        cmnDataFromJSON(matrix.Row(rowIndex), jsonValue[jsonRowIndex]);
    }
}
#endif // CISST_HAS_JSON

#endif // _vctDataFunctionsFixedSizeMatrixJSON_h
