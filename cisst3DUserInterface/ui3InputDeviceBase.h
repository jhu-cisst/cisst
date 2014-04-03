/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-05-23

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _ui3InputDeviceBase_h
#define _ui3InputDeviceBase_h


#include <cisst3DUserInterface/ui3ForwardDeclarations.h>

/*!
 Abstract class that provides the structure for implementing interfaces between
 the SAW user interface manager and user input devices.
*/
class ui3InputDeviceBase
{
public:
    /*!
     Typedef for input device actions (for example: clicking, grabbing)
    */
    typedef unsigned int InputAction;

    /*!
     Constructor
    */
    ui3InputDeviceBase();

    /*!
     Destructor
    */
    virtual ~ui3InputDeviceBase();
};

#endif // _ui3InputDeviceBase_h

