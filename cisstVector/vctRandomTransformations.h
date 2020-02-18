/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2007-02-11

  (C) Copyright 2005-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctRandomTransformations_h
#define _vctRandomTransformations_h

/*!
  \file
  \brief Definition of vctRandom for transformations.
*/

#include <cisstCommon/cmnRandomSequence.h>
#include <cisstCommon/cmnPortability.h>

#include <cisstVector/vctMatrixRotation3Ref.h>
#include <cisstVector/vctForwardDeclarations.h>

#include <cisstVector/vctExport.h>

/*!
  \ingroup cisstVector

  Define the global function vctRandom to initialize different
  types of transformation with a random value.

  \note The function uses the global instance of cmnRandomSequence to
  extract random values.  As we have a vague plan to allow for
  multiple random sequence objects to coexist, these interfaces may
  need to be changed.
*/
//@{
template <class _containerType>
void vctRandom(vctMatrixRotation3Base<_containerType> & matrixRotation);

#ifndef DOXYGEN
#ifdef CISST_COMPILER_IS_MSVC
template CISST_EXPORT void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> > &);
template CISST_EXPORT void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> > &);
template CISST_EXPORT void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> > &);
template CISST_EXPORT void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> > &);
template CISST_EXPORT void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 4, 1> > &);
template CISST_EXPORT void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrixRef<double, 3, 3, 1, 4> > &);
template CISST_EXPORT void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 4, 1> > &);
template CISST_EXPORT void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrixRef<float, 3, 3, 1, 4> > &);
#endif
#endif // DOXYGEN


template <class _containerType>
CISST_EXPORT void vctRandom(vctQuaternionRotation3Base<_containerType> & quaternionRotation);

#ifndef DOXYGEN
#ifdef CISST_COMPILER_IS_MSVC
template CISST_EXPORT void vctRandom(vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > &);
template CISST_EXPORT void vctRandom(vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > &);
#endif
#endif // DOXYGEN


template <class _elementType>
CISST_EXPORT void vctRandom(vctAxisAngleRotation3<_elementType> & axisAngleRotation);

#ifndef DOXYGEN
#ifdef CISST_COMPILER_IS_MSVC
template CISST_EXPORT void vctRandom(vctAxisAngleRotation3<double> &);
template CISST_EXPORT void vctRandom(vctAxisAngleRotation3<float> &);
#endif
#endif // DOXYGEN


template <class _containerType>
CISST_EXPORT void vctRandom(vctRodriguezRotation3Base<_containerType> & rodriguezRotation);

#ifndef DOXYGEN
#ifdef CISST_COMPILER_IS_MSVC
template CISST_EXPORT void vctRandom(vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > &);
template CISST_EXPORT void vctRandom(vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > &);
#endif
#endif // DOXYGEN


template <vctEulerRotation3Order::OrderType _order>
CISST_EXPORT void vctRandom(vctEulerRotation3<_order> & eulerRotation);

#ifndef DOXYGEN
#ifdef CISST_COMPILER_IS_MSVC
template CISST_EXPORT void vctRandom(vctEulerRotation3<vctEulerRotation3Order::ZYZ> &);
template CISST_EXPORT void vctRandom(vctEulerRotation3<vctEulerRotation3Order::ZYX> &);
template CISST_EXPORT void vctRandom(vctEulerRotation3<vctEulerRotation3Order::ZXZ> &);
template CISST_EXPORT void vctRandom(vctEulerRotation3<vctEulerRotation3Order::YZX> &);
#endif
#endif // DOXYGEN


template <class _containerType>
CISST_EXPORT void vctRandom(vctMatrixRotation2Base<_containerType> & matrixRotation);

#ifndef DOXYGEN
#ifdef CISST_COMPILER_IS_MSVC
template CISST_EXPORT void vctRandom(vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > &);
template CISST_EXPORT void vctRandom(vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > &);
#endif
#endif // DOXYGEN

CISST_EXPORT void vctRandom(vctAngleRotation2 & angleRotation);
//@}

#endif  // _vctRandomTransformations_h
