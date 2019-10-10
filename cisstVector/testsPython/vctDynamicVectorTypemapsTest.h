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
#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctRandom.h>

typedef vct::size_type size_type;

template <class _elementType>
class vctDynamicVectorTypemapsTest
{

protected:

    vctDynamicVector<_elementType> copy;

public:

    vctDynamicVectorTypemapsTest()
    {}

    void in_argout_vctDynamicVector_ref(vctDynamicVector<_elementType> & param, size_type sizeFactor) {
        copy.SetSize(param.size());
        copy.Assign(param);
        param += 1;

        if (sizeFactor != 0) {
            size_type size = param.size();
            size_type newsize = size * sizeFactor;
            param.resize(newsize);

            // TODO: is there a better way to do this?
            for (size_type i = size; i < newsize; i++) {
                param[i] = param[i % size];
            }
        }
    }

    void in_vctDynamicVectorRef(vctDynamicVectorRef<_elementType> param, size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.size());
        copy.Assign(param);
        param += 1;
    }

    void in_vctDynamicConstVectorRef(vctDynamicConstVectorRef<_elementType> param, size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.size());
        copy.Assign(param);
    }

    void in_argout_const_vctDynamicConstVectorRef_ref(const vctDynamicConstVectorRef<_elementType> & param, size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.size());
        copy.Assign(param);
    }

    void in_argout_const_vctDynamicVectorRef_ref(const vctDynamicVectorRef<_elementType> & param, size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.size());
        copy.Assign(param);
    }

    void in_vctDynamicVector(vctDynamicVector<_elementType> param, size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.size());
        copy.Assign(param);
    }

    void in_argout_const_vctDynamicVector_ref(const vctDynamicVector<_elementType> & param, size_type CMN_UNUSED(dummy)) {
        copy.SetSize(param.size());
        copy.Assign(param);
    }

    vctDynamicVector<_elementType> out_vctDynamicVector(size_type size) {
        copy.SetSize(size);
        vctRandom(copy, 0, 10);
        return copy;
    }

    vctDynamicVector<_elementType> &out_vctDynamicVector_ref(size_type size) {
        copy.SetSize(size);
        vctRandom(copy, 0, 10);
        return copy;
    }

    const vctDynamicVector<_elementType> &out_const_vctDynamicVector_ref(size_type size) {
        copy.SetSize(size);
        vctRandom(copy, 0, 10);
        return copy;
    }

    vctDynamicVectorRef<_elementType> out_vctDynamicVectorRef(size_type size) {
        copy.SetSize(size);
        vctRandom(copy, 0, 10);
        return vctDynamicVectorRef<_elementType>(copy);
    }

    vctDynamicConstVectorRef<_elementType> out_vctDynamicConstVectorRef(size_type size) {
        copy.SetSize(size);
        vctRandom(copy, 0, 10);
        return vctDynamicConstVectorRef<_elementType>(copy);
    }

    inline _elementType __getitem__(size_type index) const
    CISST_THROW (std::out_of_range) {
        return copy.at(index);
    }

    inline void __setitem__(size_type index, _elementType value)
    CISST_THROW (std::out_of_range) {
        copy.at(index) = value;
    }

    inline size_type size() const {
        return copy.size();
    }
};
