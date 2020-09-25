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

typedef vct::size_type size_type;

template <class _elementType>
class vctDynamicMatrixTypemapsTest
{

protected:

    vctDynamicMatrix<_elementType> copy;

public:

    vctDynamicMatrixTypemapsTest()
    {}

    void in_argout_vctDynamicMatrix_ref(vctDynamicMatrix<_elementType> & param, size_type sizeFactor) {
        copy.SetSize(param.sizes(), param.StorageOrder());
        copy.Assign(param);
        param += 1;

        if (sizeFactor != 0) {
            size_type rowsOld = param.rows();
            size_type colsOld = param.cols();
            size_type rowsNew = rowsOld * sizeFactor;
            size_type colsNew = colsOld * sizeFactor;
            param.resize(rowsNew, colsNew);

            // TODO: is there a better way to do this?
            for (size_type r = 0; r < rowsNew; r++) {
                for (size_type c = 0; c < colsNew; c++) {
                    param.at(r, c) = param.at(r % rowsOld, c % colsOld);
                }
            }
        }
    }

    void in_vctDynamicMatrixRef(vctDynamicMatrixRef<_elementType> param, size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.sizes(), param.StorageOrder());
        copy.Assign(param);
        param += 1;
    }

    void in_vctDynamicConstMatrixRef(vctDynamicConstMatrixRef<_elementType> param, size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.sizes(), param.StorageOrder());
        copy.Assign(param);
    }

    void in_argout_const_vctDynamicConstMatrixRef_ref(const vctDynamicConstMatrixRef<_elementType> & param, size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.sizes(), param.StorageOrder());
        copy.Assign(param);
    }

    void in_argout_const_vctDynamicMatrixRef_ref(const vctDynamicMatrixRef<_elementType> & param, size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.sizes(), param.StorageOrder());
        copy.Assign(param);
    }

    void in_vctDynamicMatrix(vctDynamicMatrix<_elementType> param, size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.sizes(), param.StorageOrder());
        copy.Assign(param);
    }

    void in_argout_const_vctDynamicMatrix_ref(const vctDynamicMatrix<_elementType> & param, size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.sizes(), param.StorageOrder());
        copy.Assign(param);
    }

    vctDynamicMatrix<_elementType> out_vctDynamicMatrix(size_type rows, size_type cols) {
        copy.SetSize(rows, cols);
        vctRandom(copy, 0, 10);     // TODO: this is actually not random!
        return copy;
    }

    vctDynamicMatrix<_elementType> &out_vctDynamicMatrix_ref(size_type rows, size_type cols) {
        copy.SetSize(rows, cols);
        vctRandom(copy, 0, 10);
        return copy;
    }

    const vctDynamicMatrix<_elementType> &out_const_vctDynamicMatrix_ref(size_type rows, size_type cols) {
        copy.SetSize(rows, cols);
        vctRandom(copy, 0, 10);
        return copy;
    }

    vctDynamicMatrixRef<_elementType> out_vctDynamicMatrixRef(size_type rows, size_type cols) {
        copy.SetSize(rows, cols);
        vctRandom(copy, 0, 10);
        return copy;
    }

    vctDynamicConstMatrixRef<_elementType> out_vctDynamicConstMatrixRef(size_type rows, size_type cols) {
        copy.SetSize(rows, cols);
        vctRandom(copy, 0, 10);
        return copy;
    }

    inline _elementType GetItem(size_type rowIndex, size_type colIndex) const
    CISST_THROW (std::out_of_range) {
        return copy.at(rowIndex, colIndex);
    }

    inline void SetItem(size_type rowIndex, size_type colIndex, _elementType value)
    CISST_THROW (std::out_of_range) {
        copy.at(rowIndex, colIndex) = value;
    }

    inline size_type rows() const {
        return copy.rows();
    }

    inline size_type cols() const {
        return copy.cols();
    }

    inline char StorageOrder(void) const {
        return copy.StorageOrder() ? 'C' : 'F';
    }
};
