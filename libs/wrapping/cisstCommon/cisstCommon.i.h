/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstCommon.i.h 8 2009-01-04 21:13:48Z adeguet1 $

  Author(s):	Anton Deguet
  Created on:   2004-10-06

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
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

   For any wrapper using %import "cisstCommon.i", the file
   cisstCommon.i.h should be included in the %header %{ ... %} section
   of the interface file. */


#ifndef _cisstCommon_i_h
#define _cisstCommon_i_h


/* Put header files here */
#include "cisstCommon/cmnGenericObject.h"
#include "cisstCommon/cmnGenericObjectProxy.h"
#include "cisstCommon/cmnObjectRegister.h"
#include "cisstCommon/cmnClassServicesBase.h"
#include "cisstCommon/cmnClassRegister.h"
#include "cisstCommon/cmnTypeTraits.h"
#include "cisstCommon/cmnCallbackStreambuf.h"
#include "cisstCommon/cmnPath.h"
#include "cisstCommon/cmnDataObject.h"

#endif // _cisstCommon_i_h

