/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2005-08-20

  (C) Copyright 2005-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstVector/vctMatrixRotation3Base.h>
#include <cisstVector/vctMatrixRotation3.h>

// instantiated for rotation with allocation row major
template <>
const vctMatrixRotation3ConstBase<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> >::RotationValueType &
vctMatrixRotation3ConstBase<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> >::Identity()
{
    static const RotationValueType result(ThisType::Eye());
    return result;
}

template <>
const vctMatrixRotation3ConstBase<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> >::RotationValueType &
vctMatrixRotation3ConstBase<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> >::Identity()
{
    static const RotationValueType result(ThisType::Eye());
    return result;
}

// instantiated for rotation with allocation col major
template <>
const vctMatrixRotation3ConstBase<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> >::RotationValueType &
vctMatrixRotation3ConstBase<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> >::Identity()
{
    static const RotationValueType result(ThisType::Eye());
    return result;
}

template <>
const vctMatrixRotation3ConstBase<vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> >::RotationValueType &
vctMatrixRotation3ConstBase<vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> >::Identity()
{
    static const RotationValueType result(ThisType::Eye());
    return result;
}

// instantiated for rotation with reference on compact row major 4x4 matrix
template <>
const vctMatrixRotation3ConstBase<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> >::RotationValueType &
vctMatrixRotation3ConstBase<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> >::Identity()
{
    return vctMatrixRotation3ConstBase<vctFixedSizeMatrix<double, 3, 3> >::Identity();
}

template <>
const vctMatrixRotation3ConstBase<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> >::RotationValueType &
vctMatrixRotation3ConstBase<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> >::Identity()
{
    return vctMatrixRotation3ConstBase<vctFixedSizeMatrix<float, 3, 3> >::Identity();
}

// instantiated for rotation with reference on compact col major 4x4 matrix
template <>
const vctMatrixRotation3ConstBase<vctFixedSizeMatrixRef<double, 3, 3, 1, 4> >::RotationValueType &
vctMatrixRotation3ConstBase<vctFixedSizeMatrixRef<double, 3, 3, 1, 4> >::Identity()
{
    return vctMatrixRotation3ConstBase<vctFixedSizeMatrix<double, 3, 3> >::Identity();
}

template <>
const vctMatrixRotation3ConstBase<vctFixedSizeMatrixRef<float, 3, 3, 1, 4> >::RotationValueType &
vctMatrixRotation3ConstBase<vctFixedSizeMatrixRef<float, 3, 3, 1, 4> >::Identity()
{
    return vctMatrixRotation3ConstBase<vctFixedSizeMatrix<float, 3, 3> >::Identity();
}


// force the instantiation of the templated classes
template class vctMatrixRotation3ConstBase<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> >;
template class vctMatrixRotation3ConstBase<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> >;
template class vctMatrixRotation3ConstBase<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> >;
template class vctMatrixRotation3ConstBase<vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> >;

template class vctMatrixRotation3ConstBase<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> >;
template class vctMatrixRotation3ConstBase<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> >;

