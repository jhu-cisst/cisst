/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ofri Sadowsky
  Created on: 2006-02-15

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#pragma once
#ifndef _vctPrintf_h
#define _vctPrintf_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnPrintf.h>
#include <cisstVector/vctFixedSizeConstVectorBase.h>
#include <cisstVector/vctFixedSizeConstMatrixBase.h>
#include <cisstVector/vctDynamicConstVectorBase.h>
#include <cisstVector/vctDynamicConstMatrixBase.h>

template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
bool cmnTypePrintf(cmnPrintfParser & parser,
                   const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector)
{
    if (!parser.MatchOutputWithFormatChar(vector[0]) ) {
        return false;
    }

    vct::size_type counter;
    for (counter = 0; counter < _size; ++counter) {
        cmnTypePrintf(parser, vector[counter]);
        if (counter != (_size-1))
            parser.RawOutput(" ");
    }

    return true;
}

template <class _vectorOwnerType, class _elementType>
bool cmnTypePrintf(cmnPrintfParser & parser,
                   const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector)
{
    const vct::size_type vectorSize = vector.size();
    if (vectorSize < 1)
        return true;

    if (!parser.MatchOutputWithFormatChar(vector[0]) ) {
        return false;
    }

    vct::size_type counter;
    for (counter = 0; counter < vectorSize; ++counter) {
        const _elementType element = vector[counter];
        cmnTypePrintf(parser, element);
        if (counter != (vectorSize-1))
            parser.RawOutput(" ");
    }

    return true;
}


template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride,
class _elementType, class _dataPtrType>
    inline bool cmnTypePrintf(cmnPrintfParser & parser,
    const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride,
    _elementType, _dataPtrType> & matrix)
{
    if (!parser.MatchOutputWithFormatChar(matrix.Element(0,0)) ) {
        return false;
    }

    vct::size_type rowCounter, colCounter;
    for (rowCounter = 0; rowCounter < _rows; ++rowCounter) {
        for (colCounter = 0; colCounter < _cols; ++colCounter) {
            cmnTypePrintf(parser, matrix.Element(rowCounter, colCounter));
            if (colCounter != (_cols-1))
                parser.RawOutput(" ");
        }
        if (rowCounter != (_rows - 1))
            parser.RawOutput("\n");
    }

    return true;
}


template <class _matrixOwnerType, class _elementType>
bool cmnTypePrintf(cmnPrintfParser & parser,
                   const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix)
{
    const vct::size_type rows = matrix.rows();
    const vct::size_type cols = matrix.cols();

    if (matrix.size() < 1)
        return true;

    if (!parser.MatchOutputWithFormatChar(matrix.Element(0,0)) ) {
        return false;
    }

    vct::size_type rowCounter, colCounter;
    for (rowCounter = 0; rowCounter < rows; ++rowCounter) {
        for (colCounter = 0; colCounter < cols; ++colCounter) {
            cmnTypePrintf(parser, matrix.Element(rowCounter, colCounter));
            if (colCounter != (cols - 1))
                parser.RawOutput(" ");
        }
        if (rowCounter != (rows - 1))
            parser.RawOutput("\n");
    }

    return true;
}


#endif  // _vctPrintf_h

