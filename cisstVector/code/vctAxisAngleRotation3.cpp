/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2004-01-13

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctAxisAngleRotation3.h>
#include <cisstVector/vctQuaternionRotation3Base.h>
#include <cisstVector/vctMatrixRotation3Base.h>

template<>
const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3<double>::Identity()
{
    static const vctAxisAngleRotation3<double> result(vctFixedSizeVector<double, DIMENSION>(1.0, 0.0, 0.0), 0.0);
    return result;
}


template<>
const vctAxisAngleRotation3<float> &
vctAxisAngleRotation3<float>::Identity()
{
    static const vctAxisAngleRotation3<float> result(vctFixedSizeVector<float, DIMENSION>(1.0f, 0.0f, 0.0f), 0.0);
    return result;
}


template <class _elementType, class _containerType>
void
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<_elementType> & axisAngleRotation,
                             const vctQuaternionRotation3Base<_containerType> & quaternionRotation) {
    const _elementType r = quaternionRotation.R();
    axisAngleRotation.Angle() = acos(r) * 2.0;
    double oneMinusR2 = 1.0 - r * r;
    if (oneMinusR2 < cmnTypeTraits<_elementType>::Tolerance())
        oneMinusR2 = 0.0;
    _elementType sinAngle = _elementType(sqrt(oneMinusR2));
    if (vctUnaryOperations<_elementType>::AbsValue::Operate(sinAngle) > cmnTypeTraits<_elementType>::Tolerance()) {
        axisAngleRotation.Axis().X() = quaternionRotation.X() / sinAngle;
        axisAngleRotation.Axis().Y() = quaternionRotation.Y() / sinAngle;
        axisAngleRotation.Axis().Z() = quaternionRotation.Z() / sinAngle;
    } else {
        axisAngleRotation.Axis().X() = quaternionRotation.X();
        axisAngleRotation.Axis().Y() = quaternionRotation.Y();
        axisAngleRotation.Axis().Z() = quaternionRotation.Z();
    }
}


template <class _elementType, class _containerType>
void
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<_elementType> & axisAngleRotation,
                             const vctMatrixRotation3Base<_containerType> & matrixRotation) {

    typedef vctAxisAngleRotation3<_elementType> AxisAngleRotationType;
    typedef typename AxisAngleRotationType::NormType NormType;
    typedef typename AxisAngleRotationType::AngleType AngleType;

    const NormType normTolerance = cmnTypeTraits<NormType>::Tolerance();
    const NormType trace = matrixRotation.Element(0, 0) + matrixRotation.Element(1, 1) + matrixRotation.Element(2, 2); // 2 * cos(angle) + 1
    const NormType xSin = matrixRotation.Element(2, 1) - matrixRotation.Element(1, 2); // 2 * x * sin(angle)
    const NormType ySin = matrixRotation.Element(0, 2) - matrixRotation.Element(2, 0); // 2 * y * sin(angle)
    const NormType zSin = matrixRotation.Element(1, 0) - matrixRotation.Element(0, 1); // 2 * z * sin(angle)
    const NormType normSquare = xSin * xSin + ySin * ySin + zSin * zSin;
    NormType norm;
    if (normSquare < normTolerance) {
        norm = 0.0;
    } else {
        norm = sqrt(normSquare); // 2 * |sin(angle)|
    }

    // either 0 or PI
    if (norm == 0.0) {
        const NormType traceMinus3 = trace - NormType(3);
        // if the angle is 0, then cos(angle) = 1, and trace = 2*cos(angle) + 1 = 3
        if ( (traceMinus3 > -normTolerance) && (traceMinus3 < normTolerance) ) {
            axisAngleRotation.Angle() = _elementType(0);
            axisAngleRotation.Axis().Assign(_elementType(0), _elementType(0), _elementType(1));
            return;
        }
        // since norm is already 0, we are in the other case, i.e., angle-PI, but we just want
        // to assert that trace = -1
        CMN_ASSERT( (trace > (NormType(-1.0) - normTolerance)) && (trace < (NormType(-1.0) + normTolerance)) );
        // the diagonal is [k_x*k_x*v + c ,  k_y*k_y*v + c,  k_z*k_z*v + c]
        // c = -1 ;  v = (1 - c) = 2
        NormType xSquare = (matrixRotation.Element(0, 0) + 1.0) / 2.0;
        NormType ySquare = (matrixRotation.Element(1, 1) + 1.0) / 2.0;
        NormType zSquare = (matrixRotation.Element(2, 2) + 1.0) / 2.0;
        if (xSquare < normTolerance)
            xSquare = 0.0;
        if (ySquare < normTolerance)
            ySquare = 0.0;
        if (zSquare < normTolerance)
            zSquare = 0.0;
        NormType x = sqrt(xSquare);
        NormType y = sqrt(ySquare);
        NormType z = sqrt(zSquare);
        // we arbitrarily decide the k_x is positive, if it's zero then k_y is positive, and if both are zero, then k_z is positive
        if (x > 0.0) {
            if (matrixRotation.Element(1, 0) < 0.0) // Element(1,0) = k_x*k_y*v , where v=2, so we just need to check its sign
                y = -y;
            if (matrixRotation.Element(2, 0) < 0.0) // Element(2,0) = k_x*k_z*v
                z = -z;
        } else if (y > 0.0) {
            if (matrixRotation.Element(2, 1) < 0.0) // Element(2,1) = k_y*k_z*v
                z = -z;
        } else {
            z = 1.0; // x and y are zero, Z has to be one
        }
        axisAngleRotation.Axis().Assign(_elementType(x), _elementType(y), _elementType(z));
        axisAngleRotation.Angle() = _elementType(cmnPI);
        return;
    }

    const AngleType angle = atan2(norm / 2.0, (trace - 1.0) / 2.0);
    axisAngleRotation.Axis().Assign(_elementType(xSin), _elementType(ySin), _elementType(zSin));
    axisAngleRotation.Axis().NormalizedSelf();
    axisAngleRotation.Angle() = _elementType(angle);
}


template <class _elementType, class _containerType>
void
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<_elementType> & axisAngleRotation,
                             const vctRodriguezRotation3Base<_containerType> & rodriguezRotation) {

    typedef vctAxisAngleRotation3<_elementType> AxisAngleRotationType;
    typedef typename AxisAngleRotationType::value_type value_type;
    typedef typename AxisAngleRotationType::NormType NormType;
    typedef typename AxisAngleRotationType::AngleType AngleType;

    const NormType axisLength = rodriguezRotation.Norm();
    const value_type axisTolerance = cmnTypeTraits<value_type>::Tolerance();
    const AngleType angle = (axisLength < axisTolerance) ? 0.0 : axisLength;
    axisAngleRotation.Angle() = angle;
    if (angle == 0.0) {
        axisAngleRotation.Axis().Assign(value_type(1), value_type(0), value_type(0));
    } else {
        axisAngleRotation.Axis().Assign(rodriguezRotation.X() / static_cast<value_type>(axisLength),
                                        rodriguezRotation.Y() / static_cast<value_type>(axisLength),
                                        rodriguezRotation.Z() / static_cast<value_type>(axisLength));
    }
}


// force the instantiation of the templated classes
template class vctAxisAngleRotation3<double>;
template class vctAxisAngleRotation3<float>;


// force instantiation of helper functions
template void
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
template void
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<float> & axisAngleRotation,
                             const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation);

template void
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > & matrixRotation);
template void
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<float> & axisAngleRotation,
                             const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > & matrixRotation);

template void
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctMatrixRotation3Base<vctFixedSizeConstMatrixRef<double, 3, 3, 4, 1> > & matrixRotation);
template void
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<float> & axisAngleRotation,
                             const vctMatrixRotation3Base<vctFixedSizeConstMatrixRef<float, 3, 3, 4, 1> > & matrixRotation);

template void
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctMatrixRotation3Base<vctFixedSizeConstMatrixRef<double, 3, 3, 1, 4> > & matrixRotation);
template void
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<float> & axisAngleRotation,
                             const vctMatrixRotation3Base<vctFixedSizeConstMatrixRef<float, 3, 3, 1, 4> > & matrixRotation);

template void
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > & rodriguezRotation);
template void
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<float> & axisAngleRotation,
                             const vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > & rodriguezRotation);
