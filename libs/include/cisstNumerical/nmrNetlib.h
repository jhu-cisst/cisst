/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrNetlib.h,v 1.6 2007/04/26 19:33:57 anton Exp $

  Author(s): Anton Deguet
  Created on: 2005-11-24

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
*/


#ifndef _nmrNetlib_h
#define _nmrNetlib_h

#include <cisstConfig.h>

// sanity check
#if CISST_HAS_CNETLIB && CISST_HAS_CISSTNETLIB
#warning "Can't have both cnetlib and cisstNetlib"
#endif

// load correct header file
#if CISST_HAS_CNETLIB
#define CISST_HAS_NETLIB
#include <cnetlib.h>
// f2c.h defines these as macros
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#elif CISST_HAS_CISSTNETLIB
#define CISST_HAS_NETLIB
#include <cisstNetlib.h>
#endif


/*!
  \var typedef long int F_INTEGER

  Type used for sizes in Fortran routines.  This type differs from the
  default size_type in cisst (unsigned int) therefore an explicit cast
  should be used whenever a Fortran routine is called. */ 
typedef long int F_INTEGER;


#endif // _nmrNetlib_h

