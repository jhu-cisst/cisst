/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of mtsInterfaceOutput
*/

#ifndef _mtsInterfaceOutput_h
#define _mtsInterfaceOutput_h

#include <cisstMultiTask/mtsInterface.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>


/*!
  \ingroup cisstMultiTask
*/
class CISST_EXPORT mtsInterfaceOutput: public mtsInterface
{
 public:
    mtsInterfaceOutput(const std::string & name, mtsComponent * component);
};

#endif // _mtsInterfaceOutput_h
