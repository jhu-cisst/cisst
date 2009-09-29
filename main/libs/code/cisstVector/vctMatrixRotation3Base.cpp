/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctMatrixRotation3Base.cpp 75 2009-02-24 16:47:20Z adeguet1 $

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


template <class _containerType>
vctMatrixRotation3Base<_containerType> &
vctMatrixRotation3Base<_containerType>::FromRaw(const vctAxisAngleRotation3<typename _containerType::value_type> & axisAngleRotation) {

    typedef vctAxisAngleRotation3<value_type> AxisAngleType;

    const typename AxisAngleType::AngleType angle = axisAngleRotation.Angle();
    const typename AxisAngleType::AxisType axis = axisAngleRotation.Axis();
    
    const AngleType sinAngle = AngleType(sin(angle));
    const AngleType cosAngle = AngleType(cos(angle));
    const AngleType CompCosAngle = 1 - cosAngle;
    
    // first column
    this->Element(0, 0) = value_type(axis[0] * axis[0] * CompCosAngle + cosAngle);
    this->Element(1, 0) = value_type(axis[0] * axis[1] * CompCosAngle + axis[2] * sinAngle);
    this->Element(2, 0) = value_type(axis[0] * axis[2] * CompCosAngle - axis[1] * sinAngle);
    
    // second column
    this->Element(0, 1) = value_type(axis[1] * axis[0] * CompCosAngle - axis[2] * sinAngle);
    this->Element(1, 1) = value_type(axis[1] * axis[1] * CompCosAngle + cosAngle);
    this->Element(2, 1) = value_type(axis[1] * axis[2] * CompCosAngle + axis[0] * sinAngle);
    
    // third column
    this->Element(0, 2) = value_type(axis[2] * axis[0] * CompCosAngle + axis[1] * sinAngle);
    this->Element(1, 2) = value_type(axis[2] * axis[1] * CompCosAngle - axis[0] * sinAngle);
    this->Element(2, 2) = value_type(axis[2] * axis[2] * CompCosAngle + cosAngle);
    
    return *this;
}




template <class _matrixType, class _quaternionType>
void
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<_matrixType> & matrixRotation,
                              const vctQuaternionRotation3Base<_quaternionType> & quaternionRotation) {

    typedef typename _matrixType::value_type value_type;

    value_type xx = quaternionRotation.X() * quaternionRotation.X();
    value_type xy = quaternionRotation.X() * quaternionRotation.Y();
    value_type xz = quaternionRotation.X() * quaternionRotation.Z();
    value_type xr = quaternionRotation.X() * quaternionRotation.R();
    value_type yy = quaternionRotation.Y() * quaternionRotation.Y();
    value_type yz = quaternionRotation.Y() * quaternionRotation.Z();
    value_type yr = quaternionRotation.Y() * quaternionRotation.R();
    value_type zz = quaternionRotation.Z() * quaternionRotation.Z();
    value_type zr = quaternionRotation.Z() * quaternionRotation.R();
    matrixRotation.Assign(1 - 2 * (yy + zz), 2 * (xy - zr),     2 * (xz + yr),
                          2 * (xy + zr),     1 - 2 * (xx + zz), 2 * (yz - xr),
                          2 * (xz - yr),     2 * (yz + xr),     1 - 2 * (xx + yy));
}



// force the instantiation of the templated classes
template class vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> >;
template class vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> >;
template class vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> >;
template class vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> >;

template class vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> >;
template class vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> >;
template class vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 1, 4> >;
template class vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 1, 4> >;


// force instantiation of helper functions
template void
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
template void
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
template void
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation);
template void
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation);

template void
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
template void
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 1, 4> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
template void
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation);
template void
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 1, 4> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation);

