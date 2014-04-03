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


#include <cisst3DUserInterface/ui3BehaviorBase.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>
#include <cisst3DUserInterface/ui3VisibleList.h>
#include <cisst3DUserInterface/ui3Widget3D.h>

// forward declaration for our visible objects
class SimpleBehaviorVisibleObject;

// Always include last!
#include <ui3BehaviorsExport.h>

class CISST_EXPORT SimpleBehavior: public ui3BehaviorBase
{
public:
    SimpleBehavior(const std::string & name);
    ~SimpleBehavior();

    void Startup(void) {}
    void Cleanup(void) {}
    void ConfigureMenuBar(void);
    bool RunForeground(void);
    bool RunBackground(void);
    bool RunNoInput(void);
    void OnQuit(void);
    void OnStart(void);
    void Configure(const std::string & CMN_UNUSED(configFile)) {}
    bool SaveConfiguration(const std::string & CMN_UNUSED(configFile)) { return true; }
    inline ui3VisibleObject * GetVisibleObject(void) {
        return this->VisibleList;
    }

protected:
    void PrimaryMasterButtonCallback(const prmEventButton & event);
    void ToggleColor(void);
    void UpdateRelativePosition(void);
    StateType PreviousState;
    bool PreviousMaM;
    vctDouble3 Position, PreviousCursorPosition;
    bool Following;

    ui3VisibleList * VisibleList;
    SimpleBehaviorVisibleObject * VisibleObject1;
    SimpleBehaviorVisibleObject * VisibleObject2;
    double Counter;
};
