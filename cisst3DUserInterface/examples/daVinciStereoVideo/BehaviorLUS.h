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


#include <cisstParameterTypes/prmPositionJointGet.h>
#include <cisst3DUserInterface/ui3BehaviorBase.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>
#include <cisst3DUserInterface/ui3VisibleList.h>
#include <cisst3DUserInterface/ui3Manager.h>
#include <cisst3DUserInterface/ui3ImagePlane.h>
#include <cisst3DUserInterface/ui3SlaveArm.h>

#include <list>

#define DEPTH           -200

// forward declarations
class BehaviorLUSProbeHead;
class BehaviorLUSProbeJoint;
class BehaviorLUSProbeShaft;
class BehaviorLUSBackground;
class BehaviorLUSOutline;
class BehaviorLUSText;
class BehaviorLUSMarker;
struct MarkerType;


class BehaviorLUS : public ui3BehaviorBase
{
public:
    BehaviorLUS(const std::string & name);
    ~BehaviorLUS();

    void Startup(void);
    void Cleanup(void);
    void ConfigureMenuBar(void);
    bool RunForeground(void);
    bool RunBackground(void);
    bool RunNoInput(void);
    void SetUpScene(void);
    void Configure(const std::string & configFile);
    bool SaveConfiguration(const std::string & configFile);
    inline ui3VisibleObject * GetVisibleObject(void) {
        return this->VisibleList;
    }
    void SetJoints(double A1, double A2, double insertion, double roll);
    void SetProbeColor(double r, double g, double b);
    void SetText(BehaviorLUSText *obj, const std::string & text);
    void CheckLimits(double p, double y, double i, double r);
    void GetMeasurement(void);
    void AddMarker();
    void RemoveLastMarker();

    void MasterClutchPedalCallback(const prmEventButton & payload);
    void CameraControlPedalCallback(const prmEventButton & payload);
    void DropMarkerCallback(void);
    void RemoveMarkerCallback(void);
    vctFrm3 GetCurrentCursorPositionWRTECM(void);
    vctFrm3 GetCurrentCursorPositionWRTECMRCM(void);
    

protected:
    unsigned long Ticker;
    void FirstButtonCallback(void);
    void EnableMapButtonCallback(void);
    void ReSetMapButtonCallback(void);
    void Master_clutch_callback(void);
    void PrimaryMasterButtonCallback(const prmEventButton & event);
    void SecondaryMasterButtonCallback(const prmEventButton & event);
    void UpdateCursorPosition(void);
    void UpdateVisibleMap(void);

    StateType PreviousState;
    bool PreviousMaM;
    bool RightMTMOpen, prevRightMTMOpen, LeftMTMOpen;
    bool ClutchPressed,CameraPressed, MarkerDropped, MarkerRemoved;
    bool Following;
    bool MapEnabled;

    vctDouble3 PreviousCursorPosition;
    vctDouble3 CursorPosition;
    vctDouble3 PreviousSlavePosition;
    vctDouble3 Offset;
    vctDouble3 CursorOffset;
    vctFrm3 Position;
    

    void OnStreamSample(svlSample* sample, int streamindex);
    ui3ImagePlane* ImagePlane;

    ui3SlaveArm * Slave1;
    ui3SlaveArm * ECM1;
    prmPositionCartesianGet Slave1Position;
    prmPositionCartesianGet ECM1Position;
 
    mtsFunctionRead GetJointPositionSlave;
    mtsFunctionRead GetCartesianPositionSlave;
    mtsFunctionRead GetJointPositionECM;
    prmPositionJointGet JointsSlave;
    prmPositionJointGet JointsECM;

#if 0
    mtsFunctionRead GetJointPositionECM;
    prmPositionJointGet JointsECM;
#endif
    bool MeasurementActive;
    vctDouble3 MeasurePoint1;
    
    typedef  std::list<MarkerType*> MarkersType;
    MarkersType Markers;
    

private:

    ui3VisibleList * VisibleList; // all actors for this behavior
    ui3VisibleList * ProbeList; // all actors moving wrt the slave arm
    ui3VisibleList * ProbeListJoint1;
    ui3VisibleList * ProbeListJoint2;
    ui3VisibleList * ProbeListJoint3;
    ui3VisibleList * ProbeListShaft;
    ui3VisibleList * BackgroundList;
    ui3VisibleList * TextList;
    ui3VisibleList * MarkerList;

    int MarkerCount;
    BehaviorLUSProbeHead  *ProbeHead;
    BehaviorLUSProbeJoint *ProbeJoint1;
    BehaviorLUSProbeJoint *ProbeJoint2;
    BehaviorLUSProbeJoint *ProbeJoint3;
    BehaviorLUSProbeShaft *ProbeShaft;
    BehaviorLUSBackground *Backgrounds;
    BehaviorLUSOutline    *Outline;
    BehaviorLUSText       *WarningText, * MeasureText;
    BehaviorLUSMarker     *MapCursor;
    BehaviorLUSMarker * MyMarkers[20];

    vctFrm3 ECMtoECMRCM;
    vctFrm3 ECMRCMtoVTK;
    double ECMRCMtoVTKscale;
    vctDouble3 CenterRotatedTranslated;

};

