/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):	Anton Deguet
  Created on:	2005-08-24

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctQuaternionRotation3Base.h>
#include <cisstVector/vctMatrixRotation3Base.h>

template<>
const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > &
vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> >::Identity()
{
    static const ThisType result(0.0, 0.0, 0.0, 1.0);
    return result;
}

template<>
const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > &
vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> >::Identity()
{
    static const ThisType result(0.0f, 0.0f, 0.0f, 1.0f);
    return result;
}


template <class _quaternionType, class _matrixType>
void
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<_quaternionType> & quaternionRotation,
                                  const vctMatrixRotation3Base<_matrixType> & matrixRotation)
{
    typedef typename _quaternionType::value_type value_type;
    typedef typename _quaternionType::TypeTraits TypeTraits;
    typedef typename _quaternionType::NormType NormType;

    NormType trace, traceInverse;
    trace = NormType(1.0) + matrixRotation.Element(0, 0) + matrixRotation.Element(1, 1) + matrixRotation.Element(2, 2);
    if (vctUnaryOperations<NormType>::AbsValue::Operate(trace) > cmnTypeTraits<NormType>::Tolerance()) {
        trace = NormType(sqrt(trace) * 2.0);
        traceInverse = NormType(1.0) / trace;
        quaternionRotation.X() = value_type((matrixRotation.Element(2, 1) - matrixRotation.Element(1, 2)) * traceInverse);
        quaternionRotation.Y() = value_type((matrixRotation.Element(0, 2) - matrixRotation.Element(2, 0)) * traceInverse);
        quaternionRotation.Z() = value_type((matrixRotation.Element(1, 0) - matrixRotation.Element(0, 1)) * traceInverse);
        quaternionRotation.R() = value_type(0.25 * trace);
    } else 
        if (matrixRotation.Element(0, 0) > matrixRotation.Element(1, 1) && matrixRotation.Element(0, 0) > matrixRotation.Element(2, 2))  {
            trace  = NormType(sqrt(1.0 + matrixRotation.Element(0, 0) - matrixRotation.Element(1, 1) - matrixRotation.Element(2, 2)) * 2.0);
            traceInverse = NormType(1.0) / trace;
            quaternionRotation.X() = value_type(0.25 * trace);
            quaternionRotation.Y() = value_type((matrixRotation.Element(0, 1) + matrixRotation.Element(1, 0)) * traceInverse);
            quaternionRotation.Z() = value_type((matrixRotation.Element(2, 0) + matrixRotation.Element(0, 2)) * traceInverse);
            quaternionRotation.R() = value_type((matrixRotation.Element(1, 2) - matrixRotation.Element(2, 1)) * traceInverse);
        } else if (matrixRotation.Element(1, 1) > matrixRotation.Element(2, 2)) {
            trace  = NormType(sqrt(1.0 + matrixRotation.Element(1, 1) - matrixRotation.Element(0, 0) - matrixRotation.Element(2, 2)) * 2.0);
            traceInverse = NormType(1.0) / trace;
            quaternionRotation.X() = value_type((matrixRotation.Element(0, 1) + matrixRotation.Element(1, 0)) * traceInverse);
            quaternionRotation.Y() = value_type(0.25 * trace);
            quaternionRotation.Z() = value_type((matrixRotation.Element(1, 2) + matrixRotation.Element(2, 1)) * traceInverse);
            quaternionRotation.R() = value_type((matrixRotation.Element(2, 0) - matrixRotation.Element(0, 2)) * traceInverse);
        } else {
            trace  = NormType(sqrt(1.0 + matrixRotation.Element(2, 2) - matrixRotation.Element(0, 0) - matrixRotation.Element(1, 1)) * 2.0);
            traceInverse = NormType(1.0) / trace;
            quaternionRotation.X() = value_type((matrixRotation.Element(2, 0) + matrixRotation.Element(0, 2)) * traceInverse);
            quaternionRotation.Y() = value_type((matrixRotation.Element(1, 2) + matrixRotation.Element(2, 1)) * traceInverse);
            quaternionRotation.Z() = value_type(0.25 * trace);
            quaternionRotation.R() = value_type((matrixRotation.Element(0, 1) - matrixRotation.Element(1, 0)) * traceInverse);
        }
}


// force the instantiation of the templated classes
template class vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> >;
template class vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> >;

// force instantiation of helper functions
template void
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> > & matrixRotation);
template void
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> > & matrixRotation);
template void
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> > & matrixRotation);
template void
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> > & matrixRotation);

template void
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> > & matrixRotation);
template void
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 1, 4> > & matrixRotation);
template void
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> > & matrixRotation);
template void
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 1, 4> > & matrixRotation);

