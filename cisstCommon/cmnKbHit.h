/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2013-12-22

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#pragma once

#ifndef _cmnKbHit_h
#define _cmnKbHit_h

#include <cisstCommon/cmnPortability.h>

// Always the last cisst include
#include <cisstCommon/cmnExport.h>

/*! Non blocking wait for a key press.  This can be used in a loop
  allowing the user to press a key at anytime using something like
  `while (!cmnKbHit())`.  The key that was pressed can be retrieved
  using cmnGetChar().
*/
int CISST_EXPORT cmnKbHit(void);

#endif // _cmnKbHit_h

