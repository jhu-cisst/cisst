/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: BehaviorLUS.h 309 2009-05-05 01:26:24Z adeguet1 $

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

#define DEPTH           -200
#define _PI             3.14159265358979

// forward declarations
class BehaviorLUSProbeHead;
class BehaviorLUSProbeJoint;
class BehaviorLUSProbeShaft;
class BehaviorLUSBackground;


class BehaviorLUS : public ui3BehaviorBase
{
public:
    BehaviorLUS(const std::string & name, ui3Manager * manager);
    ~BehaviorLUS();

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

    //method to queary joint space of the arm
    //method to set joint orientations?


    }
    void SetJoints(double A1, double A2, double insertion, double roll);

protected:
    unsigned long Ticker;
    void FirstButtonCallback(void);
    void EnableMapButtonCallback(void);
    void PrimaryMasterButtonCallback(const prmEventButton & event);


    StateType PreviousState;
    bool PreviousMaM;
    vctDouble3 PreviousCursorPosition;
    vctDouble3 Offset;
    vctFrm3 Position, ProbePosition;
    bool Following;

    void OnStreamSample(svlSample* sample, int streamindex);
    ui3ImagePlane* ImagePlane;

    ui3SlaveArm * Slave1;
    prmPositionCartesianGet Slave1Position;

private:
    BehaviorLUSProbeHead * ProbeHead;

    BehaviorLUSProbeJoint *ProbeJoint1;
    BehaviorLUSProbeJoint *ProbeJoint2;
    BehaviorLUSProbeJoint *ProbeJoint3;
    BehaviorLUSProbeShaft *ProbeShaft;
    BehaviorLUSBackground *Backgrounds;

    ui3VisibleList * VisibleList; // all actors for this behavior
    ui3VisibleList * ProbeList; // all actors moving wrt the slave arm
    bool MapEnabled;
};

