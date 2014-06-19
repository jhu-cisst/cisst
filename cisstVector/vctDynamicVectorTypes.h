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
#ifndef _vctDynamicVectorTypes_h
#define _vctDynamicVectorTypes_h

/*!
  \file
  \brief Typedef for dynamic vectors
*/

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicConstVectorRef.h>
#include <cisstVector/vctDynamicVectorRef.h>

#include <cisstVector/vctDataFunctionsDynamicVector.h>

//@{
/*! Define some types of dynamic vectors */
typedef vctDynamicVector<double> vctDoubleVec;
typedef vctDynamicVector<double> vctVec;
typedef vctDynamicVector<float> vctFloatVec;
typedef vctDynamicVector<int> vctIntVec;
typedef vctDynamicVector<unsigned int> vctUIntVec;
typedef vctDynamicVector<char> vctCharVec;
typedef vctDynamicVector<unsigned char> vctUCharVec;
typedef vctDynamicVector<bool> vctBoolVec;
typedef vctDynamicVector<short> vctShortVec;
typedef vctDynamicVector<unsigned short> vctUShortVec;
typedef vctDynamicVector<long> vctLongVec;
typedef vctDynamicVector<unsigned long> vctULongVec;
//@}


#endif  // _vctDynamicVectorTypes_h

