/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2009-04-03

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
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstParameterTypes/prmForwardDeclarations.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstParameterTypes/prmPositionCartesianSet.h>
#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3CursorBase.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

/*!
  Defines a master arm with cursor and callbacks
*/
class CISST_EXPORT ui3MasterArm: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
    friend class ui3Manager;

 public:

    enum RoleType {PRIMARY, SECONDARY};

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
                          mtsDevice * clutchDevice, const std::string & clutchInterface,
                          const RoleType & role);

    virtual bool SetInput(const std::string & positionDeviceInterface, const std::string & positionInterface,
                          const std::string & buttonDeviceInterface, const std::string & buttonInterface,
                          const std::string & clutchDeviceInterface, const std::string & clutchInterface,
                          const RoleType & role);

    virtual bool SetTransformation(const vctFrm3 & transformation = vctFrm3::Identity(),
                                   double scale = 1.0);

    virtual void SetCursorPosition(const vctDouble3 & position);

    virtual void SetCursorPosition(const prmPositionCartesianSet & position);

    virtual bool SetCursor(ui3CursorBase * cursor);

    void SetScaleFactor(const mtsDouble & factor);

 protected:

    // arm name
    std::string Name;

    // event handlers
    void ButtonEventHandler(const prmEventButton & buttonEvent);
    void ClutchEventHandler(const prmEventButton & buttonEvent);

    // cursors
    ui3CursorBase * Cursor;

    // button state, might be a better implementation for this (Anton)
    bool ButtonPressed;
    bool ButtonReleased;

    // role
    RoleType Role;

    // transformation between inputs and scene
    vctFrm3 Transformation;
    double Scale;
    double ScaleFactor;

    // positions in the state table, for behaviors to read
    prmPositionCartesianGet CartesianPosition;
    mtsFunctionRead GetCartesianPosition;

    // cursor position
    vctFrm3 CursorPosition;

    // arm clutch
    bool Clutched;
    vctDouble3 ClutchedOutPosition;

    // ui3Manager used
    ui3Manager * Manager;

    inline bool SetManager(ui3Manager * manager) {
        this->Manager = manager;
        return true;
    }

    // used by the ui3Manager
    void PreRun(void);
    void UpdateCursorPosition(void);
    void Hide(void);
    void Show(void);

    // used to figure out which object is selected or to be selected
    ui3Selectable * Selected;
    double HighestIntention;
    ui3Selectable * ToBeSelected;

    void UpdateIntention(ui3Selectable * selectable);

    // check if we are currently over a menu and if pressed over menu
    bool IsOverMenu;
    bool PressedOverMenu;
};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3MasterArm)


#endif // _ui3MasterArm_h
