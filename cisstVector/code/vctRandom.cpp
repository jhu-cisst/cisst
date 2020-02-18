/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2005-01-31

  (C) Copyright 2005-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnConstants.h>

#include <cisstVector/vctRandom.h>

#include <cisstVector/vctMatrixRotation3Base.h>
#include <cisstVector/vctQuaternionRotation3Base.h>
#include <cisstVector/vctAxisAngleRotation3.h>
#include <cisstVector/vctRodriguezRotation3.h>

#include <cisstVector/vctMatrixRotation2Base.h>
#include <cisstVector/vctAngleRotation2.h>


template <class _containerType>
void vctRandom(vctMatrixRotation3Base<_containerType> & matrixRotation) {
    typedef typename _containerType::value_type value_type;
    vctQuaternionRotation3Base<vctFixedSizeVector<value_type, 4> > quaternionRotation;
    vctRandom(quaternionRotation);
    matrixRotation.From(quaternionRotation);
}

template void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> > &);
template void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> > &);
template void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> > &);
template void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> > &);
template void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> > &);
template void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 1, 4> > &);
template void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> > &);
template void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 1, 4> > &);


template <class _containerType>
void vctRandom(vctQuaternionRotation3Base<_containerType> & quaternionRotation) {
    typedef typename _containerType::value_type value_type;
    vctRandom(quaternionRotation, (value_type) -1.0, (value_type) 1.0);
    quaternionRotation.NormalizedSelf();
}

template void vctRandom(vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > &);
template void vctRandom(vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > &);



template <class _elementType>
void vctRandom(vctAxisAngleRotation3<_elementType> & axisAngleRotation) {
    typedef _elementType value_type;
    vctQuaternionRotation3Base<vctFixedSizeVector<value_type, 4> > quaternionRotation;
    vctRandom(quaternionRotation);
    axisAngleRotation.FromRaw(quaternionRotation);
}

template void vctRandom(vctAxisAngleRotation3<double> &);
template void vctRandom(vctAxisAngleRotation3<float> &);



template <class _containerType>
void vctRandom(vctRodriguezRotation3Base<_containerType> & rodriguezRotation) {
    typedef typename _containerType::value_type value_type;
    vctAxisAngleRotation3<value_type> axisAngleRotation;
    vctRandom(axisAngleRotation);
    rodriguezRotation.FromRaw(axisAngleRotation);
}

template void vctRandom(vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > &);
template void vctRandom(vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > &);



template <vctEulerRotation3Order::OrderType _order>
void vctRandom(vctEulerRotation3<_order> & eulerRotation) {
    vctRandom(eulerRotation.GetAngles(), -cmnPI, cmnPI);
}

template void vctRandom(vctEulerRotation3<vctEulerRotation3Order::ZYZ> &);
template void vctRandom(vctEulerRotation3<vctEulerRotation3Order::ZYX> &);
template void vctRandom(vctEulerRotation3<vctEulerRotation3Order::ZXZ> &);
template void vctRandom(vctEulerRotation3<vctEulerRotation3Order::YZX> &);



template <class _containerType>
void vctRandom(vctMatrixRotation2Base<_containerType> & matrixRotation) {
    vctAngleRotation2 angleRotation;
    vctRandom(angleRotation);
    matrixRotation.From(angleRotation);
}

template void vctRandom(vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > &);
template void vctRandom(vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > &);



void vctRandom(vctAngleRotation2 & angleRotation) {
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(0.0, 2 * cmnPI,
                                      angleRotation.Angle());
}
