/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2009-11-08

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Header files from cisstVector required to compile the SWIG generated Python wrappers
 */
#pragma once

#ifndef _vctPython_h
#define _vctPython_h

#include <cisstCommon/cmnPython.h>
#include <cisstVector/vctPythonUtilities.h>
#include <cisstVector/vctFixedSizeConstVectorBase.h>
#include <cisstVector/vctDynamicConstVectorBase.h>
#include <cisstVector/vctFixedSizeConstMatrixBase.h>
#include <cisstVector/vctDynamicConstMatrixBase.h>
#include <cisstVector/vctDynamicConstNArrayBase.h>
#include <cisstVector/vctTransformationTypes.h>

#endif // _vctPython_h
