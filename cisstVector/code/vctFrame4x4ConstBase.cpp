/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2007-09-14

  (C) Copyright 2007-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstVector/vctFrame4x4ConstBase.h>
#include <cisstVector/vctTransformationTypes.h>


/* Fixed size, row major */
template <>
const vctFrame4x4ConstBase<vctFixedSizeMatrix<double, 4, 4> >::FrameValueType &
vctFrame4x4ConstBase<vctFixedSizeMatrix<double, 4, 4> >::Identity(void)
{
    static const FrameValueType result(ThisType::Eye());
    return result;
}

template <>
const vctFrame4x4ConstBase<vctFixedSizeMatrix<float, 4, 4> >::FrameValueType &
vctFrame4x4ConstBase<vctFixedSizeMatrix<float, 4, 4> >::Identity(void)
{
    static const FrameValueType result(ThisType::Eye());
    return result;
}

/* Fixed size, col major */
template <>
const vctFrame4x4ConstBase<vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR> >::FrameValueType &
vctFrame4x4ConstBase<vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR> >::Identity(void)
{
    static const FrameValueType result(ThisType::Eye());
    return result;
}

template <>
const vctFrame4x4ConstBase<vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR> >::FrameValueType &
vctFrame4x4ConstBase<vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR> >::Identity(void)
{
    static const FrameValueType result(ThisType::Eye());
    return result;
}
