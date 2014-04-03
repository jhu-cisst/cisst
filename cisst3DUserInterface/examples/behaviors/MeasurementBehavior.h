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

class MeasurementBehaviorVisibleObject;

// Always include last!
#include <ui3BehaviorsExport.h>

class CISST_EXPORT MeasurementBehavior: public ui3BehaviorBase
{
public:
    MeasurementBehavior(const std::string & name);
    ~MeasurementBehavior();
    
    void Startup(void);
    void Cleanup(void);
    void ConfigureMenuBar(void);
    bool RunForeground(void);
    bool RunBackground(void);
    bool RunNoInput(void);
    void Configure(const std::string & configFile);
    bool SaveConfiguration(const std::string & configFile);
    inline ui3VisibleObject * GetVisibleObject(void) {
        return this->VisibleList;
    }
    
protected:
    unsigned long Ticker;
    void PrimaryMasterButtonCallback(const prmEventButton & event);
    void StartStopMeasure(const prmEventButton & event);
    
    StateType PreviousState;
    bool PreviousMaM;
    vctDouble3 PreviousCursorPosition;
    vctDouble3 Offset;
    vctFrm3 Position;
    bool Measuring;
    
    ui3SlaveArm * Slave1;
    prmPositionCartesianGet Slave1Position;
    
    void SetMeasurePoint1(void);
    void GetMeasurement(void);
    
private:
    ui3VisibleList * VisibleList;
    MeasurementBehaviorVisibleObject * VisibleObject;
    bool RightMTMOpen;
    bool ClutchMeasureStarted; // for clutch based measure
    bool MeasurementActive;
    bool Transition;
    vctDouble3 MeasurePoint1;
    int DelayToGrab;
};

