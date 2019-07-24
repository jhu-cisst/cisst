/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Daniel Li, Anton Deguet
  Created on: 2009-05-20

  (C) Copyright 2009-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/********************************
 PLACEHOLDER STRINGS TO LOOK FOR:

 TODO       todo
********************************/

#include <iostream>


template <class _elementType, vct::size_type _rows, vct::size_type _cols>
class vctFixedSizeMatrixTypemapsTest
{

protected:

    vctFixedSizeMatrix<_elementType, _rows, _cols> copy;

public:

    vctFixedSizeMatrixTypemapsTest()
    {}

    void in_vctFixedSizeMatrix(vctFixedSizeMatrix<_elementType, _rows, _cols> param) {
        copy.Assign(param);
    }

    vctFixedSizeMatrix<_elementType, _rows, _cols> out_vctFixedSizeMatrix(void) {
        _elementType min = 0;
        _elementType max = 10;
        vctRandom(copy, min, max);     // TODO: this is actually not random!
        return copy;
    }

    void in_argout_vctFixedSizeMatrix_ref(vctFixedSizeMatrix<_elementType, _rows, _cols> &param) {
        copy.Assign(param);
        param += 1;
    }

    vctFixedSizeMatrix<_elementType, _rows, _cols> &out_vctFixedSizeMatrix_ref(void) {
        _elementType min = 0;
        _elementType max = 0;
        vctRandom(copy, min, max);
        return copy;
    }

    void in_argout_const_vctFixedSizeMatrix_ref(const vctFixedSizeMatrix<_elementType, _rows, _cols> &param) {
        copy.Assign(param);
    }

    const vctFixedSizeMatrix<_elementType, _rows, _cols> &out_const_vctFixedSizeMatrix_ref(void) {
        _elementType min = 0;
        _elementType max = 0;
        vctRandom(copy, min, max);
        return copy;
    }

    inline _elementType GetItem(vct::size_type rowIndex, vct::size_type colIndex) const
    CISST_THROW (std::out_of_range) {
        return copy.at(rowIndex, colIndex);
    }

    inline void SetItem(vct::size_type rowIndex, vct::size_type colIndex, _elementType value)
    CISST_THROW (std::out_of_range) {
        copy.at(rowIndex, colIndex) = value;
    }

    inline vct::size_type rows(void) const {
        return copy.rows();
    }

    inline vct::size_type cols(void) const {
        return copy.cols();
    }

    inline vctFixedSizeVector<vct::size_type, 2> sizes(void) const {
        return copy.sizes();
    }
};
