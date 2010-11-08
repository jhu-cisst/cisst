/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):	Anton Deguet
  Created on:	2005-10-25

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctRodriguezRotation3Base.h>
#include <cisstVector/vctQuaternionRotation3Base.h>
#include <cisstVector/vctMatrixRotation3Base.h>


template<>
const vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > &
vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> >::Identity()
{
    static const ThisType result(0.0, 0.0, 0.0);
    return result;
}


template<>
const vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > &
vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> >::Identity()
{
    static const ThisType result(0.0f, 0.0f, 0.0f);
    return result;
}



template <class _rodriguezType, class _quaternionType>
void
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<_rodriguezType> & rodriguezRotation,
                                 const vctQuaternionRotation3Base<_quaternionType> & quaternionRotation)
{
    typedef typename _rodriguezType::value_type value_type;
    typedef typename _rodriguezType::AngleType AngleType;
    typedef typename _rodriguezType::NormType NormType;
    typedef typename _rodriguezType::TypeTraits TypeTraits;

    const NormType r = quaternionRotation.R();
    const AngleType angle = acos(r) * 2;
    AngleType oneMinusR2 = 1.0 - r * r;
    if (oneMinusR2 < TypeTraits::Tolerance())
        oneMinusR2 = 0.0;
    AngleType sinAngle = sqrt(oneMinusR2);
    if (vctUnaryOperations<AngleType>::AbsValue::Operate(sinAngle) > TypeTraits::Tolerance()) {
        rodriguezRotation.X() = (value_type)(quaternionRotation.X() / sinAngle);
        rodriguezRotation.Y() = (value_type)(quaternionRotation.Y() / sinAngle);
        rodriguezRotation.Z() = (value_type)(quaternionRotation.Z() / sinAngle);
    } else {
        rodriguezRotation.X() = quaternionRotation.X();
        rodriguezRotation.Y() = quaternionRotation.Y();
        rodriguezRotation.Z() = quaternionRotation.Z();
    }
    rodriguezRotation.Multiply((value_type) angle);
}


template <class _rodriguezType, class _matrixType>
void
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<_rodriguezType> & rodriguezRotation,
                                 const vctMatrixRotation3Base<_matrixType> & matrixRotation)
{
    typedef typename _rodriguezType::value_type value_type;
    typedef typename _rodriguezType::AngleType AngleType;
    typedef typename _rodriguezType::NormType NormType;
    typedef typename _rodriguezType::TypeTraits TypeTraits;

    const NormType normTolerance = TypeTraits::Tolerance();
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
            rodriguezRotation.SetAll(value_type(0));
            return;
        }
        // since norm is already 0, we are in the other case, i.e., angle-PI, but we just want
        // to assert that trace = -1
        assert( (trace > (NormType(-1) - normTolerance)) && (trace < (NormType(-1) + normTolerance)) );
        // the diagonal is [k_x*k_x*v + c ,  k_y*k_y*v + c,  k_z*k_z*v + c]
        // c = -1 ;  v = (1 - c) = 2
        NormType xSquare = (matrixRotation.Element(0, 0) + 1) / 2;
        NormType ySquare = (matrixRotation.Element(1, 1) + 1) / 2;
        NormType zSquare = (matrixRotation.Element(2, 2) + 1) / 2;
        if (xSquare < normTolerance)
            xSquare = 0;
        if (ySquare < normTolerance)
            ySquare = 0;
        if (zSquare < normTolerance)
            zSquare = 0;
        NormType x = sqrt(xSquare);
        NormType y = sqrt(ySquare);
        NormType z = sqrt(zSquare);
        // we arbitrarily decide the k_x is positive, if it's zero then k_y is positive, and if both are zero, then k_z is positive
        if (x > 0) {
            if (matrixRotation.Element(1, 0) < 0) // Element(1,0) = k_x*k_y*v , where v=2, so we just need to check its sign
                y = -y;
            if (matrixRotation.Element(2, 0) < 0) // Element(2,0) = k_x*k_z*v
                z = -z;
        } else if (y > 0) {
            if (matrixRotation.Element(2, 1) < 0) // Element(2,1) = k_y*k_z*v
                z = -z;
        }
        else {
            z = 1.0; // x and y are zero, Z has to be one
        }
        rodriguezRotation.Assign(value_type(x), value_type(y), value_type(z));
        rodriguezRotation.Multiply(value_type(cmnPI));
        return;
    }

    AngleType angle = atan2(norm / 2, (trace - 1) / 2);
    rodriguezRotation.Assign(value_type(xSin), value_type(ySin), value_type(zSin));
    rodriguezRotation.Multiply(value_type(angle / norm));
}


// Force the instantiation of the templated classes
template class vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> >;
template class vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> >;

// force instantiation of helper functions
// fixed / fixed
template void
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > & rodriguezRotation,
                                 const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
template void
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > & rodriguezRotation,
                                 const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation);

template void
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > & rodriguezRotation,
                                 const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > & matrixRotation);
template void
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > & rodriguezRotation,
                                 const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > & matrixRotation);

