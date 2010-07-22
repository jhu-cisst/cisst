/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2008-11-13

  (C) Copyright 2008-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsInterfaceInput_h
#define _mtsInterfaceInput_h

#include <cisstMultiTask/mtsInterfaceRequiredOrInput.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \file
  \brief Declaration of mtsInterfaceInput
 */


/*!
  \ingroup cisstMultiTask

 */

class CISST_EXPORT mtsInterfaceInput: public mtsInterfaceRequiredOrInput
{
 protected:

    /*! Default constructor. Does nothing, should not be used. */
    mtsInterfaceInput(void) {}

 public:

    /*! Constructor. Sets the name.

      \param interfaceName Name of required interface
    */
    mtsInterfaceInput(const std::string & interfaceName);

    /*! Default destructor. */
    virtual ~mtsInterfaceInput();
};


#endif // _mtsInterfaceInput_h

