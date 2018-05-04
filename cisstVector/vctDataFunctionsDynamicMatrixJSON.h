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

#ifndef _vctDataFunctionsDynamicMatrixJSON_h
#define _vctDataFunctionsDynamicMatrixJSON_h

#include <cisstCommon/cmnDataFunctionsJSON.h>
#include <cisstVector/vctDynamicMatrixBase.h>
#include <cisstVector/vctDataFunctionsDynamicVectorJSON.h>

#if CISST_HAS_JSON
template <class _elementType>
class cmnDataJSON<vctDynamicMatrix<_elementType> >
{
public:
    typedef vctDynamicMatrix<_elementType> DataType;
    typedef vctDynamicConstVectorRef<_elementType> RowRefType;
    static void SerializeText(const DataType & matrix,
                              Json::Value & jsonValue) {
        const size_t numberOfRows = matrix.rows();
        int jsonRowIndex = 0;
        typedef typename RowRefType::const_iterator const_row_iterator;
        for (size_t rowIndex = 0;
             rowIndex < numberOfRows;
             ++rowIndex, ++jsonRowIndex) {
            const const_row_iterator end = matrix.Row(rowIndex).end();
            const_row_iterator iter;
            int colIndex = 0;
            for (iter = matrix.Row(rowIndex).begin();
                 iter != end;
                 ++colIndex, ++iter) {
                cmnDataJSON<_elementType>::SerializeText(*iter, jsonValue[static_cast<int>(rowIndex)][colIndex]);
            }
        }
    }

    static void DeSerializeText(DataType & matrix,
                                const Json::Value & jsonValue)
        CISST_THROW(std::runtime_error)
    {
        // get the matrix size from JSON and resize
        const size_t numberOfRows = jsonValue.size();
        size_t numberOfColumns;
        if (numberOfRows > 0) {
            // get number of columns from first element
            numberOfColumns = jsonValue[0].size();
        } else {
            numberOfColumns = 0;
        }
        // empty matrix
        if ((numberOfRows == 0) || (numberOfColumns == 0)) {
            matrix.SetSize(0, 0);
            return;
        }
        matrix.SetSize(numberOfRows, numberOfColumns);
        int jsonRowIndex = 0;
        typedef vctDynamicVector<_elementType> RowValueType;
        RowValueType temporaryRow;
        for (size_t rowIndex = 0;
             rowIndex < numberOfRows;
             ++rowIndex, ++jsonRowIndex) {
            // deserialize to different vector
            cmnDataJSON<RowValueType>::DeSerializeText(temporaryRow, jsonValue[jsonRowIndex]);
            // make sure both rows have same size
            if (numberOfColumns != jsonValue[jsonRowIndex].size()) {
                cmnThrow("cmnDataJSON<vctDynamicMatrix>::DeSerializeText: row sizes don't match");
            }
            matrix.Row(rowIndex).Assign(temporaryRow);
        }
    }
};

#endif // CISST_HAS_JSON

#endif // _vctDataFunctionsDynamicMatrixJSON_h
