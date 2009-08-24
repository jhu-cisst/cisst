/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: robCollaborativeControlForce.h 683 2009-08-14 21:40:14Z gsevinc1 $

  Author(s): Gorkem Sevinc, Anton Deguet
  Created on: 2009-08-20

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robCollaborativeControlForce_h
#define _robCollaborativeControlForce_h

#include <cisstCommon.h>
#include <cisstParameterTypes.h>
#include <cisstVector.h>

class CISST_EXPORT robCollaborativeControlForce {

public:
    robCollaborativeControlForce(void);
    ~robCollaborativeControlForce(void) {};
    /*!
        Set Parameters function takes arguments from the parent class to mutate the values of;
        Force Limit, Scale Factor Force Coefficient, Force Mode, Master & Slave Clutch.
        These values are currently being used by devicesTutorialExample3 GUI via 
        devSensableHDMasterSlave class.
    */
    void SetParameters(const mtsDouble & commandedForceLimit, const mtsDouble & commandedScaleFactor, 
                       const mtsDouble & commandedForceCoefficient, const mtsInt & commandedForceMode, 
                       const mtsBool & commandedMasterClutchGUI, const mtsBool & commandedSlaveClutchGUI,
                       const mtsBool & commandedMasterSlaveClutchGUI);

    /*!
        Update function's goal is to carry out position to force control. Two position parameters
        and two clutch parameters are provided by the parent class. This function uses those positions
        and the clutch parameters to calculate the desired force values for teleoperation. 
        ** Designed for Phantom Omni teleoperation, has not been tested on other platforms.
    */
    void Update(const prmPositionCartesianGet & p1, 
                const prmPositionCartesianGet & p2,
                const bool ClutchMaster,
                const bool ClutchSlave,
                vctFixedSizeVector<double, 6> & ForceMaster,
                vctFixedSizeVector<double, 6> & ForceSlave);

protected:
    /*! 
        Parameters being used by the Update function for teleoperation
        
        vct3 declarations include the offsets for the workspace, clutch overall,
        
        clutch left and right, and the goals for first and second device.
        prm declarations include the forces, current and temporary positions. 
        
        Other declarations are mostly used to manipulate the above mentioned
        data, such as temporary booleans, scale factor, force limit, etc.
    */
    vct3 WorkspaceOffset;
    vct3 ClutchOffset;
    vct3 LeftClutchOffset;
    vct3 RightClutchOffset;
    vct3 LeftClutchMSOffset;
    vct3 RightClutchMSOffset;
    vct3 Error;
    vct3 p1Goal;
    vct3 p2Goal;
    vct3 p1RGoal;
    vct3 p2RGoal;
   
    prmForceCartesianSet    firstDeviceForce;
    prmForceCartesianSet    secondDeviceForce;
    prmPositionCartesianGet p1;
    prmPositionCartesianGet p2;
    prmPositionCartesianGet p1Clutched;
    prmPositionCartesianGet p2Clutched;
    prmPositionCartesianGet p1R;
    prmPositionCartesianGet p2R;
    prmPositionCartesianGet p1Last;
    prmPositionCartesianGet p2Last;

    bool        firstIteration;
    bool        clutchDone;
    bool        bothClutched;
    bool        clutchOffsetAdd;
    mtsBool     MasterClutch;
    mtsBool     SlaveClutch;
    mtsBool     MasterSlaveClutch;
    mtsInt      clutchMode;
    mtsInt      ForceMode;
    mtsDouble   ForceMasterCoefficient;
    mtsDouble   ScaleFactor;
    mtsDouble   FMax;
    double      ForceFeedNormMaster;
    double      ForceFeedNormSlave;
    int         MasterDeviceNo;
    int         SlaveDeviceNo;

private:

};

#endif //robCollaborativeControlForce
