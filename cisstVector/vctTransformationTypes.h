/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2003-09-12

  (C) Copyright 2003-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctTransformationTypes_h
#define _vctTransformationTypes_h

/*!
  \file
  \brief Typedef for different transformations
*/

#include <cisstVector/vctFixedSizeMatrixTypes.h>
#include <cisstVector/vctBarycentricVector.h>
#include <cisstVector/vctQuaternion.h>
#include <cisstVector/vctAxisAngleRotation3.h>
#include <cisstVector/vctRodriguezRotation3.h>
#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctEulerRotation3.h>
#include <cisstVector/vctAngleRotation2.h>
#include <cisstVector/vctMatrixRotation2.h>
#include <cisstVector/vctFrameBase.h>
#include <cisstVector/vctFrame4x4.h>

#include <cisstVector/vctDataFunctionsTransformations.h>

/*! Define a vector of barycentric coordinates of 1 double. */
typedef vctBarycentricVector<double, 1> vctBarycentric1;
/*! Define a vector of barycentric coordinates of 2 doubles. */
typedef vctBarycentricVector<double, 2> vctBarycentric2;
/*! Define a vector of barycentric coordinates of 3 doubles. */
typedef vctBarycentricVector<double, 3> vctBarycentric3;
/*! Define a vector of barycentric coordinates of 4 doubles. */
typedef vctBarycentricVector<double, 4> vctBarycentric4;


/*! Define a quaternion of doubles. */
//@{
typedef vctQuaternion<double> vctDoubleQuat;
typedef vctQuaternion<double> vctQuat;
//@}
/*! Define a quaternion of floats. */
typedef vctQuaternion<float> vctFloatQuat;


/*! Define a rotation in dimension 3 using a unit quaternion of
  doubles. */
//@{
typedef vctQuaternionRotation3<double> vctDoubleQuatRot3;
typedef vctQuaternionRotation3<double> vctQuatRot3;
//@}
/*! Define a rotation in dimension 3 using a unit quaternion of
  floats. */
//@{
typedef vctQuaternionRotation3<float> vctFloatQuatRot3;
//@}


/*! Define a rotation in dimension 3 using an axis of doubles and an
  angle (of type NormType). */
//@{
typedef vctAxisAngleRotation3<double> vctDoubleAxAnRot3;
typedef vctAxisAngleRotation3<double> vctAxAnRot3;
//@}
/*! Define a rotation in dimension 3 using an axis of doubles and an
  angle (of type NormType). */
//@{
typedef vctAxisAngleRotation3<float> vctFloatAxAnRot3;
//@}


/*! Define a rotation in dimension 3 using a single vector a.k.a
  Rodriguez representation. */
//@{
typedef vctRodriguezRotation3<double> vctDoubleRodRot3;
typedef vctRodriguezRotation3<double> vctRodRot3;
//@}
/*! Define a rotation in dimension 3 using  a single vector a.k.a
  Rodriguez representation. */
//@{
typedef vctRodriguezRotation3<float> vctFloatRodRot3;
//@}

// Euler angle typedefs are in vctEulerRotation3.h

/*! Define a rotation in dimension 3 using a 3 by 3 matrix of
  doubles. */
//@{
typedef vctMatrixRotation3<double, VCT_ROW_MAJOR> vctDoubleMatRot3;
typedef vctMatrixRotation3<double, VCT_ROW_MAJOR> vctMatRot3;
typedef vctMatrixRotation3<double, VCT_ROW_MAJOR> vctDoubleRot3;
typedef vctMatrixRotation3<double, VCT_ROW_MAJOR> vctRot3;
//@}
/*! Define a rotation in dimension 3 using a 3 by 3 matrix of
  float. */
//@{
typedef vctMatrixRotation3<float, VCT_ROW_MAJOR> vctFloatMatRot3;
typedef vctMatrixRotation3<float, VCT_ROW_MAJOR> vctFloatRot3;
//@}

/*! Define a transformation in dimension 3 using a unit quaternion of
  doubles for its rotation. */
//@{
typedef vctFrameBase<vctDoubleQuatRot3> vctDoubleQuatFrm3;
typedef vctFrameBase<vctQuatRot3> vctQuatFrm3;
//@}

/*! Define a transformation in dimension 3 using a unit quaternion of
  floats for its rotation. */
//@{
typedef vctFrameBase<vctFloatQuatRot3> vctFloatQuatFrm3;
//@}

/*! Define a transformation in dimension 3 using a 3 by 3 matrix of
  doubles for its rotation. */
//@{
typedef vctFrameBase<vctDoubleMatRot3> vctDoubleMatFrm3;
typedef vctFrameBase<vctMatRot3> vctMatFrm3;
typedef vctFrameBase<vctDoubleRot3> vctDoubleFrm3;
typedef vctFrameBase<vctRot3> vctFrm3;
//@}


/*! Define a transformation in dimension 3 using a 3 by 3 matrix of
  floats for its rotation. */
//@{
typedef vctFrameBase<vctFloatMatRot3> vctFloatMatFrm3;
typedef vctFrameBase<vctFloatRot3> vctFloatFrm3;
//@}


/*! Define a transformation in dimension 3 using a 4 by 4 matrix of
  doubles. */
//@{
typedef vctFrame4x4<double, VCT_ROW_MAJOR> vctDoubleFrm4x4;
typedef vctFrame4x4<double, VCT_ROW_MAJOR> vctFrm4x4;
//@}

/*! Define a transformation in dimension 3 using a 4 by 4 matrix of
  floats. */
//@{
typedef vctFrame4x4<float, VCT_ROW_MAJOR> vctFloatFrm4x4;
//@}



/*! Define a rotation in dimension 2 using an
  angle (of type AngleType). */
//@{
typedef vctAngleRotation2 vctDoubleAnRot2;
typedef vctAngleRotation2 vctAnRot2;
//@}
/*! Define a rotation in dimension 2 using an
  angle (of type AngleType). */
//@{
typedef vctAngleRotation2 vctFloatAnRot2;
//@}


/*! Define a rotation in dimension 2 using a 2 by 2 matrix of
  doubles. */
//@{
typedef vctMatrixRotation2<double> vctDoubleMatRot2;
typedef vctMatrixRotation2<double> vctMatRot2;
typedef vctMatrixRotation2<double> vctDoubleRot2;
typedef vctMatrixRotation2<double> vctRot2;
//@}
/*! Define a rotation in dimension 2 using a 2 by 2 matrix of
  float. */
//@{
typedef vctMatrixRotation2<float> vctFloatMatRot2;
typedef vctMatrixRotation2<float> vctFloatRot2;
//@}


/*! Define a transformation in dimension 2 using a 2 by 2 matrix of
  doubles for its rotation. */
//@{
typedef vctFrameBase<vctDoubleMatRot2> vctDoubleMatFrm2;
typedef vctFrameBase<vctMatRot2> vctMatFrm2;
typedef vctFrameBase<vctDoubleRot2> vctDoubleFrm2;
typedef vctFrameBase<vctRot2> vctFrm2;
//@}


/*! Define a transformation in dimension 2 using a 2 by 2 matrix of
  floats for its rotation. */
//@{
typedef vctFrameBase<vctFloatMatRot2> vctFloatMatFrm2;
typedef vctFrameBase<vctFloatRot2> vctFloatFrm2;
//@}


#endif  // _vctTransformationTypes_h

