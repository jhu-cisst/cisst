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
#include <cisstVector/vctDynamicNArray.h>

template <class _elementType, vct::size_type _dimension>
class vctDynamicNArrayTypemapsTest
{

public:
    typedef vctDynamicNArray<_elementType, _dimension> ArrayType;

protected:
    ArrayType copy;

public:

    vctDynamicNArrayTypemapsTest()
    {}

    void in_argout_vctDynamicNArray_ref(vctDynamicNArray<_elementType, _dimension> & param, vct::size_type sizeFactor) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
        param += 1;

        if (sizeFactor != 0) {
            const vctFixedSizeVector<vct::size_type, _dimension> sizesOld(param.sizes());
            // const vct::size_type sizeOld = sizesOld.ProductOfElements();
            const vctFixedSizeVector<vct::size_type, _dimension> sizesNew(sizesOld * sizeFactor);
            const vct::size_type sizeNew = sizesNew.ProductOfElements();
            param.SetSize(sizesNew);

            // Fill all elements with a non-zero value
            for (vct::size_type i = 0; i < sizeNew; i++) {
                param.at(i) = 17;       // TODO: modify this so it only fills the NEW elements, not ALL elements, with 17
            }
        }
    }

    void in_vctDynamicNArrayRef(vctDynamicNArrayRef<_elementType, _dimension> param, vct::size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
        param += 1;
    }

    void in_vctDynamicConstNArrayRef(vctDynamicConstNArrayRef<_elementType, _dimension> param, vct::size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
    }

    void in_argout_const_vctDynamicConstNArrayRef_ref(const vctDynamicConstNArrayRef<_elementType, _dimension> & param, vct::size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
    }

    void in_argout_const_vctDynamicNArrayRef_ref(const vctDynamicNArrayRef<_elementType, _dimension> & param, vct::size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
    }

    void in_vctDynamicNArray(vctDynamicNArray<_elementType, _dimension> param, vct::size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
    }

    void in_argout_const_vctDynamicNArray_ref(const vctDynamicNArray<_elementType, _dimension> & param, vct::size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
    }

    vctDynamicNArray<_elementType, _dimension> out_vctDynamicNArray(vctFixedSizeVector<vct::size_type, _dimension> sizes) {
        copy.SetSize(sizes);
        vctRandom(copy, 0, 10);     // TODO: this is actually not random!
        return copy;
    }

    vctDynamicNArray<_elementType, _dimension> & out_vctDynamicNArray_ref(vctFixedSizeVector<vct::size_type, _dimension> sizes) {
        copy.SetSize(sizes);
        vctRandom(copy, 0, 10);
        return copy;
    }

    const vctDynamicNArray<_elementType, _dimension> & out_const_vctDynamicNArray_ref(vctFixedSizeVector<vct::size_type, _dimension> sizes) {
        copy.SetSize(sizes);
        vctRandom(copy, 0, 10);
        return copy;
    }

    vctDynamicNArrayRef<_elementType, _dimension> out_vctDynamicNArrayRef(vctFixedSizeVector<vct::size_type, _dimension> sizes) {
        copy.SetSize(sizes);
        vctRandom(copy, 0, 10);
        return copy;
    }

    vctDynamicConstNArrayRef<_elementType, _dimension> out_vctDynamicConstNArrayRef(vctFixedSizeVector<vct::size_type, _dimension> sizes) {
        copy.SetSize(sizes);
        vctRandom(copy, 0, 10);
        return copy;
    }

    inline vct::size_type Dim(void) const {
        return _dimension;
    }

    inline _elementType GetItem(const vct::size_type metaIndex) const
    CISST_THROW (std::out_of_range) {
        return copy.at(metaIndex);
    }

    inline void SetItem(const vct::size_type metaIndex, _elementType value)
    CISST_THROW (std::out_of_range) {
        copy.at(metaIndex) = value;
    }

    inline void sizes(vctFixedSizeVector<vct::size_type, _dimension> & shape) const {
        shape.Assign(copy.sizes());
    }

    inline static int sizeOfSizes(void) {
        return sizeof(vct::size_type);
    }
};
