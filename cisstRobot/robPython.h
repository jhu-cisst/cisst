/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2016-03-21

  (C) Copyright 2016 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Header files from cisstRobot required to compile the SWIG generated Python wrappers
 */
#pragma once

#ifndef _robPython_h
#define _robPython_h

#include <cisstCommon/cmnPython.h>
#include <cisstVector/vctPython.h>

#include <cisstRobot/robManipulator.h>
#include <cisstRobot/robDH.h>
#include <cisstRobot/robModifiedDH.h>
#include <cisstRobot/robHayati.h>

#endif // _robPython_h
