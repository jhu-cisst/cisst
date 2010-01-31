/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstDevices.i.h 1018 2010-01-05 16:51:35Z adeguet1 $

  Author(s):	Anton Deguet
  Created on:   2008-01-17

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
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

   For any wrapper using %import "cisstDevices.i", the file
   cisstDevices.i.h should be included in the %header %{ ... %} section
   of the interface file. */


#ifndef _cisstDevices_i_h
#define _cisstDevices_i_h


/* Put header files here */
#include "cisstCommon/cisstCommon.i.h"
#include "cisstVector/cisstVector.i.h"
#include "cisstMultiTask/cisstMultiTask.i.h"
#include "cisstParameterTypes/cisstParameterTypes.i.h"

#include "cisstDevices/devNDISerial.h"

#endif // _cisstDevices_i_h
