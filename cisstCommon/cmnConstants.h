/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2005-10-17

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Declaration of various constants
  \ingroup cisstCommon
*/
#pragma once

#ifndef _cmnConstants_h
#define _cmnConstants_h


#include <cisstCommon/cmnExport.h>


/*! PI */
const double cmnPI = 3.1415926535897932384626433832795029;

/*! PI / 2 */
const double cmnPI_2 = 1.5707963267948966192313216916397514;

/*! PI / 4 */
const double cmnPI_4 = 0.7853981633974483096156608458198757;

/*! PI / 180 : convert degrees to radians */
const double cmnPI_180 = cmnPI / 180.0;

/*! 180 / PI : convert radians to degrees */
const double cmn180_PI = 180.0 / cmnPI;


#endif // _cmnConstants_h

