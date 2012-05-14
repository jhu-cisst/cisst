/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstStealthLinkPython.h 1623 2010-07-13 19:34:48Z wliu25 $

  Author(s):	Anton Deguet
  Created on:	2009-11-08

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Header files from cisstStealthLink required to compile the SWIG generated Python wrappers
 */
#pragma once

#ifndef _cisstStealthlinkPython_h
#define _cisstStealthlinkPython_h

#include <cisstParameterTypes/prmPython.h>

#if CISST_HAS_XML
    #include <cisstStealthlink/mtsStealthlinkTypes.h>
    #include <cisstStealthlink/mtsStealthlink.h>
#endif

#include <cisstStealthlink/mtsStealthlinkControllerComponent.h>

#endif // _cisstStealthlinkPython_h
