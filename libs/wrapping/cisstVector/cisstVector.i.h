/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2005-08-21

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/* This file is to be used only for the generation of SWIG wrappers.
   It includes all the regular header files from the libraries as well
   as some header files created only for the wrapping process
   (e.g. vctDynamicMatrixRotation3.h).

   For any wrapper using %import "cisstVector.i", the file
   cisstVector.i.h should be included in the %header %{ ... %} section
   of the interface file. */


#ifndef _cisstVector_i_h
#define _cisstVector_i_h


/* Put header files here */
#include "cisstVector/vctFixedSizeVector.h"
#include "cisstVector/vctFixedSizeMatrix.h"

#include "cisstVector/vctDynamicVector.h"
#include "cisstVector/vctDynamicMatrix.h"

#include "cisstVector/vctRandom.h"

// #include "cisstVector/vctQuaternionBase.h"

// #include "cisstVector/vctAxisAngleRotation3.h"

// #include "cisstVector/vctFrameBase.h"

#include "cisstVector/vctFixedSizeVectorTypes.h"
#include "cisstVector/vctFixedSizeMatrixTypes.h"

// Hack for Swig 1.3.35 - 1.3.40(and probably 1.3.34)
#if (SWIG_VERSION == 0x010334) || (SWIG_VERSION == 0x010335) || (SWIG_VERSION == 0x010336) || (SWIG_VERSION == 0x010337) || (SWIG_VERSION == 0x010338) || (SWIG_VERSION == 0x010339) || (SWIG_VERSION == 0x010340)
#define SWIGTYPE_p_vctDynamicMatrixTdouble_t  SWIGTYPE_p_vctDynamicMatrixT_double_t
#define SWIGTYPE_p_vctDynamicMatrixTint_t  SWIGTYPE_p_vctDynamicMatrixT_int_t
#define SWIGTYPE_p_vctDynamicMatrixTlong_t  SWIGTYPE_p_vctDynamicMatrixT_long_t
#define SWIGTYPE_p_vctDynamicMatrixTshort_t  SWIGTYPE_p_vctDynamicMatrixT_short_t
#endif

#endif // _cisstVector_i_h

