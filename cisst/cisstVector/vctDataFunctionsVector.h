/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#pragma once

#ifndef _vctDataFunctionsVector_h
#define _vctDataFunctionsVector_h

#include <cisstCommon/cmnDataFunctions.h>

template <class _vectorType>
bool vctDataFindInVectorScalarIndex(const _vectorType & data, const size_t & index,
                                    size_t & elementIndex, size_t & inElementIndex)
{
    const size_t scalarNumber = cmnDataScalarNumber(data);
    if (cmnDataScalarNumberIsFixed(data.Element(0))) {
        const size_t scalarNumberPerElement = cmnDataScalarNumber(data.Element(0));
        if (scalarNumberPerElement == 0) {
            return false;
        }
        if (index < scalarNumber) {
            elementIndex = index / scalarNumberPerElement;
            inElementIndex = index % scalarNumberPerElement;
            return true;
        }
        return false;
    }

    bool indexFound = false;
    size_t scalarCounter = 0;
    size_t lastScalarInElement = 0;
    size_t firstScalarInElement = 0;
    size_t numberOfScalarsInElement = 0;
    elementIndex = 0;
    do {
        numberOfScalarsInElement = cmnDataScalarNumber(data.Element(elementIndex));
        firstScalarInElement = scalarCounter;
        lastScalarInElement = scalarCounter + numberOfScalarsInElement - 1;
        scalarCounter = lastScalarInElement + 1;
        elementIndex++;
        indexFound = ((index >= firstScalarInElement) && (index <= lastScalarInElement));
    } while ((!indexFound)
             && (elementIndex < data.size()));
    if (indexFound) {
        elementIndex--;
        inElementIndex = index - firstScalarInElement;
        return true;
    }
    return false;
}

#endif // _vctDataFunctionsVector_h
