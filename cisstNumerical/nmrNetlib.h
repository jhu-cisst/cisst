/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Anton Deguet, Simon Leonard
  Created on: 2005-11-24

  (C) Copyright 2005-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
*/


#ifndef _nmrNetlib_h
#define _nmrNetlib_h

#include <cisstNumerical/nmrConfig.h>

#if CISST_HAS_CISSTNETLIB

#define CISST_HAS_NETLIB
#include <cisstNetlib.h>

/*! Check if cisstNetlib has a version, i.e. it has for versions
  greater or equal to 2.  In Version 2 we introduced typedefs to make
  sure we use the correct Fortran types. */
#ifndef CISSTNETLIB_VERSION
typedef long int CISSTNETLIB_INTEGER;
typedef long int CISSTNETLIB_LOGICAL;
typedef float CISSTNETLIB_FLOAT;
typedef double CISSTNETLIB_DOUBLE;
#endif // CISSTNETLIB_VERSION

#endif // CISST_HAS_CISSTNETLIB

#endif // _nmrNetlib_h

