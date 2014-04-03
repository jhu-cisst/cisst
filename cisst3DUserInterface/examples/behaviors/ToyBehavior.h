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


#include <cisst3DUserInterface.h>

#include "cs3DObjectViewer.h"

class ToyBehaviorVisibleObject;
class Widget;

class ToyBehavior : public ui3BehaviorBase
{
    public:
        ToyBehavior(const std::string & name);
        ~ToyBehavior();

        void Startup(void);
        void Cleanup(void);
        void ConfigureMenuBar(void);
        bool RunForeground(void);
        bool RunBackground(void);
        bool RunNoInput(void);
        void OnStart(void);
        void Configure(const std::string & configFile);
        bool SaveConfiguration(const std::string & configFile);
        inline ui3VisibleObject * GetVisibleObject(void) {
            return this->VisibleList;
        }

    protected:
        unsigned long Ticker;
        void FirstButtonCallback(void);
        void PrimaryMasterButtonCallback(const prmEventButton & event);
        void AddSphereCallback(void);
        void AddCubeCallback(void);
        void AddCylinderCallback(void);
        ui3VisibleObject * FindClosestShape(void);



        StateType PreviousState;
        bool PreviousMaM;
        vctDouble3 PreviousCursorPosition;
        vctDouble3 Offset;
        vctFrm3 Position;
        bool Following;

        ui3SlaveArm * Slave1;
        prmPositionCartesianGet Slave1Position;
        
        typedef std::list<vctFrm3> ListFrameType;
        ListFrameType Frames;
    private:
        ui3VisibleList * VisibleList;
        Widget * WidgetObject;
        bool RightMTMOpen;
        bool Transition;

};

