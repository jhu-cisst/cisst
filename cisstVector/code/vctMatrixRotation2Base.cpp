/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2005-12-01

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstVector/vctMatrixRotation2Base.h>


template <>
const vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > &
vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> >::Identity()
{
    static const ThisType result(ThisType::Eye());
    return result;
}


template <>
const vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > &
vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> >::Identity()
{
    static const ThisType result(ThisType::Eye());
    return result;
}



template <class _containerType>
vctMatrixRotation2Base<_containerType> &
vctMatrixRotation2Base<_containerType>::FromRaw(const vctAngleRotation2 & angleRotation) {
    typedef typename _containerType::value_type value_type;
    const typename vctAngleRotation2::AngleType angle = angleRotation.Angle();
    const value_type sinAngle = value_type(sin(angle));
    const value_type cosAngle = value_type(cos(angle));
    // first column
    this->Element(0, 0) = cosAngle;
    this->Element(1, 0) = -sinAngle;
    // second column
    this->Element(0, 1) = sinAngle;
    this->Element(1, 1) = cosAngle;
    return *this;
}



// specialize for fixed size matrices
template <>
vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > &
vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> >::NormalizedSelf(void) {
    this->FromRaw(vctAngleRotation2(*this, VCT_DO_NOT_NORMALIZE));
    return *this;
}

template <>
vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > &
vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> >::NormalizedSelf(void) {
    this->FromRaw(vctAngleRotation2(*this, VCT_DO_NOT_NORMALIZE));
    return *this;
}


// force the instantiation of the templated classes
template class vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> >;
template class vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> >;

