/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:	2009-04-03

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _ui3MasterArm_h
#define _ui3MasterArm_h

#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstParameterTypes/prmForwardDeclarations.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3CursorBase.h>


/*!
  Defines a master arm with cursor and callbacks
*/
class ui3MasterArm
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    
public:
    /*!
     Constructor
    */
    ui3MasterArm(const std::string & name);

    /*!
     Destructor
    */
    virtual ~ui3MasterArm();

    virtual bool SetInput(mtsDevice * positionDevice, const std::string & positionInterface,
                          mtsDevice * buttonDevice, const std::string & buttonInterface,
                          mtsDevice * clutchDevice, const std::string & clutchInterface);
    
    virtual bool SetGeometry(const vctFrm3 & transformation = vctFrm3::Identity(),
                             double scale = 1.0);

    virtual bool SetCursor(ui3CursorBase * cursor);

protected:

    // event handlers
    void ButtonEventHandler(const prmEventButton & buttonEvent);
    void ClutchEventHandler(const prmEventButton & buttonEvent);

    // cursors
    ui3CursorBase * Cursor;

    // button state, might be a better implementation for this (Anton)
    bool ButtonPressed;
    bool ButtonReleased;

    // transformation between inputs and scene
    vctFrm3 Transform;
    vctFrm3 LeftTransform;

    // positions in the state table, for behaviors to read
    prmPositionCartesianGet Position;

    // arm clutch
    bool Clutch;

    // scale
    double Scale;
};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3MasterArm)


#endif // _ui3MasterArm_h
