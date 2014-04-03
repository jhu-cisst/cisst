/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2004-02-12

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstVector/vctFrameBase.h>
#include <cisstVector/vctTransformationTypes.h>
#include <cisstVector/vctMatrixRotation3Base.h>
#include <cisstVector/vctQuaternionRotation3Base.h>

template <>
const vctFrameBase<vctDoubleMatRot3> &
vctFrameBase<vctDoubleMatRot3>::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}

template <>
const vctFrameBase<vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > > &
vctFrameBase<vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > >::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}



template <>
const vctFrameBase<vctFloatMatRot3> &
vctFrameBase<vctFloatMatRot3>::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}

template <>
const vctFrameBase<vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > > &
vctFrameBase<vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > >::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}



template <>
const vctFrameBase<vctDoubleQuatRot3> &
vctFrameBase<vctDoubleQuatRot3>::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}

template <>
const vctFrameBase<vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > > &
vctFrameBase<vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > >::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}



template <>
const vctFrameBase<vctFloatQuatRot3> &
vctFrameBase<vctFloatQuatRot3>::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}

template <>
const vctFrameBase<vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > > &
vctFrameBase<vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > >::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}



template <>
const vctFrameBase<vctDoubleMatRot2> &
vctFrameBase<vctDoubleMatRot2>::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}

template <>
const vctFrameBase<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > > &
vctFrameBase<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > >::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}



template <>
const vctFrameBase<vctFloatMatRot2> &
vctFrameBase<vctFloatMatRot2>::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}

template <>
const vctFrameBase<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > > &
vctFrameBase<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > >::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}
