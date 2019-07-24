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

template <class _elementType, vct::size_type _size>
class vctFixedSizeVectorTypemapsTest
{

protected:

    vctFixedSizeVector<_elementType, _size> copy;

public:

    vctFixedSizeVectorTypemapsTest()
    {}

    void in_argout_vctFixedSizeVector_ref(vctFixedSizeVector<_elementType, _size> &param) {
        copy.Assign(param);
        param += 1;
    }

    vctFixedSizeVector<_elementType, _size> &out_vctFixedSizeVector_ref(void) {
        _elementType min = 0;
        _elementType max = 10;
        vctRandom(copy, min, max);
        return copy;
    }

    void in_vctFixedSizeVector(vctFixedSizeVector<_elementType, _size> param) {
        copy.Assign(param);
    }

    vctFixedSizeVector<_elementType, _size> out_vctFixedSizeVector(void) {
        _elementType min = 0;
        _elementType max = 10;
        vctRandom(copy, min, max);
        return copy;
    }

    void in_argout_const_vctFixedSizeVector_ref(const vctFixedSizeVector<_elementType, _size> &param) {
        copy.Assign(param);
    }

    const vctFixedSizeVector<_elementType, _size> &out_const_vctFixedSizeVector_ref(void) {
        _elementType min = 0;
        _elementType max = 10;
        vctRandom(copy, min, max);
        return copy;
    }

    inline _elementType __getitem__(vct::size_type index) const
    CISST_THROW (std::out_of_range) {
        return copy.at(index);
    }

    inline void __setitem__(vct::size_type index, _elementType value)
    CISST_THROW (std::out_of_range) {
        copy.at(index) = value;
    }

    inline vct::size_type size(void) const {
        return copy.size();
    }
};
