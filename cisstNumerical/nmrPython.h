/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2010-07-12

  (C) Copyright 2010-2016 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Header files from cisstNumerical required to compile the SWIG generated Python wrappers
 */
#pragma once

#ifndef _nmrPython_h
#define _ntmPython_h

#include <cisstCommon/cmnPython.h>
#include <cisstVector/vctPython.h>

#include <cisstNumerical/nmrNetlib.h>

#if CISST_HAS_CISSTNETLIB
#include <cisstNumerical/nmrSVD.h>
#include <cisstNumerical/nmrPInverse.h>
#include <cisstNumerical/nmrRegistrationRigid.h>

// Following is declared in cisstNumerical.i
std::pair<vctFrm3, double> nmrRegistrationRigid(const vctDynamicMatrix<double> &dataSet1, const vctDynamicMatrix<double> &dataSet2)
{
    if ((dataSet1.cols() != 3) || (dataSet2.cols() != 3))
        cmnThrow(std::runtime_error("nmrRegistrationRigid: column size must be 3"));
    if (dataSet1.rows() != dataSet2.rows())
        cmnThrow(std::runtime_error("nmrRegistrationRigid: number of data points (row sizes) must be equal"));
    const vct3 *ptr1 = reinterpret_cast<const vct3 *>(dataSet1.Pointer());
    vctDynamicConstVectorRef<vct3> d1(dataSet1.rows(), ptr1, 1);
    const vct3 *ptr2 = reinterpret_cast<const vct3 *>(dataSet2.Pointer());
    vctDynamicConstVectorRef<vct3> d2(dataSet2.rows(), ptr2, 1);
    vctFrm3 transform;
    double fre;
    if (!nmrRegistrationRigid(d1, d2, transform, &fre))
        cmnThrow(std::runtime_error("nmrRegistrationRigid: failed to compute registration"));
    return std::pair<vctFrm3, double>(transform, fre);
}

#endif
 
#endif // _nmrPython_h
