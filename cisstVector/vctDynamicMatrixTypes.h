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
#ifndef _vctDynamicMatrixTypes_h
#define _vctDynamicMatrixTypes_h

/*!
  \file
  \brief Typedef for dynamic matrices
*/

#include <cisstVector/vctDynamicMatrix.h>

#include <cisstVector/vctDataFunctionsDynamicMatrix.h>

//@{
/*! Define some types of dynamic matrices */
typedef vctDynamicMatrix<double> vctDoubleMat;
typedef vctDynamicMatrix<double> vctMat;
typedef vctDynamicMatrix<float> vctFloatMat;
typedef vctDynamicMatrix<int> vctIntMat;
//@}

#endif  // _vctDynamicMatrixTypes_h

