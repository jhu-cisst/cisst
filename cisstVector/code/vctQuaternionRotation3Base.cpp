/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2005-08-24

  (C) Copyright 2005-2012 Johns Hopkins University (JHU), All Rights
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


// force the instantiation of the templated classes
template class vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> >;
template class vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> >;
