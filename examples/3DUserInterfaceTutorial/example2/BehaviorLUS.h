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
class BehaviorLUSOutline;
class BehaviorLUSText;
class BehaviorLUSMarker;



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
    void SetProbeColor(double r, double g, double b);
    void SetText(BehaviorLUSText *obj, const std::string & text);
    void CheckLimits(double p, double y, double i, double r);
    void GetMeasurement(vctFixedSizeVector<double,3u> pos);
    void AddMarker();
    void RemoveLastMarker();

    void mtm_right_button_callback(const prmEventButton & payload);
    void DropMarkerCallback(void);
    void RemoveMarkerCallback(void);
    

protected:
    unsigned long Ticker;
    void FirstButtonCallback(void);
    void EnableMapButtonCallback(void);
    void Master_clutch_callback(void);
    void PrimaryMasterButtonCallback(const prmEventButton & event);


    StateType PreviousState;
    bool PreviousMaM;
    bool RightMTMOpen, prevRightMTMOpen;
    bool isRightMTMOpen(double grip);
    
    vctDouble3 PreviousCursorPosition;
    vctDouble3 PreviousSlavePosition;
    vctDouble3 Offset;
    vctDouble3 CursorOffset;
    vctFrm3 Position, ProbePosition;
    bool Following;

    void OnStreamSample(svlSample* sample, int streamindex);
    ui3ImagePlane* ImagePlane;

    ui3SlaveArm * Slave1;
    ui3SlaveArm * ECM1;
    ui3MasterArm * RMaster;
    prmPositionCartesianGet Slave1Position;
    prmPositionCartesianGet ECM1Position;
    
    mtsFunctionRead GetJointPositionSlave;
    prmPositionJointGet JointsSlave;
    
    void UpdateMap(prmPositionCartesianGet & ecmFrame); // ANTON TO FIX , double insertion);
    
    mtsFunctionRead GetJointPositionECM;
    prmPositionJointGet JointsECM;
    
    bool MeasurementActive;
    bool MapEnabled;
    vctDouble3 MeasurePoint1;

private:
    int MarkerCount;
    BehaviorLUSProbeHead  *ProbeHead;
    BehaviorLUSProbeJoint *ProbeJoint1;
    BehaviorLUSProbeJoint *ProbeJoint2;
    BehaviorLUSProbeJoint *ProbeJoint3;
    BehaviorLUSProbeShaft *ProbeShaft;
    BehaviorLUSBackground *Backgrounds;
    BehaviorLUSOutline    *Outline;
    BehaviorLUSText       *WarningText, * MeasureText;
    BehaviorLUSMarker     *MapCursor, *m;



    ui3VisibleList * VisibleList; // all actors for this behavior
    ui3VisibleList * ProbeList; // all actors moving wrt the slave arm
    ui3VisibleList * ProbeListJoint1;
    ui3VisibleList * ProbeListJoint2;
    ui3VisibleList * ProbeListJoint3;
    ui3VisibleList * ProbeListShaft;
    ui3VisibleList * BackgroundList;
    ui3VisibleList * TextList;
    ui3VisibleList * MarkerList;
    ui3VisibleList * MapCursorList;
    ui3VisibleList * AxesList;

    ui3VisibleAxes * ProbeAxes;
    ui3VisibleAxes * AxesJoint1;
    ui3VisibleAxes * AxesJoint2;
    ui3VisibleAxes * AxesJoint3;
    ui3VisibleAxes * AxesShaft;
};

