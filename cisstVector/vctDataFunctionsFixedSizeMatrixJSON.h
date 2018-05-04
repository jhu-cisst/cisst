/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012-2018 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <cisstVector/vctDataFunctionsFixedSizeVectorJSON.h>

#if CISST_HAS_JSON
template <class _elementType, vct::size_type _rows, vct::size_type _cols>
class cmnDataJSON<vctFixedSizeMatrix<_elementType, _rows, _cols> >
{
public:
    typedef vctFixedSizeMatrix<_elementType, _rows, _cols> DataType;

    static void SerializeText(const DataType matrix, Json::Value & jsonValue)
    {
        const size_t numberOfRows = matrix.rows();
        int jsonRowIndex = 0;
        for (size_t rowIndex = 0;
             rowIndex < numberOfRows;
             ++rowIndex, ++jsonRowIndex) {
            cmnDataJSON<typename DataType::RowValueType>::SerializeText(matrix.Row(rowIndex), jsonValue[jsonRowIndex]);
        }
    }

    static void DeSerializeText(DataType & matrix, const Json::Value & jsonValue)
        CISST_THROW(std::runtime_error)
    {
        // make sure both matrices have the same number of rows
        if (matrix.rows() != jsonValue.size()) {
            cmnThrow("cmnDataJSON<vctFixedSizeMatrix>::DeSerializeText: matrix number of rows don't match");
        }
        const size_t numberOfRows = matrix.rows();
        int jsonRowIndex = 0;
        typename DataType::RowValueType temporaryRow;
        for (size_t rowIndex = 0;
             rowIndex < numberOfRows;
             ++rowIndex, ++jsonRowIndex) {
            cmnDataJSON<typename DataType::RowValueType>::DeSerializeText(temporaryRow, jsonValue[jsonRowIndex]);
            matrix.Row(rowIndex).Assign(temporaryRow);
        }
    }
};

#endif // CISST_HAS_JSON

#endif // _vctDataFunctionsFixedSizeMatrixJSON_h
